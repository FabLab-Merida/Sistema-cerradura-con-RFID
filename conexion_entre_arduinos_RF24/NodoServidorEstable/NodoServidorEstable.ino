#include <SPI.h>
#include "RF24.h"
#include <MFRC522.h>
/************/
/* Constantes */

# define SERIAL_BITRATE 115200

/* RFID */
#define RFID_PIN_RESET 9 // Pin 9 para el reset del fid
#define RFID_PIN_SPI 10

/* RF radio */
#define RF_PIN_CE 9
#define RF_PIN_CS 10
#define RF_CHANNEL 108 // Por encima de la mayoría de canales WiFi
#define RF_READ_PIPE_NUMBER 1 // http://maniacalbits.blogspot.com/2013/04/rf24-addressing-nrf24l01-radios-require.html

/* Inicializaciones de clase */

//MFRC522 mfrc522 (RFID_PIN_SPI, RFID_PIN_RESET);
MFRC522::MIFARE_Key clave = {keyByte: {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
// Configura la radio en el bus SPI
RF24 radio (RF_PIN_CE, RF_PIN_CS);

uint8_t direcciones_pipes[][6] = {"2mas", "2nod"};
/*
 * Definicion de pipes
 * **/

bool AccesoPermitido[1] = {false};

void setup()
{
    Serial.begin(SERIAL_BITRATE);
    delay(1000);
    radio.begin();
    radio.setChannel(RF_CHANNEL);
    radio.setPALevel(RF24_PA_MAX); // 0dBm
    radio.setDataRate( RF24_250KBPS ) ;

    radio.openReadingPipe(1, direcciones_pipes[0]);
    radio.openWritingPipe(direcciones_pipes[1]);

    radio.startListening();

//    mfrc522.PCD_Init();
    Serial.println("Setup Finalizado");
}
bool newRequest = false;
void loop()
{   
  comprobarRadio();
  responder();
    //  else {
      
    //   radio.write(direcciones_pipes[1], "YES", sizeof("YES"));
    // }
    
    // // Mostrar información de la tarjeta por el monitor serie

}

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


void comprobarRadio() {
  if (radio.available())
    {
      while (radio.available())
      {
        radio.read( &clave, MFRC522::MF_KEY_SIZE );
      }
      newRequest = true;
      Serial.println(F("UID de la tarjeta:"));
      mostrarByteArray(clave.keyByte, MFRC522::MF_KEY_SIZE);
      Serial.print("Coinciden los valores? ");
      if (clave.keyByte[0] == 0x1A) {
        AccesoPermitido[0] = true;
        Serial.println("Si");
      } else {
        AccesoPermitido[0] = false;
        Serial.println("No");
      }
    }
}
void responder() {
  if (newRequest) {
    radio.stopListening();
    bool rslt;
    delay(1000);
    rslt = radio.write( &AccesoPermitido, sizeof(AccesoPermitido) );
    radio.startListening();
    Serial.println("Respondido");
    newRequest = false;
  }
  
}
