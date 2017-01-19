/*
 *  MR Robot 0.1
 *  TODO
 *  - speed detection using timer interupt
 *  - error calc first distance in the log
 *  - keep track of previous decision de avoid going at the same place again
 *  - evolve dto to add information type
 *  - create log (print f, serial, over radio)
 *  - add possibility de receive message (to change mode for instance)
 *  - change motor speed (need to change pin)
 */ 

#include "stdinclude.h"
#include "sensing.h"
#include "sound.h"
#include "motion.h"
#include "looks.h"
#include <printf.h>

/*********** Pin definition *******************************/
#define SONG_PIN         A0		// piezzo buzzer
#define MODE_POT_PIN     A1   // potentiometer used to detect mode
#define PIN_A_IB         A2   // direction motor A
#define PIN_A_IA         A3		// speed motor A for now just a digital
#define PIN_B_IB         A4   // direction motor B
#define PIN_B_IA         A5		// speed motor B for now just a digital
// pin0 not used (Serial RX)
// pin1 not used (Serial TX)
#define SPEED_SENS_INT   INT0 // pin2 used as interupt(INT0)
#define SPEED_SENS_PIN   2    // pin2 speed sensor
// pin3 not used (INT1)
#define TRIGGER_PIN      4	  // ultrasonic sensor trigger
#define ECHO_PIN         5		// ultrasonic sensor echo
#define NECK_PIN         6		// servo motor
#define RADIO_CE_PIN     7	  // radio CE
#define RADIO_CSN_PIN    8	  // radio 
#define GREEN_PIN        9    // flashing light green
#define RED_PIN          10    // red light
#define RADIO_MOSI_PIN   11   // radio MOSI - not configurable
#define RADIO_MISO_PIN   12   // radio MISO - not configurable
#define RADIO_SCK_PIN    13   // radio SCK - not configurable

/********** Config  *****************************************/

const long SERIAL_BAUD = 115200;
const uint64_t pipe = 0xABCDABCD71LL;
ModeEnum mode = WAIT;
const int ANGLE_SAMPLE = 10; 
const int ANGLE_WINDOW = 30;
const int DISTANCE_STOP = 40;
const int DISTANCE_NOGO = 40;
const int WHEEL_HOLES = 20;
const int WHEEL_PERMIMETER_CM = 22;

/*********** Modules  *************************/
Neck          *robotNeck;
Buzzer        *buzzer;
UltraSonicEye *soni;
Wheels         *wheels;
RadioCom      *radio;
FlashingLight *lightGreen;
Light         *lightRed;

/*************** Speed counter *****************************/
static volatile unsigned int speedCounter = 0;
static volatile unsigned long debounce = 0; // Rebound time.
static volatile unsigned long lastCheckTime = 0;
static volatile unsigned long lastCheckCounter = 0;
static bool moveForward = false;

void doCountSpeedSensor()  // counts from the speed sensor
{
  if(digitalRead(SPEED_SENS_PIN) && (micros() - debounce> 500) && digitalRead (SPEED_SENS_PIN)){
    debounce = micros();
    speedCounter++;  // increase +1 the counter value
  }
} 

const char* getModeName(ModeEnum mode){
	switch (mode)
	{
	case AVOID: return "Avoid";
	case TEST: return "Test";
	case WAIT: return "Wait";
	}
}

/*********** Arduino Setup  *********************************/
void setup() {
    printf_begin();
    Serial.begin(SERIAL_BAUD);
    
  printf("Setup robot\n");
  checkMode();

  // init all parts
  printf("Init speed sensor on interupt 0\n");
  attachInterrupt(SPEED_SENS_INT, doCountSpeedSensor, RISING);

  printf("Setup neck\n");
  robotNeck = new Neck(NECK_PIN, true, 5);
  robotNeck->attach();
  
  printf("Setup buzzer\n");
  buzzer = new Buzzer(SONG_PIN);

  printf("Setup eye front\n");
  soni = new UltraSonicEye(TRIGGER_PIN, ECHO_PIN);

  printf("Setup wheels\n");
  wheels = new Wheels(PIN_A_IB, PIN_A_IA, PIN_B_IB, PIN_B_IA, WHEEL_HOLES, WHEEL_PERMIMETER_CM);

  printf("Setup radio output\n");
  radio = new RadioCom(RADIO_CE_PIN, RADIO_CSN_PIN, pipe);

  printf("Setup green light\n");
  lightGreen = new FlashingLight(GREEN_PIN, 200);
  lightGreen->turnOff();

  printf("Setup red light\n");
  lightRed = new Light(RED_PIN);
  lightRed->turnOn();

  printf("Setup complete\n");
}


