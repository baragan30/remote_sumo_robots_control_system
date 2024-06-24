#ifndef ROBOT_DRIVER_H
#define ROBOT_DRIVER_H

#include "ObstaclesDetector.h"
#include "Motor.h"
#include "LineDetector.h"
#include "spi_slave_connection.h"
#include "Actions.h"

enum ROBOT_DRIVER_STATE{
    ROBOT_DRIVER_IDLE_STATE = 0x00,
    ROBOT_DRIVER_REMOTE_CONTROL_STATE = 0x01,
    ROBOT_DRIVER_AUTONOMOUS_STATE = 0x02,
};
class RobotDriver
{
public:

    RobotDriver(SpiSlaveConnection& spiConnection);
    void config();
    void drive();   
    void autonomousTest();
    void autonomous();
private:
    LineDetector lineDec;
    ObstaclesDetector obsDec;
    Motor motor;
    SpiSlaveConnection &spiConnection;
    ROBOT_DRIVER_STATE state;
    ActionClock action1, action2;
    unsigned long lastTransmissionClock = millis();

    static const unsigned long turn360around = 1000;
    static const unsigned long turn180around = turn360around/2;
    static const unsigned long turn165around = turn360around*11/24;
    static const unsigned long turn150around = turn360around/2;
    static const unsigned long turn120around = turn360around/3;
    static const unsigned long turn90around = turn360around/4;
    static const unsigned long turn60around = turn360around/6;
    static const unsigned long turn45around = turn360around/8;
    static const unsigned long turn30around = turn360around/12;
};

#endif