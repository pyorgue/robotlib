#include "stdinclude.h"
#include "motion.h"

// Neck implementation
Neck::Neck(int pin, bool reversed = false, int turnInterval = 5){
	_pin = pin;
	_turnInterval = turnInterval;
	_position = 0;
	_reversed = reversed;
	_servo = new Servo();
}

void Neck::attach(){
	_servo->attach(_pin);
	_servo->write(90);
	_position = 90;
}

void Neck::turn(int angle){
	int way = 1;
	if (_reversed){
		angle = 180 - angle;
	}

	if (_position >= angle){
		way = -1;
	}
	for (int pos = _position; way == -1 ? pos >= angle : pos <= angle; pos += (1 * way))
	{
		_servo->write(pos);
		delay(_turnInterval);
	}
	_position = angle;
}

void Neck::turnLeft(){
	turn(45);
}

void Neck::turnRight(){
	turn(135);
}

void Neck::turnCenter(){
	turn(90);
}

// Wheels implementation (assuming two motor positive in output middle)

Wheels::Wheels(int dirMotorRight, int speedMotorRight, int dirMotorLeft, int speedMotorLeft, unsigned int speedSensorHoles, unsigned int wheelPerimeterCm){
	_dirMotorRight = dirMotorRight;
	_speedMotorRight = speedMotorRight;
	_dirMotorLeft = dirMotorLeft;
	_speedMotorLeft = speedMotorLeft;
  _speedSensorHoles = speedSensorHoles;
  _speedSensorAngles = 360 / speedSensorHoles;
  _wheelPerimeterCm = wheelPerimeterCm;

	// init motor A
	pinMode(_dirMotorRight, OUTPUT);
	pinMode(_speedMotorRight, OUTPUT); //TODO control speed with ...

	// init motor B
	pinMode(_dirMotorLeft, OUTPUT);
	pinMode(_speedMotorLeft, OUTPUT);

	stop(0);
}

bool Wheels::_turnAngle(int angle, volatile unsigned int& counter){
  counter = 0;
  unsigned long timeWhenLastUpdate = millis();
  int lastCounter = 0;
  while(counter * _speedSensorAngles / 2 < angle){
    unsigned long newCheck = millis();
    if(lastCounter != counter){
      lastCounter = counter;
      timeWhenLastUpdate = newCheck;
      printf("Waiting - angle wanted = %d, current angle = %d, counter = %d\n", angle, counter * _speedSensorAngles, counter);
    } else if(newCheck - timeWhenLastUpdate > 2000){
      hardStop();
      printf("Can't trurn - angle wanted = %d, current angle = %d, counter = %d, last check = %lu, new check = %lu \n", angle, counter * _speedSensorAngles, counter);
      return false;
    }
  }
  hardStop();
  printf("After stop - angle wanted = %d, current angle = %d, counter = %d\n", angle, counter * _speedSensorAngles, counter);
  counter = 0;
  return true;
}

void Wheels::_turnLeft(){
  digitalWrite(_dirMotorRight, HIGH); // direction = forward
  digitalWrite(_speedMotorRight, LOW); 
  digitalWrite(_dirMotorLeft, LOW); // direction = backward
  digitalWrite(_speedMotorLeft, HIGH);
}

void Wheels::_turnRight(){
  digitalWrite(_dirMotorRight, LOW); // direction = backward
  digitalWrite(_speedMotorRight, HIGH); // TODO speed
  digitalWrite(_dirMotorLeft, HIGH); // direction = forward
  digitalWrite(_speedMotorLeft, LOW); // TODO speed
}

void Wheels::turnRight(int milisecond){
	_turnRight();
	delay(milisecond);
	stop();
}

bool Wheels::turnRightAngle(int angle, volatile unsigned int& counter){
  _turnRight();
  return _turnAngle(angle, counter);
}

bool Wheels::turnLeftAngle(int angle, volatile unsigned int& counter){
  _turnLeft();
  return _turnAngle(angle, counter);
}

void Wheels::turnLeft(int milisecond){
	_turnLeft();
	delay(milisecond);
	stop();
}


float Wheels::_calcDistance(unsigned int counter){
  return float(counter * _speedSensorAngles)/360.0 * float(_wheelPerimeterCm);
}
  
bool Wheels::_moveDistanceCm(unsigned int distanceCm, volatile unsigned int& counter){
  counter = 0;
  unsigned long timeWhenLastUpdate = millis();
  int lastCounter = 0;
  float halfDistanceOneHole = _calcDistance(1)/2.0;
  while((float)distanceCm - _calcDistance(counter) > halfDistanceOneHole){
    unsigned long newCheck = millis();
    if(lastCounter != counter){
      lastCounter = counter;
      timeWhenLastUpdate = newCheck;
      printf("Waiting - distance wanted (mm) = %d, current distance = %d, counter = %d\n", distanceCm * 10, int(_calcDistance(counter) * 10), counter);
    } else if(newCheck - timeWhenLastUpdate > 2000){
      hardStop();
      printf("Can't move - distance wanted = %d, current distance = %d, counter = %d, last check = %lu, new check = %lu \n", distanceCm, int(_calcDistance(counter) * 10), counter);
      return false;
    }
  }
  printf("Before stop - distance wanted (mm) = %d, current distance = %d, counter = %d\n", distanceCm * 10 , int(_calcDistance(counter) * 10), counter);
  hardStop();
  printf("After stop - distance wanted (mm) = %d, current distance = %d, counter = %d\n", distanceCm * 10, int(_calcDistance(counter) *10) , counter);
  counter = 0;  
  return true;
}

void Wheels::moveForward(){
  digitalWrite(_dirMotorRight, HIGH); // direction = forward
  digitalWrite(_speedMotorRight, LOW); // TODO speed
  digitalWrite(_dirMotorLeft, HIGH); // direction = forward
  digitalWrite(_speedMotorLeft, LOW); 
}

void Wheels::moveForward(int milisecond){
	moveForward();
	delay(milisecond);
	stop();
}

bool Wheels::moveForward(unsigned int distanceCm, volatile unsigned int& counter){
  moveForward();
  return _moveDistanceCm(distanceCm, counter);
}

void Wheels::moveBackward(){
  digitalWrite(_dirMotorRight, LOW); // direction = backward
  digitalWrite(_speedMotorRight, HIGH);
  digitalWrite(_dirMotorLeft, LOW); // direction = backward
  digitalWrite(_speedMotorLeft, HIGH);
}

void Wheels::moveBackward(int milisecond){
	moveBackward();
	delay(milisecond);
	stop();
}

bool Wheels::moveBackward(unsigned int distanceCm, volatile unsigned int& counter){
  moveBackward();
  return _moveDistanceCm(distanceCm, counter);
}

void Wheels::stop(int delayMs){
	digitalWrite(_dirMotorRight, LOW);
	digitalWrite(_speedMotorRight, LOW);
	digitalWrite(_dirMotorLeft, LOW);
	digitalWrite(_speedMotorLeft, LOW);
	delay(delayMs);
}

void Wheels::stop(){
	stop(100);
}

void Wheels::hardStop(){
  digitalWrite(_dirMotorRight, HIGH);
  digitalWrite(_speedMotorRight, HIGH);
  digitalWrite(_dirMotorLeft, HIGH);
  digitalWrite(_speedMotorLeft, HIGH);
}

