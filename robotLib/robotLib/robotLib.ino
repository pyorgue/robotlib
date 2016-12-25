/*
 *  MR Robot 0.1
 *  TODO
 *  - error calc first distance in the log
 *  - keep track of previous decision de avoid going at the same place again
 *  - externalize communication into the lib
 *  - evolve dto to add information type
 *  - create log (print f, serial, over radio)
 *  - add possibility de receive message (to change mode for instance)
 *  - change motor speed (need to change pin)
 */ 

#include <SPI.h>
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

/*********** Modules  *************************/
Neck          *robotNeck;
Buzzer        *buzzer;
UltraSonicEye *soni;
Weels         *weels;
RadioCom      *radio;
FlashingLight *lightGreen;
Light         *lightRed;

/*********** Arduino Setup  *********************************/
void setup() {
	printf_begin();
	Serial.begin(SERIAL_BAUD);
  Serial.print("Setup robot\n");
  checkMode();

  // init all parts
  Serial.print("Setup neck\n");
  robotNeck = new Neck(NECK_PIN, true, 5);
  robotNeck->attach();
  
  Serial.print("Setup buzzer\n");
  buzzer = new Buzzer(SONG_PIN);

  Serial.print("Setup eye front\n");
  soni = new UltraSonicEye(TRIGGER_PIN, ECHO_PIN);

  Serial.print("Setup weels\n");
  weels = new Weels(PIN_A_IB, PIN_A_IA, PIN_B_IB, PIN_B_IA);

  Serial.print("Setup radio output\n");
  radio = new RadioCom(RADIO_CE_PIN, RADIO_CSN_PIN, pipe);

  Serial.print("Setup green light\n");
  lightGreen = new FlashingLight(GREEN_PIN, 200);
  lightGreen->turnOff();

  Serial.print("Setup red light\n");
  lightRed = new Light(RED_PIN);
  lightRed->turnOn();

  Serial.print("Setup complete\n");
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
  lightGreen->turnOn();
  lightRed->turnOn();
  
  //send a setup message
  radio->sendData(0);

  // init neck
  Serial.print("test neck \n");
  robotNeck->turnRight();
  robotNeck->turnLeft();
  robotNeck->turnCenter();

  // test motor
  Serial.print("test motor fwd\n");
  weels->moveForward(1000);

  Serial.print("test motor backward\n");
  weels->moveBackward(1000);

  Serial.print("test motor right\n");
  weels->turnRight(1000);

  Serial.print("test motor left\n");
  weels->turnLeft(1000);

  // buzzer test
  Serial.print("test buzzer\n");
  int spiderman[] = { D6, D6, D6, D6, G6, G6, D6, D6, A7, A7, D6, };
  buzzer->playSound(spiderman, sizeof(spiderman) / sizeof(*spiderman), 150);

  // end testing
  lightGreen->turnOff();
  lightRed->turnOff();
  delay(6000);

}

void avoidLoop(){
	lightGreen->update();
	int distance = soni->look();

	//obstacle found
	if (distance < DISTANCE_STOP){
    //stop the robot
    weels->stop();
    lightGreen->turnOff();
    lightRed->turnOn();
    buzzer->playUhoh();
		radio->sendData(distance);
		Serial.print("Stop\n");
		
		//record best distance and best angle window
    int distances[180/ANGLE_SAMPLE] = {0};
    int windowDistances[180/ANGLE_SAMPLE] = {0};
    int bestWindow = -1;
    int bestDistance = 0;
    int samplePerWindow = ANGLE_WINDOW/ANGLE_SAMPLE;
    
    for(int angle=0; angle < 180; angle = angle + ANGLE_SAMPLE){
      robotNeck->turn(angle);
      int distance = soni->lookAccuratly(2);
      int currentAngleNum = angle / ANGLE_SAMPLE;
      distances[currentAngleNum] = distance;
      
      printf("Angle num %d (%d deg) distance %dcm\n", currentAngleNum, angle, distance);
      for(int i =currentAngleNum -  samplePerWindow/2; i< currentAngleNum +  samplePerWindow/2 +1; i++){
        if(i<0 || i>= sizeof(windowDistances)){
          continue;
        }
        int adjustedDistance = distance < DISTANCE_NOGO ? -1000 : distance;
        windowDistances[i] += adjustedDistance;
        printf("Appends %dcm to window num %d ([%3d , %3d]) deg, total %dcm\n", adjustedDistance, i, (i -  samplePerWindow/2) * ANGLE_SAMPLE, (i +  samplePerWindow/2) * ANGLE_SAMPLE , windowDistances[i]);
        if(windowDistances[i] > bestDistance){
           bestDistance = windowDistances[i];
           bestWindow = i;
           printf("Best window %d best overall distance %dcm\n", bestWindow, bestDistance);
        }
      }
    }

    //TODO if LOG
    for(int i = 0; i < sizeof(distances) / sizeof(int) ; i++){
      printf("Angle %3d deg - Distance %5d cm - Window [%3d , %3d] %8d cm\n", i*ANGLE_SAMPLE , distances[i], (i - samplePerWindow/2) * ANGLE_SAMPLE, (i +  samplePerWindow/2) * ANGLE_SAMPLE, windowDistances[i] );
    }

    printf("best window %d best distance %dcm \n", bestWindow, bestDistance);
    // turn to the middle of the best window
    if(bestWindow !=-1 && windowDistances[bestWindow] > 0){
      int bestAngle = bestWindow * ANGLE_SAMPLE;
      robotNeck->turn(bestAngle);
      printf("Turn %d deg\n", bestAngle);
      if(bestAngle < 90) {
        weels->moveBackward(300);
        weels->turnLeft((90 - bestAngle) * 10);
      }
      else if(bestAngle >= 90) {
        weels->moveBackward(300);
        weels->turnRight((bestAngle - 90) * 10);
      }
    } else {
      //u turn
      Serial.println(F("U turn."));
      robotNeck->turnCenter();

      weels->moveBackward(300);
      weels->turnLeft(900);
    }
    robotNeck->turnCenter();

		distance = soni->lookAccuratly(5);
		//bonne distance
		if (distance > DISTANCE_STOP){
			Serial.print("Move Forward\n");
      lightGreen->turnOn();
      lightRed->turnOff();
			weels->moveForward();
		}
	}
}

