/*
 Mule Machine

 Constants: Pinout and Calibration
 Setup: initialize pins, perform a bootup animation
 Loop: Wait for lever to be pulled. check conditions + make drink
*/

// PINOUT DEFINITION
const int ledOrange_Vodka = 4;
const int ledYellow_GingerBeer = 5;
const int ledGreen_Lime =  6;

const int pump_Vodka = 9;
const int pump_GingerBeer = 10;
const int pump_Lime =  11;

bool lever_is_pulled = false;
const int lever_buttonPin = 2;

bool cupSensor_cupPresent = false;
const int cupSensor_buttonPin = 3;

//TODO numDrinksTilEmpty

// MULE CALIBRATION
const int calibration_time_pour_duration = 1000; //1 second pour at a time
const double calibration_ratio_Lime = 0.1; // 0.0625 Cups per serving --> 0.5/0.75 % of calibration_time_pour_duration spent pouring lime juice
const double calibration_ratio_GingerBeer = 0.7; // 0.5 Cups per serving --> 0.5/0.75 % of calibration_time_pour_duration spent pouring ginger beer
const double calibration_ratio_Vodka = 0.2; // 0.1875 Cups per serving % of calibration_time_pour_duration spent pouring vodka

// FUNCTION: setup()
void setup() {
  // Setup the pins as input/output
  pinMode(lever_buttonPin, INPUT_PULLUP);
  pinMode(cupSensor_buttonPin, INPUT_PULLUP);
  // set the LEDs and Pumps as output
  pinMode(ledOrange_Vodka, OUTPUT);
  pinMode(ledYellow_GingerBeer, OUTPUT);
  pinMode(ledGreen_Lime, OUTPUT);
  digitalWrite(ledOrange_Vodka, LOW);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledGreen_Lime, LOW);
  pinMode(pump_Vodka, OUTPUT);
  pinMode(pump_GingerBeer, OUTPUT);
  pinMode(pump_Lime, OUTPUT);
  digitalWrite(pump_Vodka, LOW);
  digitalWrite(pump_GingerBeer, LOW);
  digitalWrite(pump_Lime, LOW);
  
  // Perform an LED animation to let you know the machine is up and runnning
  delay(1000);
  digitalWrite(ledOrange_Vodka, HIGH);
  delay(100);
  digitalWrite(ledOrange_Vodka, LOW);
  digitalWrite(ledYellow_GingerBeer, HIGH);
  delay(100);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledGreen_Lime, HIGH);
  delay(100);
  digitalWrite(ledGreen_Lime, LOW);
  digitalWrite(ledYellow_GingerBeer, HIGH);
  delay(100);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledOrange_Vodka, HIGH);
  delay(100);
  digitalWrite(ledOrange_Vodka, LOW);
  digitalWrite(ledYellow_GingerBeer, HIGH);
  delay(100);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledGreen_Lime, HIGH);
  delay(100);
  digitalWrite(ledGreen_Lime, LOW);
  digitalWrite(ledYellow_GingerBeer, HIGH);
  delay(100);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledOrange_Vodka, HIGH);
  delay(100);
  digitalWrite(ledOrange_Vodka, LOW);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledGreen_Lime, LOW);

  lever_is_pulled = false;
  cupSensor_cupPresent = false;
}


// FUNCTION: loop()
void loop() {
  // Perform a short animation every 3 seconds to let you know machine is alive
  digitalWrite(ledOrange_Vodka, HIGH);
  delay(25);
  digitalWrite(ledOrange_Vodka, LOW);
  digitalWrite(ledYellow_GingerBeer, HIGH);
  delay(25);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledGreen_Lime, HIGH);
  delay(25);
  digitalWrite(ledGreen_Lime, LOW);
  delay(3000);
  digitalWrite(ledOrange_Vodka, LOW);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledGreen_Lime, LOW);

  //TODO if(numDrinksTilEmpty < 3) flash RED LED
  
  // Wait for Mule Lever (Button) Press
  // NOTE LOW=="Pulled" as the pull up resistor keeps the button value HIGH by default and pushing
  // the button creates a LOW signal.
  // When button pressed: call functions for Vodka, GB, and Lime to pour until button is depressed.
  lever_is_pulled = digitalRead(lever_buttonPin) == LOW; //&& CUP_present && numDrinksTilEmpty > 0
  if (lever_is_pulled) {
       // Initiate Mule Making:
       // show initialization animation
       digitalWrite(ledOrange_Vodka, LOW);
       digitalWrite(ledYellow_GingerBeer, LOW);
       digitalWrite(ledGreen_Lime, LOW);
       delay(100);
       digitalWrite(ledOrange_Vodka, HIGH);
       digitalWrite(ledYellow_GingerBeer, HIGH);
       digitalWrite(ledGreen_Lime, HIGH);
       delay(100);
       digitalWrite(ledOrange_Vodka, LOW);
       digitalWrite(ledYellow_GingerBeer, LOW);
       digitalWrite(ledGreen_Lime, LOW);
       delay(100);
       digitalWrite(ledOrange_Vodka, HIGH);
       digitalWrite(ledYellow_GingerBeer, HIGH);
       digitalWrite(ledGreen_Lime, HIGH);
       delay(100);
       digitalWrite(ledOrange_Vodka, LOW);
       digitalWrite(ledYellow_GingerBeer, LOW);
       digitalWrite(ledGreen_Lime, LOW);
       delay(100);
       digitalWrite(ledOrange_Vodka, HIGH);
       digitalWrite(ledYellow_GingerBeer, HIGH);
       digitalWrite(ledGreen_Lime, HIGH);
       delay(100);
       digitalWrite(ledOrange_Vodka, LOW);
       digitalWrite(ledYellow_GingerBeer, LOW);
       digitalWrite(ledGreen_Lime, LOW);
       while(lever_is_pulled){
           // MIX DRINK
           digitalWrite(ledYellow_GingerBeer, HIGH);
           digitalWrite(pump_GingerBeer, HIGH);
           delay(10);
           digitalWrite(ledOrange_Vodka, HIGH);
           digitalWrite(pump_Vodka, HIGH);
           delay(10);           
           digitalWrite(ledGreen_Lime, HIGH);
           digitalWrite(pump_Lime, HIGH);

           delay((calibration_ratio_Lime * calibration_time_pour_duration));
           digitalWrite(pump_Lime, LOW);
           digitalWrite(ledGreen_Lime, LOW);

           delay((calibration_ratio_Vodka * calibration_time_pour_duration) - (calibration_ratio_Lime * calibration_time_pour_duration) - 10);
           digitalWrite(pump_Vodka, LOW);
           digitalWrite(ledOrange_Vodka, LOW);

           delay((calibration_ratio_GingerBeer * calibration_time_pour_duration) - (calibration_ratio_Vodka * calibration_time_pour_duration) - (calibration_ratio_Lime * calibration_time_pour_duration) - 20);
           digitalWrite(pump_GingerBeer, LOW);
           digitalWrite(ledYellow_GingerBeer, LOW);
                     
           lever_is_pulled = digitalRead(lever_buttonPin) == LOW;
       }
       digitalWrite(ledOrange_Vodka, LOW);
       digitalWrite(ledYellow_GingerBeer, LOW);
       digitalWrite(ledGreen_Lime, LOW);
  } 
  //else if (pressed && No Cup present) {
  // TODO: SHOW an ERROR (RED LED) next to Cup spot
  //}
}
