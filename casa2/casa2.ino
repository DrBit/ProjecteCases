/*

  DrBit.ln

  Projecte Cases. Casa 2

  created 13 Jun 2006
  modified 13 Aug 2012
  by DrBit

  This project is about ....&&*******

This project contains parts of code from:

---------------------------------
 4D Systems Displays

---------------------------------
Example project by Tom Igoe
This example code is in the public domain.
http://www.arduino.cc/en/Tutorial/Midi

This part of code simply outputs MIDI data through selected project

-------------------------------------
Doctor Bit (drbit.nl) Example of using TTP229_LSF for capacitive touch sensing.
http://www.drbit.nl/2017/08/capacitive-sensors-new-project/
This example code is in the public domain.
 
//////////////////////////////////////////////////////////////////
// Connections
//
// The ttp229 16ch Module from robodtDyn Vcc is 5V, be careful
// other modules can use different voltages, check it before connecting.
//
// Uno:   SDA <-> A4 (SDA)
//        SCL <-> A5 (SCL)
//
// Mega:  SDA <-> 20 (SDA)   
//        SCL <-> 21 (SCL)
//
// Leo:   SDA <-> 2 (SDA)
//        SCL <-> 3 (SCL)
//
//////////////////////////////////////////////////////////////////

----------------------------------------
Adafruit NeoPixel Library

https://github.com/adafruit/Adafruit_NeoPixel

Arduino library for controlling single-wire-based LED pixels and strip such as the Adafruit 
60 LED/meter Digital LED strip, the Adafruit FLORA RGB Smart Pixel, the Adafruit Breadboard-friendly 
RGB Smart Pixel, the Adafruit NeoPixel Stick, and the Adafruit NeoPixel Shield.

Compatibility notes: Port A is not supported on any AVR processors at this time


*/


#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include "Wire.h"
#include <genieArduino.h>

// FLAGS
// #define MIDI_ENABLED
// #define CAPTOUCH_ENABLED
// #define DEBUGLEDS

// Neopixels
#define PIN 7
#define NUM_LEDS 26
#define LED_TYPE NEO_GRB
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, LED_TYPE + NEO_KHZ800);
#define refreshRateMs 50  // refresh rate of led calcultaions in milliseconds

#define NumberOfLedGroups 3
int LedGroups [NumberOfLedGroups][NUM_LEDS] = {
  {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26},  // Group 0 - tots els leds
  {1,2,3,4,5,6,7,8,9,10,11,12,13,0,0,0,0,0,0,0,0,0,0,0,0,0},  // Group 1 - part 1
  {14,15,16,17,18,19,20,21,22,23,24,25,26,0,0,0,0,0,0,0,0,0,0,0,0,0}  // Group 2 - part 2
};




// Audio
uint32_t prevTime, millisDelay = 0;
uint32_t ClockPrevTime, ClockNow = 0;
uint32_t startPlay = 0; 
unsigned long trackDuration = 0;
unsigned long lastRefresh = 0;

// Colors effects
uint32_t color  = 0xFF0000; // Start red
uint8_t  mode   = 0, // Current animation effect
         offset = 0; // Position of spinny eyes
int counterInt1 = 0;
byte counterByte1=0;


// actions timing

int currentStep = 0;    // start from this step
unsigned long startTime = 0;  
unsigned long nextStepTime = 0;
boolean pause_play = false;

int lastButtonPressed = 0;



#define numberOfSteps 18
int timeActions [numberOfSteps][3] = {
  //{minuts,segons,milisegons}
  {17,04,0},   // Time Action 0 (track duration)

  {0,14,0},   // 1 - Llum nit
  {2,06,0},   // 2 - Llum caseta

  {2,16,0},   // 3 - sol lampara i off nit
  {2,31,0},   // 4 - S'apaga llum casa
  {4,24,0},   // 5 - nit on - sol off

  {4,30,0},   // 6 - Llum caseta

  {4,42,0},   // 7 - 1 Llamp
  {4,42,500}, // 8 - 1 Llamp off

  {4,45,0},   // 9 - 2 Llamps
  {4,45,500}, // 10 - 2 Llamps
  {4,46,0},   // 11 - 2 Llamps
  {4,46,500}, // 12 - 2 Llamps

  {5,00,0},   // 13 - 1 Llamp
  {5,00,500}, // 14 - 1 Llamp off

  {10,23,0},  // 15 - sol on - nit off
  {11,5,0},   // 16 - Efecte policia on
  // {11,20,0},  // 16 - Efecte policia off
  {17,00,0},  // 17 - tot fosc
};

boolean stepdone [numberOfSteps];


// PINOUT
#define relay1 2
#define relay2 3
#define mosfet1 6
#define mosfet2 4
#define mosfet3 5

#define flash relay1
#define mp3 relay2
#define w100LED mosfet2
#define bulb mosfet3
#define LEDCASA mosfet1

// intensities analogs
int LEDs_int [3];

#define startButtonLED 27
#define start_button 29


// Displays
Genie display1; // Genie Display 1
Genie display2; // Genie Display 2
#define RESETLINE1 23  // Reset pin attached to Display 1
#define RESETLINE2 24  // Reset pin attached to Display 1


// Capacitive sensors
#define TTP229_LSF 0x57 // I2C device address of TTP229_LSF (the addressing without the R/W bit) -> 01010111 = 0x57

// Midi
boolean MIDI_enabled = false;


