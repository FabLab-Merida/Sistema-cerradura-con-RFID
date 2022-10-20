// http://maniacbug.github.io/RF24/classRF24.html
	//~ - CONNECTIONS: nRF24L01 Modules See:
	//~ http://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo
	//~ 1 - GND
	//~ 2 - VCC 3.3V !!! NOT 5V
	//~ 3 - CE to Arduino pin 9
	//~ 4 - CSN to Arduino pin 10
	//~ 5 - SCK to Arduino pin 13
	//~ 6 - MOSI to Arduino pin 11
	//~ 7 - MISO to Arduino pin 12
	//~ 8 - UNUSED

#include <SPI.h>
//~ #include <nRF24L01.h>
//~ #include <RF24.h>
#include "/mnt/sdb1/SGT-Docs/ModelRailways/NRF24Control/RF24mod/nRF24L01mod.h"
#include "/mnt/sdb1/SGT-Docs/ModelRailways/NRF24Control/RF24mod/RF24mod.h"
#include "/mnt/sdb1/SGT-Docs/ModelRailways/NRF24Control/RF24mod/RF24mod.cpp"
#include "/mnt/sdb1/SGT-Docs/ModelRailways/NRF24Control/RF24mod/RF24_config_mod.h"

#define CE_PIN   9
#define CSN_PIN 10

#define ledPin A5
#define dirnPin 4
#define potPin A4

//~ const int thisID = 2; // UNO
const int thisID = 1; // Breadboard


const uint64_t deviceID = systemID + thisID;

int speedVal;
int otherData;
bool newRequest = false;


RF24 radio(CE_PIN, CSN_PIN);

int recVal[2];  
int ackData[3] = {0,0,0};

unsigned long prevLedMillis;
unsigned long intervalMillis;
unsigned long ledOnInterval = 10;
unsigned long ledOffInterval = 1000;
bool ledOn = false;

void setup() {
	
	Serial.begin(9600);
	pinMode(ledPin, OUTPUT);
	pinMode(dirnPin, INPUT_PULLUP);
	ackData[0] = thisID;
	delay(1000);
	Serial.println("Hand Controller Starting");
	radio.begin();
	radio.setDataRate(RF24_250KBPS);
	radio.setChannel(10); // default 76
	radio.enableAckPayload();
	radio.openReadingPipe(1,deviceID);
	radio.writeAckPayload(1, ackData, sizeof(ackData));
	radio.startListening();
}

void loop() {
	updateValues();
	checkRadio();
	blinkLed();
}

//=================

void updateValues() {
	if (newRequest == true) {
		int speedVal = analogRead(potPin);
		byte dirVal = digitalRead(dirnPin);
		if (dirVal == HIGH) {
			ackData[1] = speedVal;
		}
		else {
			ackData[1] = -speedVal;
		}
		ackData[2] = 77;  // for future expansion
		radio.writeAckPayload(1, ackData, sizeof(ackData));
		newRequest = false;
	}
}

//==================

void blinkLed() {
	if (millis() - prevLedMillis > intervalMillis) {
		prevLedMillis += intervalMillis;
		if (ledOn == true) {
			digitalWrite(ledPin, LOW);
			intervalMillis = ledOffInterval;
			ledOn = false;
		}
		else {
			digitalWrite(ledPin, HIGH);
			intervalMillis = ledOnInterval;
			ledOn = true;
		}
	}
}

//==================

void checkRadio() {
	if ( radio.available() ) {
		bool done = false;
		done = radio.read(recVal, sizeof(recVal) );
		newRequest = true;
		Serial.print("X = ");
		Serial.print(recVal[0]);
		Serial.print(" Y = ");      
		Serial.println(recVal[1]);
		//~ radio.writeAckPayload(1, ackData, ackLen);
	}
}
