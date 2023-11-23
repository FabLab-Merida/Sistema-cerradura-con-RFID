from flask import jsonify, Blueprint, request, make_response, render_template
from src import database
from . import ui
from flask_restx import Resource, Api

doors = Blueprint('doors', __name__)
ui.register_blueprint(doors, url_prefix='/doors')


@doors.route("/edit/<door_id>")
def get_edit_page(door_id):
    door:database.Doors = database.session.query(database.Doors).filter(database.Doors.id_puerta == door_id).one()
    return render_template("doors/edit_form.html",door=door)
