// sensing.h

#ifndef _SENSING_h
#define _SENSING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <NewPing.h>



class UltraSonicEye{
public:
	UltraSonicEye(int triggerPin, int echoPin);
	unsigned int look() const;
	unsigned int lookAccuratly() const;
private:
	int _triggerPin;
	int _echoPin;
	int MAX_DISTANCE_CM = 300;
	NewPing *_sonar;
};
#endif

