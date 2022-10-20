// This version works with the ProMicro and an Toshiba TA8428K

//  The NRF24 uses pins 9-13 on an Uno

// http://maniacbug.github.io/RF24/classRF24.html
	//~ - CONNECTIONS: nRF24L01 Modules See:
	//~ http://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo
	//~ 1 - GND
	//~ 2 - VCC 3.3V !!! NOT 5V
	//~ 3 - CE to Arduino pin 9  changed to 8
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
#include "/mnt/sdb1/SGT-Docs/ModelRailways/NRF24Control/DevelopVersion/CommonRxTx.h"

#define CE_PIN   8
#define CSN_PIN 10

// NOTE: the "LL" at the end of the constant is "LongLong" type

const byte numSlaves = 1;

int sendVal[2] = {1,2}; // this is just a dummy so I can send something
int ackMessg[3]; // values from the hand-helds

unsigned long currentMillis;
unsigned long prevMillis;
unsigned long txIntervalMillis = 50;  //1000;
int txVal = 0;


int slaveData[numSlaves][3];
int potVal[numSlaves];
byte rxFails[numSlaves];
byte maxFails = 8;
bool newData = false;

const byte numMotors = 2; 
const byte motorID[2] = {1,2};	// these correspond with the IDs sent by the slaves
								// the idea is to pick out the data for these 2 motors
byte motorPin[2][2] = {{5,6},{3,9}}; // both pins are PWM
int motorPwr[2][2] = {{0,0},{0,0}}; // space for speed and other
char motorDirn[2] = {'F','F'};
const byte mSpeed = 0;  // index into array
const byte mOther = 1;
int motorDeadBand = 30;

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

void setup() {

	delay(4000); // to give time to see the messages

	Serial.begin(9600);
	Serial1.begin(9600);
	for (byte n = 0; n < numSlaves; n++) {
		slaveData[n][0] = slaveID[n];
	}
	Serial.println("Track Control Starting");
	radio.begin();
	radio.setDataRate(dataRate);
	radio.setChannel(channelNum);
	radio.enableAckPayload();
	radio.setRetries(2,5); // delay, count  -- delay = 250µsecs + n*250
	
		// check pins are correct
	for (byte n = 0; n < numMotors; n++) {
		Serial.print("Motor Pins ");
		Serial.print(motorPin[n][0]);
		Serial.print(' ');
		Serial.print(motorPin[n][1]);
	}
}

//====================

void loop() {

	getHandHeldData();
	if (newData == true) {
		//~ showData();
		updateMotorPwr();
		outputMotorPwr();
		newData = false;
	}
}

//====================

void updateMotorPwr() {

	for (byte m = 0; m < numMotors; m++) {
		for (byte n = 0; n < numSlaves; n++) {
			if (slaveData[n][0] == motorID[m]) {
				motorPwr[m][mSpeed] = slaveData[n][1] / 4;
				motorPwr[m][mOther] = slaveData[n][2];
				Serial.print("Motor ");
				Serial.print(m);
				Serial.print(" Speed ");
				Serial.print(motorPwr[m][mSpeed]);
				Serial.print(" Other ");
				Serial.print(motorPwr[m][mOther]);
				Serial.print(" Fails ");
				Serial.print(rxFails[n]);
				Serial.println();
				
				Serial1.print("Motor ");
				Serial1.print(m);
				Serial1.print(" Speed ");
				Serial1.print(motorPwr[m][mSpeed]);
				Serial1.print(" Other ");
				Serial1.print(motorPwr[m][mOther]);
				Serial1.print(" Fails ");
				Serial1.print(rxFails[n]);
				Serial1.println();
			}
		}
	}
	Serial.println();
}

//====================

void outputMotorPwr() {

	for (byte n = 0; n < numMotors; n++) {
		if (abs(motorPwr[n][mSpeed]) <= motorDeadBand) {
			analogWrite(motorPin[n][0], 0); // both LOW = Stop but not Brake
			analogWrite(motorPin[n][1], 0);
		}
		else if (motorPwr[n][mSpeed] < -motorDeadBand) {
			char drn = 'R';
			if (drn != motorDirn[n]) {
				motorDirn[n] = drn;
				analogWrite(motorPin[n][0], 0);
				analogWrite(motorPin[n][1], 0);
			}
			analogWrite(motorPin[n][0], -motorPwr[n][mSpeed]);
			analogWrite(motorPin[n][1], 0);
		}
		else {
			char drn = 'F';
			if (drn != motorDirn[n]) {
				motorDirn[n] = drn;
				analogWrite(motorPin[n][0], 0);
				analogWrite(motorPin[n][1], 0);
			}
			analogWrite(motorPin[n][0], 0);
			analogWrite(motorPin[n][1], motorPwr[n][mSpeed]);
		}
	}
}

//================

void showData() {

	for (byte m = 0; m < numSlaves; m++) {
		Serial.print(" ID ");
		Serial.print(slaveData[m][0]);
		Serial.print("  Speed: ");
		Serial.print(slaveData[m][1]);
		Serial.print("  Other: ");
		Serial.print(slaveData[m][2]);
		Serial.println();
	}

}

//====================

void getHandHeldData() {
	currentMillis = millis();
	if (currentMillis - prevMillis >= txIntervalMillis) {
		for (byte n = 0; n < numSlaves; n++) {
			const uint64_t txId = systemID + slaveData[n][0];
			radio.openWritingPipe(txId);
			
			char rslt;
			unsigned long start = micros();
			rslt = radio.write( sendVal, sizeof(sendVal) );
			Serial.print("RSLT ");
			Serial.println(rslt);
			Serial1.print("RSLT ");
			Serial1.println(rslt);
			if (rslt != 'K') {
				rxFails[n] += 1;
			}
			else {
				rxFails[n] = 0;
			}
			unsigned long end = micros();
			Serial.print("RadioMicros ");
			Serial.println(end - start);
			Serial1.print("RadioMicros ");
			Serial1.println(end - start);
			if (rxFails[n] >= 8) { // if there have been less than 8 fails continue using the existing data
				for (byte m = 1; m < 3; m++) { // initialize values to 0
					slaveData[n][m] = 0;
				}
			}
			if ( radio.isAckPayloadAvailable() ) {
				radio.read(ackMessg,sizeof(ackMessg));

            }
		}
		newData = true;
		prevMillis = millis(); // deliberately not using += txIntervalMillis to allow for time taken to get data
	}
}