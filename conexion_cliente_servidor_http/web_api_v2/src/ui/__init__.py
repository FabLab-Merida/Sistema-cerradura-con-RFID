from flask import Blueprint

ui = Blueprint('ui', __name__,template_folder='templates')
from . import main_page
from . import doors
