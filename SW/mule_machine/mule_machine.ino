//#include <SoftPWM.h>
//#include <SoftPWM_timer.h>

/*
 Mule Machine
 @Author: Evan Stoddart 
 Email: estods76@gmail.com Github: estods3
 Constants: Pinout and Calibration
 Setup: Initialize pins and variables, perform a bootup animation
 Loop: Perform Standby animation every few seconds, wait for cup to be placed on machine, make drink
*/

// PINOUT DEFINITION
//Buttons, LEDs, Pumps
//HARDWARE: <MM1.1
//inputs
//const int cup_present_buttonPin = 5; //PD5, physical pin 11                 
//const int manual_override_button_pin = 6; //PD6, physical pin 12
//const int tuner_strength_pin = A4; //PC4, physical pin 27
//const int tuner_sour_pin = A5; //PC5, physical pin 28
////outputs
//const int ledGreen_Lime = 2; // PD2, physical pin 4
//const int ledYellow_GingerBeer = 3; // PD3, physical pin 5
//const int ledOrange_Vodka = 4; // PD4, physical pin 6
//const int pump_Vodka = A1; //PC1,A1, physical pin 24
//const int pump_GingerBeer = A2; //PC2,A2, physical pin 25
//const int pump_Lime =  A3; //PC3,A3, physical pin 26

//TODO HARDWARE: MM1.2, change to use real PWM not SoftPWM Lib
//inputs
const int cup_present_buttonPin = 4; //PD4, physical pin 6                 
const int manual_override_button_pin = 7; //PD7, physical pin 13
const int tuner_strength_pin = A4; //PC4, physical pin 27
const int tuner_sour_pin = A5; //PC5, physical pin 28
//outputs (PWM)
const int ledGreen_Lime = 5; // PD5, physical pin 11
const int ledYellow_GingerBeer = 3; // PD3, physical pin 5
const int ledOrange_Vodka = 6; // PD6, physical pin 12
const int pump_Vodka = 9; //PB1, physical pin 15
const int pump_GingerBeer = 10; //PB2, physical pin 16
const int pump_Lime =  11; //PB3, physical pin 17

//ANIMATION DELAYS
//Bootup, Standby Mode, Mixing Mode
const int bootup_blink_duration = 100;
const int standby_blink_duration = 25;
const int standby_animation_interval = 3000;
const int mixingmode_starting_blink_duration = 100;

//PUMP CURRENT SPIKE DELAY
// prevent pumps drawing too much current from Arduino when turning on all at once.
const int current_spike_delay = 25;

// MIXING MODE CALIBRATION
bool cup_is_present;
bool manual_override;
const int pour_duration_per_cycle = 150*4; //ms.

// Nominal Recipe
// 0.5 Cups per serving GB--> 2/3rd (100, so GB pump runs at 100%) of Drink should be Ginger Beer
// 0.0625 Cups per serving Lime --> Of remaining 1/3 (50 = 1/3 * 100), 1/4 (0.25) should be Lime
// 0.1875 Cups per serving Vodka --> Of remaining 1/3 (50 = 1/3 * 100), 3/4 (0.75) should be Vodka
// -----------------------------------------------------------------------
const int nominal_setting_ginger_beer = 100; //100%
const int nominal_setting_lime = (50) * 0.25; //37.5%
const int nominal_setting_vodka = (50) * 0.75; //12.5%

int temp_analog_reading;
int temp_mapped_analog_reading;
const int numReadings  = 10;
int readings_strength[numReadings];
int readings_sour[numReadings];
int readIndex_sour;
int readIndex_strength;
int calibration_percentage_Lime;
int calibration_percentage_GingerBeer;
int calibration_percentage_Vodka;
int time_passed_ginger_beer_start; //calculation

//TODO numDrinksTilEmpty

