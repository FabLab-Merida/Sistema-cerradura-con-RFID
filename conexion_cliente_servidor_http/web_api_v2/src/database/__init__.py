from datetime import datetime

from sqlalchemy import create_engine, Column, Integer, String, ForeignKey, Table, DateTime, Boolean, Float
from sqlalchemy.orm import sessionmaker, relationship
# Import the scoped_session class
from sqlalchemy.orm import scoped_session

from .base import Base


# Create the engine and the Session class

# engine = create_engine('sqlite:////home/pi/DATA/my_database.db')
engine = create_engine('sqlite:///test.db')
Session = sessionmaker(bind=engine)
from .models import *
# Define a many-to-many relationship between "Doors" and "Usuarios"
# doors_usuarios = Table('doors_usuarios', Base.metadata,
#     Column('door_id', Integer, ForeignKey('doors.id_puerta')),
#     Column('usuario_rfid', String, ForeignKey('usuarios.rfid'))
# )

# Create the tables
Base.metadata.create_all(engine)


# Create an instance of the scoped_session class
session = scoped_session(sessionmaker(bind=engine))
