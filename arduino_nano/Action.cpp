//
// Created by Baragan on 12/12/2021.
//
#include <Arduino.h>
#include "Actions.h"
ActionClock::ActionClock(unsigned long  durationMs, int8_t speedLeft, int8_t speedRight) {
    this->speedRight = speedRight;
    this->speedLeft = speedLeft;
    this->durationMs = durationMs;
    this->clockStart = millis();
}

bool ActionClock::isDone() {
    return millis() - clockStart >= durationMs;
}
bool ActionClock::isUnfolding()  {
    return !isDone();
}

void ActionClock::print() {
    Serial.print("d = ");
    Serial.print(speedRight);
    Serial.print(" s = ");
    Serial.println(speedLeft);
    Serial.print(clockStart);
    Serial.print(" - ");
    Serial.println(durationMs);
    Serial.println();
}