// FUNCTION: setup()
void setup() {

  Serial.begin(9600);
  Serial.println(F("SUM,Vodka,Lime,GingerBeer")); 

  // set the input pins
  pinMode(tuner_strength_pin, INPUT);
  pinMode(tuner_sour_pin, INPUT);
  pinMode(cup_present_buttonPin, INPUT_PULLUP);
  pinMode(manual_override_button_pin, INPUT_PULLUP);
  // set the LEDs and Pumps as output
  pinMode(ledOrange_Vodka, OUTPUT);
  pinMode(ledYellow_GingerBeer, OUTPUT);
  pinMode(ledGreen_Lime, OUTPUT);
  //MMv1.2+
  pinMode(pump_Vodka, OUTPUT);
  pinMode(pump_GingerBeer, OUTPUT);
  pinMode(pump_Lime, OUTPUT);
  // Initialize calibration variables and get initial readings
  readings_strength[numReadings] = {};
  readings_sour[numReadings] = {};
  readIndex_sour = 0;
  readIndex_strength = 0;
  update_calibration_from_user_settings();

  // Initialize PWM Interface for LEDs and Pumps
  //MMv1.1
  //SoftPWMBegin();
  //SoftPWMSet(pump_Vodka, 0);
  //SoftPWMSet(pump_GingerBeer, 0);
  //SoftPWMSet(pump_Lime, 0);
  //SoftPWMSet(ledOrange_Vodka, 0);
  //SoftPWMSet(ledYellow_GingerBeer, 0);
  //SoftPWMSet(ledGreen_Lime, 0);
  //SoftPWMSetFadeTime(ALL, 0, 0);

  // Initial Digital Inputs as OFF (false)
  cup_is_present = false;
  manual_override = false;
  
  // Perform an LED Animation to let you know the machine is up and runnning
  animate_LEDs_for_set_time_OYG(' ', ' ', ' ', bootup_blink_duration);
  animate_LEDs_for_set_time_OYG('O', ' ', ' ', bootup_blink_duration);
  animate_LEDs_for_set_time_OYG(' ', 'Y', ' ', bootup_blink_duration);
  animate_LEDs_for_set_time_OYG(' ', ' ', 'G', bootup_blink_duration);
  animate_LEDs_for_set_time_OYG(' ', 'Y', ' ', bootup_blink_duration);
  animate_LEDs_for_set_time_OYG('O', ' ', ' ', bootup_blink_duration);
  animate_LEDs_for_set_time_OYG(' ', 'Y', ' ', bootup_blink_duration);
  animate_LEDs_for_set_time_OYG(' ', ' ', 'G', bootup_blink_duration);
  animate_LEDs_for_set_time_OYG(' ', 'Y', ' ', bootup_blink_duration);
  animate_LEDs_for_set_time_OYG('O', ' ', ' ', bootup_blink_duration);
  animate_LEDs_for_set_time_OYG(' ', 'Y', ' ', bootup_blink_duration);
  animate_LEDs_for_set_time_OYG(' ', ' ', 'G', bootup_blink_duration);
  animate_LEDs_for_set_time_OYG(' ', ' ', ' ', 0);
}

// FUNCTION: loop()
void loop() {
   update_calibration_from_user_settings();
  
  // STANDBY MODE (Heartbeat)
  // Perform a short animation every 3 seconds to let you know machine is alive
  animate_LEDs_for_set_time_OYG(' ', ' ', 'G', standby_blink_duration);
  animate_LEDs_for_set_time_OYG(' ', 'Y', ' ', standby_blink_duration);
  animate_LEDs_for_set_time_OYG('O', ' ', ' ', standby_blink_duration);
  animate_LEDs_for_set_time_OYG(' ', ' ', ' ', standby_animation_interval);

  //TODO if(numDrinksTilEmpty < 3) flash RED LED
  
  // Wait for Copper Cup (Button Press)
  // NOTE LOW=="CUP PRESENT" as the pull up resistor keeps the button value HIGH by default and pushing
  // the button creates a LOW signal.
  // When button pressed: call functions for Vodka, GB, and Lime to pour until button is unpressed.
  cup_is_present = digitalRead(cup_present_buttonPin) == LOW; // CUP_present && numDrinksTilEmpty > 0
  manual_override = digitalRead(manual_override_button_pin) == LOW;
  if (cup_is_present || manual_override) {
       // Initiate Mule Making:
       // show initialization animation
       animate_LEDs_for_set_time_OYG(' ', ' ', ' ', mixingmode_starting_blink_duration);
       animate_LEDs_for_set_time_OYG('O', 'Y', 'G', mixingmode_starting_blink_duration);
       animate_LEDs_for_set_time_OYG(' ', ' ', ' ', mixingmode_starting_blink_duration);
       animate_LEDs_for_set_time_OYG('O', 'Y', 'G', mixingmode_starting_blink_duration);
       animate_LEDs_for_set_time_OYG(' ', ' ', ' ', mixingmode_starting_blink_duration);
       animate_LEDs_for_set_time_OYG('O', 'Y', 'G', mixingmode_starting_blink_duration);
       animate_LEDs_for_set_time_OYG(' ', ' ', ' ', mixingmode_starting_blink_duration);
       animate_LEDs_for_set_time_OYG('O', 'Y', 'G', mixingmode_starting_blink_duration);
       animate_LEDs_for_set_time_OYG(' ', ' ', ' ', 0);
       while(cup_is_present || manual_override){
           
           update_calibration_from_user_settings();
           
           // START MIXING DRINK
           // Start by switching all pumps to "ON". 
           
           // use a current_spike_delay to prevent the current from spiking all at once 
           // and resetting the Arduino.
           
           //MMv1.1
           //SoftPWMSetPercent(ledYellow_GingerBeer, calibration_percentage_GingerBeer);
           //SoftPWMSetPercent(pump_GingerBeer, calibration_percentage_GingerBeer);
           //MMv1.2+
           analogWrite(ledYellow_GingerBeer, int((calibration_percentage_GingerBeer / 100.0) * 255));
           analogWrite(pump_GingerBeer, int((calibration_percentage_GingerBeer / 100.0) * 255));
           
           // PWM Vodka Pump (Speed Controlled by User)
           delay(current_spike_delay);
           //MMv1.1
           //SoftPWMSetPercent(ledOrange_Vodka, calibration_percentage_Vodka);
           //SoftPWMSetPercent(pump_Vodka, calibration_percentage_Vodka);
           //MMv1.2+
           analogWrite(ledOrange_Vodka, int((calibration_percentage_Vodka / 100.0) * 255));
           analogWrite(pump_Vodka, int((calibration_percentage_Vodka / 100.0) * 255));
           
           // PWM Lime Pump (Speed Controlled by User)
           delay(current_spike_delay);
           //MMv1.1
           //SoftPWMSetPercent(ledGreen_Lime, calibration_percentage_Lime);
           //SoftPWMSetPercent(pump_Lime, calibration_percentage_Lime);
           //MMv1.2+
           analogWrite(ledGreen_Lime, int((calibration_percentage_Lime / 100.0) * 255));
           analogWrite(pump_Lime, int((calibration_percentage_Lime / 100.0) * 255));
           
           time_passed_ginger_beer_start = 2*current_spike_delay;
           if((pour_duration_per_cycle - time_passed_ginger_beer_start) > 0){
               delay(pour_duration_per_cycle - time_passed_ginger_beer_start);
           }

           //Read User Input for Mixing Mode
           cup_is_present = digitalRead(cup_present_buttonPin) == LOW;
           manual_override = digitalRead(manual_override_button_pin) == LOW;
       }
       //MMv1.2+
       analogWrite(pump_GingerBeer, int(0));
       analogWrite(pump_Vodka, int(0));
       analogWrite(pump_Lime, int(0));
       //MMv1.1
       //SoftPWMSetPercent(ALL, 0);
  }
}

