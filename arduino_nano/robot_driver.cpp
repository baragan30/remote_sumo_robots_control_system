#include "robot_driver.h"

RobotDriver::RobotDriver(SpiSlaveConnection& spiConnection) 
: spiConnection(spiConnection){

}
void RobotDriver::config(){
    motor.config();
    lineDec.config();
    obsDec.config();
    spiConnection.init();
    state = ROBOT_DRIVER_IDLE_STATE;
    // state = ROBOT_DRIVER_AUTONOMOUS_STATE;
}


void RobotDriver::drive(){
    uint8_t data[SpiSlaveConnection::BUFFER_CAPACTITY];
    uint8_t size = spiConnection.getData(data);
    uint8_t* p = data, *end = data + size;
    for(uint8_t *p = data; p < end; p++){
        Serial.print(p[0]);
        Serial.print(' ');
    }if(size != 0){
        Serial.println();
        Serial.print("Transmission : ");
        Serial.println(millis() - lastTransmissionClock);
    }
    
    while (p < end) {
        switch (*p)
        {
        case COMMAND_CONNECTION:
            if(state != ROBOT_DRIVER_IDLE_STATE){
                if(p[1]){
                    state = ROBOT_DRIVER_REMOTE_CONTROL_STATE;
                    motor.stop();
                }
            }
            p+=2;
            break;
        case COMMAND_STRATEGY:
            if(p[1] == 0x00)
                state = ROBOT_DRIVER_IDLE_STATE;
            p+=2;
            break;
        case COMMAND_MOTOR_POWER:
            state = ROBOT_DRIVER_REMOTE_CONTROL_STATE;
            motor.move(p[1], p[2]);
            p+=3;
            lastTransmissionClock = millis();
            break;
        default:
            p++;
            break;
        }
    }
    switch (state){
        case ROBOT_DRIVER_IDLE_STATE:
            motor.stop();
            lineDec.read();
            spiConnection.addData(COMMAND_RING_EDGE_DATA,lineDec.getSensorsData());
            break;
        case ROBOT_DRIVER_REMOTE_CONTROL_STATE:
            if(millis() - lastTransmissionClock > 300u){
                state = ROBOT_DRIVER_AUTONOMOUS_STATE;
                Serial.println("autonomous");
                break;
            }else if(millis() - lastTransmissionClock > 100u){
                motor.stop();
            }
            lineDec.read();
            spiConnection.addData(COMMAND_RING_EDGE_DATA,lineDec.getSensorsData());
            obsDec.detect();
            spiConnection.addData(COMMAND_DISTANCE_DATA, obsDec.measurements);
            break;
        case ROBOT_DRIVER_AUTONOMOUS_STATE:
            // autonomousTest();
            autonomous();
            break;
        default:
            Serial.println("default");
            break;
    }

}


void RobotDriver::autonomousTest() {
    static unsigned long clock;
    obsDec.detect();
    if(millis() - clock > 2000){
        clock = millis();
    }else if(millis() - clock > 800){
        motor.stop();
    }
    else if(millis() - clock > 400){
        motor.move(25, -25);
    }
    else {
        motor.move(-25,25);
    }
}

void RobotDriver::autonomous(){
    
    if(action1.isUnfolding()){
        motor.move(action1.speedLeft, action1.speedRight);
        return;
    }else if(action2.isUnfolding()){
        motor.move(action2.speedLeft, action2.speedRight);
        return;
    }
    
    lineDec.read();
    if(lineDec.isHitten()){
        int8_t relativePosition = lineDec.getRelativePosition();
        int8_t left,right;
        unsigned long action1Time = 100,action2Time;
        motor.backward();
        switch (relativePosition)
        {
        case -2:
            action2Time = turn120around;
            left = 100;
            right = -100;
            obsDec.setZone(EXTREM_RIGHT_ZONE);
            obsDec.move(0);
            break;
        case -1:
            action2Time = turn150around;
            left = 100;
            right = -100;
            obsDec.setZone(EXTREM_RIGHT_ZONE);
            obsDec.move(0);
            break;
        case 0:
            action2Time = turn180around;
            left = 100;
            right = -100;
            obsDec.setZone(EXTREM_RIGHT_ZONE);
            obsDec.move(0);
            break;
        case 1:
            action2Time = turn150around;
            left = -100;
            right = +100;
            obsDec.setZone(EXTREME_LEFT_ZONE);
            obsDec.move(180);
            break;
        case 2:
            action2Time = turn120around;
            left = -100;
            right = +100;
            obsDec.setZone(EXTREME_LEFT_ZONE);
            obsDec.move(180);
            break;
        default:
            break;
        }
        action1 = ActionClock(action1Time,-100, -100);
        action2 = ActionClock(action1Time + action2Time,left, right);
        return;
    }
    obsDec.detect();
    int16_t pos = obsDec.enemyPosition;
    uint8_t distance = obsDec.enemyDistance;
    const uint8_t engageDistance = 10;

    switch (pos) {
         case 0 ... 59:    //Right
            motor.right();
            break;
        case 60 ... 74:
            if(obsDec.enemyDistance <= engageDistance)
                motor.frontRight();
            else
                motor.right();
            break;
        case 75 ... 105:
            motor.forward();
            break;
        case 106 ... 120: 
            if(obsDec.enemyDistance <= engageDistance)
               motor.frontLeft();
            else
                motor.left();
            break;
        case 121 ... 180: //Left
            motor.left();
            break;
        default:   //-1
            motor.move(15,15);//move slow forward
            break;
    }
    
}
