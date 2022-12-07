import json
import logging
import sqlite3

logger = logging.getLogger()
DATOS = "prueba.json"
schema = {
    "puertas": [1,2],
    "usuarios": [
        {
            "nombre_usuario": "Usuario1",
            "rfid": "AAAA-BBBB-CCCC-DDDD",
            "puertas_acceso": [1]
        },
{
            "nombre_usuario": "Usuario1",
            "rfid": "26-196-179-180",
            "puertas_acceso": [1]
        }
    ]

}


class Database:
    def __init__(self):
        self.db = {}
        try:
            with open(DATOS, "r") as f:
                self.db = json.load(f)
        except FileNotFoundError:
            pass
            # logger.exception("Error initializing db")
        self.db = schema
    def save(self):
        with open(DATOS, "w") as f:
            json.dump(self.db, f)

    def verifify_access(self, door_node, rfid_code):
        door_node = int(door_node)

        def comprueba(user):
            return user.get("rfid") == rfid_code
        #matches = list(filter(lambda x: x.get("rfid_id") == rfid_code, self.db["usuarios"]))
        matches = list(filter(comprueba, self.db["usuarios"]))
        if len(matches) == 0:
            return None
        if len(matches) == 1:
            return door_node in matches[0].get("puertas_acceso")
        return False

if __name__ == '__main__':
    db = Database()
    db.db = schema
    print(db.verifify_access(1,"AAAA-BBBB-CCCC-DDDD"))