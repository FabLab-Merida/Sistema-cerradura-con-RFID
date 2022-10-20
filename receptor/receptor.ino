#include <SPI.h>  
#include "RF24.h" 
#include <MFRC522.h>

RF24 myRadio (9,10); 
struct package
{
  int id=0;
  float temperature = 0.0;
  char  text[100] ="empty";
};
byte addresses[][6] = {"0"}; 
typedef struct package Package;
Package data;
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
  delay(1000);
  myRadio.begin(); 
  myRadio.setChannel(115); 
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS ) ; 
  myRadio.openReadingPipe(1, addresses[0]);
  myRadio.startListening();
}
void loop()
{
  if ( myRadio.available()) 
  {
    while (myRadio.available())
    {
      myRadio.read( &keyA, MFRC522::MF_KEY_SIZE );
    }

    mostrarByteArray(keyA.keyByte, MFRC522::MF_KEY_SIZE);
    //Serial.print("\nPackage:");
    //Serial.print(data.id);
    //Serial.print("\n");
    //Serial.println(data.temperature);
    //Serial.println(data.text);
  }
}
