/*
 *  MR Robot 0.1
 *  TODO 
 *  - keep track of previous decision de avoid going at the same place again
 *  - externalize communication into the lib
 *  - evolve dto to add information type
 *  - create log (print f, serial, over radio)
 *  - add possibility de receive message (to change mode for instance)
 *  - change motor speed (need to change pin)
 */ 

#include <SPI.h>
#include "RF24.h"
#include <Servo.h>
#include <LiquidCrystal.h>
#include "sensing.h"
#include "sound.h"
#include "motion.h"
#include "looks.h"
#include "printf.h"


/*********** Pin definition *******************************/
int SONG_PIN =        A0;		// piezzo buzzer
int MODE_POT_PIN =    A1;   // potentiometer used to detect mode
int PIN_A_IB =        A2;   // direction motor A
int PIN_A_IA =        A3;		// speed motor A for now just a digital
int PIN_B_IB =        A4;   // direction motor B
int PIN_B_IA =        A5;		// speed motor B for now just a digital
// pin0 not used
// pin1 not used
int GREEN_PIN =       2;	  // flashing light green
int RED_PIN =         3;		// red light
int TRIGGER_PIN =     4;	  // ultrasonic sensor trigger
int ECHO_PIN =        5;		// ultrasonic sensor echo
int NECK_PIN =        6;		// servo motor
int RADIO_CE_PIN =    7;	  // radio CE
int RADIO_CSN_PIN =   8;	  // radio 
//pin9 not used
//pin10 not used
int RADIO_MOSI_PIN =  11;   // radio MOSI - not configurable
int RADIO_MISO_PIN =  12;   // radio MISO - not configurable
int RADIO_SCK_PIN =   13;   // radio SCK - not configurable

/********** Config  *****************************************/
enum modeEnum {
  TEST,
  AVOID,
  WAIT
  };
const long SERIAL_BAUD = 115200;
const uint64_t pipe = 0xABCDABCD71LL;
modeEnum mode = WAIT;
const int ANGLE_SAMPLE = 5; 
const int ANGLE_WINDOW = 20;
const int DISTANCE_STOP = 40;
const int DISTANCE_NOGO = 40;

/*********** Modules initialisation *************************/
Neck          robotNeck(NECK_PIN, true, 5);
Buzzer        buzzer(SONG_PIN);
UltraSonicEye soni(TRIGGER_PIN, ECHO_PIN);
Weels         weels(PIN_A_IB, PIN_A_IA, PIN_B_IB, PIN_B_IA);
RF24          radio(RADIO_CE_PIN, RADIO_CSN_PIN);
FlashingLight lightGreen(GREEN_PIN, 200);
Light         lightRed(RED_PIN);

/****************** Communication stuctur ***********************/
typedef struct dataStruct{
	unsigned long _micros;
	int distance;
} RobotData;

/*********** Arduino Setup  *********************************/
void setup() {
	printf_begin();
	Serial.begin(SERIAL_BAUD);
  checkMode();

	/*********** Radio stuff **********/
	// TODO put that in the lib
	Serial.print("init radio\n");
	radio.begin();

	// Set the PA Level low to prevent power supply related issues RF24_PA_MAX is default.
	radio.setPALevel(RF24_PA_LOW);

	// Enable auto acknowledgement
	radio.setAutoAck(true);

	// set delay and number of retry (250 microseconds, 3 times)
	radio.setRetries(1, 1);

	// Open a writing and reading pipe on each radio, with opposite addresses
	radio.openWritingPipe(pipe);

	// print diagnostic (need printf to be initiated)
	radio.printDetails();

	// stop listening so we can talk
	radio.stopListening();

	/*********** End Radio stuff **********/

	// init neck
	robotNeck.attach();

	//ready
	lightGreen.turnOff();
	lightRed.turnOn();
}

void checkMode(){
  long potValue = analogRead(MODE_POT_PIN);
  modeEnum previousMode = mode;
  
  mode = AVOID;
  if(potValue < 330){
    mode = TEST;
  } else if (potValue > 660) {
    mode = WAIT;
    while(true){
      if(analogRead(MODE_POT_PIN) <=660){
        break;
      }
    }
  }

  if(previousMode != mode){
    printf("Changing mode from %s to %s\n", getModeName(previousMode), getModeName(mode));
  }
}

