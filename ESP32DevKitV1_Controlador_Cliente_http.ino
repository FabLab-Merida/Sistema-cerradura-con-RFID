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
#define LED_PIN_DENEGADO 7
#define LED_PIN_PERMITIDO 8
#define LED_PIN_LISTO 5



const char* ssid     = "FABLAB";
const char* password = "FabLabMerida2020";

const char* host = "158.49.92.110";
const char* nodopuerta   = "1";
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


void setup()
{
    Serial.begin(SERIAL_BITRATE);
    SPI.begin();
    mfrc522.PCD_Init();
    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    delay(2000);
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Setup Finalizado");
}

/** Prubeas **/
void loop()
{
    if (!mfrc522.PICC_IsNewCardPresent()) {
        Serial.println("Esperando tarjeta");
        delay(2000);
        return;
    }

    // Si hay una tarjeta cerca, que la eleccione
    // En caso contrario que no continúe
    if (!mfrc522.PICC_ReadCardSerial()) {
        Serial.println("Esperando tarjeta");
        delay(2000);
        return;
    }
    mostrarByteArray(mfrc522.uid.uidByte, mfrc522.uid.size);  // Motrar el UID
    String strUID1 = String(mfrc522.uid.uidByte[0]) + "-" + String(mfrc522.uid.uidByte[1]) + "-" + String(mfrc522.uid.uidByte[2]) + "-" + String(mfrc522.uid.uidByte[3]);
    Serial.println(strUID1);
    autenticar(strUID1);
}
bool autenticar (String privateKey) {
      Serial.print("connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    HTTPClient http;
    const int httpPort = 5000;

    // We now create a URI for the request
    String url = "http://158.49.92.110";
    url += "/";
    url += "?nodo=";
    url += "1";
    url += "&rfid=";
    url += privateKey;
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


        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println("Acceso Concedido");

            } else if (httpCode == 500) {
              Serial.println("Usuario no encontrado");
            } else if (httpCode == 401) {
              Serial.println("Acceso denegado");
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();

    Serial.println();
    Serial.println("closing connection");
    return true;
}

