/*

  DrBit.ln

  Projecte Cases. (Casa 1)

  created 13 Jun 2016
  modified 24 Aug 2017
  by DrBit

  This project is part of an art piece called "Projecte cases". The project is a theater play that 
  involves 2 persons (as public) and an miniature house. The public will be presented with a narrative
  sound track and the track itself will call for interaction with the house.
  This will be acomplished through the same audience's own actions togeteher with visual effects and
  sensors and automations triggered by this piece of software.


This project contains parts of code from the following sources:

----------------------------------------
Adafruit NeoPixel Library

https://github.com/adafruit/Adafruit_NeoPixel

Arduino library for controlling single-wire-based LED pixels and strip such as the Adafruit 
60 LED/meter Digital LED strip, the Adafruit FLORA RGB Smart Pixel, the Adafruit Breadboard-friendly 
RGB Smart Pixel, the Adafruit NeoPixel Stick, and the Adafruit NeoPixel Shield.

Compatibility notes: Port A is not supported on any AVR processors at this time
This project uses neopixels with 3 channels only (RGB)


*/


#include <Adafruit_NeoPixel.h>


// Buttons and outputs
#define startButton 7 
#define relay1 4
#define relay2 5
#define relay3 6
#define debugLed1 13

#define pixelsPIN 8
#define BRIGHTNESS 255
#define NUM_LEDS 20

int playButton = relay2;
int stopButton = relay1;
int mist = relay3; 
int lastButtonPressed = 0;

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
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, pixelsPIN, NEO_GRB + NEO_KHZ800);
#define numberOfSteps 15s
int timeActions [numberOfSteps][3] = {
	//{minuts,segons,milisegons}
	{9,05,0},		// Time Action 0 (track duration)


	{0,13,0},		// Time Action 1 - Llum llit fade fast.
	//----------------------------------------------------
	{0,38,0},		// Time Action 2 - Llum baix flaix on
	{0,38,50},		// Time Action 3 - Llum baix flaix off
	{0,38,400},		// Time Action 4 - Llum baix flaix on
	{0,38,500},		// Time Action 5 - Llum baix flaix off
	{0,38,700},		// Time Action 6 - Llum baix flaix on
	{0,38,800},		// Time Action 7 - Llum baix flaix off
	{0,39,300},		// Time Action 8 - Llum baix fade in 4s
	//----------------------------------------------------
	
	{2,01,0},		// Time Action 9 - 2 focos blancs ON DALT
	{2,33,0},		// Time Action 10 - tots focos DALT vermells (fade 3s)
	{3,49,0},		// Time Action 11 - FX 1
	{3,59,0},		// Time Action 12 - FX 2
	{4,25,0},		// Time Action 13 - final FX
	{9,00,0},		// Time Action 14
};


boolean stepdone [numberOfSteps];
	//mist,led1,led2,led3}


void setup () {
	// DEBUG
	Serial.begin(115200);
	Serial.println ("Projecte Cases Start...\n");
	Serial.println ("Casa 1\n");


	Serial.println (".Setting pins");
	pinMode (playButton, OUTPUT);
	pinMode (stopButton, OUTPUT);
	pinMode (mist, OUTPUT);
	pinMode (debugLed1, OUTPUT);
	pinMode (startButton, INPUT_PULLUP);

	digitalWrite (debugLed1, LOW);

	Serial.print (".Track Duration: ");
	trackDuration = convertTime(0);
	Serial.print (trackDuration);
	Serial.println (" milliseconds total");


	Serial.println (".Start neopixels driver");
	pixels.begin();
	pixels.setBrightness(BRIGHTNESS); // set brightness of neopixels
	fullAllLEDS (0xFF0000);
	delay (1000);
	fullAllLEDS (0x00FF00);
	delay (1000);
	fullAllLEDS (0x0000FF);
	delay (1000);
	fullAllLEDS (0xFFFFFF);
	delay (3000);
	clearAllLEDS ();
	resetCounters();
	
	//Serial.println (".Testing neopixels");
	//testing_leds (30000, 5000);		// (milliseconds) total ms, portion for each FX
	Serial.println (".END setup\n");
}

