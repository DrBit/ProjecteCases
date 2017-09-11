/*

  DrBit.ln

  Projecte Cases. (Casa 3)

  created 24 Aug 2017
  modified 20 Aug 2017
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
This project uses neopixels with 4 channels (RGB and White)


*/

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// DEBUG FLAGS
// #define DEBUG
// #define DEBUGLEDS
// #define DEBUG_NOAUDIO
// #define DEBUG_JUMP

#define refreshRateMs 50  // refresh rate of led calcultaions in milliseconds
// Buttons and outputs
#define startButton 5
#define startButtonLED 7
#define relay1 41
#define relay2 43
#define debugLed1 13
#define pixelsPIN 6
int mp3 = relay1;
int washingmachine = relay2;
int lastButtonPressed = 0;
#define NUM_ELEM(x) (sizeof (x) / sizeof (*(x)))


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

// Neopixesl

#define LED_TYPE NEO_GRBW
// #define LED_TYPE NEO_GRB
#define NUM_LEDS 25

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_LEDS, pixelsPIN, LED_TYPE + NEO_KHZ800);

byte neopix_gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };


// actions timing

int currentStep = 0;		// start from this step
unsigned long startTime	= 0;	
unsigned long nextStepTime = 0;
boolean pause_play = false;

#define numberOfSteps 38
#define NumberOfLedGroups 12

