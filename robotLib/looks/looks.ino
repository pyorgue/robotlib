#include "../robotLib/looks.h"

int THINKING_LIGHT_PIN = 13;
int LIGHT_PIN = 12;


Light light(LIGHT_PIN);
FlashingLight flashingLight(THINKING_LIGHT_PIN, 1000);


void setup() {
	Serial.begin(9600);
	Serial.print("Hello");
	Serial.print("Example \"looks.h\" library");

	//turn on light
	light.turnOn();
	flashingLight.turnOn();
}

void loop() {
	//update flashing light
	flashingLight.update();
}