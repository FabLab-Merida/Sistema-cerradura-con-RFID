#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <MFRC522.h>

/****************** User Config ***************************/
/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7,8 */
RF24 radio(7, 8);
/**********************************************************/
/* RFID interface configuration apart from SPI bus */
#define RfidRst    6                                           // RFID reset pin
#define RfidSel    3                                          // RFID select pin
/*********************************************************/

byte rf_Address[6] = {"3_PTX"};                               // Radio pipe reference of this RFIDReader,
MFRC522 mfrc522(RfidSel, RfidRst);                             // Instantiate the reader MFRC522

byte RFID_Data[4];                                             // Data to Rx unit
bool resetRFID;                                                // Reset data frrom Rx

unsigned long prevMillisA;
unsigned long intervalA = 500;
unsigned long rfStatusTimeOut;
byte rfidTxCount ;
bool blockMfrc522;

const int RF_OK_Out = 4;
const int RFID_OK_Out = 5;
//byte RF_OK_Out   = 3; // Pin activacion
//byte RFID_OK_Out = 4; // Pin de echo

void setup() {
    pinMode(RF_OK_Out, OUTPUT);
    pinMode(RFID_OK_Out, OUTPUT);

    Serial.begin(115200);
    Serial.println(F("*** STARTING TRANSMITTER *** "));

    SPI.begin();                                                  // Inicializar bus SPI
    mfrc522.PCD_Init();                                          // Initialize the  MFRC522 reader
    delay(1000);

    // Setup and configure radio
    radio.begin();
    radio.setChannel(108);                                       // Above most Wifi Channels
    radio.setDataRate( RF24_250KBPS );
    radio.setRetries(5, 10);                                     // Delay, count
    radio.enableAckPayload();                                    // Allow optional ack payloads
    radio.enableDynamicPayloads();                               // Ack payloads are dynamic payload
}
void loop(void)
{
    if ( !blockMfrc522) getRFID();                               // Read the RFID tag... when not transmitting

    if (RFID_Data[0] == 0 && RFID_Data[1] == 0 && RFID_Data[2] == 0 && RFID_Data[3] == 0 )
    {
        digitalWrite(RFID_OK_Out, LOW);                            // Handle the RFID led..
        Serial.println("Rfid NO Ok");
    }
    else
    {
        digitalWrite(RFID_OK_Out, HIGH);
        Serial.println("Rfid Ok");
    }
    //******************************************************
    if ( millis() - prevMillisA > intervalA )
    {
        prevMillisA = millis();
        blockMfrc522 = true;
        send_rfid();                                             // Transmitt the last read RFID
        blockMfrc522 = false;
    }
    //******************************************************
    if ( millis() - rfStatusTimeOut > 2000)                      // Update RF24 comm status
    {
        digitalWrite( RF_OK_Out, LOW);
        Serial.println("Acceso NO Ok");
    }
    else
    {
        digitalWrite( RF_OK_Out, HIGH);
        Serial.println("Acceso Ok");
    }
    delay(2000);
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void send_rfid()
{
  radio.openWritingPipe(rf_Address);                                 // Open the writing pipe with own reference
  bool rfTxStatus = (radio.write(&RFID_Data, sizeof(RFID_Data)));     // Send the RFID info
  if (rfTxStatus)          // PROBLEM : THIS BOOL NEVER RETRUNS TRUE
  {
    if ( radio.isAckPayloadAvailable() )                              // Sent. Now read the Ack Pay load from Rx module..
    {
      radio.read(&resetRFID, sizeof(resetRFID));                      // If ackPayLoad is True , reset RFID data
    }
    if (resetRFID)
    {
      RFID_Data[0] = 0;
      RFID_Data[1] = 0;
      RFID_Data[2] = 0;
      RFID_Data[3] = 0;
    }
    rfStatusTimeOut = millis();                                     // Hold the RF_OK LED
  }
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  // FUNCTION TO READ RFID TAG AND CONVERT IT TO A 8 BYTE DATA
int getRFID()
{
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
        return 0;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
        return 0;
    }
    for (int i = 0; i < mfrc522.uid.size; i++) {
        RFID_Data[i] = mfrc522.uid.uidByte[i];
    }
    char scannedRFID[10];
    sprintf ( scannedRFID, "%02X%02X%02X%02X", RFID_Data[0], RFID_Data[1], RFID_Data[2], RFID_Data[3]);
    Serial.println(scannedRFID);

    mfrc522.PICC_HaltA();
    return 1;
  }
