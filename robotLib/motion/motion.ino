#include "../robotLib/motion.h"

int NECK_PIN = 9;

Neck robotNeck(NECK_PIN, 10);

void setup() {
	Serial.begin(9600);
	Serial.print("Hello");
	Serial.print("Example \"motion.h\" library");

	//initialize the neck
	robotNeck.attach();
	robotNeck.turnCenter();
}

void loop() {
	robotNeck.turnCenter();
	delay(1000);
	robotNeck.turnRight();
	delay(1000);
	robotNeck.turnCenter();
	delay(1000);
	robotNeck.turnLeft();
	delay(1000);
}