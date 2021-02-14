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

void setup() {
	Serial.begin(9600);
	sensor.begin();												//  initialise sensor AM2320
	pinMode(prd, OUTPUT);										// pin mode for FS 1000A
}
void loop() {
	// read sensor and calculate bl999_data[3][4][5] for skeep temperature	
	sensor.read();												//  read data from sensor
	int temperature = (int)10 * sensor.tem;
	int humidity = (int)10 * sensor.hum;
	bl999_data[3] = (int)(temperature & 15);
	bl999_data[4] = (int)((temperature & 240) >> 4);
	bl999_data[5] = (int)((temperature & 3840) >> 8);
	bl999_data[6] = (int)(humidity & 15);
	bl999_data[7] = (int)((humidity & 240) >> 4);
	
	Serial.print("T from sensor: ");
	Serial.print(temperature);
	Serial.print(", H from sensor: ");
	Serial.println(humidity);

	// recalculate temperature for checking
	int temp = (((int)bl999_data[5] << 8)
		| ((int)bl999_data[4] << 4)
		| (int)bl999_data[3]);
	if ((bl999_data[5] >> 2) == 3) {
		//negative number, use two's compliment conversion
		temp = ~temp + 1;
		//clear higher bits and convert to negative
		temp = -1 * (temp & 4095);
	}
	if (temp |= temperature) {
		Serial.println(" Check calculation of temperature");
	}



	// проверяем номер канала
	//	int temp = ((bl999_data[1] & 1) << 1) | ((bl999_data[1] & 2) >> 1);
	
	// Вычисляем check sum
		//Sum first 8 nibbles
	unsigned int sum = 0;
	for (int i = 0; i < BL999_DATA_ARRAY_SIZE - 1; i++) {
		sum += bl999_data[i];
	}
	//clear higher bits
	sum &= 15;
	delay(10000);
	// отправляем даннаые 
		//static byte bl999_data[BL999_DATA_ARRAY_SIZE] = {5, 7, 0, 1, 7, 0, 15, 15, 0};
		// 5 7 0 1 7 0 15 15 3
		//
		//Serial.println();
		//for (byte i = 0; i < 3; i++) {
		//    digitalWrite(prd, HIGH);
		//    delay(550); 
		//    digitalWrite(prd, LOW);
		//    delay(9000);
		//    for (byte i = 0; i < BL999_DATA_ARRAY_SIZE; i++) {
		//        digitalWrite(prd, HIGH);
		//        delay(550); 
		//        digitalWrite(prd, LOW);
		//        delay(9000); 
		//        Serial.print(bl999_data[i], BIN);
		//    }
		//}
	// Смотрим что показывает метео станция



		//  delay(30000);                                             //  приостанавливаем выполнение скетча на 30 секунду




}

