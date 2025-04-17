#include "constants.h"
#include "packet.h"
#include <math.h>
#define TRIG 24
#define ECHO 25
#define TIMEOUT 4000
#define SPEED_OF_SOUND 340 

void setupUltrasonic() // Code for the ultrasonic sensor
{
  /*DDRD |= (1 << 7); // set trigger pin to output
  PORTD &= ~(1 << 7); // write LOW to trigger pin
  DDRD &= ~(1 << 6); // set echo pin to input*/
  pinMode(TRIG, OUTPUT);
  digitalWrite(TRIG, LOW);
  pinMode(ECHO, INPUT);
}

uint32_t readUltrasonic() { // detect distance of ultrasonic sensor from any objects in front of it
  /*PORTD |= (1 << 7); // emit pulse from ultasonic sensor
  delayMicroseconds(100); // delay 100 microseconds
  PORTD &= ~(1 << 7); // stop emitting sound from ultrasonic sensor*/
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(100);
  digitalWrite(TRIG, LOW);
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
