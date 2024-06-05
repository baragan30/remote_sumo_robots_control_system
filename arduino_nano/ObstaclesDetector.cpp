#include "ObstaclesDetector.h"

//Read the distance from ultrasonic to the closest object in cm
void ObstaclesDetector::config(){
    myservo.attach(SERVO_PIN);
    digitalWrite(SONIC_TRIGG, LOW);
    pinMode(SONIC_ECHO,INPUT);
    pinMode(SONIC_TRIGG,OUTPUT);
    digitalWrite(SONIC_TRIGG, LOW);
    myservo.write(90);
    clockStart = millis();
    setZone(ALL_ZONE);

}

void ObstaclesDetector::setZone(ObstaclaDetectorZone zone){
    switch (zone) {
        case ALL_ZONE:
            degreeMin = ALL_ZONE_MIN;
            degreeMax = ALL_ZONE_MAX;
            break;
        case EXTREME_LEFT_ZONE:
            degreeMin = EXTREME_LEFT_ZONE_MIN;
            degreeMax = EXTREME_LEFT_ZONE_MAX;
            break;
        case EXTREM_RIGHT_ZONE:
            degreeMin = EXTREME_RIGHT_ZONE_MIN;
            degreeMax = EXTREME_RIGHT_ZONE_MAX;
            break;
        case FRONT_ZONE:
            degreeMin = FRONT_ZONE_MIN;
            degreeMax = FRONT_ZONE_MAX;
            break;
        default:
            degreeMin = 90;
            degreeMax = 90;
            break;
    }
    for(int i = 0 ; i < NR_MEASUREMENTS; i ++){
        if (degree_measure[i] < degreeMin && degree_measure[i] > degreeMax){
            measurements[i] = 0;
        }
    }
}

int ObstaclesDetector::getIndex(uint8_t position){
    if(position % DEGREE_STEP != 0)
        return -1;
    return (180 -position) / DEGREE_STEP;
}

void ObstaclesDetector::detect(){
    if (millis() - clockStart < delayTime)  // Do Nothing
        return;
    clockStart = millis();
    uint8_t position = (uint8_t)myservo.read();
    int index = getIndex(position);
    if(index != -1){
        measurements[index] = ultrasonicRead();
    }
    switch (state) {
        case IDLE:      //SCAN
        case MOVING_LEFT:
            if(position > degreeMin)
                myservo.write(position - 1);
            else{
                state = MOVING_RIGHT;
                myservo.write(position + 1);
            }
            break;

        case MOVING_RIGHT:
            if(position < degreeMax)
                myservo.write(position + 1);
            else{
                state = MOVING_LEFT;
                myservo.write(position - 1);
            }
            break;
        default:
            break;
    }
}

void ObstaclesDetector::print(){
    Serial.print(state);
    Serial.print(" ");
    Serial.print(degreeMin);
    Serial.print(" ");
    Serial.println(degreeMax);
    for(int i = 0 ; i < NR_MEASUREMENTS; i ++){
        Serial.print(degree_measure[i]);
        if (degree_measure[i] < 10)
            Serial.print("   ");
        else if (degree_measure[i] < 100)
            Serial.print("  ");
        else
            Serial.print(" ");
    }
    Serial.println();
    for(int i = 0 ; i < NR_MEASUREMENTS; i ++){
        Serial.print((int)measurements[i]);
        if (measurements[i] < 10)
            Serial.print("   ");
        else if (measurements[i] < 100){
            Serial.print("  ");
        }else{
            Serial.print(" ");
        }
    }
    Serial.println();
}



// -------------- PRIVATE FUNCTIONS -------------------------------

//-----------Ultrasonic functions ------------
/**
 * @brief return distance in centimenter
 * 
 * @return int 
 */
uint8_t ObstaclesDetector::ultrasonicRead() {
    digitalWrite(SONIC_TRIGG, HIGH);
    // Wait 10 microseconds
    unsigned int clock = micros();
    while(micros() - clock <= 10);
    digitalWrite(SONIC_TRIGG, LOW);
    noInterrupts();
    unsigned long duration = pulseIn(SONIC_ECHO, HIGH);
    interrupts();
    uint8_t distance = min((duration*SPEED_OF_SOUND)/20000,0xFF);
    return distance;
}
