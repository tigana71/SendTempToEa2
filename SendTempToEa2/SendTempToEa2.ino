/*
 Name:		SendTempToEa2.ino
 Created:	11.02.2021 23:45:20
 Author:	mike
*/
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

// static byte bl999_data[BL999_DATA_ARRAY_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

void setup() {
	Serial.begin(9600);
	sensor.begin();                                           //  инициируем работу с модулем AM2320
	pinMode(prd, OUTPUT);
}
void loop() {
	sensor.read();                                           //  читаем показания датчика
	Serial.print("T: ");
	Serial.print(sensor.tem);
	Serial.print(", H: ");
	Serial.println(sensor.hum);

	//пробуем отправить данные
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



	//  delay(30000);                                             //  приостанавливаем выполнение скетча на 30 секунду




}

