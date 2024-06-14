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
    // for(uint8_t *p = data; p < end; p++){
    //     Serial.print(p[0]);
    //     Serial.print(' ');
    // }if(size != 0){
    //     Serial.println();
    //     Serial.print("Transmission : ");
    //     Serial.println(millis() - lastTransmissionClock);
    // }
    
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
            break;
        default:
            p++;
            break;
        }
        lastTransmissionClock = millis();
    }

    switch (state){
        case ROBOT_DRIVER_IDLE_STATE:
            motor.stop();
            lineDec.read();
            spiConnection.addData(COMMAND_RING_EDGE_DATA,lineDec.getSensorsData());
            break;
        case ROBOT_DRIVER_REMOTE_CONTROL_STATE:
            if(millis() - lastTransmissionClock > (unsigned int)300){
                state = ROBOT_DRIVER_AUTONOMOUS_STATE;
            }else if(millis() - lastTransmissionClock > (unsigned int)100){
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
    lineDec.read();
    if(action1.isUnfolding()){
        motor.move(action1.speedLeft, action1.speedRight);
        return;
    }else if(action2.isUnfolding()){
        motor.move(action2.speedLeft, action2.speedRight);
        return;
    }
    
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
            break;
        case -1:
            action2Time = turn150around;
            left = 100;
            right = -100;
            break;
        case 0:
            action2Time = turn180around;
            left = 100;
            right = -100;
            break;
        case 1:
            action2Time = turn150around;
            left = -100;
            right = +100;
            break;
        case 2:
            action2Time = turn120around;
            left = -100;
            right = +100;
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
    const uint8_t engageDistance = 40;

        switch (pos) {
            case 0 ... 39:                 //Force Right
                motor.forceRight();
                break;
            case 40 ... 69:                //Slow Right
                if(distance <= engageDistance)
                    motor.right();
                else
                    motor.forceRight();
                break;
            case 70 ... 110:
                motor.forward();
                break;
            case 111 ... 140:             //Force Left
                if(obsDec.enemyDistance <= engageDistance)
                    motor.left();
                else
                    motor.forceLeft();
                break;
            case 141 ... 180:             // Slow Left
                motor.forceLeft();
                break;
        default:   //-1
            motor.move(40,40);//move slow forward
            break;
    }
    
    
}
