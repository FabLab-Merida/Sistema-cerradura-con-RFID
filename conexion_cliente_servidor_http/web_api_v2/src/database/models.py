from datetime import datetime

from sqlalchemy import Column, Integer, String, ForeignKey, DateTime, Boolean, Float, Table
from sqlalchemy.orm import relationship

from .base import Base

usuario_puerta = Table(
    'usuario_puerta', Base.metadata,
Column('rfid_usuario', String, ForeignKey('Usuarios.rfid'), primary_key=True),
    Column('id_puerta', Integer, ForeignKey('Doors.id_puerta'), primary_key=True)

)

class Usuarios(Base):
    __tablename__ = 'Usuarios'

    rfid = Column(String, primary_key=True)
    nombre = Column(String)
    apellidos = Column(String)

    puertas = relationship(
        "Doors",
        secondary=usuario_puerta,
        back_populates="usuarios"
    )
class Doors(Base):
    __tablename__ = 'Doors'

    id_puerta = Column(Integer, primary_key=True)
    nombre = Column(String)
    codigo = Column(Float)
    usuarios = relationship(
        "Usuarios",
        secondary=usuario_puerta,
        back_populates="puertas"
    )



# Create a join table to store associations between users and doors


# class AccessLog(Base):
#     __tablename__ = 'AccessLog'
#
#     entry_id = Column(Integer, primary_key=True)
#     id_puerta = Column(Integer, ForeignKey('Doors.id_puerta'))
#
#     rfid = Column(String, ForeignKey('Usuarios.rfid'))
#     timestamp = Column(DateTime, default=datetime.utcnow)
#     acceso_concedido = Column(Boolean, default=False)


