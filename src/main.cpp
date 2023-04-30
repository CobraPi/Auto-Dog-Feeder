#include <Arduino.h>
//#include <mytime.h>
/**
 *
 * HX711 library for Arduino - example file
 * https://github.com/bogde/HX711
 *
 * MIT License
 * (c) 2018 Bogdan Necula
 *
**/
//#include <HX711.h>
#include <auto_dog_feeder.h>
#include <AccelStepper.h>
#include <SPI.h>
// HX711 circuit wiring
//const int LOADCELL_DOUT_PIN = 3;
//const int LOADCELL_SCK_PIN = 2;
#define STEP 3
#define DIR 6
#define EN 8
AccelStepper stepper(AccelStepper::DRIVER, STEP, DIR);
uint8_t testTicker = 0;
//MyTime mytime;
long startTime;
//HX711 scale;
AutoDogFeeder feeder;

void setup() {
  Serial.begin(115200);
  //Serial.println("HX711 Demo");
  
  //Serial.println("Serial started");
  //init_ultrasonic(2, 3);
  feeder.init(); 
  
  //stepper.setEnablePin(10);
  //stepper.setPinsInverted(false, false, true);
  //stepper.setSpeed(1000);
  //stepper.enableOutputs();
  //Serial.println("Initializing the scale");
  //mytime.set_time(0, 0);
  // Initialize library with data output pin, clock input pin and gain factor.
  // Channel selection is made by passing the appropriate gain:
  // - With a gain factor of 64 or 128, channel A is selected
  // - With a gain factor of 32, channel B is selected
  // By omitting the gain factor parameter, the library
  // default "128" (Channel A) is used here.
  //scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, 128);

 /* 
  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());			// print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));  	// print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);	// print the average of 5 readings from the ADC minus tare weight (not set) divided
						// by the SCALE parameter (not set yet)

  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();				        // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
						// by the SCALE parameter set with set_scale

  Serial.println("Readings:");
  */
 startTime = millis();
}

uint8_t stepDelay = 20;
long timer;
void loop() {
 //Serial.println(millis() - timer);
  //timer = millis();
  if(Serial.available()) {
    char c = Serial.read();
    switch(c) {
      case 's':
        feeder.start_autofeeding(0, 1);
        Serial.println("Autofeeding start");
        break;
      case 't':
        feeder.stop_autofeeding();
        Serial.println("Autofeeding stop");
        break;
      case 'd':
       DateTime now = feeder.get_time();
        Serial.println(now.hour());
        //Serial.println(get_ultrasonic_distance());
        break;
      case 'w':
        uint16_t weight = feeder.get_weight();
        Serial.println(weight);  
    }
  }
  
  //stepper.run(); 
  
  feeder.run();
  /* 
  for(int i=0; i< 1024; i++) { 
    digitalWrite(6, LOW);
    delayMicroseconds(stepDelay);
    digitalWrite(6, HIGH);
    delayMicroseconds(stepDelay);
  }
  */
  //run_ultrasonic(); 
  //Clock time;   
  //mytime.get_time(time);
  //mytime.tick();
  //double grams = scale.get_units() * 10.0; 
  //Serial.print("one reading:\t"); Serial.print(grams);
  //if (Serial.available()) { 
  //char c = Serial.read();  
  //if(millis() - startTime > 1000) { 
  //Serial.print("Years: "); Serial.print(time.years);
  //Serial.print(" | Months: "); Serial.print(time.months);  
  //Serial.print(" | Weeks: "); Serial.print(time.weeks);  
  //Serial.print(" | Days: "); Serial.print(time.days);  
  //Serial.print(" | Hours: "); Serial.print(time.hours);  
  //Serial.print(" | Minutes: "); Serial.print(time.minutes);  
  //Serial.print(" | Seconds: "); Serial.print(time.seconds);  
  //Serial.println();
  //startTime = millis(); 
  //}
  //Serial.print("\t| average:\t");
  //Serial.println(scale.get_units(10), 1);

  //scale.power_down();			        // put the ADC in sleep mode
  //delay(1000);
  //scale.power_up();
}
