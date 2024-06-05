#include "LineDetector.h"

void LineDetector::config(){
  pinMode(S1_PIN,INPUT);
  pinMode(S2_PIN,INPUT);
  pinMode(S3_PIN,INPUT);
  pinMode(S4_PIN,INPUT);
  pinMode(S5_PIN,INPUT);
  pinMode(CLP_PIN,INPUT);
}
 

uint8_t LineDetector::read(){
  uint8_t s1 = !digitalRead(S1_PIN);// 0 if reflected the line, 1 if not
  uint8_t s2 = !digitalRead(S2_PIN);
  uint8_t s3 = !digitalRead(S3_PIN);
  uint8_t s4 = !digitalRead(S4_PIN);
  uint8_t s5 = !digitalRead(S5_PIN);
  sensorData =  s1 | (s2<<1) | (s3 << 2) | (s4 <<3 ) | (s5 << 4);

  // nr of not reflexted sensors
  uint8_t nr = s1+ s2 + s3 + s4 + s5;
  // weighted arithmetic mean
  if(nr != 0)
    relativePosition = (( (s5)*2 + s4) - (s2 + (s1) *2))/nr;
  else 
    relativePosition = 0;
  return sensorData;
}

uint8_t LineDetector::getSensorsData(){
  return sensorData ;
}

bool LineDetector::isHitten(){
  return sensorData != 0;
}

int8_t LineDetector::getRelativePosition(){
  return  relativePosition;
}

void LineDetector::print(){
    Serial.print("line = ");
    for (int8_t aBit = 4; aBit >=0 ; aBit--)
      Serial.print((sensorData & 1 << aBit)!=0);
    Serial.print(", relativePosition = ");
    Serial.print(relativePosition);
    Serial.print(", isInLine = ");
    uint8_t is = getSensorsData();
    Serial.println(is,HEX);
}
