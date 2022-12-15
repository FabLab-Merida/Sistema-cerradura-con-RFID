from flask import Flask, render_template, redirect, url_for, request

import database

app = Flask(__name__)
# from database import Session, Usuarios, Doors
from src import database as db
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
    # Get the "puerta" and "rfid" parameters from the request
    puerta = request.args.get('puerta')
    if puerta:
        puerta = int(puerta)
    rfid = request.args.get('rfid')

    # Find the user with the given RFID in the "Usuarios" table
    user = db.session.query(db.Usuarios).filter_by(rfid=rfid).first()

    # If the user was not found, return an error message
    if user is None:
        return "Error de la base de datos. Usuario no encontrado", 500

    # Check if the user has access to the given "puerta"
    acceso_concedido = puerta in [door.id_puerta for door in user.puertas]

    # Create a new AccessLog object and set the "acceso_concedido" value
    access_log = db.AccessLog(id_puerta=puerta, rfid=rfid, acceso_concedido=acceso_concedido)

    # Add the access log to the database and commit the changes
    db.session.add(access_log)
    db.session.commit()

    # Return a success or error message
    if acceso_concedido:
        return "Acceso concedido", 200
    else:
        return "Acceso denegado", 401
