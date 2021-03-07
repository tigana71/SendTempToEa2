/*
 Name:		SendTempToEa2.ino
 Created:	11.02.2021 23:45:20
 Author:	mike
*/
// Some reference material
// we record pulses
// 0101 0111 0010 1111 1100 1000 1111 1111 1011

// Write them to bl999_data
// 1010     !! = 10 !! 4 bit ID of remote sensor
// 1110     !! = 14 !!+2 bit ID of remote sensor 101011=43 + 01 = 1 number of chanal
// 0100     !! = 0  !! battery condition
// 1111     !! = 15 !! temperature
// 0011     !! = 14 !! temperature
// 0001     !! = 0  !! temperature below zero
// 1111     !! = 15 !! humidity
// 1111     !! = 15 !! humidity
// 1101     !! = 3 ????? !! check sum

// Data below correspond to
// Channel: 1
// PowerUUID: 43
// Battery is Ok
// Temperature: 31.90
// Humidity: 99%

#include <Wire.h>
#include <iarduino_AM2320.h>

iarduino_AM2320 sensor;									//  iarduino_AM2320
#define prd 4											// pin DATA  FS1000A 
#define CHANNEL_PIN A0

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
int channel = 2;
// https://cxem.net/arduino/arduino129.php


static byte bl999_data[BL999_DATA_ARRAY_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };


unsigned long timing = 0;
unsigned long timingtocompare = 0;
int pulseis = LOW;
static volatile unsigned long timelenth = BL999_DIVIDER_PULSE_LENGTH;
int bitnumber = 0;
boolean startbit = true;
int j = 0;


void setup() {
	Serial.begin(115200);
	sensor.begin();												//  initialise sensor AM2320
	pinMode(prd, OUTPUT);										// pin mode for FS 1000A

	// CANNEL NUMBER
	int analogchannelValue = analogRead(CHANNEL_PIN);
	//for BL999 channel nmber only from 1 to 3
	int channel = 2;
	// https://cxem.net/arduino/arduino129.php
	if (analogchannelValue < 100) channel = 1;
	else if (analogchannelValue < 900) channel = 3;
	else channel = 2;

	// POWER ID
	randomSeed(analogchannelValue);
	int PowerID = random(1, 64);
		
	// Check for errors and then delete
	channel = 2;
	Serial.println((String) "ValuePIN A0 =" + analogchannelValue + "Channel =" + channel + "PowerID =" + PowerID);
	
	


	bl999_data[0] = 15;											// 1010
	bl999_data[1] = 15;											// 0111	PowerUUID=101001=43 + chanel 01=1
	bl999_data[2] = 15;											// battery condition ???? =0000
	bl999_data[3] = 15; //(int)(temperature & 15);
	bl999_data[4] = 15; //(int)((temperature & 240) >> 4);
	bl999_data[5] = 15; //(int)((temperature & 3840) >> 8);
	bl999_data[6] = 15; //(int)(humidity & 15);
	bl999_data[7] = 15; //(int)((humidity & 240) >> 4);
	bl999_data[8] = 15;
	
	int sum = 0;
	for (byte i = 0; i < BL999_DATA_ARRAY_SIZE - 1; i++) {
		sum += bl999_data[i];
	}
	// clear higher bits
	sum &= 15;
	//returns true if calculated check sum matches received
	bl999_data[BL999_DATA_ARRAY_SIZE - 1] = sum;
	

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
				startbit = !startbit;
				bitnumber = 0;
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
		else {
			timelenth = BL999_DIVIDER_PULSE_LENGTH;
			++bitnumber;
			if (bitnumber > 37) {
				bitnumber = 0;
				++j;
				startbit = true;
				if (j > 1) {
					Serial.println((String)" 36 bit send *" + j);
					j = 0;
					for (byte i = 0; i < 36; i++) {
						Serial.print(_bl999_GetbitfromDataArray(i));
					}
					Serial.println();
					sensor.read();												//  read data from sensor
					int temperature = (int)10 * sensor.tem;
					int humidity = (int)sensor.hum;

					bl999_data[3] = (int)(temperature & 15);
					bl999_data[4] = (int)((temperature & 240) >> 4);
					bl999_data[5] = (int)((temperature & 3840) >> 8);
					bl999_data[6] = (int)(humidity & 15);
					bl999_data[7] = (int)((humidity & 240) >> 4);

					Serial.println((String) "CEHCOP AM2320:  T=" + temperature + "*C, PH=" + humidity + "%");
					delay(30000);
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
		
	//Serial.println(dataArrayIndex);
	//Serial.println(bitInNibble);
	//Serial.println((bl999_data[dataArrayIndex] >> bitInNibble) & 1);

	return (bl999_data[dataArrayIndex] >> bitInNibble) & 1 ;
}
		

