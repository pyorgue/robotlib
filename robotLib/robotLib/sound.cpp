#include "stdinclude.h"
#include "sound.h"

Buzzer::Buzzer(int pin)
{
	_pin = pin;
	tone(8, 100, 1);
}

/**
* \brief Play a note
* \param note The note frequency
* \param duration in milliseconds
*/
void Buzzer::playNote(float note, float duration) const
{
	int x;
	// Convert the frequency to microseconds
	float microsecondsPerWave = 1000000 / note;
	// Calculate how many milliseconds there are per HIGH/LOW cycles.
	float millisecondsPerCycle = 1000 / (microsecondsPerWave * 2);
	// Multiply noteDuration * number or cycles per millisecond
	float loopTime = duration * millisecondsPerCycle;
	// Play the note for the calculated loopTime.
	for (x = 0; x < loopTime; x++)
	{
		digitalWrite(_pin, HIGH);
		delayMicroseconds(microsecondsPerWave);
		digitalWrite(_pin, LOW);
		delayMicroseconds(microsecondsPerWave);
	}
}

void Buzzer::playNote2(float note, float duration) const
{
	tone(_pin, note, duration);
	delay(1 + duration);
}

/**
* \brief Play R2 D2 song
*/
void Buzzer::playR2D2() const
{
	playNote(LA7, 100); //A
	playNote(G7, 100); //G
	playNote(E7, 100); //E
	playNote(C7, 100); //C
	playNote(D7, 100); //D
	playNote(B7, 100); //B
	playNote(F7, 100); //F
	playNote(C8, 100); //C
	playNote(LA7, 100); //A
	playNote(G7, 100); //G
	playNote(E7, 100); //E
	playNote(C7, 100); //C
	playNote(D7, 100); //D
	playNote(B7, 100); //B
	playNote(F7, 100); //F
	playNote(C8, 100); //C
}

void Buzzer::playSound(int* melody, int sizeMelody, int duration) const
{
	for (int note = 0; note < sizeMelody; note++){
		playNote2(melody[note], duration);
	}
}

/**
* \brief Play R2 D2 song
*/
void Buzzer::play2R2D2() const
{
	int r2d2[] = { LA7, G7, E7, C7, D7, B7, F7, C8, LA7, G7, E7, C7, D7, B7, F7, C8 };
	playSound(r2d2, sizeof(r2d2) / sizeof(*r2d2), 150);
}

/**
* \brief Play R2 D2 song
*/
void Buzzer::playAriel() const
{
	playNote2(1047, 300); //C
	delay(50);
	playNote2(1175, 300); //D
	delay(50);
	playNote2(1245, 600); //D#
	delay(250);

	playNote2(1175, 300); //D
	delay(50);
	playNote2(1245, 300); //D#
	delay(50);
	playNote2(1397, 600); //F
	delay(250);

	playNote2(1047, 300); //C
	delay(50);
	playNote2(1175, 300); //D
	delay(50);
	playNote2(1245, 500); //D#
	delay(50);
	playNote2(1175, 300); //D
	delay(50);
	playNote2(1245, 300); //D#
	delay(50);
	playNote2(1175, 300); //D
	delay(50);
	playNote2(1245, 300); //D#
	delay(50);
	playNote2(1397, 600); //F
	delay(50);
}



void Buzzer::playCloseEncounters() const {
	playNote2(932,300); //B b
	delay(50);
	playNote2(1047,300); //C
	delay(50);
	playNote2(831,300); //A b
	delay(50);
	playNote2(415,300); //A b
	delay(50);
	playNote2(622,500); //E b
	delay(500);

	playNote2(466,300); //B b
	delay(100);
	playNote2(524,300); //C
	delay(100);
	playNote2(415,300); //A b
	delay(100);
	playNote2(208,300); //A b
	delay(100);
	playNote2(311,500); //E b
	delay(500);

	playNote2(233,300); //B b
	delay(200);
	playNote2(262,300); //C
	delay(200);
	playNote2(208,300); //A b
	delay(500);
	playNote2(104,300); //A bp
	delay(550);
	playNote2(156,500); //E b
}


void Buzzer::playBonk() const {
	playNote2(125,500); //C
	delay(100);
	playUhoh();
}

void Buzzer::playUhoh() const {
	playNote2(415,100); //C
	delay(80);
	playNote2(279,100); //C
	delay(80);
	}

void Buzzer::playSqueak() const{
	for (int i=100; i<3000; i=i*1.05) {
	playNote2(i,20);
	}
	delay(10);
	for (int i=100; i<200; i=i*1.15) {
	playNote2(i,60);
	}
	for (int i=400; i>100; i=i*.85) {
	playNote2(i,60);
	}
	for (int i = 100; i<5000; i = i*1.45) {
	playNote2( i, 60);
	}
	delay(10);
	for (int i = 100; i<6000; i = i*1.5) {
	playNote2( i, 20);
	}
}

void Buzzer::playWaka() const{
	for (int i = 1000; i<3000; i = i*1.05) {
	playNote2( i, 10);
	}
	delay(100);
	for (int i = 2000; i>1000; i = i*.95) {
	playNote2( i, 10);
	}
	for (int i = 1000; i<3000; i = i*1.05) {
	playNote2( i, 10);
	}
	delay(100);
	for (int i = 2000; i>1000; i = i*.95) {
	playNote2( i, 10);
	}
	for (int i = 1000; i<3000; i = i*1.05) {
	playNote2( i, 10);
	}
	delay(100);
	for (int i = 2000; i>1000; i = i*.95) {
	playNote2( i, 10);
	}
	for (int i = 1000; i<3000; i = i*1.05) {
	playNote2( i, 10);
	}
	delay(100);
	for (int i = 2000; i>1000; i = i*.95) {
	playNote2( i, 10);
	}
}

void Buzzer::playCatCall() const{
	for (int i = 1000; i<5000; i = i*1.05) {
	playNote2( i, 10);
	}
	delay(300);

	for (int i = 1000; i<3000; i = i*1.03) {
	playNote2( i, 10);
	}
	for (int i = 3000; i>1000; i = i*.97) {
	playNote2( i, 10);
	}
	}

void Buzzer::playOhhh() const{
	for (int i = 1000; i<2000; i = i*1.02) {
	playNote2( i, 10);
	}
	for (int i = 2000; i>1000; i = i*.98) {
	playNote2( i, 10);
	}
}
