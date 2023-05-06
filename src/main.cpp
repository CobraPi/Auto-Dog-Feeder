#include <Arduino.h>
#include <HX711.h>
#include <RTClib.h>
#include <AccelStepper.h>
#include <SPI.h>

enum FeederState {
  IDLE,
  START_FEEDING,
  FEEDING,
  STOP_FEEDING,
};

FeederState feederState;

#define PIN_INDICATOR_LED 13
unsigned long startTimer;
/*
  Functions for feed tone
*/
#define PIN_TONE 12
uint16_t sweepFreq = 5000;
void play_feed_tone2() {
  for(int i = 0; i < sweepFreq; i++)  
    tone(PIN_TONE, i, 1);
  for(int i = sweepFreq; i >0; i--)  
    tone(PIN_TONE, i, 1);
  noTone(PIN_TONE);
}

void play_feed_tone() {
  for(int i = sweepFreq; i >0; i--)  
    tone(PIN_TONE, i, 1);
  for(int i = 0; i < sweepFreq; i++)  
    tone(PIN_TONE, i, 1);
  noTone(PIN_TONE);
}
/*
  Functions for setting up and moving stepper
*/
#define PIN_STEPPER_EN0 8
#define PIN_STEPPER_EN1 9
AccelStepper stepper; // pins 2,3,4,5
long feedSpeed = 300;
long acceleration = 70;
long distance = -35000;
long feedCount;

void init_stepper() {
  pinMode(PIN_STEPPER_EN0, OUTPUT);
  pinMode(PIN_STEPPER_EN1, OUTPUT); 
  stepper.setAcceleration(acceleration);
  stepper.setMaxSpeed(feedSpeed);
}

void enable_stepper() {
  digitalWrite(PIN_STEPPER_EN0, HIGH);
  digitalWrite(PIN_STEPPER_EN1, HIGH);
}

void disable_stepper() {
  digitalWrite(PIN_STEPPER_EN0, LOW);
  digitalWrite(PIN_STEPPER_EN1, LOW);
}

void test_stepper() {
  enable_stepper();
  if(stepper.distanceToGo() == 0) {
    distance *= -1;
    stepper.move(distance);
  } 
  stepper.run();
}

/*
    Functions for setting up and reading the scale 
*/
#define TARGET_FOOD_WEIGHT 700
#define PIN_SCALE_DOUT 6
#define PIN_SCALE_SCK 7
HX711 scale;
int16_t currentFoodWeight, targetFoodWeight;
void init_scale() {
  scale.begin(PIN_SCALE_DOUT, PIN_SCALE_SCK);
  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();				        // reset the scale to 0
}

double read_scale() {
  while(!scale.is_ready()) stepper.run();
  double grams = scale.get_units() * 10.0; 
  return grams;
}


/*
  Functions for real time clock
*/
RTC_DS3231 rtc;
DateTime currentTime;
uint8_t alreadyFed;
uint8_t feedingHours[2];
uint8_t feedingHour0;
uint8_t feedingHour1;
uint8_t feedingMins;
uint8_t feedingSecs;
void init_rtc() {
  rtc.begin();
}

void autofeed() {
  currentTime = rtc.now(); 
  switch(feederState) {
    case IDLE:
      // check time to see if we should start feeding
        for(int i = 0; i < sizeof(feedingHours) / sizeof(feedingHours[0]); i++) { 
          if(feedingHours[i] == (uint8_t) currentTime.hour() && alreadyFed != (uint8_t) currentTime.hour()) {
            feederState = START_FEEDING;
            alreadyFed = currentTime.hour();
          }
        }
      noTone(PIN_TONE);
      break;
    case START_FEEDING:
      feedCount++; 
      // start moving auger motor
      play_feed_tone();
      enable_stepper();
      stepper.move(distance);
      feederState = FEEDING;
    case FEEDING:
      currentFoodWeight = read_scale();
      // monitor food weight and stop when we reach the desired weight
      if(currentFoodWeight > targetFoodWeight || stepper.distanceToGo() == 0)
        feederState = STOP_FEEDING; 
    break;
    case STOP_FEEDING:
      stepper.stop();
      stepper.setCurrentPosition(0);
      disable_stepper();
      play_feed_tone2();
      feederState = IDLE;
      // stop the auger since we've reached our target weight
      break;
  }
  stepper.run();
}

void setup() {
  Serial.begin(115200);
  init_scale();  
  init_rtc(); 
  init_stepper();
  feedCount = 0; 
  feederState = START_FEEDING; 
  alreadyFed = 100;
  feedingHours[0] = 9;
  feedingHours[1] = 21;
  targetFoodWeight = TARGET_FOOD_WEIGHT;
}

void loop() { 
  if(Serial.available()) {
    char c = Serial.read();
      if(c == 'a') {
        feederState = START_FEEDING;
      } 
      else if(c == 's') {
        feederState = STOP_FEEDING;
      } 
      // set the day feed time
      else if(c == 'd') {
        int hour0 = Serial.parseInt();
        feedingHours[0] = hour0;
        Serial.print("Set day feed time to "); Serial.println(feedingHours[0]); 
      } 
      // set the night feed time
      else if(c ==  'n') {
        int hour1 = Serial.parseInt();
        feedingHours[1] = hour1;
        Serial.print("Set night feed time to "); Serial.println(feedingHours[1]); 
      } 
      // set the target food weight 
      else if(c == 'w') {
        int weight = Serial.parseInt();
        targetFoodWeight = weight;
        Serial.print("Set target food weight to "); Serial.print(targetFoodWeight); Serial.println(" grams");
      } 
      // print current scale weight 
      else if(c ==  'r') {
        int curWeight = read_scale();
        Serial.print("Current scale weight: "); Serial.println(curWeight);
      } 
      else if(c ==  't') {
        scale.tare();
        Serial.println("Taring scale");
      }
    }
  autofeed();
  //test_stepper(); 
}
