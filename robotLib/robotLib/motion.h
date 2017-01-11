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
	Wheels(int dirMotorRight, int speedMotorRight, int dirMotorLeft, int speedMotorLeft, int speedSensorHoles);
	void turnRight(int milisecond);
  bool turnRightAngle(int angle, volatile unsigned int& counter);
  bool turnLeftAngle(int angle, volatile unsigned int& counter);
	void turnLeft(int milisecond);
	void moveForward(int milisecond);
	void moveForward();
	void moveBackward(int milisecond);
	void moveBackward();
	void stop(int delayMs);
	void stop();
  void hardStop();
private:
  void _turnLeft();
  void _turnRight();
  bool _turnAngle(int angle, volatile unsigned int& counter);
	int _dirMotorRight;
	int _speedMotorRight;
	int _dirMotorLeft;
	int _speedMotorLeft;
  unsigned int _speedSensorHoles;
  unsigned int _speedSensorAngles;
};

#endif

