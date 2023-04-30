#include "auto_dog_feeder.h"

AutoDogFeeder::AutoDogFeeder() {
    _state = INIT;
}


void AutoDogFeeder::init() {
    //pinMode(_pinStep, OUTPUT);
    //pinMode(_pinDir, OUTPUT);
    //pinMode(_pinEn, OUTPUT);
    //_augerStepper.setEnablePin(_pinEn);
    //AccelStepper _augerStepper; 
    //_augerStepper.setMaxSpeed(3000);
    _stepDir = 0;
    _stepSpeed = 3000;
    tone(PIN_TONE, 5000);
    delay(100);
    noTone(PIN_TONE);
    _unJamRetry = 0;
    _feedingTimes[0] = 9;
    _feedingTimes[1] = 21;
    //_scale.begin(PIN_SCALE_DOUT, PIN_SCALE_SCK, 128);
    //_scale.set_scale(2280.f);
    //_scale.tare();

}

void AutoDogFeeder::start_autofeeding(uint8_t hours, uint8_t minutes) {
    _targetWaitTimeHours = hours;
    _targetWaitTimeMinutes = minutes;
    _state = START_FEEDING;
}

void AutoDogFeeder::stop_autofeeding() {
    _state = IDLE;
}

DateTime AutoDogFeeder::get_time() {
    return _currentTime;
}

uint16_t AutoDogFeeder::get_weight() {
    return _currentFoodWeight;
}

void AutoDogFeeder::run() {
    //Serial.print("   | "); 
    //Serial.println(_state); 
    _currentTime = rtc.now();
    
    switch (_state) {
        case INIT:
            // Initialize the Indicator LED pin
            pinMode(PIN_INDICATOR_LED, OUTPUT);
            // Initiallize LED state
            _ledState = 1;
            // Initialize the feeder clock 
            // Initialize Food Servo 
            _positionClosed = 0;
            _positionOpen = 90;
            // Initialize food scale with gain of 128
            
            // Change states
            _state = IDLE;
            //rtc.begin();
            // Set LED timer
            _ledTimerStart = millis();
            _targetFoodWeight = 200; // grams
            _jamTimeoutValue = 7000; // Milliseconds
            break;
        case IDLE:
            // Flashing LED at IDLE state speed
            if(millis() - _ledTimerStart > LED_IDLE_FLASH_SPEED) {
                _ledState = !_ledState; 
                _ledTimerStart = millis();
            }
            Serial.println("in idle");
            break;
        // Reset feeding timer
        case START_FEEDING:
            // Open the food servo
            //_foodServo.write(_positionOpen);
            _jamTimeoutValue = 7000; 
            _feedingTimer = millis();
            _state = FEEDING;
            //_augerStepper.enableOutputs(); 
            //_augerStepper.move(_stepSpeed); 
            _flipFlag = true;
            _flipTimer = millis();
            _flipTime = 50;
            _flipFreq = 9000;
            tone(PIN_TONE, _flipFreq);
            delay(200);
            noTone(PIN_TONE);
            break;
        // Currently filling food bowl 
        case FEEDING:
           _augerStepper.run();
            _currentFoodWeight = _scale.get_units() * 10.0; 
            //_step(1, 30);
            if(millis() - _ledTimerStart > LED_FEED_FLASH_SPEED) {
                _ledState = !_ledState; 
                _ledTimerStart = millis();
            
            }
            // We have reached the desired weight, stop food flow
            if(_currentFoodWeight >= _targetFoodWeight) 
                _state = STOP_FEEDING;
            // If we haven't reached the desired weight by _jamTimeoutValue, then
            // assume that we're jammed 
            if(millis() - _feedingTimer > _jamTimeoutValue) {
                _state = JAMMED;
                _augerStepper.move(-_stepSpeed); 
            }
            break; 
        case STOP_FEEDING:
            // Close the food servo
            //_foodServo.write(_positionClosed);
            _unJamRetry = 0; 
            //noTone(PIN_TONE);
            _state = WAITING_WITH_FOOD;
            //_augerStepper.stop();
            //_augerStepper.setCurrentPosition(0);
            //_augerStepper.disableOutputs(); 
            break;
        case WAITING_WITH_FOOD:
            if(millis() - _ledTimerStart > LED_WAITING_WITH_FOOD_FLASH_SPEED) {
                _ledState = !_ledState; 
                
                _currentFoodWeight = _scale.get_units() * 10.0; 
                _ledTimerStart = millis();
            }
            if(_currentFoodWeight <= 0)
                _state = WAITING_NO_FOOD;

            /* Check time and feed if necessary
            //for(int i = 0; i < sizeof(_feedingTimes) / sizeof(_feedingTimes[0]); i++) { 
                // If the time matches a value in the array and we haven't already fed
                if(_feedingTimes[i] == _currentTime.hour() && _feedingHour != _currentTime.hour()) {
                    _state = START_FEEDING;
                    _feedingHour = _currentTime.hour();
                }
            }
            */
            break;
        case WAITING_NO_FOOD:
            if(millis() - _ledTimerStart > LED_WAITING_NO_FOOD_FLASH_SPEED) {
                _ledState = !_ledState; 
                _ledTimerStart = millis();
                _currentFoodWeight = _scale.get_units() * 10.0; 
            }
            if(_currentFoodWeight > 0)
                _state = WAITING_WITH_FOOD;
            /* 
            // Check times and feed if necessary 
            for(int i = 0; i < sizeof(_feedingTimes) / sizeof(_feedingTimes[0]); i++) { 
                // If the time matches a value in the array and we haven't already fed
                if(_feedingTimes[i] == _currentTime.hour() && _feedingHour != _currentTime.hour()) {
                    _state = START_FEEDING;
                    _feedingHour = _currentTime.hour();
                } 
            */
            break; 
        case KODA_EATING:
            break;
        case JAMMED:
            //_foodServo.write(_positionClosed);
            //noTone(PIN_TONE);
            _jamTimeoutValue = 3000;
            _unJamRetry++;
            if(_unJamRetry < 3) {
                _state = UNJAMMING;
            }
            else
                _state = JAM_TIMEOUT; 
            _jamTimeoutTimer = millis();
            break;
        case UNJAMMING:
            //_step(1, 70); 
            if(millis() - _jamTimeoutTimer > _jamTimeoutValue) {
                _state = START_FEEDING;
            }
            break;
        case JAM_TIMEOUT:
            _unJamRetry = 0;
            //_augerStepper.stop();
            //_augerStepper.setCurrentPosition(0);
            //_augerStepper.disableOutputs(); 
            analogWrite(PIN_INDICATOR_LED, 100);
            break;
        }
    if(_state != JAMMED && _state != UNJAMMING && _state != UNJAMMING)
        digitalWrite(PIN_INDICATOR_LED, _ledState); 
    //_augerStepper.run();
    //Serial.println(_time.minutes);
    //Serial.println(_targetWaitTimeMinutes);
}