#include<SPI.h>
#include<nRF24L01.h>
#include<RF24.h>
#include <MFRC522.h>
byte rf_Address[][6] = {"3_PTX"};   
RF24 radio(8, 9);
int rec[1] = {2};
byte RFID_Data[4];
MFRC522::MIFARE_Key keyA = {keyByte: {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
void mostrarByteArray(byte* buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
void setup()
{
  Serial.begin(115200);
  radio.begin();
  delay(100);
  radio.setChannel(108);
  radio.setDataRate(RF24_250KBPS);
  radio.setAutoAck(true);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.openReadingPipe(1,rf_Address[0]);
  radio.startListening();

  Serial.println("Setup complete");
}
void loop()
{
  Serial.println("radio");
  if (radio.available()){
    Serial.println("radio disponible");
    while (radio.available())
    {
      radio.read( &keyA, MFRC522::MF_KEY_SIZE );
    }
    
    radio.read( &RFID_Data, sizeof(RFID_Data) );
    mostrarByteArray(keyA.keyByte, MFRC522::MF_KEY_SIZE);
    Serial.print("Data got is: ");
    char scannedRFID[10];
    sprintf(scannedRFID, "%02X%02X%02X%02X", RFID_Data[0], RFID_Data[1], RFID_Data[2], RFID_Data[3]);
    Serial.println(scannedRFID);
  }
  delay(2000);
}
