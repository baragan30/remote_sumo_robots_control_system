#ifndef LINEDETECTOR_H
#define LINEDETECTOR_H

#include <Arduino.h>

class LineDetector{
  public:
  //initialize Line Detector
  void config();
  //must be read as often as possible
  uint8_t read();
  //if the robot is Hitten (front sensor)
  bool isHitten();
  //return last readed output  of 5 line sensour
  uint8_t getSensorsData();
  int8_t getRelativePosition();
  void print();


  private:
    // The relative position of the ring edge within the range of [-2, 2], where:
    // -2 indicates far left, 0 indicates center, and 2 indicates far right.
    int8_t relativePosition;

    // Sensor data represented as a 5-bit integer where each bit corresponds to a sensor's state:
    // 0 means light is reflected and 1 means no light reflection.
    // Bit 0 corresponds to the far left sensor and Bit 4 to the far right sensor.
    uint8_t sensorData;
    static constexpr uint8_t S1_PIN = A0;
    static constexpr uint8_t S2_PIN = A1;
    static constexpr uint8_t S3_PIN = A2;
    static constexpr uint8_t S4_PIN = A3;
    static constexpr uint8_t S5_PIN = A4;
    static constexpr uint8_t CLP_PIN = A5;
};
#endif
