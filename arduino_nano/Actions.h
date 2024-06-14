//
// Created by Baragan on 12/12/2021.
//

#ifndef CORSAIR_ACTIONS_H
#define CORSAIR_ACTIONS_H

#include <stdint.h>

enum Action {
    NONE_ACTION,MOVING_FORWARD,MOVING_BACKWARD,MOVING_SIDE,STOP_ACTION};

//handle an Action (duration timne start , time end)
class ActionClock{
private:
    unsigned long durationMs;
    unsigned long clockStart;
public:
    int8_t speedLeft;
    int8_t speedRight;
    ActionClock(unsigned long durationMs,int8_t speedLeft, int8_t speedRight);
    ActionClock(){};
    bool isDone();
    bool isUnfolding();
    void print();
};

#endif //CORSAIR_ACTIONS_H
