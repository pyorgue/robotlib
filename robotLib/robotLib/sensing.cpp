#include "stdinclude.h"
#include "sensing.h"


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
unsigned int UltraSonicEye::lookAccuratly(int sample) const
{
	int distance = _sonar->convert_cm(_sonar->ping_median(sample));
	return distance <= 0 ? MAX_DISTANCE_CM : distance;
}

/**
 * \brief build a radio message sender
 * \param cePin The CE Pin of the radio RF24 module (usually 7 on Arduino Uno)
 * \param csnPin The CSN Pin of the radio RF24 module (usually 8 on Arduino Uno)
 * \param pipe  The pipe communication chanel
 * \param highStrength True if the communication is sent using a high strength signal
 */
RadioCom::RadioCom(int cePin, int csnPin, uint64_t pipe, bool debug = true, bool highStrength = false){
  _radio = new RF24(cePin, csnPin);
  _radio->begin();
  _debugMode = debug;

  // Set the PA Level low to prevent power supply related issues RF24_PA_MAX is default.
  if(highStrength){
    _radio->setPALevel(RF24_PA_MAX);
  }
  else{
    _radio->setPALevel(RF24_PA_LOW);
  }
  // Enable auto acknowledgement
  _radio->setAutoAck(true);

  // set delay and number of retry (250 microseconds, 3 times)
  _radio->setRetries(1, 1);

  // Open a writing and reading pipe on each radio, with opposite addresses
  if(_debugMode){
    _radio->openWritingPipe(pipe);
  }
  // print diagnostic (need printf to be initiated)
  _radio->printDetails();

  // stop listening so we can talk
  _radio->stopListening();
}

/**
 * \brief Send a radio message
 * \param msg The mesage to send
 * \returnTrue if the message has been sent successfully
 */
bool RadioCom::sendData(RadioMsg msg){
  if(_debugMode){
    printf("Distance = %dcm, time = %ums \n", msg.distance, msg._micros);
  }
  if (!_radio->write(&msg, sizeof(RadioMsg))){
    if(_debugMode){
      printf("Failed sending setup message. \n");
    }
    return false;
  }
  return true;
}   

bool RadioCom::sendData(int distance){
  RadioMsg msg;
  msg.distance = distance;
  msg._micros = millis();
  return sendData(msg);
}

