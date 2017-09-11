

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif


  Serial.begin (9600);
  pinMode (6, OUTPUT);
  pinMode (5, OUTPUT);


  digitalWrite (6, HIGH);
  analogWrite (5, 0);



}

int in0 = 0;
int in1 = 0;
int in2 = 0;
int in3 = 0;

boolean IDLE = true; 

void loop() {
  while (IDLE) {
    in0 = analogRead(A0);
    in1 = analogRead(A1);
    in2 = analogRead(A2);
    in3 = analogRead(A3);

    in0 = map(in0, 0, 1023, 0, 255);
    in1 = map(in1, 0, 1023, 0, 255);
    in2 = map(in2, 0, 1023, 0, 255);
    in3 = map(in3, 0, 1023, 0, 255);

    analogWrite (5,in0);

    if (in1 > 100) {
    	digitalWrite (6,LOW);
    	delay (300);
    	digitalWrite (6, HIGH);
    }


    Serial.print (in0);
    Serial.print (",");
    Serial.print (in1);
    Serial.print (",");
    Serial.print (in2);
    Serial.print (",");
    Serial.println (in3);

    delay (100);

  }
}