
#include <SPI.h>
#include "RF24.h"
#include <MFRC522.h>
 
#define RST_PIN         6          // Configurable, see typical pin layout above
#define SS_PIN          3         // Configurable, see typical pin layout above


void mostrarByteArray(byte* buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

RF24 myRadio (9, 10);
byte addresses[6] = {"0"};
const int TriggerPin = 4;      //Trig pin
const int EchoPin = 3;         //Echo pin
long Duration = 0;
struct package
{
  int id = 1;
  float temperature = 18.3;
  char  text[100] = "Text to be transmitted";
};
typedef struct package Package;
Package data;
void setup()
{
  pinMode(TriggerPin, OUTPUT); // Trigger is an output pin
  pinMode(EchoPin, INPUT);     // Echo is an input pin
  Serial.begin(115200);
  SPI.begin();			// Init SPI bus
  mfrc522.PCD_Init();		// Init MFRC522
  delay(1000);
  myRadio.begin();
  myRadio.setChannel(115);
  myRadio.setPALevel(RF24_PA_MAX);
  myRadio.setDataRate( RF24_250KBPS ) ;
  myRadio.openWritingPipe( addresses[0]);
  delay(1000);
}

void loop()
{
  // Si no hay una tarjeta cerca no sigue el programa
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
  myRadio.write(&mfrc522.uid.uidByte, mfrc522.uid.size);
  delay(1000);
}