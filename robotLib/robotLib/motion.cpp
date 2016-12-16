#include "motion.h"
#include "Servo.h"

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

// Weels implementation (assuming two motor positive in output middle)

Weels::Weels(int dirMotorRight, int speedMotorRight, int dirMotorLeft, int speedMotorLeft){
	_dirMotorRight = dirMotorRight;
	_speedMotorRight = speedMotorRight;
	_dirMotorLeft = dirMotorLeft;
	_speedMotorLeft = speedMotorLeft;

	// init motor A
	pinMode(_dirMotorRight, OUTPUT);
	pinMode(_speedMotorRight, OUTPUT); //TODO control speed with ...

	// init motor B
	pinMode(_dirMotorLeft, OUTPUT);
	pinMode(_speedMotorLeft, OUTPUT);

	stop(0);
}

void Weels::turnRight(int milisecond){
	stop();
	digitalWrite(_dirMotorRight, LOW); // direction = backward
	digitalWrite(_speedMotorRight, HIGH); // TODO speed
	digitalWrite(_dirMotorLeft, HIGH); // direction = forward
	digitalWrite(_speedMotorLeft, LOW); // TODO speed
	delay(milisecond);
	stop();

}
void Weels::turnLeft(int milisecond){
	stop();
	digitalWrite(_dirMotorRight, HIGH); // direction = forward
	digitalWrite(_speedMotorRight, LOW); 
	digitalWrite(_dirMotorLeft, LOW); // direction = backward
	digitalWrite(_speedMotorLeft, HIGH);
	delay(milisecond);
	stop();
}

void Weels::moveForward(){
	stop();
	digitalWrite(_dirMotorRight, HIGH); // direction = forward
	digitalWrite(_speedMotorRight, LOW); // TODO speed
	digitalWrite(_dirMotorLeft, HIGH); // direction = forward
	digitalWrite(_speedMotorLeft, LOW); 
}

void Weels::moveForward(int milisecond){
	moveForward();
	delay(milisecond);
	stop();
}

void Weels::moveBackward(){
	stop();
	digitalWrite(_dirMotorRight, LOW); // direction = backward
	digitalWrite(_speedMotorRight, HIGH);
	digitalWrite(_dirMotorLeft, LOW); // direction = backward
	digitalWrite(_speedMotorLeft, HIGH);
}

void Weels::moveBackward(int milisecond){
	moveBackward();
	delay(milisecond);
	stop();
}

void Weels::stop(int delayMs){
	digitalWrite(_dirMotorRight, LOW);
	digitalWrite(_speedMotorRight, LOW);
	digitalWrite(_dirMotorLeft, LOW);
	digitalWrite(_speedMotorLeft, LOW);
	delay(delayMs);
}

void Weels::stop(){
	stop(500);

}