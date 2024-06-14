#ifndef OBSTACLES_DETECTOR
#define OBSTACLES_DETECTOR

#include <Arduino.h>
#include <Servo.h>
#include <stdint.h>

enum ObstaclesDetectorState{IDLE, MOVING_LEFT, MOVING_RIGHT};

enum ObstaclaDetectorZone{ALL_ZONE,EXTREME_LEFT_ZONE,EXTREM_RIGHT_ZONE,FRONT_ZONE,IDLE_ZONE};
#define ALL_ZONE_MIN 0
#define ALL_ZONE_MAX 180

#define EXTREME_LEFT_ZONE_MIN 105
#define EXTREME_LEFT_ZONE_MAX 180

#define EXTREME_RIGHT_ZONE_MIN 0
#define EXTREME_RIGHT_ZONE_MAX 75

#define FRONT_ZONE_MIN 30
#define FRONT_ZONE_MAX 150


class ObstaclesDetector{
private:
    static constexpr uint8_t SERVO_PIN = 7; //Meters per Second
    static constexpr uint8_t SONIC_ECHO = 8;
    static constexpr uint8_t SONIC_TRIGG = 4;

    static constexpr uint16_t SPEED_OF_SOUND = 343; //Meters per Second
    static constexpr uint8_t NR_MEASUREMENTS = 13;
    static constexpr uint8_t DEGREE_STEP = 180/(NR_MEASUREMENTS - 1);
    static constexpr uint8_t DELAY_SERVO_TIME = 3;
    
    ObstaclesDetectorState state;
    //angles at which we want to measure
    const uint8_t degree_measure[NR_MEASUREMENTS] =     {0,  15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180}; 
    
    uint8_t degreeMin;
    uint8_t degreeMax;
    unsigned long clockStart;

    int getIndex(uint8_t position);
    void compute_enemy_position();
   
    Servo myservo;
public :
     //from 0 to 180(0 for right and ), -1 if it's not known
    int16_t enemyPosition;
    // from 1 to 255, 0 for not known
    uint8_t enemyDistance;
    //value of measures
    uint8_t measurements[NR_MEASUREMENTS] =             {0,0,0,0,0,0,0,0,0,0,0,0,0};
    void config();
    //set a state (Search for obstacles or for enemy)
    void setZone( ObstaclaDetectorZone zone);
    uint8_t ultrasonicRead();
    uint8_t getServoPosition();
    void move(uint8_t degree);
    //needed to call from time to time(as often as possible)
    void detect();
    void print();
};


#endif
