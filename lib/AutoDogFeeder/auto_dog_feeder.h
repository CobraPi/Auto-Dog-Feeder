#include <Arduino.h>
#include <HX711.h>
#include <AccelStepper.h>
#include <RTClib.h>

//#define PIN_FOOD_SERVO 6
#define PIN_SCALE_DOUT 7
#define PIN_SCALE_SCK 6
#define PIN_INDICATOR_LED 13
#define PIN_TONE 12

#define PIN_STEP 8
#define PIN_DIR  9
#define PIN_EN   10


#define LED_IDLE_FLASH_SPEED 1000 // milliseconds
#define LED_FEED_FLASH_SPEED 100 // milliseconds
#define LED_WAITING_WITH_FOOD_FLASH_SPEED 200 // milliseconds 
#define LED_WAITING_NO_FOOD_FLASH_SPEED 600 // 

enum FeederState {
    INIT,   // Inital state that sets the parameters for the feeder 
    IDLE,   // Idle state, not doing anything but keeping time
    /* 
        Active machine states
    */ 
    START_FEEDING,
    FEEDING,
    STOP_FEEDING,
    WAITING_WITH_FOOD,
    WAITING_NO_FOOD,
    KODA_EATING,
    /*
        Jammed states 
    */ 
    JAMMED,
    UNJAMMING,
    JAM_TIMEOUT
};

class AutoDogFeeder {
    public:
        AutoDogFeeder();
        // Supply the stepper pins
        void init(); 
        // This will start the auto feeding routine
        void start_autofeeding(uint8_t hours, uint8_t minutes);
        void stop_autofeeding(); 
        /* This accepts a list of hours (in 24h time) representing the
           corresponding feeding times. This list can be as long as you like  
        */ 
        void set_feeding_hours(uint8_t *hours); 
        /* 
            This will adjust the time (in 24 hour time)
        */
        void adjust_time(uint8_t hours, uint8_t minutes);
        DateTime get_time();
        uint16_t get_weight(); 
        // This function contains most of the logic for controlling the feeder 
        void run();
    protected:
        HX711 _scale;
        AccelStepper _augerStepper;
        RTC_DS3231 rtc;
    private:
        DateTime _currentTime;
        uint8_t _feedingHour;
        uint8_t *_feedingTimes;
        long _stepSpeed; 
        bool _stepDir;
        FeederState _state;
        void _check_feedtime();
        double _currentFoodWeight, _targetFoodWeight;
        uint8_t _positionClosed, _positionOpen;
        uint8_t _targetWaitTimeHours, _targetWaitTimeMinutes;
        bool _ledState; 
        unsigned long _ledTimerStart, _ledTimerFlashTimeMs;
        unsigned long _feedingTimer;
        unsigned long _jamTimeoutTimer, _jamTimeoutValue;
        uint8_t _unJamRetry;
        uint8_t _pinStep, _pinDir, _pinEn;
        bool _flipFlag, _flipping;
        unsigned long _flipTime, _flipTimer, _flipFreq;
};