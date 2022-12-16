from datetime import datetime

from sqlalchemy import create_engine, Column, Integer, String, ForeignKey, Table, DateTime, Boolean, Float
from sqlalchemy.orm import sessionmaker, relationship, declarative_base
# Import the scoped_session class
from sqlalchemy.orm import scoped_session
Base = declarative_base()
# Create the engine and the Session class
engine = create_engine('sqlite:///my_database.db')
Session = sessionmaker(bind=engine)

# Define a many-to-many relationship between "Doors" and "Usuarios"
doors_usuarios = Table('doors_usuarios', Base.metadata,
    Column('door_id', Integer, ForeignKey('doors.id_puerta')),
    Column('usuario_rfid', String, ForeignKey('usuarios.rfid'))
)


class Doors(Base):
    __tablename__ = 'doors'

    id_puerta = Column(Integer, primary_key=True)
    nombre = Column(String)
    usuarios = relationship('Usuarios', secondary=doors_usuarios, back_populates='puertas')
    codigo = Column(Float)


class Usuarios(Base):
    __tablename__ = 'usuarios'

    rfid = Column(String, primary_key=True)
    nombre = Column(String)
    apellidos = Column(String)
    puertas = relationship('Doors', secondary=doors_usuarios, back_populates='usuarios')


class AccessLog(Base):
    __tablename__ = 'access_log'

    entry_id = Column(Integer, primary_key=True)
    id_puerta = Column(Integer, ForeignKey('doors.id_puerta'))
    rfid = Column(String, ForeignKey('usuarios.rfid'))
    timestamp = Column(DateTime, default=datetime.utcnow)
    acceso_concedido = Column(Boolean, default=False)


# Create the tables
Base.metadata.create_all(engine)


# Create an instance of the scoped_session class
session = scoped_session(sessionmaker(bind=engine))