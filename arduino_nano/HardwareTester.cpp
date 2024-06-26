#include "HardwareTester.h"
#include "Servo.h"
#include "Actions.h"


[[noreturn]] void HardwareTester::testMotorDirectionForwardBackward(){
    motor.config();
    while(true){
        for (int direction = -100 ; direction <= 100; direction+=20){
            for (int speed = 100 ;speed!=-300 ; speed -=200){
                Serial.print("d = ");
                Serial.print(direction);
                Serial.print("   s = ");
                Serial.println(speed);
                motor.move(direction , speed);
                delay(1000);
                motor.stop();
                Serial.println("-------------------------------------------------");
                delay(1000);
            }
        }
    }
}

[[noreturn]] void HardwareTester::testMotorShortcuts(){
    Motor motor;
    motor.config();
    int d = 250;
    int d2 = 2000;
    while(true){

        motor.forward();
        delay(d);
        motor.stop();
        delay(d2);

        motor.backward();
        delay(d);
        motor.stop();
        delay(d2);

        motor.forceRight();
        delay(d);
        motor.stop();
        delay(d2);

        motor.forceLeft();
        delay(d);
        motor.stop();
        delay(d2);

        motor.right();
        delay(d);
        motor.stop();
        delay(d2);

        motor.left();
        delay(d);
        motor.stop();
        delay(d2);

    }
}

[[noreturn]] void HardwareTester::testMotorDirections(){
    motor.config();
    Servo servo;
    servo.attach(A1);
    while(true){
        for(int left = -100; left <= 100; left += 50){
            for(int right = -100; right <= 100; right += 50){
                motor.move(left, right);
                servo.write(90);
                delay(2000);
                motor.stop();
                delay(1000);
            }
        }
        

    }
}
[[noreturn]] void HardwareTester::testMotorAdjustTimeRotate(){

    //1500 seems perfect
    motor.config();
    while(true){
        for(int time = 1300; time <= 1600; time +=100){
            // motor.move(100,-100);
            motor.forceRight();
            delay(time);
            Serial.println(time);
            motor.stop();
            delay(1000);
        }
    }
}

[[noreturn]] void HardwareTester::testMotorSpeed(){
    motor.config();
    while(1){
        for (int speed = -100 ; speed <= 100; speed+=10){
            motor.move(0,speed);
            Serial.println(speed);
            delay(100);
            motor.stop();
            delay(200);

        }
        for (int speed = 100 ; speed >= -100; speed-=10){
            motor.move(0,speed);
            Serial.println(speed);
            delay(100);
            motor.stop();
            delay(200);
        }
    }
}

[[noreturn]] void HardwareTester::testMotorDirectionMinimumspeed(){
    motor.config();
    int d = 250;
    int d2 = 2000;
    while(true){
        motor.move(1,1);
        delay(d);
        motor.stop();
        delay(d2);

        motor.move(-1,-1);
        delay(d);
        motor.stop();
        delay(d2);

        motor.move(-1,1);
        delay(d);
        motor.stop();
        delay(d2);

         motor.move(1,-1);
        delay(d);
        motor.stop();
        delay(d2);
    }
}

[[noreturn]] void HardwareTester::testMotorMinimumSpeedBackwardForward(){
    motor.config();
    while(true){
        motor.move(0,1);
        delay(1000);
        motor.stop();
        delay(500);
        motor.move(0,-1);
        delay(1000);
        motor.stop();
        delay(500);
    }
}

[[noreturn]] void HardwareTester::testUltrasonic(){
    obsDec.config();
    while(true){
        unsigned long clock = micros();
        Serial.print(obsDec.ultrasonicRead());
        Serial.print(" ");
        Serial.println(micros() - clock);
        delay(10);
    }
}
[[noreturn]] void HardwareTester::testServo(){
    obsDec.config();

    while(true){
        for(int i = 0 ; i <= 180 ; i += 2){
            obsDec.move(i);
            Serial.println(i);
            delay(3);
        }
        delay(1000);
        obsDec.move(90);
        delay(1000);
        obsDec.move(0);
        delay(1000);
    }
}

[[noreturn]] void HardwareTester::testObstaclesDetector(){
    obsDec.config();
    long clock = millis();
    int i = 0 ;
    while(true){
        if(millis() - clock  > 1000){
            obsDec.print();
            clock = millis();
        }
        obsDec.detect();
    }
}

[[noreturn]] void HardwareTester::testLineDetector(){
    lineDec.config();
    while(true){
        lineDec.read();
        lineDec.print();
        delay(200);
    }
}

[[noreturn]] void HardwareTester::testSpiSlaveConnection(){
    spiConnection.init();
    uint8_t data[SpiSlaveConnection::BUFFER_CAPACTITY];
    uint8_t size = spiConnection.getData(data);
    uint8_t* p = data, *end = data + size;
    for(uint8_t *p = data; p < end; p++){
        Serial.print(p[0]);
        Serial.print(' ');
    }if(size != 0){
        Serial.println();
    }else{
        Serial.println("No receive");
    }
    uint8_t measurements[] = {1,2,3,4,5,6,7,8,9,10,11,12,13};
    spiConnection.addData(COMMAND_DISTANCE_DATA, measurements);
    spiConnection.addData(COMMAND_RING_EDGE_DATA,14);
}