// int color_calid = 0xFFFFFF;
int color_calid = pixels.Color(255, 255, 155);  // R,G,B ?

void loop () {

	int buttonPressed = getButtonPressed ();

	// check if we have to start
	if (buttonPressed) {
		Serial.println ("button pressed. Start play");
  		lastButtonPressed = buttonPressed;

		// Start playing song
		Serial.println ("Starting audio.");
		digitalWrite (playButton, HIGH);
		delay(250);
		digitalWrite (playButton, LOW);
		// in case of sleep (20 seconds);
		delay(500);
		digitalWrite (playButton, HIGH);
		delay(250);
		digitalWrite (playButton, LOW);

		delay(250);
		unsigned long startTime = millis();		// start counting time


		// start timing
		Serial.print (".Track Duration: ");
		trackDuration = convertTime(0);
		Serial.print (trackDuration);
		Serial.println (" milliseconds total");

		// check time in case we have to do something
		int currentStep = 0;
		unsigned long nextStepTime = convertTime (1);

		Serial.println ("Start playing actions.");
		resetCounters ();	// Reset variables
		// Enter into play loop
		while ((millis () - startTime) < trackDuration) {

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
			if (millis () - lastRefresh > 10) {
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
}



void executeAllCurrentSteps (int currentStep) {
	int thisStep =0;
	if (currentStep >= 0) {		// Avoid triggering on -1
		uint32_t color_baix = pixels.Color(255, 100, 10);
		uint32_t color_blanc_dalt = pixels.Color(255, 255, 255);
		uint32_t color_vermell_dalt = pixels.Color(255, 0, 0);
		uint32_t no_color = pixels.Color(0, 0, 0);



		if (currentStep >=0 & !stepdone[0]) {			// Action 0 (from the beginning)
			Serial.println ("step 0");
			clearAllLEDS ();
			stepdone[0] = true;
		}

		if (currentStep >=1 & !stepdone[1]) {			// Time Action 1 - Llum llit fade fast.
			Serial.println ("action 1");
			int a = fadeTo (7,color_baix,3);
			if (a == 1) stepdone[1] = true;
		}
//----------------------------------------------------

		if (currentStep >= 2 & !stepdone[2]) {		// Time Action 2 - Llum baix flaix on
			Serial.println ("action 2");
			int a = fadeTo (8,color_baix,100);
			if (a == 1) stepdone[2] = true;
		}

		if (currentStep >= 3 & !stepdone[3]) {		// Time Action 3 - Llum baix flaix off
			Serial.println ("action 3");
			int a = fadeTo (8,0x000000,100);
			if (a == 1) stepdone[3] = true;
		}

		if (currentStep >=4 & !stepdone[4]) {		// Time Action 4 - Llum baix flaix on
			int a = fadeTo (8,color_baix,100);
			Serial.println ("action 4");
			if (a == 1) stepdone[4] = true;
		}

		if (currentStep >= 5 & !stepdone[5]) {		// Time Action 5 - Llum baix flaix off
			Serial.println ("action 5");
			int a = fadeTo (8,0x000000,100);
			if (a == 1) stepdone[5] = true;
		}

		if (currentStep >= 6 & !stepdone[6]) {		// Time Action 6 - Llum baix flaix on
			Serial.println ("action 6");
			int a = fadeTo (8,color_baix,100);
			if (a == 1) stepdone[6] = true;
		}		

		if (currentStep >= 7 & !stepdone[7]) {		// Time Action 7 - Llum baix flaix off
			Serial.println ("action 7");
			int a = fadeTo (8,0x000000,100);
			if (a == 1) stepdone[7] = true;
		}

		thisStep = 8;
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 8 - Llum baix fade in 4s
			Serial.print ("action "); Serial.println (thisStep);
			int a = fadeTo (8,color_baix,1);
			if (a == 1) stepdone[thisStep] = true;
		}

//----------------------------------------------------

		thisStep = 9;
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 8 - Llum baix fade in 4s
			Serial.print ("action "); Serial.println (thisStep);
			int a = fadeTo (5,color_blanc_dalt,80);
			a = a + fadeTo (6,color_blanc_dalt,80);
			if (a == 2) stepdone[thisStep] = true;
		}

		// {0,40,0},		// Time Action 10 - tots focos DALT vermells (fade 3s)
		thisStep = 10;
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 8 - Llum baix fade in 4s
			Serial.print ("action "); Serial.println (thisStep);
			int a = fadeTo (0,color_vermell_dalt,1);
			a = a + fadeTo (1,color_vermell_dalt,1);
			a = a + fadeTo (2,color_vermell_dalt,1);
			a = a + fadeTo (3,color_vermell_dalt,1);
			a = a + fadeTo (4,color_vermell_dalt,1);
			a = a + fadeTo (5,color_vermell_dalt,1);
			a = a + fadeTo (6,color_vermell_dalt,1);
			if (a == 7) stepdone[thisStep] = true;
		}

		//{0,50,0},		// Time Action 11 - FX 1
		thisStep = 11;
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 8 - Llum baix fade in 4s
			Serial.print ("action "); Serial.println (thisStep);
			int rnum = random(3);
			if (rnum == 1) theaterChase(0x0000FF, 50);
			if (rnum == 2) theaterChase(0x00FF00, 50);

		}

		//{0,50,0},		// Time Action 12 - FX 2
		thisStep = 12;
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 8 - Llum baix fade in 4s
			stepdone[11] = true;
			Serial.print ("action "); Serial.println (thisStep);
			Strobe(100, 100, 1);
			//randomPixels(520);
		}

		//{1,05,0},		// Time Action 13 - final FX */
		thisStep = 13;
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 8 - Llum baix fade in 4s
			stepdone[12] = true;
			Serial.print ("action "); Serial.println (thisStep);			
			int a = fadeTo (0,color_vermell_dalt,1);
			a = a + fadeTo (1,color_vermell_dalt,1);
			a = a + fadeTo (2,color_vermell_dalt,1);
			a = a + fadeTo (3,color_vermell_dalt,1);
			a = a + fadeTo (4,color_vermell_dalt,1);
			a = a + fadeTo (5,color_vermell_dalt,1);
			a = a + fadeTo (6,color_vermell_dalt,1);
			if (a == 7) stepdone[thisStep] = true;
		}

				//{1,05,0},		// Time Action 13 - final FX */
		thisStep = 14;
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 8 - Llum baix fade in 4s
			//stepdone[13] = true;
			Serial.print ("action "); Serial.println (thisStep);			
			int a = fadeTo (0,no_color,30);
			a = a + fadeTo (1,no_color,30);
			a = a + fadeTo (2,no_color,30);
			a = a + fadeTo (3,no_color,30);
			a = a + fadeTo (4,no_color,30);
			a = a + fadeTo (5,no_color,30);
			a = a + fadeTo (6,no_color,30);
			a = a + fadeTo (7,no_color,30);
			a = a + fadeTo (8,no_color,30);
			if (a == 9) stepdone[thisStep] = true;
		}

		//----------------------------------------------------

		pixels.show();
	}
}


