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

//high pulse length of the bit's divider
#define BL999_DIVIDER_PULSE_LENGTH 550

//length of the start bit low pulse
#define BL999_START_BIT_LENGTH 9000

//binary 1 low pulse length
#define BL999_BIT_1_LENGTH 3900

//binary 0 low puls length
#define BL999_BIT_0_LENGTH 1850

#define BL999_DATA_ARRAY_SIZE 9

static byte bl999_data[BL999_DATA_ARRAY_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static volatile boolean bl999_sendallbits = false;
static volatile boolean bl999_message_ready = false;

void setup() {
	Serial.begin(115200);
	sensor.begin();												//  initialise sensor AM2320
	pinMode(prd, OUTPUT);										// pin mode for FS 1000A
	bl999_data[0] = 15;											// 1010
	bl999_data[1] = 15;											// 0111	PowerUUID=101001=43 + chanel 01=1
	bl999_data[2] = 15;											// battery condition ???? =0000
	bl999_data[3] = 15; //(int)(temperature & 15);
	bl999_data[4] = 15; //(int)((temperature & 240) >> 4);
	bl999_data[5] = 15; //(int)((temperature & 3840) >> 8);
	bl999_data[6] = 15; //(int)(humidity & 15);
	bl999_data[7] = 15; //(int)((humidity & 240) >> 4);
}
void loop() {
	ms = millis();
		// ������� ������������� ������ 500 ��   
		if ((ms - ms1) > 500 || ms < ms1) {
			ms1 = ms;
			// ����������� ���������       
			digitalWrite(13, led_stat);
			led_stat = !led_stat;
		}
	}

boolean _bl999_nextbit(byte ii, byte jj) {
	return ((bl999_data[ii] >> jj) & 1);
}
void _bl999_fillDataArray(byte bitNumber, byte value) {
	byte dataArrayIndex = bitNumber / 4;
	byte bitInNibble = bitNumber % 4;

	if (bitInNibble == 0) {
		// if it's the first bit in nibble -
		// clear nibble since it could be filled with random data at this time
		bl999_data[dataArrayIndex] = 0;
	}

	//Write all nibbles in reversed order
	//so it will be easier to do calculations later
	bl999_data[dataArrayIndex] |= (value << bitInNibble);
}
		