/// ---------
/// FUNCTIONS
/// ---------
/*
 * FUNCTION: animate_LEDs_for_set_time_OYG 
 * DEF: perform an animation using all three LEDs.
 */
void animate_LEDs_for_set_time_OYG(char O_LED, char Y_LED, char G_LED, int delay_time) {
  if(O_LED == 'O'){ digitalWrite(ledOrange_Vodka, HIGH); } else{ digitalWrite(ledOrange_Vodka, LOW); }
  if(Y_LED == 'Y'){ digitalWrite(ledYellow_GingerBeer, HIGH); } else{ digitalWrite(ledYellow_GingerBeer, LOW); }
  if(G_LED == 'G'){ digitalWrite(ledGreen_Lime, HIGH); } else{ digitalWrite(ledGreen_Lime, LOW); }
  delay(delay_time);
}

/*
 * FUNCTION: update_calibration_from_user_settings 
 * DEF: update the calibration percentages from the users settings using potentiometers.
 */
void update_calibration_from_user_settings(){
  // Update Recipe from Calibration
  // (READ numReadings numer of samples from pots)
  for(int i=1; i< numReadings; i++){
    calibration_percentage_Vodka = smooth_strength();
    calibration_percentage_Lime = smooth_sour();
  }
  calibration_percentage_GingerBeer = 150 - calibration_percentage_Vodka - calibration_percentage_Lime;
  
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

/*
 * FUNCTION: smooth_sour 
 * DEF: performs moving average on sour potentiometer.
 */
//NOTE: output is flipped max-to-low to match 5V and GND placement on schematic (5V should be on left pin when facing POT)
int smooth_sour() { /* function smooth */
  // read the sensor into the next position in the array
  temp_analog_reading = analogRead(tuner_sour_pin);
  temp_mapped_analog_reading = map(temp_analog_reading, 0, 1023, 2*nominal_setting_lime, 0);
  readings_sour[readIndex_sour] = temp_mapped_analog_reading;
  readIndex_sour = readIndex_sour + 1;
  if (readIndex_sour >= numReadings) {
    readIndex_sour = 0;
  }
  
  // Compute Total of all Readings
  int total = 0;
  for(int i=0; i < numReadings; i++){
    total = total + readings_sour[i];
  }
  // Calculate the Average
  long average = total / numReadings;
  return average;
}

/*
 * FUNCTION: smooth_strength
 * DEF: performs moving average on strength potentiometer.
 */
//NOTE: output is flipped max-to-low to match 5V and GND placement on schematic (5V should be on left pin when facing POT)
int smooth_strength() { /* function smooth */
  // read the sensor into the next position in the array
  temp_analog_reading = analogRead(tuner_strength_pin);
  temp_mapped_analog_reading = map(temp_analog_reading, 0, 1023, 2*nominal_setting_vodka, 0);
  readings_strength[readIndex_strength] = temp_mapped_analog_reading;
  readIndex_strength = readIndex_strength + 1;
  if (readIndex_strength >= numReadings) {
    readIndex_strength = 0;
  }
  
  // Compute Total of all Readings
  int total = 0;
  for(int i=0; i < numReadings; i++){
    total = total + readings_strength[i];
  }
  // Calculate the Average
  int average = total / numReadings;
  return average;
}
