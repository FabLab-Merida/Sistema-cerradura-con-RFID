# Sistema-cerradura-con-RFID

## Objetivo
El objetivo de este proyecto es prototipar una cerradura electrónica que lea el codigo de una tarjeta RFID, la envie al servidor por radiofrecuencia para este luego decidir si tiene el acceso permitido o no.
Adicionalmente el servidor registrará la hora y la persona que manipula la cerradura.

## Materiales de los que se dispone
- 2 Placas Arduino UNO
- 3 Modulos NRF24L01
- 1 Lector RFID RC522
- 1 Raspberry pi 4
- Leds

## Documentacion adicional:
Vease [el directorio de documentacion del proyecto](Documentacion). 

Hay un documento donde se detalla la linea temporal de este proyecto: [Linea Temporal](Documentacion/timeline.md)

# Partes del proyecto

## Conexión entre 2 Arduino
Los archivos utilizados están disponibles en la subcarpeta [prueba_3_estable](prueba_3_estable)

El programa del nodo estará constantemente a la espera de que se acerque una tarjeta rfid. Cuando esta se detecte, encenderá la radio para poder enviar por radiofrecuencia el codigo del rfid al servidor.

Cuando el servidor recibe algun codigo rfid, deja de escuchar por radio y comprueba si esa ID está autorizada o no. Posteriormente encenderá la radio en modo escritura y enviará la respuesta (permitido o no). Para finalizar se apagará el modo de escritura y encenderá el modo de escucha a la espera de que otro nodo de puerta envie el ID de la tarjeta Rfid para autorizar un nuevo acceso.

## Conexion cliente servidor con ESP32 mediante http
