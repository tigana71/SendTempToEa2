/*
 Name:		SendTempToEa2.ino
 Created:	11.02.2021 23:45:20
 Author:	mike
*/

#include <Wire.h>
#include <iarduino_AM2320.h>
#include <Narcoleptic.h>

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

//30 sec length
#define BL999_30_SEC_LENGTH 27000

#define BL999_DATA_ARRAY_SIZE 9

//36 bits are sent in packets by 4 bits (nibbles)
#define BL999_BITS_PER_PACKET 4

//for BL999 channel nmber only from 1 to 3
#define CHANNEL 2

static byte bl999_data[BL999_DATA_ARRAY_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };


unsigned long timing = 0;
unsigned long timingtocompare = 0;
int pulseis = HIGH;
static volatile unsigned long timelenth = 0;
int bitnumber = 36;
int j = 4;
int sum = 0;
int temperature = 0;
int humidity = 0;

void setup() {
	Serial.begin(115200);
	pinMode(prd, OUTPUT);										// pin mode for FS 1000A	
	sensor.begin();												//  initialise sensor AM2320
	// CANNEL NUMBER AND PowerID
	randomSeed(analogRead(RANDOM_POWER_ID_PIN));
	int PowerID = random(1, 64);
	bl999_data[0] = PowerID >> 2;	
	bl999_data[1] = (((PowerID << 2) | (CHANNEL << 1) | (CHANNEL >> 1)) & 15);
	bl999_data[2] = 0;											//battery condition OK	

	
	// Nacroleptic treaks															
	// Narcoleptic.disableMillis(); Do not disable millis - we need it for our delayCal() function.
	Narcoleptic.disableTimer1();
	Narcoleptic.disableTimer2();
	Narcoleptic.disableSerial();
	//  Narcoleptic.disableADC();  // !!! enabling this causes >100uA consumption !!!
	//Narcoleptic.disableWire();
	//Narcoleptic.disableSPI();

	// Another tweaks to lower the power consumption
	ADCSRA &= ~(1 << ADEN); //Disable ADC
	ACSR = (1 << ACD); //Disable the analog comparator
	//
	// ATmega48/88/168/328
	DIDR0 = B00111111; //Disable digital input buffers on all ADC0-ADC5 pins
	DIDR1 = (1 << AIN1D) | (1 << AIN0D); //Disable digital input buffer on AIN1/0
	//
	// ATtiny25/45/85
	//Disable digital input buffers on all ADC0-ADC3 + AIN1/0 pins
	//DIDR0 = (1<<ADC0D)|(1<<ADC2D)|(1<<ADC3D)|(1<<ADC1D)|(1<<AIN1D)|(1<<AIN0D);
}

void loop() {
	timing = micros();
	if ((timing - timingtocompare) > timelenth ) {
		timingtocompare = timing;

		if (pulseis == HIGH) {
			if (bitnumber > 35) {
				timelenth = BL999_START_BIT_LENGTH;
				bitnumber = -1;
				++j;				
			}
			else {
				if (_bl999_GetbitfromDataArray(bitnumber) == 0) {
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
		}
		// Инвертируем pulseis
		pulseis = !pulseis;
		digitalWrite(prd, pulseis);
		if (j > 3) {
			j = 0;
			sensor.read();		//  read data from sensor
			temperature = 10 * sensor.tem;
			humidity = sensor.hum - 100;
			bl999_data[3] = (temperature & 15);
			bl999_data[4] = ((temperature & 240) >> 4);
			bl999_data[5] = ((temperature & 3840) >> 8);
			bl999_data[6] = (humidity & 15);
			bl999_data[7] = ((humidity & 240) >> 4);			
			sum = 0;
			for (byte i = 0; i < BL999_DATA_ARRAY_SIZE - 1; i++) {
				sum += bl999_data[i];
			}
			sum &= 15;	// clear higher bits
			bl999_data[BL999_DATA_ARRAY_SIZE - 1] = sum;	//calculat check s			
			Narcoleptic.delay(BL999_30_SEC_LENGTH);
			timelenth = 0;			
		}
	}
}	
byte _bl999_GetbitfromDataArray(byte bitNumber) {
	byte dataArrayIndex = bitNumber / BL999_BITS_PER_PACKET;
	byte bitInNibble = bitNumber % 4;
	return (bl999_data[dataArrayIndex] >> bitInNibble) & 1 ;
}

//void stamp() {
	//Serial.print(pulseis);
	//Serial.print(bitnumber);
	//Serial.println(j);
	//Serial.println((String) "pulse is " + pulseis + "timing "+ (timing - timingtocompare) +" bitnumber " + bitnumber + " j " + j + " VAlue " + _bl999_GetbitfromDataArray(bitnumber)+"timelenth"+timelenth);
	//Serial.println((String)  "timing "+ (timing - timingtocompare) +" timelenth "+ timelenth);
//}

		

