#include "../robotLib/sound.h"

int BUZZ_PIN = 8;

Buzzer buzzer(BUZZ_PIN);


void setup() {
	Serial.begin(9600);
	Serial.print("Hello");
	Serial.print("Example \"sound.h\" library");

	//play a registered sound
	buzzer.playAriel();

	delay(2000);

	//play my own song
	int mySong[] = { LA7, G7, E7, C7, D7, B7, F7, C8, LA7, G7, E7, C7, D7, B7, F7, C8 };
	buzzer.playSound(mySong, sizeof(mySong) / sizeof(*mySong));
}

void loop() {
}