void checkMode(){
  long potValue = analogRead(MODE_POT_PIN);
  ModeEnum previousMode = mode;
  
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
  printf("test neck \n");
  robotNeck->turnRight();
  robotNeck->turnLeft();
  robotNeck->turnCenter();

  // test motor
  printf("test motor fwd\n");
  wheels->moveForward(1000);

  printf("test motor backward\n");
  wheels->moveBackward(1000);

  printf("test motor right\n");
  wheels->turnRight(1000);

  printf("test motor left\n");
  wheels->turnLeft(1000);

  printf("test speed counter\n");
  wheels->stop(2000);
  wheels->moveForward(20, speedCounter);
  wheels->stop(2000);
  wheels->turnRightAngle(180, speedCounter);
  wheels->stop(2000);
  wheels->moveForward(10, speedCounter);
  wheels->stop(2000);
  wheels->turnRightAngle(360, speedCounter);
  wheels->stop(2000);
  wheels->moveForward(10, speedCounter);
  wheels->stop(2000);
  
  // buzzer test
  printf("test buzzer\n");
  int spiderman[] = { D6, D6, D6, D6, G6, G6, D6, D6, A7, A7, D6, };
  buzzer->playSound(spiderman, sizeof(spiderman) / sizeof(*spiderman), 150);

  // end testing
  lightGreen->turnOff();
  lightRed->turnOff();
  delay(1000);

}

void avoidLoop(){

    // new distance check
    lightGreen->update();
    int distance = soni->look();

    //obstacle found
    if (distance < DISTANCE_STOP){
    //stop the robot
    wheels->hardStop();
    moveForward = false;
    lightGreen->turnOff();
    lightRed->turnOn();
    buzzer->playUhoh();
    radio->sendData(distance);
    printf("Stop\n");
        
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
      for(int i =currentAngleNum -  samplePerWindow/2; i< currentAngleNum + samplePerWindow/2 +1; i++){
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
        wheels->moveBackward(3, speedCounter);;
        if (!wheels->turnLeftAngle(90 - bestAngle, speedCounter)){
          unBlock();
        }
      }
      else if(bestAngle >= 90) {
        wheels->moveBackward(3, speedCounter);;
        if(!wheels->turnRightAngle(bestAngle - 90, speedCounter)){
          unBlock();
        }
      }
    } else {
      //u turn
      printf("U turn.\n");
      robotNeck->turnCenter();

      wheels->moveBackward(10, speedCounter);;
      wheels->turnRightAngle(180, speedCounter);
    }
    robotNeck->turnCenter();

    distance = soni->lookAccuratly(5);
    //bonne distance
    if (distance > DISTANCE_STOP){
        printf("Move Forward\n");
        lightGreen->turnOn();
        lightRed->turnOff();
        wheels->moveForward();
        lastCheckTime = millis();
        lastCheckCounter = speedCounter;
        moveForward = true;
    } 
  }

  // check robot still moving
  // TODO changing with timer interrupt
  if(moveForward && millis() - lastCheckTime > 500){
    if (speedCounter == lastCheckCounter){
      unBlock();
    }
    lastCheckTime = millis();
    lastCheckCounter = speedCounter;
  }
}

void unBlock(){
  printf("Problem not moving\n");
  wheels->stop();
  lightGreen->turnOn();
  lightRed->turnOn();
  buzzer->playR2D2();
  moveForward = false;
  wheels->moveBackward(3, speedCounter);
  wheels->turnRightAngle(270, speedCounter);
  printf("Move Forward\n");
  lightGreen->turnOn();
  lightRed->turnOff();
  wheels->moveForward();
}


