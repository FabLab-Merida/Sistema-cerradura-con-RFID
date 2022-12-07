from flask import Flask
from flask import request

from database import Database

app = Flask(__name__)
db = Database()

@app.route('/')
def hello_world():  # put application's code here
    a = request.args.get("nodo")
    if not a:
        a = 0
    else:
        a = int(a)
    print(request.args)
    if a not in db.db.get("puertas"):
        return "Puerta no existe"
    b = request.args.get("rfid")
    # match db.verifify_access(a,b):
    #     case True:
    #         return "Acceso concedido"
    #     case False:
    #         return "Acceso denegado"
    #     case None:
    #         return "Error de la base de datos. Usuario no encontrado"
    result = db.verifify_access(a, b)
    if result:
        return "Acceso concedido", 200
    if result is None:
        return "Error de la base de datos. Usuario no encontrado", 500
    else:
        return "Acceso denegado", 401




if __name__ == '__main__':
    app.run(host="0.0.0.0", port=80)
