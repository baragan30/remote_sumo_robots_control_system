#ifndef HARDWARE_TESTER_H
#define HARDWARE_TESTER_H
#include <Arduino.h>
#include "ObstaclesDetector.h"
#include "Motor.h"
#include "LineDetector.h"
#include "spi_slave_connection.h"

class HardwareTester{
private:
    LineDetector lineDec;
    ObstaclesDetector obsDec;
    Motor motor;
    SpiSlaveConnection& spiConnection;
    
public:
    HardwareTester(SpiSlaveConnection& spiConnection) 
: spiConnection(spiConnection){}
    [[noreturn]] void testMotorShortcuts();
    [[noreturn]] void testMotorDirectionForwardBackward();


    [[noreturn]] void testMotorDirections();
    [[noreturn]] void testMotorAdjustTimeRotate();

    [[noreturn]] void testMotorSpeed();

    [[noreturn]] void testMotorDirectionMinimumspeed();

    [[noreturn]] void testMotorMinimumSpeedBackwardForward();

    [[noreturn]] void testObstaclesDetector();

    [[noreturn]] void testUltrasonic();

    [[noreturn]] void testLineDetector();

    [[noreturn]] void testSpiSlaveConnection();


};

#endif
