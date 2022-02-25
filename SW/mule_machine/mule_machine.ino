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
int time_passed_ginger_beer_start; //calculation
const int numReadings  = 10;
int readings_strength[numReadings];
int readings_sour[numReadings];
int readIndex_sour  = 0;
long total_sour = 0;
int readIndex_strength = 0;
long total_strength = 0;
double calibration_ratio_Lime;
double calibration_ratio_GingerBeer;
double calibration_ratio_Vodka;

// FUNCTION: setup()
void setup() {

  Serial.begin(9600);
  Serial.println(F("SUM,Vodka,Lime,GingerBeer")); 

  for(int i=1; i< numReadings; i++){
    calibration_ratio_Vodka = smooth_strength();
    calibration_ratio_Lime = smooth_sour();
  }

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

  for(int i=1; i< numReadings; i++){
    calibration_ratio_Vodka = smooth_strength();
    calibration_ratio_Lime = smooth_sour();
  }
  calibration_ratio_GingerBeer = 1200 - calibration_ratio_Vodka - calibration_ratio_Lime;
  
  Serial.print(calibration_ratio_GingerBeer + calibration_ratio_Lime + calibration_ratio_Vodka);
  Serial.print(',');
  Serial.print(calibration_ratio_Vodka);
  Serial.print(',');
  Serial.print(calibration_ratio_Lime);
  Serial.print(',');
  Serial.println(calibration_ratio_GingerBeer);


  
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

           // UPDATE RECIPE FROM CALIBRATION
           // 0.5 Cups per serving GB--> 2/3rd of Drink should be Ginger Beer
           // 0.0625 Cups per serving Lime --> Of remaining 1/3, 1/4 should be Lime
           // 0.1875 Cups per serving Vodka --> Of remaining 1/3, 3/4 should be Vodka
           // -----------------------------------------------------------------------
           // NOMINAL: GB 800/1200, Lime 100/1200, Vodka, 300/1200 --> Total time of 1200ms
           //NOTE: Lower Limit of pump values must be at least current_spike_delay in value
           //NOTE: output is flipped max-to-low to match 5V and GND placement on schematic (5V should be on left pin when facing POT)
           for(int i=1; i< numReadings; i++){
             calibration_ratio_Vodka = smooth_strength();
             calibration_ratio_Lime = smooth_sour();
           }
           calibration_ratio_GingerBeer = 1200 - calibration_ratio_Vodka - calibration_ratio_Lime;
            
           Serial.print(calibration_ratio_GingerBeer + calibration_ratio_Lime + calibration_ratio_Vodka);
           Serial.print(',');
           Serial.print(calibration_ratio_Vodka);
           Serial.print(',');
           Serial.print(calibration_ratio_Lime);
           Serial.print(',');
           Serial.println(calibration_ratio_GingerBeer);
           
           // START MIXING DRINK
           // Start by switching all pumps to "ON". 
           // use a current_spike_delay to prevent the current from spiking all at once 
           // and resetting the Arduino.
           digitalWrite(ledYellow_GingerBeer, HIGH);
           digitalWrite(pump_GingerBeer, HIGH);
           delay(current_spike_delay);
           digitalWrite(ledOrange_Vodka, HIGH);
           digitalWrite(pump_Vodka, HIGH);
           delay(current_spike_delay);           
           digitalWrite(ledGreen_Lime, HIGH);
           digitalWrite(pump_Lime, HIGH);

           // MODULATE VOKDA AND LIME
           // turn off pumps in order of least time to greatest time
           // Either: Lime, Vokda, GB  OR  Vodka, Lime, GB
           // NOTE: ginger beer MUST be the greatest time
           //time_passed_ginger_beer_start will equal the amount of time ginger beer pump has been on.
           if((calibration_ratio_Lime + current_spike_delay) < calibration_ratio_Vodka){
             delay((calibration_ratio_Lime));
             digitalWrite(pump_Lime, LOW);
             digitalWrite(ledGreen_Lime, LOW);

             // If there is still more time needed for vokda after lime is finished, add
             // a delay for the remaining time.
             if((calibration_ratio_Vodka - calibration_ratio_Lime - current_spike_delay) > 0){
               delay(calibration_ratio_Vodka - calibration_ratio_Lime - current_spike_delay);
             }
             digitalWrite(pump_Vodka, LOW);
             digitalWrite(ledOrange_Vodka, LOW);
             
             time_passed_ginger_beer_start = calibration_ratio_Vodka + 2*current_spike_delay;
           } else {
             // subtract 1 current_spike_delay from vokda time to compensate.
             delay(calibration_ratio_Vodka - current_spike_delay);
             digitalWrite(pump_Vodka, LOW);
             digitalWrite(ledOrange_Vodka, LOW);

             // If there is still more time needed for lime after vokda is finished, add
             // a delay for the remaining time.
             if((calibration_ratio_Lime - calibration_ratio_Vodka) > 0){
               delay(calibration_ratio_Lime - calibration_ratio_Vodka);
             }
             digitalWrite(pump_Lime, LOW);
             digitalWrite(ledGreen_Lime, LOW);

             time_passed_ginger_beer_start = calibration_ratio_Lime + 2*current_spike_delay;
           }

           // If there is still more time needed to run ginger beer pump,
           // then finish remaining time with delay()
           if((calibration_ratio_GingerBeer - time_passed_ginger_beer_start) > 0){
               delay(calibration_ratio_GingerBeer - time_passed_ginger_beer_start);
           }
           digitalWrite(pump_GingerBeer, LOW);
           digitalWrite(ledYellow_GingerBeer, LOW);
                     
           cup_is_present = digitalRead(cup_present_buttonPin) == LOW;
       }
       digitalWrite(ledOrange_Vodka, LOW);
       digitalWrite(ledYellow_GingerBeer, LOW);
       digitalWrite(ledGreen_Lime, LOW);
  }
}

long smooth_sour() { /* function smooth */
  ////Perform average on sensor readings
  long average;
  // subtract the last reading:
  total_sour = total_sour - readings_sour[readIndex_sour];
  // read the sensor:
  value = analogRead(tuner_sour_pin);
  value = map(value, 0, 1023, 200, current_spike_delay);
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

long smooth_strength() { /* function smooth */
  ////Perform average on sensor readings
  long average;
  // subtract the last reading:
  total_strength = total_strength - readings_strength[readIndex_strength];
  // read the sensor:
  value = analogRead(tuner_strength_pin);
  value = map(value, 0, 1023, 500, 100 + current_spike_delay);
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
