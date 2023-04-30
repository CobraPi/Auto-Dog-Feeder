#include <Arduino.h>

#pragma once



void init_ultrasonic(uint8_t pinTrig, uint8_t pinEcho);
double get_ultrasonic_distance();
void run_ultrasonic();