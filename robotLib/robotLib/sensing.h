// sensing.h

#ifndef _SENSING_h
#define _SENSING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <NewPing.h>
#include "RF24.h"

class UltraSonicEye{
public:
	UltraSonicEye(int triggerPin, int echoPin);
	unsigned int look() const;
	unsigned int lookAccuratly(int sample) const;
private:
	int _triggerPin;
	int _echoPin;
	int MAX_DISTANCE_CM = 300;
	NewPing *_sonar;
};

typedef struct RadioMsg{
  unsigned long _micros;
  int distance;
};

class RadioCom{
public:
  RadioCom(int cePin, int csnPin, uint64_t pipe, bool debug = true, bool highStrength = false);
  bool sendData(RadioMsg msg);
  bool sendData(int distance);
private:
  RF24 *_radio;
  bool _debugMode;
};

#endif

