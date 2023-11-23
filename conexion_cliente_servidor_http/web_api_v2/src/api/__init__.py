from flask import Blueprint

api = Blueprint('api', __name__)
from . import doors
from . import users