int LedGroups [NumberOfLedGroups][NUM_LEDS] = {
	//{minuts,segons,milisegons}
	{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25},	// Group 0 - tots els dels
	//{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	//----------------------------------------------------
	{1,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	// Group 1 - Habitacio ventilador
	{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	// Group 2 - Toilet
	{5,6,7,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	// Group 3 - Habitacio nines + terrat
	{11,12,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	// Group 4 - Habitacio alce
	{9,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	// Group 5 - Habitacio cames
	{14,15,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	// Group 6 - Habitacio bragas
	{17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	// Group 7 - Cine
	//----------------------------------------------------
	{1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	// Group 8 - Habitacio ventilador NO VERGE
	{16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	// Group 9 - Habitacio bragas NOMES TAULETA
	{5,6,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	// Group 10 - Nines
	{8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},	// Group 11 - Terrat
};

int timeActions [numberOfSteps][3] = {
	//{minuts,segons,milisegons}
	{10,17,0},		// Time Action 0 (duration)

	//----------------------------------------------------
	// Ronda reconeixement
	{0,3,0},		// Time Action 1 - t01 tot vermell fade in
	{0,9,0},		// Time Action 2 - t01 tot vermell fade wiggle? not used now
	{0,11,0},		// Time Action 3 - t01 tot vermell fade out
	{0,18,0},		// Time Action 4 - t02 leds bragas 
	{0,21,0},		// Time Action 5 - t03 wc on
	{0,23,0},		// Time Action 6 - t04 alce on + miralls (sense verge on)
	{0,26,0},		// Time Action 7 - t05 Miralls
	{0,27,0},		// Time Action 8 - t06 bragas
	{0,29,0},		// Time Action 9 - t07 terrat i cine
	{0,30,0},		// Time Action 10 - t08 Alce on
	{0,32,0},		// Time Action 11 - t07 terrat i cine off
	{0,33,0},		// Time Action 12 - t09 tauleta bragas + Alce off	
	{0,36,0},		// Time Action 13 - t09 tauleta bragas off
	//----------------------------------------------------
	// Intro Audio
	{0,36,2},		// Time Action 14 - t10 bragas on
	{1,25,0},		// Time Action 15 - t11 cames on
	{1,29,0},		// Time Action 16 - t10 bragas off
	{2,05,0},		// Time Action 17 - t12 nines i wc on
	{2,30,0},		// Time Action 18 - t13 bragues on
	{2,30,2},		// Time Action 19 - t13 bragues off
	{2,55,0},		// Time Action 20 - t11 cames off
	{3,14,0},		// Time Action 21 - t12 nines i wc off
	{3,15,0},		// Time Action 22 - t14 tauleta i porno
	{4,8,0},		// Time Action 23 - t15 hab bragues
	{4,11,0},		// Time Action 24 - t16 rentadora
	{4,11,500},		// Time Action 25 - t16 rentadora off
	{4,30,0},		// Time Action 26 - t17 alce respiracions
	{4,58,0},		// Time Action 27 - off t14 i t15
	{4,58,2},		// Time Action 28 - t19 nines
	{5,20,0},		// Time Action 29 - t17 alce off
	{5,22,0},		// Time Action 30 - t19 terrat
	{5,23,0},		// Time Action 31 - t18 nines off
	{5,38,0},		// Time Action 32 - t19 terrat off
	{5,38,2},		// Time Action 33 - t20 Nines
	{6,8,0},		// Time Action 34 - T21 Terrat
	{6,23,0},		// Time Action 35 - t20 t21 Nines terrat off
	{6,23,2},		// Time Action 36 - t22 tot vermell 5 fade
	{6,53,0},		// Time Action 37 - t22 tot vermell off 
};


boolean stepdone [numberOfSteps]; //mist,led1,led2,led3}

void setup () {
	// DEBUG
	Serial.begin(250000);
	Serial.println ("Projecte Cases Start...\n");
	Serial.println ("Casa 3\n");


	Serial.println (".Setting pins");
	pinMode (relay1, OUTPUT);
	pinMode (relay2, OUTPUT);
	pinMode (debugLed1, OUTPUT);
	pinMode (startButton, INPUT_PULLUP);
	pinMode (startButtonLED, OUTPUT);

	digitalWrite (debugLed1, LOW);

	Serial.print (".Track Duration: ");
	trackDuration = convertTime(0);
	Serial.print (trackDuration);
	Serial.println (" milliseconds total");

	Serial.println (".Start neopixels driver");
	// pixels.setBrightness(BRIGHTNESS); // set brightness of neopixel
	pixels.begin();
  	// pixels.show(); // Initialize all pixels to 'off'
	
	fullAllLEDS (0xFF0000);
	delay (500);
	fullAllLEDS (0x00FF00);
	delay (500);
	fullAllLEDS (0x0000FF);
	delay (500);
	fullAllLEDS (0xFFFFFF);
	delay (500);
	fullAllLEDS (pixels.Color(0,0,0,255));
	delay (500);
	fullAllLEDS (pixels.Color(255,255,255,255));
	// clearAllLEDS ();
	
	resetCounters();

	int value;
	long time=0;
	int periode = 2000;	
	// TEST_LEDS ();
	Serial.println (".END setup\n");
  	print_commands ();

}

void loop () {

	int buttonPressed = getButtonPressed ();
	#if defined DEBUG
	buttonPressed = true;
	#endif
	// check if we have to start	
	if (buttonPressed || Serial.available() > 0) {
		while (Serial.available() > 0) {
			int a = Serial.read ();
			delay(5);
		}

    	currentStep = 0;    // start from this step

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

		startTime = millis();		// start counting time

		// start timing
		Serial.print (".Track Duration: ");
		trackDuration = convertTime(0);
		Serial.print (trackDuration);
		Serial.println (" milliseconds total");

		nextStepTime = convertTime (currentStep+1);

		Serial.println ("Start playing actions.");
		resetCounters ();	// Reset variables
		clearAllLEDS ();	// Clear all possible left overs
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
}

void executeAllCurrentSteps (int currentStep) {
	int thisStep =0;
	if (currentStep >= 0) {		// Avoid triggering on -1
	//----------------------------------------------------
		uint32_t nocolor = pixels.Color(0,0,0,0);

		thisStep = 0;
		if (currentStep >=thisStep & !stepdone[thisStep]) {			// Action 0 (from the beginning)
			clearAllLEDS ();
			stepdone[thisStep] = true;
		}

		thisStep = 1;
		if (currentStep >=thisStep & !stepdone[thisStep]) {			// Time Action 1 - t01 tot vermell
			// start_timer ();
			int a = GroupfadeTo (0,pixels.Color(255,0,0,0),5);
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
				// stop_timer();
			}
		}
	//----------------------------------------------------

		thisStep = 2;
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 2 - t01 possible efecte
			stepdone[thisStep] = true;
			// action_done (thisStep);
		}


		thisStep = 3;
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - t01 vermell fade off
			int a = GroupfadeTo (0,nocolor,5);
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 4;		// Leds bragas fast on
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (6,pixels.Color(101,0,18,0),1);
			stepdone[thisStep] = true;
			action_done (thisStep);
		}

		thisStep = 5;		// Leds wc on (pampallugues)
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			StrobeWC(pixels.Color(0,190,100,64), pixels.Color(0,25,32,7));
			// int a = GroupfadeTo (2,pixels.Color(0,190,93,64),1);
			// if (a == 1) {
				// stepdone[thisStep] = true;
				// action_done (thisStep);
			// }
		}

		thisStep = 6;		// t04 alce + miralls (no verge)
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a =GroupfadeTo (6,nocolor,1);	// borra bragas
			// a = a+ GroupfadeTo (2,nocolor,1);   // borra wc
			a = a+GroupfadeTo (4,pixels.Color(0,0,255,0),1);   // Hab alce
			a = a+GroupfadeTo (8,pixels.Color(255,0,123,0),1);   // Hab miralls no verge			
			if (a == 3) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 7;		// t05 alce + miralls (no verge)
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (4,nocolor,1);   
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 8;		// t06 bragas
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (6,pixels.Color(255, 0, 0, 22),1);   // Hab bragas
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 9;		// t07 terrat i cine
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (6,nocolor,1);   
			a = a + GroupfadeTo (8,nocolor,1);   // miralls no verge
			a = a + GroupfadeTo (11,pixels.Color(255, 0, 0, 16),1);   // terrat
			a = a + GroupfadeTo (7,pixels.Color(0, 0, 20, 10),1);   // cine
			a = a + GroupfadeTo (5,pixels.Color(253,0,0,253),1);   // cames
			if (a == 5) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}
	//----------------------------------------------------
		
		thisStep = 10;		// t08 alce
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (4,pixels.Color(0,0,255,0),1);   // alce
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 11;		// t07 terrat i cine off
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (11,nocolor,1);   // terrat
			a = a + GroupfadeTo (7,nocolor,1);   // cine
			a = a + GroupfadeTo (5,nocolor,1);   // cames
			if (a == 3) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 12;		// t09 tauleta on  - alce off
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (4,nocolor,1);   // alce
			a = a + GroupfadeTo (9,pixels.Color(253, 0, 0, 16),1);   // tauleta nit
			if (a == 2) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 13;		// t09 taultea off
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (9,nocolor,1);   // tauleta nit
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}
	//----------------------------------------------------

		thisStep = 14;		// t10 bragues
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (6,pixels.Color(101,0,18,0),0);   
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 15;		// t11 cames flash blanc
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			// GroupColor (5,pixels.Color(253,0,0,253),0); 
			StrobeCames (pixels.Color(253,0,0,253), pixels.Color(0,0,0,0));
			// stepdone[thisStep] = true;
			// action_done (thisStep);
		}

		thisStep = 16;		// t10 bragues
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (5,nocolor,0);  
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 17;		// t12 nines i wc
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (10,pixels.Color(101,0,18,0),0);  // nines
			// GroupColor (2,pixels.Color(0,190,93,64),0); // WC efecte fluorescent 
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 18;		// t13 bragues on
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (6,pixels.Color(253, 0, 0, 16),0);  
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 19;		// t13 bragues off
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (6,nocolor,0);  
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}


		thisStep = 20;		// t11 cames off
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (5,nocolor,0);  
			stepdone[15] = true;			// acaba flash cames anterior
			action_done (15);
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 21;		// t12 nines i wc off
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (10,nocolor,0);  
			// GroupColor (2,nocolor,0); // WC efecte fluorescent 
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}
	//----------------------------------------------------

		thisStep = 22;		// t14 tauleta i porno
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (9, pixels.Color(101,0,18,0),0);  // tauleta
			a = a + GroupfadeTo (7, pixels.Color(101,0,18,0),0);  // cine
			if (a == 2) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 23;		// t15 bragues
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (6, pixels.Color(101,0,0,253),0);  // bragues
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 24;		// t16 rentadora
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			Serial.println (".washing machine on!");
			digitalWrite (washingmachine, HIGH);
			stepdone[thisStep] = true;
			action_done (thisStep);
		}

		thisStep = 25;		// t16 rentadora off
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			digitalWrite (washingmachine, LOW);
			stepdone[thisStep] = true;
			action_done (thisStep);
		}

		thisStep = 26;		// t17 Alce
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (6, pixels.Color(101,0,0,253),0);  // bragues
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}
	//----------------------------------------------------

		thisStep = 27;		// t14 t15 off
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - Llum baix flaix off
			int a = GroupfadeTo (9, nocolor,0);  // t14 bragas tauleta
			a = 1 + GroupfadeTo (7, nocolor,0);  // t14
			a = 1 + GroupfadeTo (6, nocolor,0);  // t15 bragas total
			if (a == 3) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 28;		// t19 nines
		if (currentStep >= thisStep & !stepdone[thisStep]) {		
			int a = GroupfadeTo (10, pixels.Color(255,255,255),0);  // nines
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 29;		// t17 alce off
		if (currentStep >= thisStep & !stepdone[thisStep]) {
			int a = GroupfadeTo (4, nocolor,1);  // alces
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 30;		// t19 terrat
		if (currentStep >= thisStep & !stepdone[thisStep]) {
			int a = GroupfadeTo (11,pixels.Color(253,0,0,16),1);
			stepdone[thisStep] = true;
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 31;		// t18 nines off
		if (currentStep >= thisStep & !stepdone[thisStep]) {
			int a = GroupfadeTo (10, nocolor,1);  // nines
			stepdone[thisStep] = true;
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 32;		// t19 terrat off
		if (currentStep >= thisStep & !stepdone[thisStep]) {
			int a = GroupfadeTo (11,nocolor,1);
			stepdone[thisStep] = true;
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 33;		// t20 nines
		if (currentStep >= thisStep & !stepdone[thisStep]) {
			int a = GroupfadeTo (10,pixels.Color(255,255,255,0),1);
			stepdone[thisStep] = true;
			if (a == 1) stepdone[thisStep] = true;
			action_done (thisStep);
		}

		thisStep = 34;		// t21 Terrat
		if (currentStep >= thisStep & !stepdone[thisStep]) {
			int a = GroupfadeTo (11,pixels.Color(0,0,255,0),1);
			stepdone[thisStep] = true;
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 35;		// t20 t21 nines i Terrat off
		if (currentStep >= thisStep & !stepdone[thisStep]) {
			int a = GroupfadeTo (11,nocolor,1);
			a = a + GroupfadeTo (10,nocolor,1);
			stepdone[thisStep] = true;
			if (a == 2) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}


	//----------------------------------------------------

		thisStep = 36;		// t22 tot vermell
		if (currentStep >=thisStep & !stepdone[thisStep]) {	
			// Para WC ***************
			stepdone[5] = true;
			action_done (5);		
			// Para WC ***************
			int a = GroupfadeTo (0,pixels.Color(255,0,0,0),5);
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

		thisStep = 37;
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 3 - t01 vermell fade off
			int a = GroupfadeTo (0,nocolor,5);
			if (a == 1) {
				stepdone[thisStep] = true;
				action_done (thisStep);
			}
		}

/*
// Group 0 - tots els dels
// Group 1 - Habitacio ventilador
// Group 2 - Toilet
// Group 3 - Habitacio nines
// Group 4 - Habitacio alce
// Group 5 - Habitacio cames
// Group 6 - Habitacio bragas
// Group 7 - Cine
// Group 8 - Habitacio ventilador NO VERGE
// Group 9 - Habitacio bragas NOMES TAULETA


led 4 bany
led 8 terrat


		thisStep = 4;
		if (currentStep >=thisStep & !stepdone[thisStep]) {		// Time Action 4 - Llum baix flaix on
			int a = fadeTo (8,color_baix,100);
			Serial.print ("action "); Serial.println (thisStep);
			if (a == 1) stepdone[thisStep] = true;
		}

		thisStep = 5;
		if (currentStep >= thisStep & !stepdone[thisStep]) {		// Time Action 5 - Llum baix flaix off
			Serial.print ("action "); Serial.println (thisStep);
			int a = fadeTo (8,0x000000,100);
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
*/
		//----------------------------------------------------

		pixels.show();
	}
}

// example colling this funciton: fadeTo ( 0, NUM_ELEM(name_of_array) ); 
boolean GroupColor (int ledGroupindex, uint32_t newColor, uint32_t dummy) {
	for (int subIndex = 0; subIndex < NUM_LEDS; subIndex++) {
		int led_final_index = LedGroups[ledGroupindex][subIndex];
		if (led_final_index > 0) pixels.setPixelColor((led_final_index)-1, newColor);
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

	if (ledNumber == 0) return true;	// Led 0 means no led
	// get pixel color
	uint32_t actualPixelColor = pixels.getPixelColor(ledNumber-1);
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

	
	if (LED_TYPE == NEO_GRBW) {			// IF RGBW
  		actual_r = (uint8_t)(actualPixelColor >> 16);
  		actual_g = (uint8_t)(actualPixelColor >> 8);
  		actual_b = (uint8_t)(actualPixelColor);
  		actual_w = (uint8_t)(actualPixelColor >> 24);
  		//-----------------
   		new_r = (uint8_t)(newColor >> 16);
		new_g = (uint8_t)(newColor >> 8);
		new_b = (uint8_t)(newColor);
		new_w = (uint8_t)(newColor >> 24);
	}else{								// RGB				
  		actual_r = (uint8_t)(actualPixelColor >> 16);
  		actual_g = (uint8_t)(actualPixelColor >>  8);
  		actual_b = (uint8_t)actualPixelColor;
  		//-----------------
  		new_r = (uint8_t)(newColor >> 16);
		new_g = (uint8_t)(newColor >>  8);
		new_b = (uint8_t)newColor;
    }


      

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

	if (LED_TYPE == NEO_GRBW) {			// IF RGBW
		// primer mirem si hem de sumar o restar
	    if ( actual_w < new_w ) {	// Sumem
	    	if (actual_w + fadeStep > new_w) {	// Mirem si podem sumar o ens pasarem
	    		actual_w = new_w;		// Si ens passem directament agafem el color final
	    	}else{		// sino ens pasem simplement sumem el fadeStep 
	    		actual_w = actual_w + fadeStep;
	    	}
	    }else{	// restem
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

    uint32_t color_now = pixels.Color (actual_r, actual_g, actual_b, actual_w);
    #if defined DEBUGLEDS
    Serial.print (color_now,HEX);
    Serial.print ("  Target color: ");
    Serial.println (newColor,HEX);
    #endif
    
    pixels.setPixelColor(ledNumber-1, color_now);
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
  for(int i=0; i<NUM_LEDS; i++) {
    // Just for testing, enables the first led of each circle
    pixels.setPixelColor(i, new_color);
  }
  pixels.show();
}

void fullAllLEDS (uint32_t new_color) {
  for(int i=0; i<NUM_LEDS; i++) {
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

 // colorWipe(strip.Color(255, 0, 0), 2050); // Red
  // colorWipe(strip.Color(0, 255, 0), 2050); // Green
  // colorWipe(strip.Color(0, 0, 255), 2050); // Blue
  // colorWipe(strip.Color(0, 0, 0, 255), 2050); // White
        
		//if (mode_step == 0) randomPixels(10);
		if (mode_step == 0) colorWipe(pixels.Color(255, 0, 0,255), 2050);
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
  uint32_t new_color = 0xFFFFFF;
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




int timer_random = 0;
int timer_random_counter = 0;
boolean random_ON_activated = false;
boolean random_OFF_activated = false;
 
void StrobeWC(uint32_t color_on,uint32_t color_off) {

  int led = LedGroups[2][0]-1;
  uint32_t nocolor = 0x000000; 
  int periode = 100; 
  long timing = millis();
  int one = 128+127*cos(2*PI/periode*timing);

  int chance_open = random(50);
  int chance_closed = random(50);

  if (random_ON_activated || random_OFF_activated) {
	timer_random_counter++;
  	if (random_OFF_activated) {
		pixels.setPixelColor(led, color_on);
		if (timer_random_counter == timer_random) {
			timer_random_counter = 0;
			timer_random = 0;
			random_OFF_activated = false;
		}
  	}else{
		pixels.setPixelColor(led, random_OFF_activated);
		if (timer_random_counter == timer_random) {
			timer_random_counter = 0;
			timer_random = 0;
			random_ON_activated = false;
		}
  	}

  }else{
	if (chance_open == 49) {
		random_ON_activated = true;
		timer_random = random (30*6);
	}else if (chance_closed == 7) {
		random_OFF_activated = true;
		timer_random = random (30*4);
	}

	int limit = random(25);
	if (one > limit) {
		if (random (2) == 1) {
		pixels.setPixelColor(led, color_on);
	}else{
		pixels.setPixelColor(led, nocolor);
		}
	}else{
	pixels.setPixelColor(led, color_off);
	}
  }
}

void StrobeCames(uint32_t color_on,uint32_t color_off) {
  int periode = 600; 
  long timing = millis();
  int one = 128+127*cos(2*PI/periode*timing);

  if (one > (100 + random (50))) {
  	GroupColor (5, color_on,0);	// cames
  }else{
  	GroupColor (5, color_off,0);	// cames
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



  // Some example procedures showing how to display to the pixels:
  // colorWipe(strip.Color(255, 0, 0), 2050); // Red
  // colorWipe(strip.Color(0, 255, 0), 2050); // Green
  // colorWipe(strip.Color(0, 0, 255), 2050); // Blue
  // colorWipe(strip.Color(0, 0, 0, 255), 2050); // White


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<pixels.numPixels(); i++) {
    pixels.setPixelColor(i, c);
    pixels.show();
    delay(wait);
  }
}


unsigned long timerclock_begin = 0;
unsigned long timerclock_stop = 0;
boolean timer_started = false;


void start_timer () {
	if (!timer_started) {
		timerclock_begin = millis ();
		timer_started = true;		
	}
}

void stop_timer() {
	timerclock_stop = millis ();
	timer_started = false;
	print_timer();
}

void print_timer() {
	unsigned long timerclock_result = timerclock_stop - timerclock_begin;

	Serial.print ("Elapsed time: ");
	Serial.print (timerclock_result);
	Serial.print ("ms  -  ");
	int minutes = (timerclock_result / 1000) / 60;
	int seconds = (timerclock_result - (minutes*60*1000)) /1000;

	Serial.print (minutes);
	Serial.print (" minutes and ");
	Serial.print (seconds);
	Serial.println (" seconds.");
}


void action_done (int action_number) {
	Serial.print ("action "); 
	Serial.print (action_number); 
	Serial.print (" done @ "); 
	Serial.println ((millis () - startTime));
}



void CheckSerialControl () {
	if (Serial.available () > 0) {		// We have som characters
		char a = Serial.read ();

		if (a == 's') {		// s es de seguent
			currentStep++;	

			for (int g = 0; g<currentStep; g++) {		// reset counters , makes all previous steps done
				stepdone [g] = 1;	
			}

			for (int g = (numberOfSteps-1); g>currentStep; g--) {		// reset counters , makes all future steps not done
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

		if (a == 'a') {		// a de anterior
			if (currentStep != 0) {
				currentStep--;	

				for (int g = 0; g<currentStep; g++) {		// reset counters , makes all previous steps done
					stepdone [g] = 1;	
				}

				for (int g = (numberOfSteps-1); g>currentStep; g--) {		// reset counters , makes all future steps not done
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

		if (a == 'r') {	// reset
			clearAllLEDS ();
			Serial.println ("Clear LEDS");
		}

		if (a == 'f') {	// full leds
			fullAllLEDS (pixels.Color(255,255,255,255));
			Serial.println ("Full LEDS");
		}

		if (a == 'p') {	// pause
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

	if (pause_play) {		// Reset timers if pause is on
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


void TEST_LEDS () {

  	while (true) {
		Serial.println (".Testing");
		Serial.println ("Press button to start");
		int pressed = getButtonPressed();

		while ( digitalRead(startButton) ) {
			int periode = 1000;
	 		long time = millis();
			int value = 128+127*cos(2*PI/periode*time);
			analogWrite(startButtonLED, value);           // sets the value (range from 0 to 255) 
			pressed = getButtonPressed();
		}	
	}


	while (false) {			// test groups
		Serial.println (".Test LED groups\n");
		GroupColor (0, pixels.Color(0,0,255,0), 0);
		for (int a = 1; a < NumberOfLedGroups; a++) {
			GroupColor (0, pixels.Color(0,0,255,0), 0);
			GroupColor (a, pixels.Color(255,255,255,255), 0);
			pixels.show ();
			Serial.println (a);
			while (Serial.available () == 0) {
				// char a = Serial.read ();
			}
			while (Serial.available () > 0) {
				char b = Serial.read ();
			}
		}


		Serial.println ("test Start button pressed");
		analogWrite(startButtonLED, 0);

		Serial.println (".test washing machine");
		digitalWrite (washingmachine, HIGH);
		delay (500);
		digitalWrite (washingmachine, LOW);
		delay (1000);

		Serial.println (".test MP3");
		digitalWrite (mp3, HIGH);
		delay (500);
		digitalWrite (mp3, LOW);
		delay (500);
		digitalWrite (mp3, HIGH);
		delay (500);
		digitalWrite (mp3, LOW);

		Serial.println (".test LEDS");

		int in0 = 0;
		int in1 = 0;
		int in2 = 0;
		int in3 = 0;

		boolean IDLE = true; 


		//////////////////////////////////////
	    in0 = analogRead(A0);
	    in1 = analogRead(A1);
	    in2 = analogRead(A2);
	    in3 = analogRead(A3);

	    in0 = map(in0, 0, 1023, 0, 255);
	    in1 = map(in1, 0, 1023, 0, 255);
	    in2 = map(in2, 0, 1023, 0, 255);
	    in3 = map(in3, 0, 1023, 0, 255);

	    for(uint16_t i=0; i<pixels.numPixels(); i++) {
	      pixels.setPixelColor(i, pixels.Color(in0,in1,in2,in3) );
	    }
	    pixels.show();

	    Serial.print (in0);
	    Serial.print (",");
	    Serial.print (in1);
	    Serial.print (",");
	    Serial.print (in2);
	    Serial.print (",");
    	Serial.println (in3);

    	delay (50);
    }
	Serial.println (".END test\n");

	//testing_leds (30000, 5000);		// (milliseconds) total ms, portion for each FX

	//delay (3000);
}

/*
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
				
				Serial.print ("Time now: ");
				Serial.println (millis () - startTime);
			}
*/