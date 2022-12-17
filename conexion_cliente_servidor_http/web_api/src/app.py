import random

from flask import Flask, render_template, redirect, url_for, request, make_response

import src.database as database
app = Flask(__name__)
from src import database as db


diccionario_puertas = {}


def actualizar_codigo_puerta(codigo_puerta: int):
    """
    Genera un codigo de cifrado con el que el cliente cifrará el codigo rfid la proxima vez que se comunique.
    Este codigo será usado por el servidor para descifrar el mensaje y así disponer del codigo rfid
    :param codigo_puerta:
    :return: El nuevo codigo
    """
    codigo_puerta = int(codigo_puerta)
    puerta = database.session.query(db.Doors).filter(db.Doors.id_puerta == codigo_puerta).first()

    if not puerta:
        return "Invalid request", 400
    else:
        numero_random = random.random()
        puerta.codigo = numero_random
    database.session.commit()
    return numero_random

@app.route('/')
def main_page():
    # Query for all users in the database
    users = database.session.query(db.Usuarios).all()

    return render_template('main_page.html', users=users)


# Rutas UI
# Lista usuarios
@app.route('/users')
def list_users():
    # Use the session variable from the database module to query the database
    users = database.session.query(database.Usuarios).all()

    # Render the template with the list of users
    return render_template('users.html', users=users)



@app.route('/add_user')
def add_user():
    # Query for all doors in the database
    doors = database.session.query(db.Doors).all()

    return render_template('add_user.html', doors=doors)

@app.route('/api/add_user', methods=['POST'])
def api_add_user():
    # Get the data from the form
    rfid = request.form['rfid']
    nombre = request.form['nombre']
    apellidos = request.form['apellidos']
    doors = request.form.getlist('doors')
    if any(value == "" or value is None for value in [rfid, nombre, apellidos, doors]):
        return "Invalid request", 400

    # Create a new user with the data from the form
    new_user = db.Usuarios(
        rfid=rfid,
        nombre=nombre,
        apellidos=apellidos,
    )

    # Assign the user to the selected doors
    for door_id in doors:
        door = database.session.query(db.Doors).filter(db.Doors.id_puerta == door_id).first()
        new_user.puertas.append(door)

    # Add the user to the session
    database.session.add(new_user)

    # Commit the change to the database
    database.session.commit()

    return redirect(url_for('main_page'))

@app.route('/edit_user/<rfid>')
def edit_user(rfid):
    # Query for the user with the specified RFID
    user = database.session.query(db.Usuarios).filter(db.Usuarios.rfid == rfid).first()

    # Query for all doors in the database
    doors = database.session.query(db.Doors).all()

    return render_template('edit-user.html', user=user, doors=doors)


@app.route('/api/edit_user', methods=['POST'])
def api_edit_user():
    # Get the data from the form
    rfid = request.form['rfid']
    nombre = request.form['nombre']
    apellidos = request.form['apellidos']
    doors = request.form.getlist('doors')

    # Query for the user with the specified RFID
    user = database.session.query(db.Usuarios).filter(db.Usuarios.rfid == rfid).first()

    # Update the user's information
    user.nombre = nombre
    user.apellidos = apellidos

    # Assign the user to the selected doors
    user.puertas = []
    for door_id in doors:
        door = database.session.query(db.Doors).filter(db.Doors.id_puerta == door_id).first()
        user.puertas.append(door)

    # Commit the changes to the database
    database.session.commit()

    return redirect(url_for('main_page'))

@app.route('/api/delete_user', methods=['POST'])
def delete_user():
    # Get the RFID from the request body
    rfid = request.get_json()['rfid']
    if not rfid:
        return "Missing rfid in post", 400
    # Query the database for the user with the specified RFID
    user = database.session.query(database.Usuarios).filter_by(rfid=rfid).one()

    # Delete the user from the database
    database.session.delete(user)
    database.session.commit()
    response = make_response('User with RFID ' + rfid + ' deleted successfully', 200)
    return response




@app.route('/add_door')
def add_door():
    return render_template('add_door.html')