void setup() {

  // Serial debug
  Serial.begin(250000);
  Serial.println("*Projecte Cases Start");
  Serial.println ("Casa 2\n");

  #if defined CAPTOUCH_ENABLED
  // Capacitive touch sensors
  Wire.begin(); // wake up I2C bus
  #endif

  // Midi
  #if defined MIDI_ENABLED
  Serial.println (F(".Setting MIDI"));
  serialMIDIbegin ();     // on serial1 (mega only)
  #endif
    
  //PINOUTS
  Serial.println (F(".Setting PINs"));
  set_and_test_pinouts ();

  // Neopixels
  Serial.println (F(".Start neopixels driver"));
  strip.begin();
  fullWhite();      // test pixels full white

  // Display
  Serial.println (F(".Setting Displays 1 & 2"));
  Serial4DSystemsBegin ();  // on serial2 (mega only)
  display1.AttachEventHandler(myGenieEventHandler1); // Attach the user function Event Handler for processing events for display 1
  display2.AttachEventHandler(myGenieEventHandler2); // Attach the user function Event Handler for processing events for display 2

  // Audio
  Serial.print (".Track Duration: ");
  trackDuration = convertTime(0);
  Serial.print (trackDuration);
  Serial.println (" milliseconds total");

  resetCounters (); // Reset variables


  Serial.println (".END setup\n");
  print_commands ();
  // analogWrite (LEDCASA, 0);
  // analogWrite (w100LED, 0);
  // analogWrite (bulb, 0);
  // digitalWrite (startButtonLED, LOW);
  // clearLEDS();      // test pixels full white

}


unsigned long time_ledB = 0;
int value_ledB = 0;

void loop() {
 
  /*
  1.Escollir idioma
  2.Apretar start
  3.Començar cronometre i audio
  */
  int periode = 2000; 
  time_ledB = millis();
  value_ledB = 128+127*cos(2*PI/periode*time_ledB);
  analogWrite(startButtonLED, value_ledB);           // sets the value (range from 0 to 255) 

  int buttonPressed = getButtonPressed ();
  #if defined DEBUG
  buttonPressed = true;
  #endif

  // check if we have to start  
  if (buttonPressed || Serial.available() > 0) {

    // while (value_ledB > 0) {    // fade led down
    //   value_ledB --;
    //   delay (10);
    // }

    while (Serial.available() > 0) {    // remove any char remaingin in buffer
      int a = Serial.read ();
      delay(5);
    }

    Serial.println ("button pressed. Start play");
    lastButtonPressed = buttonPressed;

    #if defined DEBUG_NOAUDIO
    Serial.println ("skip button pressed. DEBUG MODE");
    #else
    // Start playing song
    Serial.println ("Starting audio.");
    digitalWrite (mp3, HIGH);
    delay(250);
    digitalWrite (mp3, LOW);
    // in case of sleep (20 seconds);
    delay(500);
    digitalWrite (mp3, HIGH);
    delay(250);
    digitalWrite (mp3, LOW);

    delay(250);
    #endif

    startTime = millis();   // start counting time
    

    // start timing
    Serial.print (".Track Duration: ");
    trackDuration = convertTime(0);
    Serial.print (trackDuration);
    Serial.println (" milliseconds total");

    currentStep = 0;    // start from this step

    nextStepTime = convertTime (currentStep+1);

    Serial.println ("Start playing actions.");
    resetCounters (); // Reset variables
    clearAllLEDS ();  // Clear all possible left overs
    // Enter into play loop
    while ((millis () - startTime) < trackDuration) {

      // Check serial for other orders
      CheckSerialControl ();

      // Check if we need to change to next step
      if ((millis () - startTime) >= nextStepTime) {

        if (currentStep < numberOfSteps) {
          currentStep++;
        }
        if (currentStep+1 <= numberOfSteps) {
          nextStepTime = convertTime (currentStep+1);
        }

        Serial.print ("Trigger step ");
        Serial.print (currentStep);

        Serial.print (" - Time now: ");
        Serial.print (millis () - startTime);

        Serial.print (" - Next trig time: ");
        Serial.println (nextStepTime);


      }
      // do what needs to be done in the current step lets keep it at 30fps
      // refresh one time every 33ms
      if (millis () - lastRefresh > refreshRateMs) {
        executeAllCurrentSteps (currentStep);
        lastRefresh = millis ();
        /*
        Serial.print ("Time now: ");
        Serial.println (millis () - startTime);*/
      }
    }
    Serial.println ("END of play. \n");
    clearAllLEDS ();
  }

  #if defined MIDI_ENABLED
  if (MIDI_enabled) {
    // Check withc channels are enbaled
    Serial.println("test");

    // Get data
    byte aa,bb = 0;       // First byte is aa , second byte is bb
    getTTP229data(&aa,&bb);

    // First check the first byte received
    int ch = 0;
    for (unsigned int mask = 0x80; mask; mask >>= 1) {
      if (mask & aa) {
        Serial.print("Channel ");
        Serial.print(ch);
        Serial.println(" enabled.");
        play_note (ch);       // Plays note according to the channel enabled
      }
      ch++;
    }

    // Now do the same for the second byte
    for (unsigned int mask = 0x80; mask; mask >>= 1) {
      if (mask & bb) {
        Serial.print("Channel ");
        Serial.print(ch);
        Serial.println(" enabled.");
        play_note (ch);       // Plays note according to the channel enabled
      }
      ch++;
    }
  }
  #endif
}


/*
  {0,14,0},   // 1 - Llum nit
  {2,06,0},   // 2 - Llum caseta

  {2,16,0},   // 3 - sol lampara i off nit
  {4,24,0},   // 4 - nit on - sol off
  {4,40,0},   // 5 - 1 Llamp
  {4,45,0},   // 6 - 2 Llamps
  {8,54,0},   // 7 - sol on - nit off
  {11,5,0},   // 8 - Efecte policia on
  {11,20,0},  // 9 - Efecte policia off
  {17,00,0},  // 10 - tot fosc

#define flash relay1
#define mp3 relay2
#define w100LED mosfet2
#define bulb mosfet3
#define LEDCASA mosfet1
*/

