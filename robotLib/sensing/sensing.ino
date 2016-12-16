#include "../robotLib/sensing.h"

int ECHO_PIN = 9;
int TRIGGER_PIN = 8;

UltraSonicEye eye(TRIGGER_PIN, ECHO_PIN);

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
}

// the loop function runs over and over again until power down or reset
void loop() {
	delay(50);
	Serial.print("Distance: " + eye.lookAccuratly());
}
