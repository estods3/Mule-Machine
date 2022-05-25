#include <SoftPWM.h>
#include <SoftPWM_timer.h>

/*
 Mule Machine
 @Author: Evan Stoddart 
 Email: estods76@gmail.com Github: estods3
 Constants: Pinout and Calibration
 Setup: initialize pins, perform a bootup animation
 Loop: Wait for cup to be placed on machine. check conditions + make drink.
*/

// PINOUT DEFINITION
//Buttons, LEDs, Pumps
//inputs
bool cup_is_present = false;
bool manual_override = false;
const int cup_present_buttonPin = 5; //PD5, physical pin 11                 
const int manual_override_button_pin = 6; //PD6, physical pin 12
const int tuner_strength_pin = A4; //PC4, physical pin 27
const int tuner_sour_pin = A5; //PC5, physical pin 28
//outputs
const int ledGreen_Lime = 2; // PD2, physical pin 4
const int ledYellow_GingerBeer = 3; // PD3, physical pin 5
const int ledOrange_Vodka = 4; // PD4, physical pin 6
const int pump_Vodka = A1; //PC1,A1, physical pin 24
const int pump_GingerBeer = A2; //PC2,A2, physical pin 25
const int pump_Lime =  A3; //PC3,A3, physical pin 26

//ANIMATION DELAYS
//Bootup, Standby Mode, Mixing Mode
const int bootup_blink_duration = 100;
const int standby_blink_duration = 25;
const int standby_animation_interval = 3000;
const int drink_starting_blink_duration = 100;

//PUMP CURRENT SPIKE DELAY
// prevent pumps drawing too much current from Arduino when turning on all at once.
const int current_spike_delay = 25;

// MIXING MODE CALIBRATION
const int pour_duration_per_cycle = 150; //ms.

// Nominal Recipe
// 0.5 Cups per serving GB--> 2/3rd of Drink should be Ginger Beer
// 0.0625 Cups per serving Lime --> Of remaining 1/3, 1/4 should be Lime
// 0.1875 Cups per serving Vodka --> Of remaining 1/3, 3/4 should be Vodka
// -----------------------------------------------------------------------
const int nominal_setting_ginger_beer = 0.666 * pour_duration_per_cycle;
const int nominal_setting_lime = (0.333 * pour_duration_per_cycle) * 0.25;
const int nominal_setting_vodka = (0.333 * pour_duration_per_cycle) * 0.75;
//100
//37.5
//12.5
//needs to add to 150

int value; //save analog value
int time_passed_ginger_beer_start; //calculation
const int numReadings  = 10;
int readings_strength[numReadings];
int readings_sour[numReadings];
int readIndex_sour  = 0;
long total_sour = 0;
int readIndex_strength = 0;
long total_strength = 0;
double calibration_percentage_Lime;
double calibration_percentage_GingerBeer;
double calibration_percentage_Vodka;

//TODO numDrinksTilEmpty

// FUNCTION: setup()
void setup() {

  Serial.begin(9600);
  Serial.println(F("SUM,Vodka,Lime,GingerBeer")); 

  update_calibration_from_user_settings();

  // set the input pins
  pinMode(tuner_strength_pin, INPUT);
  pinMode(tuner_sour_pin, INPUT);
  pinMode(cup_present_buttonPin, INPUT_PULLUP);
  pinMode(manual_override_button_pin, INPUT_PULLUP);
  // set the LEDs and Pumps as output
  pinMode(ledOrange_Vodka, OUTPUT);
  pinMode(ledYellow_GingerBeer, OUTPUT);
  pinMode(ledGreen_Lime, OUTPUT);

  // Initialize PWM Interface for LEDs and Pumps
  SoftPWMBegin();
  SoftPWMSet(pump_Vodka, 0);
  SoftPWMSet(pump_GingerBeer, 0);
  SoftPWMSet(pump_Lime, 0);
  SoftPWMSet(ledOrange_Vodka, 0);
  SoftPWMSet(ledYellow_GingerBeer, 0);
  SoftPWMSet(ledGreen_Lime, 0);
  SoftPWMSetFadeTime(ALL, 100, 100);
  
  // Perform an LED animation to let you know the machine is up and runnning
  delay(bootup_blink_duration);
  digitalWrite(ledOrange_Vodka, HIGH);
  delay(bootup_blink_duration);
  digitalWrite(ledOrange_Vodka, LOW);
  digitalWrite(ledYellow_GingerBeer, HIGH);
  delay(bootup_blink_duration);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledGreen_Lime, HIGH);
  delay(bootup_blink_duration);
  digitalWrite(ledGreen_Lime, LOW);
  digitalWrite(ledYellow_GingerBeer, HIGH);
  delay(bootup_blink_duration);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledOrange_Vodka, HIGH);
  delay(bootup_blink_duration);
  digitalWrite(ledOrange_Vodka, LOW);
  digitalWrite(ledYellow_GingerBeer, HIGH);
  delay(bootup_blink_duration);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledGreen_Lime, HIGH);
  delay(bootup_blink_duration);
  digitalWrite(ledGreen_Lime, LOW);
  digitalWrite(ledYellow_GingerBeer, HIGH);
  delay(bootup_blink_duration);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledOrange_Vodka, HIGH);
  delay(bootup_blink_duration);
  digitalWrite(ledOrange_Vodka, LOW);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledGreen_Lime, LOW);

  cup_is_present = false;
  manual_override = false;
}

