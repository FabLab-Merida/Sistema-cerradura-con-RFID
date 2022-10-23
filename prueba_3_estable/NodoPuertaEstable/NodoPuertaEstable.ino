
#include <SPI.h>
#include "RF24.h"
#include <MFRC522.h>
/*************/
/* Constantes */

#define SERIAL_BITRATE 115200

/* RFID */
#define RFID_PIN_RESET 6
#define RFID_PIN_SPI 3
/* LUD LED DE PRUEBA*/
#define LED_PIN_DENEGADO 7
#define LED_PIN_PERMITIDO 8
#define LED_PIN_LISTO 5

/* RF radio */
#define RF_PIN_CE 9
#define RF_PIN_CS 10
#define RF_CHANNEL 108 // Por encima de la mayoría de canales WiFi
//#define RF_READ_PIPE_NUMBER 1 // http://maniacalbits.blogspot.com/2013/04/rf24-addressing-nrf24l01-radios-require.html
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
RF24 radio (RF_PIN_CE, RF_PIN_CS);

uint8_t direcciones_pipes[][6] = {"2mas", "2nod"};

/*
 * Definicion de pipes
 * **/


const int TriggerPin = 4;      //Trig pin
const int EchoPin = 3;         //Echo pin
bool WaitingResponse = false;
void setup()
{
  pinMode(LED_PIN_LISTO, OUTPUT);
  pinMode(LED_PIN_PERMITIDO, OUTPUT);
  pinMode(LED_PIN_DENEGADO, OUTPUT);
  
  pinMode(TriggerPin, OUTPUT); // Trigger is an output pin
  pinMode(EchoPin, INPUT);     // Echo is an inpu
    Serial.begin(SERIAL_BITRATE);
    SPI.begin();
    mfrc522.PCD_Init();
    delay(1000);
    radio.begin();
    radio.setChannel(RF_CHANNEL);
    radio.setPALevel(RF24_PA_MAX); // 0dBm
    radio.setDataRate( RF24_250KBPS ) ;
    radio.openReadingPipe(1, direcciones_pipes[1]);
//    radio.startListening();
    radio.openWritingPipe(direcciones_pipes[0]);

    Serial.println("Setup Finalizado");
}

/** Prubeas **/


const byte numSlaves = 1;

int sendVal[2] = {0,0}; // this is just a dummy so I can send something
byte ackMessg; // values from the hand-helds

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 50;  //1000;
int txVal = 0;

int slaveData[numSlaves][3];
int potVal[numSlaves];
byte rxFails[numSlaves];
byte maxFails = 8;
bool newData = false;
bool dataReceived[1];
bool respondido = true;
bool EsperandoNewAuth = false;

void loop()
{

  if (newData == true) {
      Serial.print("El acceso es: ");
      Serial.print(dataReceived[0]);
      if (dataReceived[0] == 1) {    
        digitalWrite(LED_PIN_PERMITIDO, HIGH);
        digitalWrite(LED_PIN_DENEGADO, LOW);
        delay(4000);
        digitalWrite(LED_PIN_PERMITIDO, LOW);
        digitalWrite(LED_PIN_DENEGADO, LOW);
      } else {
        digitalWrite(LED_PIN_PERMITIDO, LOW);
        digitalWrite(LED_PIN_DENEGADO, HIGH);
        delay(4000);
        digitalWrite(LED_PIN_PERMITIDO, LOW);
        digitalWrite(LED_PIN_DENEGADO, LOW);
      }
      newData=false;
      EsperandoNewAuth = false;
  }

  if (!EsperandoNewAuth) {
    digitalWrite(LED_PIN_LISTO, HIGH);

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
    digitalWrite(LED_PIN_LISTO, LOW);
    digitalWrite(LED_PIN_PERMITIDO, LOW);
    digitalWrite(LED_PIN_DENEGADO, LOW);
    EsperandoNewAuth = true;
    procesar();
    esperar_respuesta();
  } else {
    
    esperar_respuesta();
  }


}

  // Mostrar información de la tarjeta por el monitor, serie
  



void procesar(){
    char rslt;
    rslt = radio.write(&mfrc522.uid.uidByte, mfrc522.uid.size);
          Serial.print("Data Sent: ");
    Serial.print(F("UID de la tarjeta:"));
    mostrarByteArray(mfrc522.uid.uidByte, mfrc522.uid.size);  // Motrar el UID
}
void esperar_respuesta(){
    Serial.println("Esperando respuesta");
  
    radio.startListening();

    if ( radio.available() ) {
        radio.read( &dataReceived, sizeof(dataReceived) );
        newData = true;

        radio.stopListening();
    }
}