void executeAllCurrentSteps (int currentStep) {
  int thisStep =0;
  if (currentStep >= 0) {   // Avoid triggering on -1
  //----------------------------------------------------
    uint32_t nocolor = strip.Color(0,0,0);

    thisStep = 0;
    if (currentStep >=thisStep & !stepdone[thisStep]) {     // Action 0 (from the beginning)
      clearAllLEDS ();
      stepdone[thisStep] = true;
    }

    thisStep = 1; // llum nit
    if (currentStep >=thisStep & !stepdone[thisStep]) {     
      int a = GroupfadeTo (0, strip.Color(0,0,255,0),5);
      if (a == 1) {
        stepdone[thisStep] = true;
        action_done (thisStep);
      }
    }

    thisStep = 2;   // llum caseta
    if (currentStep >=thisStep & !stepdone[thisStep]) {
      int a = INTfadeTo (LEDCASA,255,2);
      if (a == 1) {
        stepdone[thisStep] = true;
        action_done (thisStep);
      }
    }

    thisStep = 3;   // corssover nit i sol en 15 segons
    if (currentStep >=thisStep & !stepdone[thisStep]) {   
      int a = INTfadeTo (bulb,255,15);
      a = a + GroupfadeTo (0, nocolor,15);
      if (a == 2) {
        stepdone[thisStep] = true;
        action_done (thisStep);
      }
    }

    thisStep = 4;   // llum caseta
    if (currentStep >=thisStep & !stepdone[thisStep]) {
      int a = INTfadeTo (LEDCASA,0,5);
      if (a == 1) {
        stepdone[thisStep] = true;
        action_done (thisStep);
      }
    }

    thisStep = 5;   // corssover nit i sol en 15 segons
    if (currentStep >=thisStep & !stepdone[thisStep]) {   
      int a = INTfadeTo (bulb,0,15);
      a = a + GroupfadeTo (0, strip.Color(0,0,255,0),15);
      if (a == 2) {
        stepdone[thisStep] = true;
        action_done (thisStep);
      }
    }

    thisStep = 6;   // llum caseta
    if (currentStep >=thisStep & !stepdone[thisStep]) {
      int a = INTfadeTo (LEDCASA,255,2);
      if (a == 1) {
        stepdone[thisStep] = true;
        action_done (thisStep);
      }
    }

    thisStep = 7;   // un llamp!
    if (currentStep >=thisStep & !stepdone[thisStep]) {   
      int a = INTfadeTo (w100LED,255,1);
      if (a == 1) {
        stepdone[thisStep] = true;
        action_done (thisStep);
        digitalWrite (flash, HIGH);
        analogWrite (w100LED,0);
        set_LED_int (w100LED,0);
      }
    }

    thisStep = 8;   // llamp off
    if (currentStep >=thisStep & !stepdone[thisStep]) {   
      digitalWrite (flash, LOW);
      action_done (thisStep);
      stepdone[thisStep] = true;

    }

    thisStep = 9;   // un llamp!
    if (currentStep >=thisStep & !stepdone[thisStep]) {   
      int a = INTfadeTo (w100LED,255,1);
      if (a == 1) {
        stepdone[thisStep] = true;
        action_done (thisStep);
        digitalWrite (flash, HIGH);
        analogWrite (w100LED,0);
        set_LED_int (w100LED,0);        
      }
    }

    thisStep = 10;   // llamp off
    if (currentStep >=thisStep & !stepdone[thisStep]) {  
      digitalWrite (flash, LOW); 
      action_done (thisStep);
      stepdone[thisStep] = true;

    }

    thisStep = 11;   // un llamp!
    if (currentStep >=thisStep & !stepdone[thisStep]) {   
      int a = INTfadeTo (w100LED,255,1);
      if (a == 1) {
        stepdone[thisStep] = true;
        action_done (thisStep);
        digitalWrite (flash, HIGH);
        analogWrite (w100LED,0);
        set_LED_int (w100LED,0);        
      }
    }

    thisStep = 12;   // llamp off
    if (currentStep >=thisStep & !stepdone[thisStep]) {  
      digitalWrite (flash, LOW); 
      action_done (thisStep);
      stepdone[thisStep] = true;
    }

    thisStep = 13;   // un llamp!
    if (currentStep >=thisStep & !stepdone[thisStep]) {   
      int a = INTfadeTo (w100LED,255,1);
      if (a == 1) {
        stepdone[thisStep] = true;
        action_done (thisStep);
        digitalWrite (flash, HIGH);
        analogWrite (w100LED,0);
        set_LED_int (w100LED,0);        
      }
    }

    thisStep = 14;   // llamp off
    if (currentStep >=thisStep & !stepdone[thisStep]) {  
      digitalWrite (flash, LOW); 
      action_done (thisStep);
      stepdone[thisStep] = true;
    }

    thisStep = 15;   // corssover nit i sol en 15 segons
    if (currentStep >=thisStep & !stepdone[thisStep]) {   
      int a = INTfadeTo (bulb,255,15);
      a = a + GroupfadeTo (0, nocolor,15);
      a = a + INTfadeTo (LEDCASA,0,5);
      if (a == 3) {
        stepdone[thisStep] = true;
        action_done (thisStep);
      }
    }

    thisStep = 16;   // efecte policia on
    if (currentStep >=thisStep & !stepdone[thisStep]) {   
      efecte_policia ();
    }

    // // thisStep = 16;   // efecte policia off
    // if (currentStep >=thisStep & !stepdone[thisStep]) {   
    //   // stepdone[thisStep-1] = true;
    //   // stepdone[thisStep] = true;
    //   // action_done (thisStep-1);
    //   // action_done (thisStep);
    //   // efecte_policia_off();
    // }

    thisStep = 17;   // tot fosc
    if (currentStep >=thisStep & !stepdone[thisStep]) { 
      efecte_policia_off();  
      int a = INTfadeTo (bulb,0,15);
      a = a + INTfadeTo (LEDCASA,0,15);
      a = a + GroupfadeTo (0, nocolor,15);
      if (a == 3) {
        stepdone[thisStep] = true;
        stepdone[thisStep-1] = true;
        action_done (thisStep);
        action_done (thisStep-1);
      }
    }
  }
  strip.show();
}


/*

   NEOPIXELS

*/

void fullWhite() {
  
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(255,255,255, 255 ) );
    }
      strip.show();
}

void clearAllLEDS () {
  clearLEDS ();
}

void clearLEDS ( ) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0,0,0, 0 ) );
  }
  strip.show();

  analogWrite (LEDCASA, 0);
  analogWrite (w100LED, 0);
  analogWrite (bulb, 0);
  digitalWrite (startButtonLED, LOW);
  reset_LED_ints ();
  read_LED_ints ();
}

