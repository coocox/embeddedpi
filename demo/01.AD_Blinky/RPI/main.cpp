#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "arduPi.h"

int main()
{
	unsigned char i;
	char RxBuf[10];
	unsigned long ulADCTmp;
	unsigned char ch = 0;
	unsigned long ulpwm;

	pinMode(3, OUTPUT);
	digitalWrite(3, HIGH);
	delay(500);
	digitalWrite(3, LOW);
	delay(500);
	Serial.begin(115200);

    while(1){
		Serial.write(0xa5);
		Serial.write(0x40);
		Serial.write(0x01);
		Serial.write(0x03);
		Serial.write(0x01);
		Serial.write(ch);
		Serial.write(0x5a);
		delay(5);
		if(Serial.available()){
			Serial.readBytes(RxBuf, 8);	
			ulADCTmp = RxBuf[5];
			ulADCTmp <<= 8;
			ulADCTmp += RxBuf[4];
			printf("ADC: %d (mV)\n", ulADCTmp*3300/4095);
		}
		ulpwm = ulADCTmp * 10000/4096;		
		digitalWrite(3, HIGH);
		delayMicroseconds(ulpwm);
		digitalWrite(3, LOW);
		delayMicroseconds(10000 - ulpwm);
	}
}
