
/*
* Getting Started example sketch for nRF24L01+ radios
* This is a very basic example of how to send data from one node to another
* Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"
#include "printf.h"


/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);
/**********************************************************/

const uint64_t pipe = 0xABCDABCD71LL;


typedef struct dataStruct{
	unsigned long _micros;
	int distance;
} RobotData;


void setup() {
	printf_begin();
	Serial.begin(115200);
	Serial.println(F("Radio\\Reception robot"));
	radio.begin();

	// Set the PA Level low to prevent power supply related issues since this is a
	// getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
	radio.setPALevel(RF24_PA_LOW);

	radio.setAutoAck(true);
	radio.setRetries(0, 15);

	// Open a writing and reading pipe on each radio, with opposite addresses
	radio.openReadingPipe(1, pipe);

	radio.printDetails();

	// Start the radio listening for data
	radio.startListening();
}

void loop() {

	RobotData message;
	if (radio.available()){
		// Variable for the received timestamp
		while (radio.available()) {                                   // While there is data ready
			radio.read(&message, sizeof(message));                      // Get the payload
		}
		printf("Distance = %dcm, time = %ums \n", message.distance, message._micros);
	}


} // Loop