// example colling this funciton: fadeTo ( 0, NUM_ELEM(name_of_array) ); 
boolean GroupColor (int ledGroupindex, uint32_t newColor, uint32_t dummy) {
  for (int subIndex = 0; subIndex < NUM_LEDS; subIndex++) {
    int led_final_index = LedGroups[ledGroupindex][subIndex];
    if (led_final_index > 0) strip.setPixelColor((led_final_index)-1, newColor);
  }
  #if defined DEBUGLEDS
    Serial.print ("Grup Color "); Serial.println (ledGroupindex);
    Serial.print ("New Color: "); Serial.println (newColor);
  #endif
  return true;
}

// example colling this funciton: fadeTo ( 0, NUM_ELEM(name_of_array) ); 
boolean GroupfadeTo (int ledGroupindex, uint32_t newColor, int fadeSeconds) {
  int finalized_colors = 0;
  boolean fadeEnded = false;
  for (int subIndex = 0; subIndex < NUM_LEDS; subIndex++) {
     fadeEnded = fadeTo (LedGroups[ledGroupindex][subIndex], newColor, fadeSeconds);
     if (fadeEnded) finalized_colors ++;
  }
  #if defined DEBUGLEDS
    Serial.print ("Grup fade "); Serial.println (ledGroupindex);
    Serial.print ("Finished leds: "); Serial.println (finalized_colors);
    Serial.print ("Fading still: "); Serial.println (NUM_LEDS - finalized_colors);
  #endif

  if (finalized_colors == NUM_LEDS) return true;
  return false;
}

boolean fadeTo (int ledNumber, uint32_t newColor, int fadeSeconds) {
  int fps = 1000/refreshRateMs;
  int actualitzacions_a_fer = fadeSeconds*fps;
  int fadeStep = 200/actualitzacions_a_fer;
  if (fadeStep == 0) fadeStep++;
  if (fadeSeconds == 1) fadeStep = 50;
  if (fadeSeconds == 0) fadeStep = 100;


  if (ledNumber == 0) return true;  // Led 0 means no led
  // get pixel color
  uint32_t actualPixelColor = strip.getPixelColor(ledNumber-1);
  if (newColor == actualPixelColor) return true;

  uint8_t
    actual_r = 0,
    actual_g = 0,
    actual_b = 0,
    actual_w = 0,
      //-----------------
    new_r = 0,
    new_g = 0,
    new_b = 0,
    new_w = 0;

  
  if (LED_TYPE == NEO_GRBW) {     // IF RGBW
      actual_r = (uint8_t)(actualPixelColor >> 16);
      actual_g = (uint8_t)(actualPixelColor >> 8);
      actual_b = (uint8_t)(actualPixelColor);
      actual_w = (uint8_t)(actualPixelColor >> 24);
      //-----------------
      new_r = (uint8_t)(newColor >> 16);
    new_g = (uint8_t)(newColor >> 8);
    new_b = (uint8_t)(newColor);
    new_w = (uint8_t)(newColor >> 24);
  }else{                // RGB        
      actual_r = (uint8_t)(actualPixelColor >> 16);
      actual_g = (uint8_t)(actualPixelColor >>  8);
      actual_b = (uint8_t)actualPixelColor;
      //-----------------
      new_r = (uint8_t)(newColor >> 16);
    new_g = (uint8_t)(newColor >>  8);
    new_b = (uint8_t)newColor;
    }


      

    // primer mirem si hem de sumar o restar
    if ( actual_r < new_r ) { // Sumem
      if (actual_r + fadeStep > new_r) {  // Mirem si podem sumar o ens pasarem
        actual_r = new_r;   // Si ens passem directament agafem el color final
      }else{    // sino ens pasem simplement sumem el fadeStep 
        actual_r = actual_r + fadeStep;
      }
    }else{  // restem
      if (actual_r - fadeStep < new_r) {
        actual_r = new_r;
      }else{
        actual_r = actual_r - fadeStep;
      }
    }

    // primer mirem si hem de sumar o restar
    if ( actual_g < new_g ) { // Sumem
      if (actual_g + fadeStep > new_g) {  // Mirem si podem sumar o ens pasarem
        actual_g = new_g;   // Si ens passem directament agafem el color final
      }else{    // sino ens pasem simplement sumem el fadeStep 
        actual_g = actual_g + fadeStep;
      }
    }else{  // restem
      if (actual_g - fadeStep < new_g) {
        actual_g = new_g;
      }else{
        actual_g = actual_g - fadeStep;
      }
    }

    // primer mirem si hem de sumar o restar
    if ( actual_b < new_b ) { // Sumem
      if (actual_b + fadeStep > new_b) {  // Mirem si podem sumar o ens pasarem
        actual_b = new_b;   // Si ens passem directament agafem el color final
      }else{    // sino ens pasem simplement sumem el fadeStep 
        actual_b = actual_b + fadeStep;
      }
    }else{  // restem
      if (actual_b - fadeStep < new_b) {
        actual_b = new_b;
      }else{
        actual_b = actual_b - fadeStep;
      }
    }

  if (LED_TYPE == NEO_GRBW) {     // IF RGBW
    // primer mirem si hem de sumar o restar
      if ( actual_w < new_w ) { // Sumem
        if (actual_w + fadeStep > new_w) {  // Mirem si podem sumar o ens pasarem
          actual_w = new_w;   // Si ens passem directament agafem el color final
        }else{    // sino ens pasem simplement sumem el fadeStep 
          actual_w = actual_w + fadeStep;
        }
      }else{  // restem
        if (actual_w - fadeStep < new_w) {
          actual_w = new_w;
        }else{
          actual_w = actual_w - fadeStep;
        }
      }
    }

    #if defined DEBUGLEDS
    Serial.print ("Fading led ");
    Serial.print (ledNumber);
    Serial.print ("  Actual color: (");
    Serial.print (actual_r);
    Serial.print (", ");
    Serial.print (actual_g);
    Serial.print (", ");
    Serial.print (actual_b);
    Serial.print (", ");
    Serial.print (actual_w);
    Serial.print (") - ");
    #endif

    uint32_t color_now = strip.Color (actual_r, actual_g, actual_b, actual_w);
    #if defined DEBUGLEDS
    Serial.print (color_now,HEX);
    Serial.print ("  Target color: ");
    Serial.println (newColor,HEX);
    #endif
    
    strip.setPixelColor(ledNumber-1, color_now);
    return false;   // return false when we are still fading in.
}


