#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <HTTPClient.h>
/*************/
/* Constantes */

#define SERIAL_BITRATE 115200

/* RFID */
#define RFID_PIN_RESET 5
#define RFID_PIN_SPI 21
/* LUD LED DE PRUEBA*/
#define LED_PIN_LISTO 14
#define LED_PIN_DENEGADO 12
#define LED_PIN_PERMITIDO 13
#define LED_PIN_BUZZER 27

#define MASTER_KEY "26-196-179-180"


const char* ssid     = "raspi-puerta";
const char* password = "raspi-puerta";


//const char* ssid     = "REDACTED";
//const char* password = "REDACTED";

// const char* host = "192.168.0.135";
const char* host = "10.0.0.1"; // IP local del servidor
const char* httpPort = "8123";
const char* nodopuerta   = "1";
String codigo = ""; //Código para cifrar la comunicacion




/* Inicializaciones de clase */

/**
 * Muestra la array de bytes
 * @param buffer: El la array de bytes
 * @param bufferSize: El tamaño de la array de bytes
 * **/
void mostrarByteArray(byte* buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
    Serial.println("");
}

MFRC522 mfrc522 (RFID_PIN_SPI, RFID_PIN_RESET);
MFRC522::MIFARE_Key clave = {keyByte: {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
bool hacer_registro = false;
bool inicio_exitoso = false;
void setup()
{

    Serial.begin(SERIAL_BITRATE);
    SPI.begin();
    mfrc522.PCD_Init();
    delay(10);

    pinMode(LED_PIN_LISTO, OUTPUT);
    pinMode(LED_PIN_DENEGADO , OUTPUT);
    pinMode(LED_PIN_PERMITIDO, OUTPUT);
    pinMode(LED_PIN_BUZZER,OUTPUT);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    delay(2000);
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    while (WiFi.status() != WL_CONNECTED) {

        delay(500);
        Serial.print(".");
    }


    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    while (!inicio_exitoso){

    digitalWrite(LED_PIN_LISTO,HIGH);

    if (inicializa_puerta() == 403) {
      Serial.println("Error de inicializacion");
      parpadear_error();
    } else {
      inicio_exitoso = true;
    }
  }
  Serial.println("Setup Finalizado");
}

/** Prubeas **/
void loop()
{
    if (!mfrc522.PICC_IsNewCardPresent()) {
        delay(500);
        return;
    }

    // Si hay una tarjeta cerca, que la eleccione
    // En caso contrario que no continúe
    if (!mfrc522.PICC_ReadCardSerial()) {
        delay(500);
        return;
    }
    digitalWrite(LED_PIN_LISTO,LOW);
    mostrarByteArray(mfrc522.uid.uidByte, mfrc522.uid.size);  // Motrar el UID
    String strUID1 = String(mfrc522.uid.uidByte[0]) + "-" + String(mfrc522.uid.uidByte[1]) + "-" + String(mfrc522.uid.uidByte[2]) + "-" + String(mfrc522.uid.uidByte[3]);
    Serial.println(strUID1);

    if (strUID1 == MASTER_KEY) {
        digitalWrite(LED_PIN_BUZZER,HIGH);
        hacer_registro = true;
        delay(400);
        digitalWrite(LED_PIN_BUZZER,LOW);
        delay(400);
        digitalWrite(LED_PIN_BUZZER,HIGH);
        delay(400);
        digitalWrite(LED_PIN_BUZZER,LOW);

        return;
    }
    if (hacer_registro) {
        // If strUID1 matches the hardcoded string, call the registrar() function
        // with the given strUID1 and default values for the other parameters
        hacer_registro = false;
        registrar(strUID1, "unknown", "unknown", "0");
    }


    if (autenticar(strUID1)) {
      digitalWrite(LED_PIN_BUZZER,HIGH);
      digitalWrite(LED_PIN_PERMITIDO,HIGH);

    } else {
      digitalWrite(LED_PIN_DENEGADO,HIGH);
    }
    delay(1500);
    digitalWrite(LED_PIN_BUZZER,LOW);
    digitalWrite(LED_PIN_PERMITIDO,LOW);
    digitalWrite(LED_PIN_DENEGADO,LOW);
    digitalWrite(LED_PIN_LISTO,HIGH);
}

//Método para obtener el primer código para realizar el cifrado
int inicializa_puerta() {
  HTTPClient http;

    // We now create a URI for the request
    String url = "http://";
    url += host;
    url += ":";
    url += httpPort;
    url += "/api/inizializar_puerta";
    url += "?nodo=";
    url += "1";
    http.begin(url);

    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    Serial.println(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n\r\n");

    int httpCode = http.GET();
    codigo = http.getString();

    Serial.print("Puerta inicializada con codigo: " + codigo);
    return httpCode;
}


void parpadear_error(){
  digitalWrite(LED_PIN_PERMITIDO,LOW);
  digitalWrite(LED_PIN_DENEGADO,LOW);
  for (int i=0; i<10;i++) {
    digitalWrite(LED_PIN_DENEGADO,HIGH);
    digitalWrite(LED_PIN_BUZZER,HIGH);
    delay(500);
    digitalWrite(LED_PIN_PERMITIDO,LOW);
    digitalWrite(LED_PIN_DENEGADO,LOW);
    digitalWrite(LED_PIN_BUZZER,LOW);
    delay(500);
  }
}

bool autenticar (String privateKey) {
      Serial.print("connecting to ");
    Serial.println(host);


    // Use WiFiClient class to create TCP connections
    HTTPClient http;


  String* groups = split(privateKey, '-');

  // Convierte la clave a float y luego a entero
  int key = (int)(atof(codigo.c_str()) * 10000);

  // Recorre cada grupo de elementos y lo cifra multiplicándolo por la clave
  for (int i = 0; i < 4; i++) {
    int group = groups[i].toInt();
    group = group * key;
    groups[i] = String(group);
  }

// Une los grupos de elementos cifrados en un string separados por el guion
    String encryptedString = join(groups, '-');



    // We now create a URI for the request
    String url = "http://";
    url += host;
    url += ":";
    url += httpPort;
    url += "/api/verificar_acceso";
    url += "?nodo=";
    url += "1";
    url += "&rfid=";
    url += encryptedString;
    http.begin(url);

    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    Serial.println(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");

    int httpCode = http.GET();
    Serial.println("http code");
    Serial.print(httpCode);
    Serial.println();
    bool acceso_concedido = false;

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                codigo = http.getString(); //obtenemos el nuevo código
                Serial.println("Acceso Concedido");
                acceso_concedido = true;

            } else if (httpCode == 500) {
              Serial.println("Usuario no encontrado");
              acceso_concedido = false;
            } else if (httpCode == 401) {
              Serial.println("Acceso denegado");
              acceso_concedido = false;
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        http.end();

    Serial.println();
    Serial.println("closing connection");
    return acceso_concedido;
}

// This function sends an HTTP POST request to the /api/add_user route
// with the given strUID, nombre, apellidos and puertas parameters
void registrar(String strUID, String nombre, String apellidos, String puertas) {
    // Build the URL for the POST request
    String url = "http://";
    url += host;
    url += ":";
    url += httpPort;
    url += "/api/add_user";


    // Create an instance of the HTTPClient class
    HTTPClient http;
    http.begin(url);

    // Create the data to be sent in the POST request
    String data = "rfid=" + strUID + "&nombre=" + nombre + "&apellidos=" + apellidos + "&puertas=1";
    Serial.println(data);
    http.begin(url);
    // Send the POST request to the /api/add_user route
    http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // Set the request header
    //http.addBody(data); // Set the request body
    int httpCode = http.POST(data);
    if (httpCode != 200) {
        // Handle the error if the request failed
        Serial.println("Error sending POST request");
    } else {
        // Parse the response from the server
        String response = http.getString();
        Serial.println(response);
    }
}

String* split(String str, char delimiter) {
  int count = 0;
  int strLength = str.length();

  // Calcula el número de substrings que se obtendrán al separar el string por el delimitador
  for (int i = 0; i < strLength; i++) {
    if (str[i] == delimiter) {
      count++;
    }
  }

  // Crea un array de strings con el tamaño necesario
  String* result = new String[count + 1];

  int j = 0;
  int lastIndex = -1;

  // Recorre el string y separa las substrings utilizando el delimitador
  for (int i = 0; i < strLength; i++) {
    if (str[i] == delimiter) {
      result[j] = str.substring(lastIndex + 1, i);
      j++;
      lastIndex = i;
    }
  }

  // Añade la última substring al array
  result[j] = str.substring(lastIndex + 1, strLength);

  return result;
}

String join(String* strArray, char separator) {
  String result = "";

  // Recorre el array de strings y añade cada uno de ellos al resultado separándolos por el separador
  for (int i = 0; i < sizeof(strArray); i++) {
    result += strArray[i];
    if (i < sizeof(strArray) - 1) {
      result += separator;
    }
  }

  return result;
}