boolean fadeTo (int ledNumber, uint32_t newColor, int fadeStep) {

	// get pixel color
	uint32_t actualPixelColor = pixels.getPixelColor(ledNumber);
	if (newColor == actualPixelColor) return true;

	uint8_t
      actual_r = (uint8_t)(actualPixelColor >> 16),
      actual_g = (uint8_t)(actualPixelColor >>  8),
      actual_b = (uint8_t)actualPixelColor;

    uint8_t
      new_r = (uint8_t)(newColor >> 16),
      new_g = (uint8_t)(newColor >>  8),
      new_b = (uint8_t)newColor;

    // primer mirem si hem de sumar o restar
    if ( actual_r < new_r ) {	// Sumem
    	if (actual_r + fadeStep > new_r) {	// Mirem si podem sumar o ens pasarem
    		actual_r = new_r;		// Si ens passem directament agafem el color final
    	}else{		// sino ens pasem simplement sumem el fadeStep 
    		actual_r = actual_r + fadeStep;
    	}
    }else{	// restem
    	if (actual_r - fadeStep < new_r) {
    		actual_r = new_r;
    	}else{
    		actual_r = actual_r - fadeStep;
    	}
    }

    // primer mirem si hem de sumar o restar
    if ( actual_g < new_g ) {	// Sumem
    	if (actual_g + fadeStep > new_g) {	// Mirem si podem sumar o ens pasarem
    		actual_g = new_g;		// Si ens passem directament agafem el color final
    	}else{		// sino ens pasem simplement sumem el fadeStep 
    		actual_g = actual_g + fadeStep;
    	}
    }else{	// restem
    	if (actual_g - fadeStep < new_g) {
    		actual_g = new_g;
    	}else{
    		actual_g = actual_g - fadeStep;
    	}
    }

    // primer mirem si hem de sumar o restar
    if ( actual_b < new_b ) {	// Sumem
    	if (actual_b + fadeStep > new_b) {	// Mirem si podem sumar o ens pasarem
    		actual_b = new_b;		// Si ens passem directament agafem el color final
    	}else{		// sino ens pasem simplement sumem el fadeStep 
    		actual_b = actual_b + fadeStep;
    	}
    }else{	// restem
    	if (actual_b - fadeStep < new_b) {
    		actual_b = new_b;
    	}else{
    		actual_b = actual_b - fadeStep;
    	}
    }

    
    // Serial.print ("Fading led ");
    // Serial.print (ledNumber);
    // Serial.print ("  Actual color: ");
    uint32_t color_now = pixels.Color (actual_r, actual_g, actual_b);
    // Serial.print (color_now,HEX);
    // Serial.print ("  Target color: ");
    // Serial.println (newColor,HEX);
    
    pixels.setPixelColor(ledNumber, color_now);
    return false;		// return false when we are still fading in.
}


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