boolean INTfadeTo (int portNumber, int newIntensity, int fadeSeconds) {
  int fps = 1000/refreshRateMs;
  int actualitzacions_a_fer = fadeSeconds*fps;
  int fadeStep = 200/actualitzacions_a_fer;
  if (fadeStep == 0) fadeStep++;
  if (fadeSeconds == 1) fadeStep = 50;
  if (fadeSeconds == 0) fadeStep = 100;


  // get pixel color
  int actualINT = read_LED_int (portNumber);
  #if defined DEBUGLEDS
  Serial.print ("Fading port ");
  Serial.print (portNumber);
  Serial.print (" Read int: ");
  Serial.print (actualINT);
  Serial.print (" fade step: ");
  Serial.print (fadeStep);
  Serial.print ("  Target intensity: ");
  Serial.println (newIntensity);
  #endif


  if (newIntensity == actualINT) return true;

  // primer mirem si hem de sumar o restar
  if ( actualINT < newIntensity ) { // Sumem
    if (actualINT + fadeStep > newIntensity) {  // Mirem si podem sumar o ens pasarem
      actualINT = newIntensity;   // Si ens passem directament agafem el color final
    }else{    // sino ens pasem simplement sumem el fadeStep 
      actualINT = actualINT + fadeStep;
    }
  }else{  // restem
    if (actualINT - fadeStep < newIntensity) {
      actualINT = newIntensity;
    }else{
      actualINT = actualINT - fadeStep;
    }
  }

  #if defined DEBUGLEDS
  Serial.print ("Fading port ");
  Serial.print (portNumber);
  Serial.print ("  Actual intensity: ");
  Serial.print (actualINT);

  Serial.print ("  Target intensity: ");
  Serial.println (newIntensity);
  #endif
  
  analogWrite (portNumber,actualINT);
  set_LED_int (portNumber,actualINT);
  return false;   // return false when we are still fading in.
}


boolean efecte_policia () {

  int periode = 400; 
  time_ledB = millis();
  int one = 128+127*cos(2*PI/periode*time_ledB);
  int two = 255 - one;

  int limit = 127;
  if (one > limit) {
    one = 255;
  }else{
    one = 0;
  }
  
  if (two > limit) {
    two = 255;
  }else{
    two = 0;
  }

  GroupColor (1, strip.Color(one,0,two) , 0); //blue
  GroupColor (2, strip.Color(two,0,one) , 0); //red

}

boolean efecte_policia_off () {
  GroupColor (1, strip.Color(0,0,0) , 0); //blue
  GroupColor (2, strip.Color(0,0,0) , 0); //red
}

/*

   4D systems screens comunication

*/

void Serial4DSystemsBegin () {
  // Serial2.begin(9650);  // Serial1 @ 200000 (200K) Baud
  Serial1.begin(200000);  // Serial1 @ 200000 (200K) Baud
  display1.Begin(Serial1);  // Use Serial2 for talking to the Genie Library, and to the 4D Systems display #1

  // Serial3.begin(9650);  // Serial1 @ 200000 (200K) Baud
  Serial2.begin(200000);   // Serial0 @ 200000 (200K) Baud
  display2.Begin(Serial2);   // Use Serial3 for talking to the Genie Library, and to the 4D Systems display #2

  display1.AttachEventHandler(myGenieEventHandler1); // Attach the user function Event Handler for processing events for display 1
  display2.AttachEventHandler(myGenieEventHandler2); // Attach the user function Event Handler for processing events for display 2
  
  // Reset the Displays
  // THIS IS IMPORTANT AND CAN PREVENT OUT OF SYNC ISSUES, SLOW SPEED RESPONSE ETC 
  pinMode(RESETLINE1, OUTPUT);  // Set D4 on Arduino to Output to control the reset line to Display 1
  pinMode(RESETLINE2, OUTPUT);  // Set D2 on Arduino to Output to control the reset line to Display 2
  digitalWrite(RESETLINE1, 0);  // Reset Display 1
  digitalWrite(RESETLINE2, 0);  // Reset Display 2
  delay(100);
  digitalWrite(RESETLINE1, 1);  // unReset Display 1
  digitalWrite(RESETLINE2, 1);  // unReset Display 2

  //delay (3500); //let the display start up after the reset (This is important)

  //Set the brightness/Contrast of the Display - (Not needed but illustrates how) 
  //Most Displays, 1 = Display ON, 0 = Display OFF
  //For uLCD-43, uLCD-220RD, uLCD-70DT, and uLCD-35DT, use 0-15 for Brightness Control, where 0 = Display OFF, though to 15 = Max Brightness ON.
  display1.WriteContrast(15); // Display ON
  display2.WriteContrast(15); // Display ON
  
  //Write a string to the Display to identify each display 
  display1.WriteStr(0, "Pantalla 1");
  display2.WriteStr(0, "Pantalla 2");
}


/////////////////////////////////////////////////////////////////////
//
// This is the user's event handler. It is called by the DoEvents()
// when the following conditions are true
//
//    The link is in an IDLE state, and
//    There is an event to handle
//
// The event can be either a REPORT_EVENT frame sent asynchronously
// from the display or a REPORT_OBJ frame sent by the display in
// response to a READ_OBJ (genie.ReadObject) request.
//

