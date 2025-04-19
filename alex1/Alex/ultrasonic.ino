#include "constants.h"
#include "packet.h"
#include <math.h>
#define TRIG 24 //PA2
#define ECHO 25 //PA3
#define TIMEOUT 4000
#define SPEED_OF_SOUND 340 

void setupUltrasonic() // Code for the ultrasonic sensor
{
  DDRA |= 0b00000100;
  PORTA &= 0b11111011;
  DDRA &= 0b11110111;
  
  /*pinMode(TRIG, OUTPUT);
  digitalWrite(TRIG, LOW);
  pinMode(ECHO, INPUT);
  */
}

uint32_t readUltrasonic() { // detect distance of ultrasonic sensor from any objects in front of it
  /*digitalWrite(TRIG, HIGH);
  delayMicroseconds(100);
  digitalWrite(TRIG, LOW);*/
  
  PORTA |= 0b00000100;
  delayMicroseconds(100);
  PORTA &= 0b11111011;

  double duration = pulseIn(ECHO, HIGH, TIMEOUT); // measure time taken to detect echo from initial ultrasonic pulse
  double dist = duration / 2 / 10000 * SPEED_OF_SOUND; // calculate distance of object from ultrasonic sensor
  return (uint32_t) round(dist); // return distance of object from ultrasonic sensor in cm
}

void sendDist(uint32_t distance) {
  TPacket distancePacket;
  distancePacket.packetType = PACKET_TYPE_RESPONSE;
  distancePacket.command = RESP_ULTRASONIC;
  distancePacket.params[0] = distance;
  sendResponse(&distancePacket);

}
