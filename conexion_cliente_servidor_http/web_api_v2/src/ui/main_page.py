from flask import render_template

from src import database
from . import ui

@ui.route('/')
def main_page():
    doors = database.session.query(database.Doors).all()
    users = database.session.query(database.Usuarios).all()
    return render_template("main_page.html" , doors=doors, users=users)