void clearAllLEDS () {
  uint32_t new_color = 0x000000;
  for(int i=0; i<numberOfPixels; i++) {
    // Just for testing, enables the first led of each circle
    pixels.setPixelColor(i, new_color);
  }
  pixels.show();
}

void fullAllLEDS (uint32_t new_color) {
  for(int i=0; i<numberOfPixels; i++) {
    // Just for testing, enables the first led of each circle
    pixels.setPixelColor(i, new_color);
  }
  pixels.show();
}

int getButtonPressed() {
  if (!digitalRead (startButton)) return 1;   // Pressed button 1
  // if (!digitalRead (button2)) return 2;   // Pressed button 2
  // if (!digitalRead (button3)) return 3;   // Pressed button 3
  // if (!digitalRead (button4)) return 4;   // Pressed button 4
  // if (!digitalRead (button5)) return 5;   // Pressed button 5
  // if (!digitalRead (button6)) return 6;   // Pressed button 6

  lastButtonPressed = 0;
  return 0;
}


////////////////////////////////////////
// TESTS and other externa funcions
////////////////////////////////////////

void testing_leds (unsigned long total_delayFX, unsigned long step_delay) {
 	// int new_color = 0xFFFFFF;
 	ClockPrevTime = millis ();
 	int mode_step=0;
 	unsigned long next_time = step_delay;
 	while (true) {
		// Just for testing, enables the first led of each circle
		if (ClockPrevTime + next_time > millis()) {
			mode_step++;
			next_time =+ step_delay;
		}

		if (mode_step == 0) randomPixels(10);
      	if (mode_step == 1) SpinnyWheel (50);  // The value is the speed of rotation
	    if (mode_step == 2) SpinnyWheelfade (40, 1, 2);  //(int speed, int fade, int num_of_leds)
        if (mode_step == 3) rainbowCycle(1);
		if (mode_step == 4) theaterChaseRainbow (1);
        if (mode_step == 5) Strobe(10,10,2);    // Testing fade
        if (mode_step == 6) mode_step = 0; 

		if (ClockPrevTime + total_delayFX < millis ()) {
			return;
		}
	}
}