// Event Handler Function for Display 1
void myGenieEventHandler1(void)
{
  genieFrame Event;
  display1.DequeueEvent(&Event); // Remove the next queued event from the buffer, and process it below

  //If the cmd received is from a Reported Event (Events triggered from the Events tab of Workshop4 objects)
  if (Event.reportObject.cmd == GENIE_REPORT_EVENT)
  {
    if (Event.reportObject.object == GENIE_OBJ_SLIDER)                   // If the Reported Message was from a Slider
    {
      if (Event.reportObject.index == 0)                                 // If Slider0 (Index = 0)
      {
        int slider_val = display1.GetEventData(&Event);                  // Receive the event data from the Slider0
        display1.WriteObject(GENIE_OBJ_LED_DIGITS, 0, slider_val);       // Write Slider0 value of Display 1 to to LED Digits 0 of Display 1 !
      }
    }
  }

  //If the cmd received is from a Reported Object, which occurs if a Read Object (genie.ReadOject) is requested in the main code, reply processed here.
  if (Event.reportObject.cmd == GENIE_REPORT_OBJ)
  {
    if (Event.reportObject.object == GENIE_OBJ_USER_LED)                 // If the Reported Message was from a User LED
    {
      if (Event.reportObject.index == 0)                                 // If UserLed0 (Index = 0)
      {
        bool UserLed0_val = display1.GetEventData(&Event);               // Receive the event data from the UserLed0
        UserLed0_val = !UserLed0_val;                                    // Toggle the state of the User LED Variable
        display1.WriteObject(GENIE_OBJ_USER_LED, 0, UserLed0_val);       // Write UserLed0_val value back to to UserLed0
      }
    }
  }
}

// Event Handler Function for Display 2
void myGenieEventHandler2(void)
{
  genieFrame Event;
  display2.DequeueEvent(&Event); // Remove the next queued event from the buffer, and process it below

  //If the cmd received is from a Reported Event (Events triggered from the Events tab of Workshop4 objects)
  if (Event.reportObject.cmd == GENIE_REPORT_EVENT)
  {
    if (Event.reportObject.object == GENIE_OBJ_SLIDER)                   // If the Reported Message was from a Slider
    {
      if (Event.reportObject.index == 0)                                 // If Slider0 (Index = 0)
      {
        int slider_val = display2.GetEventData(&Event);                  // Receive the event data from the Slider0
        display2.WriteObject(GENIE_OBJ_LED_DIGITS, 0, slider_val);       // Write Slider0 value of Display 2 to to LED Digits 0 of Display 2
      }
    }
  }

  //If the cmd received is from a Reported Object, which occurs if a Read Object (genie.ReadOject) is requested in the main code, reply processed here.
  if (Event.reportObject.cmd == GENIE_REPORT_OBJ)
  {
    if (Event.reportObject.object == GENIE_OBJ_USER_LED)                 // If the Reported Message was from a User LED
    {
      if (Event.reportObject.index == 0)                                 // If UserLed0 (Index = 0)
      {
        bool UserLed0_val = display2.GetEventData(&Event);               // Receive the event data from the UserLed0
        UserLed0_val = !UserLed0_val;                                    // Toggle the state of the User LED Variable
        display2.WriteObject(GENIE_OBJ_USER_LED, 0, UserLed0_val);       // Write UserLed0_val value back to to UserLed0
      }
    }
  }

  /********** This can be expanded as more objects are added that need to be captured *************
  *************************************************************************************************
  Event.reportObject.cmd is used to determine the command of that event, such as an reported event
  Event.reportObject.object is used to determine the object type, such as a Slider
  Event.reportObject.index is used to determine the index of the object, such as Slider0
  genie.GetEventData(&Event) us used to save the data from the Event, into a variable.
  *************************************************************************************************/
}



/*

   Timing

*/

// convert time into ms
unsigned long convertTime (int timeStep) {

  int minutes = timeActions [timeStep][0];
  int seconds = timeActions [timeStep][1];
  int milliseconds = timeActions [timeStep][2];

  // Serial.println ("Start time conversion ");

  // Serial.print ("minutes: ");
  // Serial.println (minutes);  
  // Serial.print ("seconds: ");
  // Serial.println (seconds);
  // Serial.print ("ms: ");
  // Serial.println (milliseconds);

  
  unsigned long seconds_minutes = (minutes*60) + (seconds); 
  unsigned long msSM = seconds_minutes*1000;

  unsigned long totalMS =  msSM + milliseconds;


  // Serial.print ("seconds minutes: ");
  // Serial.println (seconds_minutes);  
  // Serial.print ("sconds minutes *1000");
  // Serial.println (msSM);

  // Serial.print ("Total milliseconds: ");
  // Serial.println (totalMS);

  return totalMS;
}

// convert time into ms
unsigned long convertTime (int minutes, int seconds, int milliseconds) {
  unsigned long seconds_minutes = (minutes*60) + (seconds); 
  unsigned long msSM = seconds_minutes*1000;
  unsigned long totalMS =  msSM + milliseconds;
  return totalMS;
}


void resetCounters () {
  counterByte1 = 0;
  counterInt1 = 0;

  for (int g = 0; g<numberOfSteps; g++) {
    stepdone [g] = 0;
  }

  lastRefresh = 0;
  prevTime = millis();
  reset_LED_ints ();
}

/*

   Testing

*/

