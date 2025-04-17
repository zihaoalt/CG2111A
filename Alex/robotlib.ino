#include <AFMotor.h>
// Direction values

// Motor control
#define FRONT_LEFT   4 // M4 on the driver shield
#define FRONT_RIGHT  3 // M1 on the driver shield
#define BACK_LEFT    1// M3 on the driver shield
#define BACK_RIGHT   2// M2 on the driver shield

AF_DCMotor motorFL(FRONT_LEFT);
AF_DCMotor motorFR(FRONT_RIGHT);
AF_DCMotor motorBL(BACK_LEFT);
AF_DCMotor motorBR(BACK_RIGHT);

void move(float speed, int direction)
{
  int speed_scaled = (speed/100.0) * 255;
  motorFL.setSpeed(speed_scaled);
  motorFR.setSpeed(speed_scaled);
  motorBL.setSpeed(speed_scaled);
  motorBR.setSpeed(speed_scaled);
//right wheels directions are reversed
  switch(direction)
    {
      case BACK:
        motorFL.run(BACKWARD);
        motorFR.run(FORWARD); 
        motorBL.run(BACKWARD); 
        motorBR.run(FORWARD); 
      break;
      case GO:
        motorFL.run(FORWARD);
        motorFR.run(BACKWARD);
        motorBL.run(FORWARD);
        motorBR.run(BACKWARD); 
      break;
      case CCW:
        motorFL.run(BACKWARD);
        motorFR.run(BACKWARD);
        motorBL.run(BACKWARD);
        motorBR.run(BACKWARD); 
      break;
      case CW:
        motorFL.run(FORWARD);
        motorFR.run(FORWARD);
        motorBL.run(FORWARD);
        motorBR.run(FORWARD); 
      break;
      case STOP:
      default:
        motorFL.run(STOP);
        motorFR.run(STOP);
        motorBL.run(STOP);
        motorBR.run(STOP);
    }
}

void forward(float dist, float speed)
{
  if (dist > 0) {
    deltaDist = dist;
  }
  else {
    deltaDist = 9999999;
  }
  newDist = forwardDist + deltaDist;
  dir = (TDirection) FORWARD;
  move(speed, FORWARD);
}

void reverse(float dist, float speed)
{
  if (dist > 0) {
    deltaDist = dist;
  }
  else {
    deltaDist = 9999999;
  }
  newDist = reverseDist + deltaDist;
  dir = (TDirection) BACKWARD;
  move(speed, BACKWARD);
}

/*void ccw(float ang, float speed)
{
  dir = (TDirection) LEFT;
  move(speed, CCW);
}
void cw(float ang, float speed)
{
  dir = (TDirection) RIGHT;
  move(speed, CW);
}*/


 void left(float ang, float speed) {
  if(ang == 0)
  {
    deltaTicks=9999999;
  }
  else {
    deltaTicks=computeDeltaTicks(ang);
    }
  targetTicks=leftReverseTicksTurns + deltaTicks;

  dir = (TDirection) LEFT;
  move(speed,LEFT);
}
  // ccw(ang, speed);



void right (float ang, float speed) {
  if(ang == 0)
  {
    deltaTicks=9999999;
  }
  else {
    deltaTicks=computeDeltaTicks(ang);
  }
  targetTicks=rightReverseTicksTurns + deltaTicks;

  dir = (TDirection) RIGHT;
  move(speed, RIGHT);
}

void stop()
{
  dir = (TDirection) STOP;
  move(0, STOP);
}

/*void left(float ang, float speed)
{
  if (ang == 0)
  {
    deltaTicks = 999999;
  }
  else 
  {
    deltaTicks = computeDeltaTicks(ang);
  }

  targetTicks = leftReverseTicksTurns + deltaTicks;
  ccw(ang, speed);
}

void right(float ang, float speed)
{
  if (ang == 0)
  {
    deltaTicks = 999999;
  }
  else 
  {
    deltaTicks = computeDeltaTicks(ang);
  }

  targetTicks = leftReverseTicksTurns + deltaTicks;
  cw(ang, speed);
}*/