void randomPixels(int wait) {
  //Serial.println (wait);
  //Serial.println (millis());
  //Serial.println (millisDelay);
  if (wait + millisDelay < millis()) {
    int randomPixel = random(6);
    int randomPixel2 = random(6);
    pixels.setPixelColor(randomPixel, 0xFFFFFF);
    pixels.setPixelColor(randomPixel2, 0xFFFFFF);
    pixels.show();
    pixels.setPixelColor(randomPixel, 0);
    pixels.setPixelColor(randomPixel2, 0);
    millisDelay = millis();
    //delay(wait);
  }
}

void rainbow(uint8_t wait) {
  if (wait + millisDelay < millis()) {
    uint16_t i;
    counterByte1 ++;
    //for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+counterByte1) & 255));
    }
    pixels.show();
    millisDelay = millis();
      //delay(wait);
    //}
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  if (wait + millisDelay < millis()) {
    uint16_t i;

    if (counterInt1 > 256*5) {
      counterInt1 = 0;
    }else{
      counterInt1 ++;
    }

    //for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
      for(i=0; i< pixels.numPixels(); i++) {
        pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + counterInt1) & 255));
      }
      pixels.show();
      millisDelay = millis();
      //delay(wait);
    //}
  }
}

//////////////////////////////////
//Theatre-style crawling lights.
////////////////////////////////// FIXING test first.

