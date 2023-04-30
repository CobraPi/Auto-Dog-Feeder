#include "ultrasonic.h"

uint8_t _measureFlag;
double _distance; 
unsigned long _measureTimer, _sampleTimer;
uint8_t _pinTrig, _pinEcho;

void init_ultrasonic(uint8_t pinTrig, uint8_t pinEcho) {
    _pinTrig = pinTrig;
    _pinEcho = pinEcho; 
    pinMode(_pinTrig, OUTPUT);
    pinMode(_pinEcho, INPUT);
    _sampleTimer = millis();
}

double get_ultrasonic_distance() {
    return _distance;
}

void measure_distance() {
    if(_measureFlag == 0) {
        attachInterrupt(digitalPinToInterrupt(_pinEcho), measure_distance, FALLING);
        _measureFlag = 1;
        _measureTimer = micros();
    }
    else if(_measureFlag == 1) {
        _distance = (micros() - _measureTimer) * 0.017;
        _measureFlag = 2;
    }
}

void run_ultrasonic() {
    if(millis() - _sampleTimer > 10) {
        _measureFlag = 0; 
        attachInterrupt(digitalPinToInterrupt(_pinEcho), measure_distance, RISING);
        unsigned long waitTime = micros(); 
        digitalWrite(_pinTrig, LOW); 
        while(micros() - waitTime < 2);
        digitalWrite(_pinTrig, HIGH);
        waitTime = micros();
        while(micros() - waitTime < 10);
        digitalWrite(_pinTrig, LOW);
        _sampleTimer = millis();
    }
}

