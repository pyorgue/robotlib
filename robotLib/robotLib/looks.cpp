#include "stdinclude.h"
#include "looks.h"


/* Control Light */
Light::Light(int pin){
	pinMode(pin, OUTPUT);
	_pin = pin;
}

void Light::turnOn() const
{
	digitalWrite(_pin, HIGH);
}

void Light::turnOff() const
{
	digitalWrite(_pin, LOW);
}

/*
Control Flashing light
TODO: use interupt instead of time interval (doesn't wait if wait is invoke somewhere else)
*/
FlashingLight::FlashingLight(int pin, int updateInterval) : Light(pin) {
	_updateInterval = updateInterval;
	_lightState = LOW;
	_lastUpdate = 0;
	_lightSwitch = LOW;
}

void FlashingLight::update(){
	if (_lightSwitch == HIGH && (millis() - _lastUpdate > _updateInterval)){
		if (_lightState == LOW){
			Light::turnOn();
			_lightState = HIGH;
		}
		else
		{
			Light::turnOff();
			_lightState = LOW;
		}
		_lastUpdate = millis();
	}
}

void FlashingLight::turnOn(){
	_lightSwitch = HIGH;
	Light::turnOn();
}

void FlashingLight::turnOff(){
	_lightSwitch = LOW;
	Light::turnOff();
}

/*
Control LCD display
*/
LCDDisplay::LCDDisplay(int rsPin, int enablePin, int d4Pin, int d5Pin, int d6Pin, int d7Pin)
{
	_lcd = new LiquidCrystal(rsPin, enablePin, d4Pin, d5Pin, d6Pin, d7Pin);
	_lcd->begin(16, 2);
	_lcd->display();
}

void LCDDisplay::print(String messageLine1, String messageLine2)
{
	//TODO control that the message is not too long
	_lcd->setCursor(0, 0);
	_lcd->print(messageLine1);
	_lcd->setCursor(0, 1);
	_lcd->print(messageLine1);
}

void LCDDisplay::printChar(int column, int line, char character)
{
	_lcd->setCursor(column, line);
	_lcd->print(character);
}

void LCDDisplay::startDisplay() const
{
	_lcd->display();
}

void LCDDisplay::stopDisplay() const
{
	_lcd->noDisplay();
}

void LCDDisplay::startBlink() const
{
	_lcd->blink();
}

void LCDDisplay::stopBlink() const
{
	_lcd->noBlink();
}