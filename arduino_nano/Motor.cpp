#include "Motor.h"

void Motor::config(){
   digitalWrite(leftP0_DIGITAL, 0);
   digitalWrite(leftP1_PWM, 0);
   digitalWrite(rightP0_DIGITAL, 0);
   digitalWrite(rightP1_PWM, 0);
   
   pinMode (leftP0_DIGITAL, OUTPUT);
   pinMode (leftP1_PWM, OUTPUT);
   pinMode (rightP0_DIGITAL, OUTPUT);
   pinMode (rightP1_PWM, OUTPUT);
}


void Motor::stop(){
  move(0,0);
}
void Motor::forward(){
   move(100,100);
}
void Motor::backward(){
   move(-100,-100);
}
void Motor::forceLeft(){
  move(-100, 100);
}
void Motor::forceRight(){
  move(100,-100);
}

void Motor::left(){
    move(-40,40);
}
void Motor::right(){
    move(40,-40);
}

void Motor::frontLeft(){
    move(1,100);
}
void Motor::frontRight(){
    move(100,1);
}


/*
 * Transform a number between 0 and PMAX in a number between POW_MIN and POWMAX
 * transposesSpeed(0) = 0
 * transposesSpeed(1) = POW_MIN
 * transposesSpeed(PMAX) = POW_MAX
 */
uint8_t Motor::speedToPower(int8_t speed){
  if (speed == 0) 
    return 0;
  speed = min(abs(speed), SPEED_MAX);
  return POW_MIN + (speed - 1) *(POW_MAX - POW_MIN)/ (SPEED_MAX - 1);
}

void Motor::move(int8_t leftPower, int8_t rightPower){
  moveWheel(leftPower,leftP0_DIGITAL, leftP1_PWM);
  moveWheel(rightPower,rightP0_DIGITAL, rightP1_PWM);
  
}
void Motor::moveWheel(int8_t speed,int P0_DIGITAL, int P1_PWM){
  uint8_t power = speedToPower(speed);
  //MOVE FORWARD
    if(speed > 0){
      digitalWrite(P0_DIGITAL, 0);
      analogWrite(P1_PWM,power);
    }else{
      digitalWrite(P0_DIGITAL, 1);
      analogWrite(P1_PWM,255 -power);
    }
}
