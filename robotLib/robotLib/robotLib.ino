#include <Servo.h>
#include <LiquidCrystal.h>
#include "sensing.h"
#include "sound.h"
#include "motion.h"
#include "looks.h"
#include "printf.h"
#include <SPI.h>
#include "RF24.h"

/*********** Pin definition *******************************/
int SONG_PIN = A0;		// piezzo buzzer
int PIN_A_IA = A3;		// speed motor A for now just a digital
int PIN_A_IB = A2;		// direction motor A
int PIN_B_IA = A5;		// speed motor B for now just a digital
int PIN_B_IB = A4;		// direction motor B
int GREEN_PIN = 2;		// flashing light green
int RED_PIN = 3;		// red light
int TRIGGER_PIN = 4;	// ultrasonic sensor trigger
int ECHO_PIN = 5;		// ultrasonic sensor echo
int NECK_PIN = 6;		// servo motor
int RADIO_CE_PIN = 7;	// radio CE
int RADIO_CSN_PIN = 8;	// radio CSN
int RADIO_MOSI_PIN = 11;// radio MOSI - not configurable
int RADIO_SCK_PIN = 13; // radio SCK - not configurable 
int RADIO_MISO_PIN = 12;// radio MISO - not configurable

/********** Config  *****************************************/
int PWM_SLOW = 50;
long SERIAL_BAUD = 115200;
bool modeForward = false;
bool clearMode = false;
static uint32_t messageCount = 0;
const uint64_t pipe = 0xABCDABCD71LL;

/*********** Modules initialisation *************************/
Neck robotNeck(NECK_PIN, true, 5);
Buzzer buzzer(SONG_PIN);
UltraSonicEye soni(TRIGGER_PIN, ECHO_PIN);
Weels weels(PIN_A_IB, PIN_A_IA, PIN_B_IB, PIN_B_IA);
RF24 radio(RADIO_CE_PIN, RADIO_CSN_PIN);
FlashingLight lightGreen(GREEN_PIN, 200);
Light lightRed(RED_PIN);

/****************** Communication stuctur ***********************/
typedef struct dataStruct{
	unsigned long _micros;
	int distance;
} RobotData;

/*********** Arduino Setup  *********************************/
void setup() {
	printf_begin();
	Serial.begin(115200);


	/*********** Radio stuff **********/
	// TODO put that in the lib
	Serial.print("init radio\n");
	radio.begin();

	// Set the PA Level low to prevent power supply related issues RF24_PA_MAX is default.
	radio.setPALevel(RF24_PA_LOW);

	// Enable auto acknowledgement
	radio.setAutoAck(true);

	// set delay and number of retry (250 microseconds, 3 times)
	radio.setRetries(1, 3);

	// Open a writing and reading pipe on each radio, with opposite addresses
	radio.openWritingPipe(pipe);

	// print diagnostic (need printf to be initiated)
	radio.printDetails();

	// stop listening so we can talk
	radio.stopListening();

	//send a setup message
	RobotData testData;
	testData.distance = 0;
	testData._micros = millis();

	if (!radio.write(&testData, sizeof(testData))){
		Serial.println(F("Failed sending setup message "));
	}

	/*********** End Radio stuff **********/


	// init neck
	robotNeck.attach();

	// init led
	lightGreen.turnOn();
	lightRed.turnOn();

	// init neck
	Serial.print("test neck \n");
	robotNeck.turnRight();
	robotNeck.turnLeft();
	robotNeck.turnCenter();

	// test motor
	Serial.print("test motor fwd\n");
	weels.moveForward(1000);

	Serial.print("test motor backward\n");
	weels.moveBackward(1000);

	Serial.print("test motor right\n");
	weels.turnRight(1000);

	Serial.print("test motor left\n");
	weels.turnLeft(1000);

	// buzzer test
	Serial.print("test buzzer\n");
	int spiderman[] = { D6, D6, D6, D6, G6, G6, D6, D6, A7, A7, D6, };
	buzzer.playSound(spiderman, sizeof(spiderman) / sizeof(*spiderman), 150);
	modeForward = false;

	//end test
	lightGreen.turnOff();
	lightRed.turnOn();
}

/*********** Arduino Loop  *********************************/
void loop() {

	lightGreen.update();
	int distance = soni.lookAccuratly();

	//obstacle found
	if (distance < 30){
		RobotData message;
		message.distance = distance;
		message._micros = millis();
		Serial.println(F("Now sending distance."));
		printf("Distance = %dcm, time = %ums \n", message.distance, message._micros);

		if (!radio.write(&message, sizeof(message))){
			Serial.println(F("Failed sending distance"));
		}

		//stop the robot
		lightGreen.turnOff();
		lightRed.turnOn();
		Serial.print("Stop\n");
		weels.stop();
		buzzer.playUhoh();

		//seek direction
		robotNeck.turnCenter();
		int distanceCenter = soni.lookAccuratly();
		robotNeck.turnLeft();
		int distanceLeft = soni.lookAccuratly();
		robotNeck.turnRight();
		int distanceRight = soni.lookAccuratly();

		//move accordingly
		if ((distanceLeft == distanceRight && distanceLeft == distanceCenter)
			|| (distanceCenter > distanceRight && distanceCenter > distanceLeft)
			|| (distanceRight < 5 && distanceLeft < 5)){
			robotNeck.turnCenter();

			weels.moveBackward(300);
			weels.turnLeft(800); // u turn
		}
		else if (distanceLeft >= distanceRight && distanceLeft >= distanceCenter){
			robotNeck.turnLeft();

			weels.moveBackward(300);
			weels.turnLeft(300);

			robotNeck.turnCenter();
		}
		else if (distanceRight >= distanceLeft && distanceRight >= distanceCenter){
			robotNeck.turnRight();

			weels.moveBackward(300);
			weels.turnRight(300);

			robotNeck.turnCenter();
		}

		distance = soni.lookAccuratly();
		//bonne distance
		if (distance > 30){
			Serial.print("Move Forward\n");

			weels.moveForward();
			lightGreen.turnOn();
			lightRed.turnOff();
		}

	}

	delay(50);
}

