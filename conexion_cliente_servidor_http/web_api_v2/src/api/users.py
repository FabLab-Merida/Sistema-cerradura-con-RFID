from flask import jsonify, Blueprint, request, make_response

from . import api
from src import database
from flask_restx import Resource, Api

users = Blueprint('users', __name__)
api.register_blueprint(users,url_prefix='/users')

_api = Api(users)
@_api.route('/add', methods=['POST'])
class UsersAdd(Resource):
    @_api.doc(
        summary="Creates a new user",
        description="Adds a new user to the database.",
        request={
            'rfid': 'RFID tag identifier',
            'nombre': 'User\'s name',
            'apellidos': 'User\'s surname',
            'door_id': 'ID of the associated door (optional)',
        },
        responses={
            201: "Successful user creation",
            400: "Invalid request data",
        },
    )
    def post(self):
        # Retrieve user data from the request form
        rfid = request.form.get('rfid')
        nombre = request.form.get('nombre')
        apellidos = request.form.get('apellidos')
        door_id = request.form.get('door_id')

        # Validate the mandatory fields
        if not rfid or not nombre or not apellidos:
            return jsonify({'message': 'Missing mandatory fields'}), 400

        # Check if the door ID is valid (if provided)
        if door_id:
            try:
                door = database.session.query(database.Doors).filter(database.Doors.id_puerta == int(door_id)).one()
            except ValueError:
                return make_response( jsonify({'message': 'Invalid door ID'}), 400)

            if not door:
                return make_response( jsonify({'message': 'Door not found'}), 400)

        # Create a new user object
        new_user = database.Usuarios(rfid=rfid, nombre=nombre, apellidos=apellidos)

        # Assign the associated door (if provided)
        if door_id:
            new_user.puertas.append(door)

        # Add the user to the database session
        database.session.add(new_user)

        # Commit the changes to the database
        database.session.commit()

        return make_response(jsonify({'message': 'User created successfully'}), 201)