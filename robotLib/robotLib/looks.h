#pragma once
#// looks.h

#ifndef _LOOKS_h
#define _LOOKS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class LiquidCrystal;

/*
The look API.
Control light, display
*/

/**
* \brief A light
*/
class Light{
public:
	/**
	* \brief Create a light
	* \param pin Pin number on the Arduino board
	*/
	Light(int pin);

	/**
	*\brief Destruct a light
	*/
	virtual ~Light()
	{
	}

	/**
	* \brief Turn on the light
	*/
	void turnOn() const;

	/**
	* \brief Turn off the light
	*/
	void turnOff() const;
private:
	int _pin;
};

/**
* \brief A flashing light - with no wait
*/
class FlashingLight : public Light{
public:
	/**
	* \brief Create a Flashing light
	* \param pin The Pin number on the Arduino board
	* \param updateInterval  The flashing interval in millisecond
	*/
	FlashingLight(int pin, int updateInterval);

	/**
	* \brief Add this to your loop to make sure the  light is flashing
	*/
	void update();

	/**
	* \brief Turn on the light
	*/
	virtual void turnOn();

	/**
	* \brief Turn off the light
	*/
	virtual void turnOff();
private:
	int _updateInterval;
	unsigned long _lastUpdate;
	int _lightState;
	int _lightSwitch;
};


class LCDDisplay
{
public:

	/**
	* \brief Create a 4 bits LCD display.
	* RW and VSS(GND) are linked to the ground
	* VCC is linked to +5V
	* 10K to +5V and GND
	* Contrast can be controlled by V0
	* http://electronics.stackexchange.com/questions/115819/arduino-tc1602-no-display-but-backlight
	*
	* \param rsPin Digital RS pin (12)
	* \param enablePin Digital Enable (E) pin (11)
	* \param d4Pin Digital D4 pin
	* \param d5Pin Digital D5 pin
	* \param d6Pin Digital D6 pin
	* \param d7Pin Digital D7 pin
	*/
	LCDDisplay(int rsPin, int enablePin, int d4Pin, int d5Pin, int d6Pin, int d7Pin);

	/**
	* \brief Remove existing display and replace by the message in parameter
	* \param messageLine1 The message to display on the first line
	* \param messageLine2 The message to display on the second line
	*/
	void print(String messageLine1, String messageLine2);

	/**
	* \brief Print a specific character somewhere in the screen
	* \param column starting from 0, the column number
	* \param line starting from 0, the line number
	* \param character The character to display
	*/
	void printChar(int column, int line, char character);

	/**
	* Turn on the display
	*/
	void startDisplay() const;

	/**
	* Turn of the display
	*/
	void stopDisplay() const;

	/**
	* \bried Turn on the blinking cursor
	*/
	void startBlink() const;

	/**
	* \bried Turn off the blinking cursor
	*/
	void stopBlink() const;

private:
	LiquidCrystal *_lcd;
};

#endif