void test_LEDS () {
  int int_mosfet1 = 0;
  int int_mosfet2 = 0;
  int int_mosfet3 = 0;

  boolean test_leds = 1;
  while (test_leds) {
    if (Serial.available () > 0) {
      char a = Serial.read ();

      if (a == '1') {
        digitalWrite (relay1, HIGH);
        delay (300);
        digitalWrite (relay1, LOW);
      }

      if ( a == '2') {
        digitalWrite (relay2, HIGH);
        delay (300);
        digitalWrite (relay2, LOW); 
      }

      if ( a == '3') {
        int_mosfet1+=10;
        analogWrite (mosfet1, int_mosfet1); 
      }

      if ( a == '4') {
        int_mosfet1-=10;
        analogWrite (mosfet1, int_mosfet1); 
      }

      if ( a == '5') {
        int_mosfet2+=10;
        analogWrite (mosfet2, int_mosfet2); 
      }

      if ( a == '6') {
        int_mosfet2-=10;
        analogWrite (mosfet2, int_mosfet2); 
      }
      if ( a == '7') {
        int_mosfet3+=10;
        analogWrite (mosfet3, int_mosfet3); 
      }

      if ( a == '8') {
        int_mosfet2-=10;
        analogWrite (mosfet3, int_mosfet3); 
      }

      if ( a == 'r') {
        for(uint16_t i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(255,0,0) );
        }
        strip.show(); 
      }

      if ( a == 'g') {
        for(uint16_t i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(0,255,0) );
        }
        strip.show(); 
      }

      if ( a == 'b') {
        for(uint16_t i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(0,0,255) );
        }
        strip.show(); 
      }
      
      if ( a == 'w') {
        for(uint16_t i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(255,255,255) );
        }
        strip.show();
      }

      if ( a == '0') {
        int_mosfet1=0;
        int_mosfet2=0;
        int_mosfet3=0;
        analogWrite (mosfet1, int_mosfet1); 
        analogWrite (mosfet2, int_mosfet2); 
        analogWrite (mosfet3, int_mosfet3); 
        for(uint16_t i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(0,0,0) );
        }
        strip.show();
      }

      if ( a == 'x') {
        int_mosfet1=255;
        int_mosfet2=255;
        int_mosfet3=255;
        analogWrite (mosfet1, int_mosfet1); 
        analogWrite (mosfet2, int_mosfet2); 
        analogWrite (mosfet3, int_mosfet3); 
        for(uint16_t i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(255,255,255) );
        }
        strip.show();
      }
    }

    static long waitPeriod = millis(); // Time now
    
    static int gaugeAddVal1 = 1; // Set the value at which the Gauge on Display 1 increases by initially
    static int gaugeVal1 = 10; // Starting Value for Gauge on Display 1
    static int gaugeAddVal2 = 2; // Set the value at which the Gauge on Display 2 increases by initially
    static int gaugeVal2 = 50; // Starting Value for Gauge on Display 2

      display1.DoEvents(); // This calls the library each loop to process the queued responses from display 1
      display2.DoEvents(); // This calls the library each loop to process the queued responses from display 2


    if (millis() >= waitPeriod)
    {
      // Write to CoolGauge0 with the value in the gaugeVal variable on Display 1
      display1.WriteObject(GENIE_OBJ_COOL_GAUGE, 0, gaugeVal1);
      
      // Simulation code for Gauge on Display 1, just to increment and decrement gauge value each loop, for animation
      gaugeVal1 += gaugeAddVal1;
      if (gaugeVal1 >= 99) gaugeAddVal1 = -1; // If the value is > or = to 99, make gauge decrease in value by 1
      if (gaugeVal1 <= 0) gaugeAddVal1 = 1; // If the value is < or = to 0, make gauge increase in value by 1

      // The results of this call will be available to myGenieEventHandler() after the display has responded
      // Do a manual read from the UserLEd0 object on Display 1
      display1.ReadObject(GENIE_OBJ_USER_LED, 0);
      
      // Write to CoolGauge0 with the value in the gaugeVal variable on Display 2
      display2.WriteObject(GENIE_OBJ_COOL_GAUGE, 0, gaugeVal2);
      
      // Simulation code for Gauge on Display 2, just to increment and decrement gauge value each loop, for animation
      gaugeVal2 += gaugeAddVal2;
      if (gaugeVal2 >= 99) gaugeAddVal2 = -2; // If the value is > or = to 99, make gauge decrease in value by 2
      if (gaugeVal2 <= 0) gaugeAddVal2 = 2; // If the value is < or = to 0, make gauge increase in value by 2

      // The results of this call will be available to myGenieEventHandler() after the display has responded
      // Do a manual read from the UserLed0 object on Display 2
      display2.ReadObject(GENIE_OBJ_USER_LED, 0);

      waitPeriod = millis() + 500; // rerun this code to update Cool Gauge and Slider in another 50ms time.
      // display1.WriteStr(0, "Message 1");
      // display2.WriteStr(0, "Message 2");
    }
  }
}

void set_and_test_pinouts () {

  pinMode (flash, OUTPUT);
  pinMode (mp3, OUTPUT);

  pinMode (LEDCASA, OUTPUT);


  pinMode (w100LED, OUTPUT);
  pinMode (bulb, OUTPUT);
  pinMode (startButtonLED, OUTPUT);
  pinMode (start_button, INPUT_PULLUP);

  Serial.println (F(".Press start button to test..."));
  long time_now = millis ();
  long time_to_wait = 3000;
  while (time_now + time_to_wait > millis ()) {
    if (digitalRead (start_button) == LOW ) {
      Serial.println (F(".Start Button Pressed!"));
      break;
    }
  }

  analogWrite (LEDCASA, 100);
  delay (300);

  analogWrite (w100LED, 100);
  delay (300);

  analogWrite (bulb, 100);
  delay (300);

  digitalWrite (startButtonLED, HIGH);
  delay (300);

  digitalWrite (flash, HIGH);
  delay (300);
  digitalWrite (flash, LOW);

}




// -----------------------------------------------------


/*

   MIDI

*/

#if defined MIDI_ENABLED

void play_note (int index) { 
  // play notes from F#-0 (0x1E) to F#-5 (0x5A) according to the received index:
  unsigned int note = 0x1E;
  for (int count = 0; count < index; count++) {note++;}
  //Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
  noteOn(0x90, note, 0x45);
  delay(100);
  //Note on channel 1 (0x90), some note value (note), silent velocity (0x00):
  // noteOn(0x90, note, 0x00);   
  // delay(100);
}

//  plays a MIDI note.  Doesn't check to see that
//  cmd is greater than 127, or that data values are  less than 127:
void noteOn(int cmd, int pitch, int velocity) {
  SerialMidiWrite(cmd);
  SerialMidiWrite(pitch);
  SerialMidiWrite(velocity);
}

// Serial Midi handlers

void serialMIDIbegin () {
  //  Set MIDI baud rate:
  Serial3.begin(31250);
}

void SerialMidiWrite (int data) {
  Serial3.write(data);
}

#endif

/*

   CAPACITIVE TOUCH SENSOR TTP229_LSF

*/

  // Get pressed capacitive touch channels
  // 2bytes High and Low. 
  // First byte contains 8 channels from 0 to 7. The most significant bit is ch 0 and the least significant bit is ch 7
  // Second byte contains 8 channels from 8 to 15. The most significant bit is ch 8 and the least significant bit is ch 15
 
#if defined CAPTOUCH_ENABLED

