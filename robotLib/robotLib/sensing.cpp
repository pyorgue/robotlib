#include "sensing.h"
#include "NewPing.h"

/**
 * \brief Build an ultrasonic eye
 * \param triggerPin The trigger pin number
 * \param echoPin The echo pin number
 */
UltraSonicEye::UltraSonicEye(int triggerPin, int echoPin)
{
	_echoPin = echoPin;
	_triggerPin = triggerPin;
	_sonar = new NewPing(_triggerPin, _echoPin, MAX_DISTANCE_CM);
}

/**
 * \brief Look
 * \return the distance in cm
 */
unsigned int UltraSonicEye::look() const
{
	int distance =_sonar->convert_cm(_sonar->ping());
	return distance <= 0 ? MAX_DISTANCE_CM : distance;
}

/**
 * \brief Look accuratly
 * \return the distance in cm
 */
unsigned int UltraSonicEye::lookAccuratly() const
{
	int distance = _sonar->convert_cm(_sonar->ping_median(5));
	return distance <= 0 ? MAX_DISTANCE_CM : distance;
}