@app.route('/save_door', methods=['POST'])
def save_door():
    # Get the data from the form
    nombre = request.form['nombre']

    # Create a new door with the data from the form
    new_door = db.Doors(
        nombre=nombre,
    )

    # Add the door to the session
    database.session.add(new_door)

    # Commit the change to the database
    database.session.commit()

    return redirect(url_for('main_page'))


@app.route('/doors')
def list_doors():
    # Use the session variable from the database module to query the database
    doors = database.session.query(database.Doors).all()

    # Render the template with the list of doors
    return render_template('doors.html', doors=doors)


@app.route('/edit_door/<id_puerta>')
def edit_door(id_puerta):
    # Query for the door with the specified ID
    door = database.session.query(db.Doors).filter(db.Doors.id_puerta == id_puerta).first()

    return render_template('edit-door.html', door=door)


@app.route('/access_log')
def access_log():
    offset = request.args.get('offset', 0, type=int)
    limit = request.args.get('limit', 50, type=int)

    access_logs = db.session.query(db.AccessLog, db.Usuarios) \
        .join(db.Usuarios, db.AccessLog.rfid == db.Usuarios.rfid) \
        .offset(offset) \
        .limit(limit) \
        .with_entities(
            db.AccessLog.id_puerta.label('id_puerta'),
            db.AccessLog.rfid.label('rfid_usuario'),
            db.AccessLog.timestamp.label('fecha_acceso'),
            db.Usuarios.nombre.label('nombre')
        ) \
        .all()

    return render_template('access_log.html', access_logs=access_logs)


@app.route('/api/verificar_acceso')
def verificar_acceso():
    """
    Cuando un cliente quiere saber si el usuario tiene acceso o no a la puerta, envia una peticion.
    :return:
    """

    # Get the "puerta" and "rfid" parameters from the request
    puerta = request.args.get('nodo')
    if puerta:
        puerta = int(puerta)
    else:
        return "Invalid request", 400

    # Obtenemos los datos de la puerta
    puerta: db.Doors = db.session.query(db.Doors).filter_by(id_puerta=puerta).first()
    # Obtenemos el codigo actual de esa puerta
    codigo_descifrado: float = puerta.codigo

    # Buscamos el usuario que tenga el rfid
    encrypted_string = request.args.get('rfid')

    encrypted_list = list(encrypted_string)

    # Recorre cada carácter de la lista y lo descifra utilizando la clave
    decrypted_list = []
    for char in encrypted_list:
        decrypted_char = chr(ord(char) / codigo_descifrado)
        decrypted_list.append(decrypted_char)

    # Convierte la lista de caracteres descifrados en un string
    rfid = "".join(decrypted_list)



    # Find the user with the given RFID in the "Usuarios" table
    user = db.session.query(db.Usuarios).filter_by(rfid=rfid).first()

    # Si el usuario no se ha encontrado devuelve error
    if user is None:
        return "Error de la base de datos. Usuario no encontrado", 500

    # Check if the user has access to the given "puerta"
    acceso_concedido = puerta in [door.id_puerta for door in user.puertas]

    # Create a new AccessLog object and set the "acceso_concedido" value
    access_log = db.AccessLog(id_puerta=puerta, rfid=rfid, acceso_concedido=acceso_concedido)

    # Add the access log to the database and commit the changes
    db.session.add(access_log)
    db.session.commit()
    nuevo_codigo = actualizar_codigo_puerta(puerta)

    # Return a success or error message
    if acceso_concedido:
        return nuevo_codigo, 200
    else:
        return "Acceso denegado", 401

@app.route('/api/inizializar_puerta')
def inicializar_puerta():
    puerta = request.args.get('nodo')
    if puerta:
        puerta = int(puerta)
    else:
        return "Invalid request", 400
    # Find the user with the given RFID in the "Usuarios" table
    puerta = db.session.query(db.Doors).filter_by(id_puerta=puerta).first()
    if not puerta:
        return "Error autentificacion", 403
    # Create a new code.
    nuevo_codigo = actualizar_codigo_puerta(puerta)

    return str(nuevo_codigo), 200


