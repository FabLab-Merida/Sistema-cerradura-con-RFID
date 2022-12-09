import json
import logging
import sqlite3

logger = logging.getLogger()
DATOS = "prueba.json"
schema = {
    "puertas": [1,2],
    "usuarios": [
        {
            "nombre": "Usuario1",
            "rfid": "AAAA-BBBB-CCCC-DDDD",
            "puertas_acceso": [1]
        },
{
            "nombre": "Usuario1",
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
        print(f"\n\nVerificando acceso de usuario para la puerta {door_node}")

        def comprueba(user):
            return user.get("rfid") == rfid_code
        #matches = list(filter(lambda x: x.get("rfid_id") == rfid_code, self.db["usuarios"]))
        matches = list(filter(comprueba, self.db["usuarios"]))
        if len(matches) == 0:
            print("EL USUARIO NO ESTA REGISTRADO")
            return None
        if len(matches) == 1:
            acceso = door_node in matches[0].get("puertas_acceso")
            print(f"EL USUARIO '{matches[0].get('nombre')}' {'' if acceso else 'NO ' }TIENE ACCESO A LA PUERTA")

            return acceso
        return False

if __name__ == '__main__':
    db = Database()
    db.db = schema
    print(db.verifify_access(1,"AAAA-BBBB-CCCC-DDDD"))