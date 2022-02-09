/*
 Mule Machine
 @Author: Evan Stoddart 
 Email: estods76@gmail.com Github: estods3
 Constants: Pinout and Calibration
 Setup: initialize pins, perform a bootup animation
 Loop: Wait for cup to be placed on machine. check conditions + make drink.
*/

// PINOUT DEFINITION
//inputs
bool cup_is_present = false;
const int cup_present_buttonPin = 5; //PD5, physical pin 11
const int reset_button_pin = 6; //PD6, physical pin 12
const int tuner_strength_pin = A4; //PC4, physical pin 27
const int tuner_sour_pin = A5; //PC5, physical pin 28
//outputs
const int ledOrange_Vodka = 2; // PD2, physical pin 4
const int ledYellow_GingerBeer = 3; // PD3, physical pin 5
const int ledGreen_Lime =  4; // PD4, physical pin 6
const int pump_Vodka = A1; //PC1,A1, physical pin 24
const int pump_GingerBeer = A2; //PC2,A2, physical pin 25
const int pump_Lime =  A3; //PC3,A3, physical pin 26

//ANIMATION DELAYS
const int bootup_blink_duration = 100;
const int standby_blink_duration = 15;
const int standby_animation_interval = 3000;
const int drink_starting_blink_duration = 100;

//PUMP CURRENT SPIKE DELAY
// prevent pumps drawing too much current from Arduino when turning on all at once.
const int current_spike_delay = 10;



//TODO numDrinksTilEmpty

// MULE CALIBRATION
int value; //save analog value
double calibration_ratio_Lime;
double calibration_ratio_GingerBeer;
double calibration_ratio_Vodka;

// FUNCTION: setup()
void setup() {
  // set the input pins
  pinMode(tuner_strength_pin, INPUT);
  pinMode(tuner_sour_pin, INPUT);
  pinMode(cup_present_buttonPin, INPUT_PULLUP);
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
}


// FUNCTION: loop()
void loop() {
  // Perform a short animation every 3 seconds to let you know machine is alive
  digitalWrite(ledOrange_Vodka, HIGH);
  delay(standby_blink_duration);
  digitalWrite(ledOrange_Vodka, LOW);
  digitalWrite(ledYellow_GingerBeer, HIGH);
  delay(standby_blink_duration);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledGreen_Lime, HIGH);
  delay(standby_blink_duration);
  digitalWrite(ledGreen_Lime, LOW);
  delay(standby_animation_interval);
  digitalWrite(ledOrange_Vodka, LOW);
  digitalWrite(ledYellow_GingerBeer, LOW);
  digitalWrite(ledGreen_Lime, LOW);

  //TODO if(numDrinksTilEmpty < 3) flash RED LED
  
  // Wait for Copper Cup (Button Press)
  // NOTE LOW=="CUP PRESENT" as the pull up resistor keeps the button value HIGH by default and pushing
  // the button creates a LOW signal.
  // When button pressed: call functions for Vodka, GB, and Lime to pour until button is depressed.
  cup_is_present = digitalRead(cup_present_buttonPin) == LOW; // CUP_present && numDrinksTilEmpty > 0
  if (cup_is_present) {
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
       while(cup_is_present){

           // UPDATE RECIPE
           // 0.5 Cups per serving GB--> 2/3rd of Drink should be Ginger Beer
           // 0.0625 Cups per serving Lime --> Of remaining 1/3, 1/4 should be Lime
           // 0.1875 Cups per serving Vodka --> Of remaining 1/3, 3/4 should be Vodka
           // -----------------------------------------------------------------------
           // NOMINAL: GB 800/1200, Lime 100/1200, Vodka, 300/1200 --> Total time of 1200ms
           //NOTE: Lower Limit of pump values must be at least current_spike_delay in value
           value = analogRead(tuner_strength_pin);
           calibration_ratio_Vodka = map(value, 0, 1023, 100 + current_spike_delay, 500);
           value = analogRead(tuner_sour_pin);
           calibration_ratio_Lime = map(value, 0, 1023, current_spike_delay, 200);
           calibration_ratio_GingerBeer = 1200 - calibration_ratio_Vodka - calibration_ratio_Lime;
           
           // MIX DRINK
           // Start by switching all pumps to "ON". use a current_spike_delay to prevent the current from spiking
           // and reseting the Arduino.
           digitalWrite(ledYellow_GingerBeer, HIGH);
           digitalWrite(pump_GingerBeer, HIGH);
           delay(current_spike_delay);
           digitalWrite(ledOrange_Vodka, HIGH);
           digitalWrite(pump_Vodka, HIGH);
           delay(current_spike_delay);           
           digitalWrite(ledGreen_Lime, HIGH);
           digitalWrite(pump_Lime, HIGH);

           // turn off pumps in order of least time to greatest time
           // NOTE: ginger beer will be the greatest time
           if((calibration_ratio_Lime+current_spike_delay) < calibration_ratio_Vodka){
             delay((calibration_ratio_Lime));
             digitalWrite(pump_Lime, LOW);
             digitalWrite(ledGreen_Lime, LOW);

             delay((calibration_ratio_Vodka) - (calibration_ratio_Lime) - current_spike_delay);
             digitalWrite(pump_Vodka, LOW);
             digitalWrite(ledOrange_Vodka, LOW);
             
           } else {
             delay(calibration_ratio_Vodka - current_spike_delay);
             digitalWrite(pump_Vodka, LOW);
             digitalWrite(ledOrange_Vodka, LOW);

             delay((calibration_ratio_Lime) - (calibration_ratio_Vodka - current_spike_delay));
             digitalWrite(pump_Lime, LOW);
             digitalWrite(ledGreen_Lime, LOW);
           }

           delay(calibration_ratio_GingerBeer - calibration_ratio_Vodka - calibration_ratio_Lime - 2*current_spike_delay);
           digitalWrite(pump_GingerBeer, LOW);
           digitalWrite(ledYellow_GingerBeer, LOW);
                     
           cup_is_present = digitalRead(cup_present_buttonPin) == LOW;
       }
       digitalWrite(ledOrange_Vodka, LOW);
       digitalWrite(ledYellow_GingerBeer, LOW);
       digitalWrite(ledGreen_Lime, LOW);
  }
}
