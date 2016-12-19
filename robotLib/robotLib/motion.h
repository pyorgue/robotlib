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

class Weels{
public:
	Weels(int dirMotorRight, int speedMotorRight, int dirMotorLeft, int speedMotorLeft);
	void turnRight(int milisecond);
	void turnLeft(int milisecond);
	void moveForward(int milisecond);
	void moveForward();
	void moveBackward(int milisecond);
	void moveBackward();
	void stop(int delayMs);
	void stop();
private:
	int _dirMotorRight;
	int _speedMotorRight;
	int _dirMotorLeft;
	int _speedMotorLeft;
};

#endif