// FUNCTION: loop()
void loop() {
   update_calibration_from_user_settings();
  
  // STANDBY MODE
  // Perform a short animation every 3 seconds to let you know machine is alive
  digitalWrite(ledGreen_Lime, HIGH);
  delay(standby_blink_duration);
  digitalWrite(ledGreen_Lime, LOW);
  digitalWrite(ledYellow_GingerBeer, HIGH);
  delay(standby_blink_duration);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledOrange_Vodka, HIGH);
  delay(standby_blink_duration);
  digitalWrite(ledOrange_Vodka, LOW);
  delay(standby_animation_interval);
  digitalWrite(ledGreen_Lime, LOW);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledOrange_Vodka, LOW);

  //TODO if(numDrinksTilEmpty < 3) flash RED LED
  
  // Wait for Copper Cup (Button Press)
  // NOTE LOW=="CUP PRESENT" as the pull up resistor keeps the button value HIGH by default and pushing
  // the button creates a LOW signal.
  // When button pressed: call functions for Vodka, GB, and Lime to pour until button is depressed.
  cup_is_present = digitalRead(cup_present_buttonPin) == LOW; // CUP_present && numDrinksTilEmpty > 0
  manual_override = digitalRead(manual_override_button_pin) == LOW;
  if (cup_is_present || manual_override) {
       // Initiate Mule Making:
       // show initialization animation
       digitalWrite(ledOrange_Vodka, LOW);
       digitalWrite(ledYellow_GingerBeer, LOW);
       digitalWrite(ledGreen_Lime, LOW);
       delay(drink_starting_blink_duration);
       digitalWrite(ledOrange_Vodka, HIGH);
       digitalWrite(ledYellow_GingerBeer, HIGH);
       digitalWrite(ledGreen_Lime, HIGH);
       delay(drink_starting_blink_duration);
       digitalWrite(ledOrange_Vodka, LOW);
       digitalWrite(ledYellow_GingerBeer, LOW);
       digitalWrite(ledGreen_Lime, LOW);
       delay(drink_starting_blink_duration);
       digitalWrite(ledOrange_Vodka, HIGH);
       digitalWrite(ledYellow_GingerBeer, HIGH);
       digitalWrite(ledGreen_Lime, HIGH);
       delay(drink_starting_blink_duration);
       digitalWrite(ledOrange_Vodka, LOW);
       digitalWrite(ledYellow_GingerBeer, LOW);
       digitalWrite(ledGreen_Lime, LOW);
       delay(drink_starting_blink_duration);
       digitalWrite(ledOrange_Vodka, HIGH);
       digitalWrite(ledYellow_GingerBeer, HIGH);
       digitalWrite(ledGreen_Lime, HIGH);
       delay(drink_starting_blink_duration);
       digitalWrite(ledOrange_Vodka, LOW);
       digitalWrite(ledYellow_GingerBeer, LOW);
       digitalWrite(ledGreen_Lime, LOW);
       while(cup_is_present || manual_override){
           
           update_calibration_from_user_settings();
           
           // START MIXING DRINK
           // Start by switching all pumps to "ON". 
           // use a current_spike_delay to prevent the current from spiking all at once 
           // and resetting the Arduino.
           SoftPWMSetPercent(ledYellow_GingerBeer, calibration_percentage_GingerBeer);
           SoftPWMSetPercent(pump_GingerBeer, calibration_percentage_GingerBeer);
           // PWM Vodka Pump (Speed Controlled by User)
           delay(current_spike_delay);
           //digitalWrite(ledOrange_Vodka, HIGH);
           SoftPWMSetPercent(ledOrange_Vodka, calibration_percentage_Vodka);
           SoftPWMSetPercent(pump_Vodka, calibration_percentage_Vodka);
           // PWM Lime Pump (Speed Controlled by User)
           delay(current_spike_delay);           
           //digitalWrite(ledGreen_Lime, HIGH);
           SoftPWMSetPercent(ledGreen_Lime, calibration_percentage_Lime);
           SoftPWMSetPercent(pump_Lime, calibration_percentage_Lime);
       
           time_passed_ginger_beer_start = 2*current_spike_delay;
           if((pour_duration_per_cycle - time_passed_ginger_beer_start) > 0){
               delay(pour_duration_per_cycle - time_passed_ginger_beer_start);
           }

           //Read User Input for Mixing Mode
           cup_is_present = digitalRead(cup_present_buttonPin) == LOW;
           manual_override = digitalRead(manual_override_button_pin) == LOW;
       }
       SoftPWMSetPercent(ALL, 0);
  }
}