const char* getModeName(modeEnum mode) 
{
   switch (mode) 
   {
      case AVOID: return "Avoid";
      case TEST: return "Test";
      case WAIT: return "Wait";
   }
}

/*********** Arduino Loop  *********************************/
void loop() {
  checkMode();
  switch(mode){
    case TEST:
      testLoop();
      break;
    case AVOID:
      avoidLoop();
      break;
    case WAIT:
      break;
  }
}

void testLoop(){
  // init led
  lightGreen.turnOn();
  lightRed.turnOn();
  
  //send a setup message
  RobotData testData;
  testData.distance = 0;
  testData._micros = millis();

  Serial.print("test radio message\n");
  if (!radio.write(&testData, sizeof(testData))){
    Serial.println(F("Failed sending setup message "));
  }

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

  // end testing
  lightGreen.turnOff();
  lightRed.turnOff();
  delay(6000);

}

void avoidLoop(){
	lightGreen.update();
	int distance = soni.look();

	//obstacle found
	if (distance < DISTANCE_STOP){
    //stop the robot
    weels.stop();
    lightGreen.turnOff();
    lightRed.turnOn();
    buzzer.playUhoh();
    
    //send the message TODO put in lib
		RobotData message;
		message.distance = distance;
		message._micros = millis();
		Serial.println(F("Now sending distance."));
		printf("Distance = %dcm, time = %ums \n", message.distance, message._micros);

		if (!radio.write(&message, sizeof(message))){
			Serial.println(F("Failed sending distance"));
		}
		Serial.print("Stop\n");
		
		//record best distance and best angle window
    int distances[180/ANGLE_SAMPLE];
    int bestDistances[180/ANGLE_WINDOW +1];
    int bestWindow = -1;
    int bestDistance = 0;
    
    for(int angle=0; angle < 180; angle = angle + ANGLE_SAMPLE){
      robotNeck.turn(angle);
      int distance = soni.look();
      distances[angle / ANGLE_SAMPLE] = distance;
      int currentWindowNum = angle / ANGLE_WINDOW;
      if(angle % ANGLE_WINDOW == 0) {
        bestDistances[currentWindowNum] = 0;
      }
      if(distance < DISTANCE_NOGO){
        bestDistances[currentWindowNum] = -1000;
      }
      else {
        bestDistances[currentWindowNum] = bestDistances[currentWindowNum] + distance;
        if(bestDistances[currentWindowNum] > bestDistance){
           bestDistance = bestDistances[currentWindowNum];
           bestWindow = currentWindowNum;
        }
      }
    }

    //TODO if LOG
    printf("%d\n", bestDistances[0]);
    for(int i = 0; i < sizeof(distances) / sizeof(int) ; i++){
      printf("Angle %3d deg - Distance %5d cm - Window %8d cm\n", i * ANGLE_SAMPLE, distances[i], bestDistances[(i * ANGLE_SAMPLE)/ANGLE_WINDOW] );
    }

    // turn to the middle of the best window
    if(bestWindow !=-1){
      int bestAngle = bestWindow * ANGLE_WINDOW + ANGLE_WINDOW / 2  ;
      robotNeck.turn(bestAngle);
      printf("Turn %d deg\n", bestAngle);
      if(bestAngle < 90) {
        weels.moveBackward(300);
        weels.turnLeft((90 - bestAngle) * 10);
      }
      else if(bestAngle >= 90) {
        weels.moveBackward(300);
        weels.turnRight((bestAngle - 90) * 10);
      }
    } else {
      //u turn
      Serial.println(F("U turn."));
      robotNeck.turnCenter();

      weels.moveBackward(300);
      weels.turnLeft(900);
    }
    robotNeck.turnCenter();

		distance = soni.lookAccuratly();
		//bonne distance
		if (distance > DISTANCE_STOP){
			Serial.print("Move Forward\n");
      lightGreen.turnOn();
      lightRed.turnOff();
			weels.moveForward();
			
		}
	}
}

