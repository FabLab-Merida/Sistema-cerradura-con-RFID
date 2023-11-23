from flask import jsonify, Blueprint, request, make_response

from . import api
from src import database
from flask_restx import Resource, Api

doors = Blueprint('doors', __name__)
api.register_blueprint(doors,url_prefix='/doors')

_api = Api(doors)
@_api.route('/list', methods=['GET'])
class DoorsList(Resource):
    @_api.doc(
        summary="Lists all doors",
        responses={
            200:"Successful retrieval of door list",
        }
    )
    def get(self):
        doors: list[database.Doors] = database.session.query(database.Doors).all()
        door_data = [{
            'id': door.id_puerta,
            'nombre': door.nombre
        }
            for door in doors]
        return make_response(jsonify(door_data),200)


@_api.route('/edit', methods=['POST'],endpoint="edit_door")
class DoorsEdit(Resource):

    @_api.doc(
        summary="Edits an existing door",
        description="Updates the name of an existing door in the database.",
        request={
            'id': 'ID of the door to be edited',
            'nombre': 'New name for the door',
        },
        responses={
            200: "Successful update of door information",
            400: "Invalid request data",
            404: "Door with specified ID not found",
        },
    )
    def post(self):
        # Get the door data from the request body
        door_id = request.form.get('id_puerta')
        new_nombre = request.form.get('nombre')

        # Retrieve the door object from the database
        door = database.session.query(database.Doors).filter(database.Doors.id_puerta == door_id).first()

        if not door:
            # Door not found, return 404 error
            return make_response(jsonify({'message': f'Door with ID {door_id} not found'}, 404))

        # Update the door object's name
        door.nombre = new_nombre

        # Commit the changes to the database
        database.session.commit()

        return make_response(jsonify({'message': 'Door updated successfully'}))


@_api.route('/add', methods=['POST'])
class DoorsAdd(Resource):
    @_api.doc(
        summary="Creates a new door",
        description="Adds a new door to the database.",
        request={
            'nombre': 'Name of the door',
        },
        responses={
            201: "Successful creation of door",
            400: "Invalid request data",
        },
    )
    def post(self):
        # Get the door data from the request body
        id = request.form.get("door_id")

        nombre = request.form.get("nombre")

        # Create a new door object
        new_door = database.Doors(id_puerta=id, nombre=nombre)

        # Add the door to the database session
        database.session.add(new_door)

        # Commit the changes to the database
        database.session.commit()

        return make_response({'message': 'Door created successfully'}, 201)