void update_calibration_from_user_settings(){
  // Update Recipe from Calibration
  // (READ numReadings numer of samples from pots)
  for(int i=1; i< numReadings; i++){
    calibration_percentage_Vodka = smooth_strength();
    calibration_percentage_Lime = smooth_sour();
  }
  calibration_percentage_GingerBeer = pour_duration_per_cycle - calibration_percentage_Vodka - calibration_percentage_Lime;
  
  //Error Checking
  if(calibration_percentage_GingerBeer > 100){
     calibration_percentage_GingerBeer = 100;
  }
  if(calibration_percentage_Vodka > 100){
     calibration_percentage_Vodka = 100;
  }
  if(calibration_percentage_Lime > 100){
     calibration_percentage_Lime = 100;
  }

  // DEBUG INTERFACE (Using Arduino Serial Monitor)
  Serial.print(calibration_percentage_GingerBeer + calibration_percentage_Lime + calibration_percentage_Vodka);
  Serial.print(',');
  Serial.print(calibration_percentage_Vodka);
  Serial.print(',');
  Serial.print(calibration_percentage_Lime);
  Serial.print(',');
  Serial.println(calibration_percentage_GingerBeer);
}

//NOTE: output is flipped max-to-low to match 5V and GND placement on schematic (5V should be on left pin when facing POT)
long smooth_sour() { /* function smooth */
  ////Perform average on sensor readings
  long average;
  // subtract the last reading:
  total_sour = total_sour - readings_sour[readIndex_sour];
  // read the sensor:
  value = analogRead(tuner_sour_pin);
  value = map(value, 0, 1023, 2*nominal_setting_lime, 0);
  readings_sour[readIndex_sour] = value;
  // add value to total:
  total_sour = total_sour + readings_sour[readIndex_sour];
  // handle index
  readIndex_sour = readIndex_sour + 1;
  if (readIndex_sour >= numReadings) {
    readIndex_sour = 0;
  }
  // calculate the average:
  average = total_sour / numReadings;

  return average;
}

//NOTE: output is flipped max-to-low to match 5V and GND placement on schematic (5V should be on left pin when facing POT)
long smooth_strength() { /* function smooth */
  ////Perform average on sensor readings
  long average;
  // subtract the last reading:
  total_strength = total_strength - readings_strength[readIndex_strength];
  // read the sensor:
  value = analogRead(tuner_strength_pin);
  value = map(value, 0, 1023, 2*nominal_setting_vodka, 0);
  readings_strength[readIndex_strength] = value;
  // add value to total:
  total_strength = total_strength + readings_strength[readIndex_strength];
  // handle index
  readIndex_strength = readIndex_strength + 1;
  if (readIndex_strength >= numReadings) {
    readIndex_strength = 0;
  }
  // calculate the average:
  average = total_strength / numReadings;

  return average;
}