void getTTP229data(byte *a, byte *b)
{

  Wire.requestFrom(TTP229_LSF, 2);    // request 6 bytes from slave device #2

  int dataN = 0;
  while(Wire.available())    // slave may send less than requested
  { 
    char c = Wire.read();    // receive a byte as character
    if (dataN == 0) *a = c;
    if (dataN == 1) *b = c;
    dataN++;
  }
}
 
void showTTP229data()
{
  byte aa,bb = 0;
  getTTP229data(&aa,&bb);
  Serial.print("data TTP229 = ");
  printByte(aa);
  printByte(bb);

  Serial.println(";");
  delay(1000);
}
 

void printByte (byte bytePrint) {
   for (unsigned int mask = 0x80; mask; mask >>= 1) {
       if (mask & bytePrint) {
           Serial.print("1");
       }
       else {
           Serial.print("0");
       }
   }
}

#endif


int getButtonPressed() {
  if (!digitalRead (start_button)) return 1;   // Pressed button 1
  // if (!digitalRead (button2)) return 2;   // Pressed button 2
  // if (!digitalRead (button3)) return 3;   // Pressed button 3
  // if (!digitalRead (button4)) return 4;   // Pressed button 4
  // if (!digitalRead (button5)) return 5;   // Pressed button 5
  // if (!digitalRead (button6)) return 6;   // Pressed button 6

  lastButtonPressed = 0;
  return 0;
}



void action_done (int action_number) {
  Serial.print ("action "); 
  Serial.print (action_number); 
  Serial.print (" done @ "); 
  Serial.println ((millis () - startTime));
}



void CheckSerialControl () {
  if (Serial.available () > 0) {    // We have som characters
    char a = Serial.read ();

    if (a == 's') {   // s es de seguent
      currentStep++;  

      for (int g = 0; g<currentStep; g++) {   // reset counters , makes all previous steps done
        stepdone [g] = 1; 
      }

      for (int g = (numberOfSteps-1); g>currentStep; g--) {   // reset counters , makes all future steps not done
        stepdone [g] = 0; 
      }

      nextStepTime = convertTime (currentStep+1);
      if (currentStep == 0) {
        startTime = millis ();
      }else{
        startTime = millis () - convertTime (currentStep);
      }

      Serial.print ("Jumped to step: ");
      Serial.print (currentStep);

      Serial.print (" - Time now: ");
      Serial.print (millis () - startTime);

      Serial.print (" - Next trig time: ");
      Serial.println (nextStepTime);
    }

    if (a == 'a') {   // a de anterior
      if (currentStep != 0) {
        currentStep--;  

        for (int g = 0; g<currentStep; g++) {   // reset counters , makes all previous steps done
          stepdone [g] = 1; 
        }

        for (int g = (numberOfSteps-1); g>currentStep; g--) {   // reset counters , makes all future steps not done
          stepdone [g] = 0; 
        }
        nextStepTime = convertTime (currentStep+1);
        if (currentStep == 0) {
          startTime = millis ();
        }else{
          startTime = millis () - convertTime (currentStep);
        }
      }else{
          startTime = millis ();
      }
      Serial.print ("Jumped to step: ");
      Serial.print (currentStep);

      Serial.print (" - Time now: ");
      Serial.print (millis () - startTime);

      Serial.print (" - Next trig time: ");
      Serial.println (nextStepTime);
    }

    if (a == 'r') { // reset
      clearAllLEDS ();
      Serial.println ("Clear LEDS");
    }

    if (a == 'f') { // full leds
      fullWhite ();     // Stripe at full
      analogWrite (LEDCASA, 100);
      analogWrite (w100LED, 100);
      analogWrite (bulb, 100);
      Serial.println ("Full LEDS");
    }

    if (a == 'p') { // pause
      if (pause_play) {
        Serial.println ("Pause off");
        pause_play = false;
        digitalWrite (mp3, HIGH);
        delay (300);
        digitalWrite (mp3, LOW);
      }else{
        pause_play = true;
        Serial.println ("Pause on");
        digitalWrite (mp3, HIGH);
        delay (300);
        digitalWrite (mp3, LOW);
        delay (300);
        digitalWrite (mp3, HIGH);
        delay (300);
        digitalWrite (mp3, LOW);
      }
    }
  }

  if (pause_play) {   // Reset timers if pause is on
    if (currentStep == 0) {
      startTime = millis ();
    }else{
      startTime = millis () - convertTime (currentStep);
    }
  }
}


void print_commands () {
  Serial.println ("** Commands **");
  Serial.println ("S - seguent");
  Serial.println ("A - anterior");
  Serial.println ("R - RESET tots leds OFF");
  Serial.println ("F - FULL tots leds ON");
  Serial.println ("P - Pause");
  Serial.println ("**************\n");
  Serial.println ("Apreta el boto lluminos per comencar o envia un caracter desde arduino serial\n");
}


void reset_LED_ints () {
  for (int a = 0; a<3; a++) {
    LEDs_int [a] = 0;
    #ifdef DEBUGLEDS
    Serial.print ("write led:");
    Serial.print (4+a);
    Serial.print (" value: ");
    Serial.println (0);
    Serial.println ("reset LEDS INTS");
    #endif
  }
}

void read_LED_ints () {
  for (int a = 0; a<3; a++) {
    LEDs_int [a] = 0;
    #ifdef DEBUGLEDS
    Serial.println ("read LEDS INTS");
    Serial.print ("read led:");
    Serial.print (4+a);
    Serial.print (" value: ");
    Serial.println (0);
    #endif
  }
}

void set_LED_int (int led_num, int new_intensity) {
  LEDs_int [led_num - 4] = new_intensity;
  #ifdef DEBUGLEDS
  Serial.print ("write led:");
  Serial.print (led_num);
  Serial.print (" value: ");
  Serial.println (new_intensity);
  #endif
}

int read_LED_int (int led_num) {
  int result = LEDs_int [led_num - 4];
  #ifdef DEBUGLEDS
  Serial.print ("read led:");
  Serial.print (led_num);
  Serial.print (" value: ");
  Serial.println (result);
  #endif
  return result;
}