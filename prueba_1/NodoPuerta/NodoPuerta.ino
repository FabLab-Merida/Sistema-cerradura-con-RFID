#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

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
#define RF_READ_PIPE_ADDRESS
/* Inicializaciones de clase */

MFRC522 mfrc522 (RFID_PIN_SPI, RFID_PIN_RESET);

// Configura la radio en el bus SPI
RF24 radio (RF_PIN_CE, RF_PIN_CS)

void setup()
{
    Serial.begin(SERIAL_BITRATE);
    delay(1000);
    myRadio.begin();
    myRadio.setChannel(RF_CHANNEL);
    myRadio.setPALevel(RF24_PA_MAX); // 0dBm
    myRadio.setDataRate( RF24_250KBPS ) ;
    myRadio.openReadingPipe(1, addresses[0]);
    myRadio.startListening();
}