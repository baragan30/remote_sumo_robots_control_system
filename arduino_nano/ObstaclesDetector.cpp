#include "ObstaclesDetector.h"

//Read the distance from ultrasonic to the closest object in cm
void ObstaclesDetector::config(){
    myservo.attach(SERVO_PIN);
    digitalWrite(SONIC_TRIGG, LOW);
    pinMode(SONIC_ECHO,INPUT);
    pinMode(SONIC_TRIGG,OUTPUT);
    digitalWrite(SONIC_TRIGG, LOW);
    move(90);
    clockStart = millis();
    compute_enemy_position();
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
        if ( degree_measure[i] < degreeMin  || degree_measure[i] > degreeMax){
            measurements[i] = 0;
        }
    }
}

int ObstaclesDetector::getIndex(uint8_t position){
    if(position % DEGREE_STEP != 0)
        return -1;
    return position / DEGREE_STEP;
}

void ObstaclesDetector::compute_enemy_position(){
    uint8_t minDistance = 0xff, minIndex = -1;
    for(int i = 0 ; i < NR_MEASUREMENTS; i ++){
        if(measurements[i] != 0 && measurements[i] < minDistance){
            minDistance = measurements[i];
            minIndex = i;
        }
    }

    if(minIndex == -1 || minDistance > 50){
        setZone(ALL_ZONE);
        enemyPosition = -1;
        enemyDistance = 0;//unknown
    }else {
        //minDistance <= 50, minindex != -1
        enemyPosition = degree_measure[minIndex];
        enemyDistance = minDistance;
        if(minDistance > 20){
            setZone(ALL_ZONE);
        }else {
            if(enemyPosition >= 150){
                setZone(EXTREME_LEFT_ZONE);
            }else if(enemyPosition <= 30){
                setZone(EXTREM_RIGHT_ZONE);
            }else{
                setZone(FRONT_ZONE);
            }
        }
    }
}

void ObstaclesDetector::detect(){
    if (millis() - clockStart < DELAY_SERVO_TIME)  // Do Nothing
        return;
    clockStart = millis();
    uint8_t position = getServoPosition();
    int index = getIndex(position);
    if(index != -1){
        measurements[index] = ultrasonicRead();
    }
    compute_enemy_position();
    switch (state) {
        case IDLE:      //SCAN
        case MOVING_LEFT:
            if(position > degreeMin)
                move(position - 1);
            else{
                state = MOVING_RIGHT;
                move(position + 1);
            }
            break;

        case MOVING_RIGHT:
            if(position < degreeMax)
                move(position + 1);
            else{
                state = MOVING_LEFT;
                move(position - 1);
            }
            break;
        default:
            break;
    }
}

uint8_t ObstaclesDetector::getServoPosition(){
    return (uint8_t)myservo.read();
}

void ObstaclesDetector::move(uint8_t degree){
    myservo.write(degree);
}

void ObstaclesDetector::print(){
    Serial.print(enemyPosition);
    Serial.print(" ");
    Serial.println(enemyDistance);
    Serial.print(degreeMin);
    Serial.print(" <-> ");
    Serial.println(degreeMax);
    for(int i = NR_MEASUREMENTS -1 ; i >= 0; i --){
        Serial.print(degree_measure[i]);
        if (degree_measure[i] < 10)
            Serial.print("   ");
        else if (degree_measure[i] < 100)
            Serial.print("  ");
        else
            Serial.print(" ");
    }
    Serial.println();
    for(int i = NR_MEASUREMENTS -1 ; i >= 0; i--){
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
    // noInterrupts();
    unsigned long duration = pulseIn(SONIC_ECHO, HIGH, 12000);
    // interrupts();
    uint8_t distance = min((duration*SPEED_OF_SOUND)/20000,0xFF);
    return distance;
}
