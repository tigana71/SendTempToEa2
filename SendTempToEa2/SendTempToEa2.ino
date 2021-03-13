/*
 Name:		SendTempToEa2.ino
 Created:	11.02.2021 23:45:20
 Author:	mike
*/

#include <Wire.h>
#include <iarduino_AM2320.h>

iarduino_AM2320 sensor;									//  iarduino_AM2320
#define prd 4											// pin DATA  FS1000A 
#define RANDOM_POWER_ID_PIN A0

//high pulse length of the bit's divider
#define BL999_DIVIDER_PULSE_LENGTH 550

//length of the start bit low pulse
#define BL999_START_BIT_LENGTH 9000

//binary 1 low pulse length
#define BL999_BIT_1_LENGTH 3900

//binary 0 low puls length
#define BL999_BIT_0_LENGTH 1850

#define BL999_DATA_ARRAY_SIZE 9

//36 bits are sent in packets by 4 bits (nibbles)
#define BL999_BITS_PER_PACKET 4

//for BL999 channel nmber only from 1 to 3
#define CHANNEL 2

static byte bl999_data[BL999_DATA_ARRAY_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };


unsigned long timing = 0;
unsigned long timingtocompare = 0;
int pulseis = LOW;
static volatile unsigned long timelenth = BL999_DIVIDER_PULSE_LENGTH;
int bitnumber = 0;
boolean startbit = true;
int j = 0;
int temperature = 0;
int humidity = 0;

void setup() {
	Serial.begin(115200);
	sensor.begin();												//  initialise sensor AM2320
	pinMode(prd, OUTPUT);										// pin mode for FS 1000A

	// CANNEL NUMBER AND PowerID
	randomSeed(analogRead(RANDOM_POWER_ID_PIN));
	//int PowerID = random(1, 64);
	int PowerID = 15;
			
	// Check for errors and then delete
	Serial.println((String) "Channel =" + CHANNEL + "PowerID =" + PowerID);
	
	bl999_data[0] = PowerID >> 2;	
	bl999_data[1] = (((PowerID << 2) | (CHANNEL << 1) | (CHANNEL >> 1)) & 15);
	bl999_data[2] = 0; //battery condition OK

	sensor.read();												//  read data from sensor
	//temperature = (int)10 * sensor.tem;
	//humidity = 100 - (int)sensor.hum;
	temperature = 227;
	humidity = 100 - (int)sensor.hum;
	
	bl999_data[3] = (int)(temperature & 15);
	bl999_data[4] = (int)((temperature & 240) >> 4);
	bl999_data[5] = (int)((temperature & 3840) >> 8);
	//bl999_data[6] = (int)(humidity & 15);
	//bl999_data[7] = (int)((humidity & 240) >> 4);
	
	bl999_data[6] = 15;
	bl999_data[7] = 15;
	Serial.println((String) "CEHCOP AM2320:  T=" + temperature + "/10*C, PH=" + (100 - humidity) + "%");
	// check sum
	int sum = 0;
	for (byte i = 0; i < BL999_DATA_ARRAY_SIZE - 1; i++) {
		sum += bl999_data[i];
	}
	sum &= 15;	// clear higher bits
	bl999_data[BL999_DATA_ARRAY_SIZE - 1] = sum;	//calculat check sum matches received
	
}
void loop() {
	timing = micros();
	if ((timing - timingtocompare) > timelenth || timing < timingtocompare) {
		//Serial.println(timing - timingtocompare);
		//Serial.println(pulseis);
		timingtocompare = timing;
		if (pulseis == HIGH) {
			if (startbit) {
				timelenth = BL999_START_BIT_LENGTH;
				startbit = false;
				bitnumber = -1;
			}
			else {
				if (_bl999_GetbitfromDataArray(bitnumber) == 0) {	// смотрим следующий бит
					timelenth = BL999_BIT_0_LENGTH;
				}
				else {
					timelenth = BL999_BIT_1_LENGTH;
				}
			}
		}
		else { //pulse is LOW
			timelenth = BL999_DIVIDER_PULSE_LENGTH;
			++bitnumber;
			//Serial.print((String) " " + bitnumber);
			//Serial.print(pulseis);
			if (bitnumber > 35) {
				++j;
				startbit = true;
				if (j > 3) {
					j = 0;					
					// pulseis = LOW;
					Serial.println("delay");
					delay(27000);

				}
			}
		}
		// Инвертируем pulseis
		pulseis = !pulseis;
		digitalWrite(prd, pulseis);
	}
	
}

byte _bl999_GetbitfromDataArray(byte bitNumber) {
	byte dataArrayIndex = bitNumber / BL999_BITS_PER_PACKET;
	byte bitInNibble = bitNumber % 4;
	return (bl999_data[dataArrayIndex] >> bitInNibble) & 1 ;
}
		