void theaterChase(uint32_t c, uint8_t wait) {
  //if (wait + millis() > millisDelay) {
    for (int j=0; j<5; j++) {  //do 10 cycles of chasing
      for (int q=0; q < 3; q++) {
        for (int i=0; i < 6; i=i+3) {
          pixels.setPixelColor(i+q, c);    //turn every third pixel on

        }
          pixels.setPixelColor(6, c);    //turn every third pixel on

        pixels.show();
       
        delay(wait);
       
        for (int i=0; i < 6; i=i+3) {
          pixels.setPixelColor(i+q, 0);        //turn every third pixel off

        }
          pixels.setPixelColor(6, 0);    //turn every third pixel on

      }
    }
  //}
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  //for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
  if (wait + millisDelay < millis()) {
    counterByte1 ++;
    for (int q=0; q < 3; q++) {
      for (int i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, Wheel( (i+counterByte1) % 255));    //turn every third pixel on
      }
      pixels.show();
      millisDelay = millis();
      //delay(wait);     
      for (int i=0; i < pixels.numPixels(); i=i+3) {
        pixels.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void SpinnyWheel (int wait) {
    if (wait + millisDelay < millis()) {
      int iCal = 10;  // By changing this value will calibrate the first circle starting at a different position
      // And match the movement on the second wheel.

      for(int i=0; i<16; i++) {
        uint32_t c = 0;   // Set default color to black
        int iMapped = i + iCal;   // iMapped is the new value that i will take once re mapped into the new position
        if (iMapped > 15) iMapped = iMapped - 16;  // Control offset and remap leds acordingly

        if(((offset + i) & 7) < 2) c = color; // 4 pixels on... if we are working on a lighted pixel give it default color
        pixels.setPixelColor(iMapped, c); // First eye
        pixels.setPixelColor(31-i, c); // Second eye (flipped)
      }

      pixels.show();
      offset++;
      millisDelay = millis();
    }
    //delay(speed);
}

void SpinnyWheelfade (int speed, int fade, int num_of_leds) {
    int iCal = 10;  // By changing this value will calibrate the first circle starting at a different position
    // And match the movement on the second wheel.

    for(int i=0; i<16; i++) {
      uint32_t c = 0;   // Set default color to black
      int iMapped = i + iCal;   // iMapped is the new value that i will take once re mapped into the new position
      if (iMapped > 15) iMapped = iMapped - 16;  // Control offset and remap leds acordingly

      if(((offset + i) & 7) < num_of_leds) {
        c = pixels.Color(0, 255, 0); // 4 pixels on... if we are working on a lighted pixel give it default color
      }

      if(((offset + i) & 7) == num_of_leds) {
        c = pixels.Color(0, 50, 0);
      }

      if(((offset + i) & 7) == 7) {
        c = pixels.Color(0, 50, 0);
      }

      // if(((offset + i) & 7) == num_of_leds+1) {
      //   c = pixels.Color(0, 10, 0);
      // }

      // if(((offset + i) & 7) == 6) {
      //   c = pixels.Color(0, 10, 0);
      // }

      pixels.setPixelColor(iMapped, c); // First eye
      pixels.setPixelColor(31-i, c); // Second eye (flipped)

    }

    pixels.show();
    offset++;
    delay(speed);
}


const int max_brightness = 15;    // Sets maximal brightnes

// Defin global var for calculating delay
unsigned int amountOfTimeOfFade = 0;
unsigned long brightnessXtime = 0;
void preCalculateAmountFade (int delayTime, int fade) {    // delay for decreassing brightness
    amountOfTimeOfFade = (delayTime / 100) * fade;  // Portion of delay time that will be dedicated to fade
    brightnessXtime =  max_brightness / amountOfTimeOfFade; // Amount of brightnes to increase or decreas per time
}

boolean strobeToggle = false;
boolean doItOnce = true;

void Strobe(uint8_t inDelayTime, uint8_t outDelayTime, uint8_t fade) {
  int new_color = 0xFFFFFF;
  //int fade = 0;   // Fade from 0 to 100, 100 being full fade and 0 no fade

  if (strobeToggle) {   // fade out
    if (doItOnce) {
      for (int k=0; k<7; k++) {    // All leds to one color
        pixels.setPixelColor(k, new_color);
      }
      pixels.show();
      preCalculateAmountFade (outDelayTime, fade); // precalculeta thigns to avoid unnecessary time delays
      doItOnce = false;
    }

    unsigned long timeIntoDelay = millis () - prevTime;
    if (timeIntoDelay < amountOfTimeOfFade) {  // While we are in the fade period time keep changing brightness
      //cada 4 millisgeons reduir 5 fins a max fade
      pixels.setBrightness ((timeIntoDelay * brightnessXtime)-max_brightness);   // Calculate amount of brignes to remove
      pixels.show();
    }
    
    if(prevTime + outDelayTime < millis()) {     // Checks if we are in the portion of delay to fade
      strobeToggle = false;
      doItOnce= true;
      prevTime = millis ();
    }
  }else{    // fade in

    if (doItOnce) {
      for (int k=0; k<7; k++) {    // All leds to one black
        pixels.setPixelColor(k, 0);
      }
      pixels.show();
      preCalculateAmountFade (inDelayTime, fade); // precalculeta thigns to avoid unnecessary time delays
      doItOnce = false;
    }

    unsigned long timeIntoDelay = millis () - prevTime;
    if (timeIntoDelay < amountOfTimeOfFade) {  // While we are in the fade period time keep changing brightness
      //cada 4 millisgeons reduir 5 fins a max fade
      pixels.setBrightness (timeIntoDelay * brightnessXtime);   // Calculate amount of brignes to remove
      pixels.show();
    }

    if(prevTime + inDelayTime < millis()) {     
      strobeToggle = true;
      doItOnce= true;
      prevTime = millis ();
    }
  }
}


void resetCounters () {
	counterByte1 = 0;
	counterInt1 = 0;

	for (int g = 0; g<numberOfSteps; g++) {
		stepdone [g] = 0;
	}

	lastRefresh = 0;
	prevTime = millis();
}
