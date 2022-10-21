
#include <SPI.h>
#include "RF24.h"
#include <MFRC522.h>
/************/
/* Constantes */

#define SERIAL_BITRATE 115200

/* RFID */
#define RFID_PIN_RESET 6
#define RFID_PIN_SPI 3

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
}

MFRC522 mfrc522 (RFID_PIN_SPI, RFID_PIN_RESET);
RF24 radio (RF_PIN_CE, RF_PIN_CS);

uint8_t direcciones_pipes[][6] = {"2mas", "2nod"};

/*
 * Definicion de pipes
 * **/


const int TriggerPin = 4;      //Trig pin
const int EchoPin = 3;         //Echo pin

void setup()
{
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
//    radio.openReadingPipe(1, direcciones_pipes[1]);
//    radio.startListening();


    Serial.println("Setup Finalizado");
}

void loop()
{
//    Serial.println("loop");
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return;
    }

    // Si hay una tarjeta cerca, que la eleccione
    // En caso contrario que no continúe
    if (!mfrc522.PICC_ReadCardSerial()) {
        return;
    }
   
    // Mostrar información de la tarjeta por el monitor serie
    Serial.print(F("UID de la tarjeta:"));
    mostrarByteArray(mfrc522.uid.uidByte, mfrc522.uid.size);  // Motrar el UID
    radio.write(&mfrc522.uid.uidByte, mfrc522.uid.size);
    delay(1000);


}
