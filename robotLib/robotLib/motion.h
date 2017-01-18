// motion.h

#ifndef _MOTION_h
#define _MOTION_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class Servo;


class Neck{
public:
	Neck(int pin, bool reversed = false, int turnInterval = 5);
	void attach();
	void turnRight();
	void turnLeft();
	void turnCenter();
  void turn(int angle);
private:
	int _pin;
	Servo *_servo;
	int _turnInterval;
	int _position;
	bool _reversed;
};

class Wheels{
  
public:
	Wheels(int dirMotorRight, int speedMotorRight, int dirMotorLeft, int speedMotorLeft, unsigned int speedSensorHoles, unsigned int wheelPerimeterCm);
	void turnRight(int milisecond);
  bool turnRightAngle(int angle, volatile unsigned int& counter);
  void turnLeft(int milisecond);
  bool turnLeftAngle(int angle, volatile unsigned int& counter);
	void moveForward(int milisecond);
  bool moveForward(unsigned int distanceCm, volatile unsigned int& counter);
	void moveForward();
	void moveBackward(int milisecond);
  bool moveBackward(unsigned int distanceCm, volatile unsigned int& counter);
	void moveBackward();
	void stop(int delayMs);
	void stop();
  void hardStop();
  
private:
  void _turnLeft();
  void _turnRight();
  bool _turnAngle(int angle, volatile unsigned int& counter);
  bool _moveDistanceCm(unsigned int distanceCm, volatile unsigned int& counter);
	int _dirMotorRight;
	int _speedMotorRight;
	int _dirMotorLeft;
	int _speedMotorLeft;
  unsigned int _speedSensorHoles;
  unsigned int _speedSensorAngles;
  unsigned int _wheelPerimeterCm;
  float Wheels::_calcDistance(unsigned int counter);
};

#endif

