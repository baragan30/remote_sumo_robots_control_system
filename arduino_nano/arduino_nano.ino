#include <Arduino.h>
#include "HardwareTester.h"
#include "spi_slave_connection.h"
#include "robot_driver.h"
SpiSlaveConnection spiConnection;
RobotDriver driver(spiConnection);
// HardwareTester tester(spiConnection);


void setup(){
  Serial.begin(115200);
  Serial.println("Start");

  //  tester.testMotorDirectionMinimumspeed();
  //  tester.testMotorShortcuts();
  //  tester.testMotorAdjustTimeRotate();
  //  tester.testMotorDirections();
  
  // tester.testUltrasonic();
  // tester.testServo();
    // tester.testObstaclesDetector();
      
      // tester.testLineDetector();
    // tester.testSpiSlaveConnection();
      driver.config();
}
void loop()
{
    driver.drive();
    // driver.autonomous();
}
ISR(SPI_STC_vect){
    spiConnection.interrupt();
}