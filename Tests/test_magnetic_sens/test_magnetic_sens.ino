void setup() {
  pinMode(7, INPUT);
  Serial.begin (115200); 
  Serial.println ("Begin: ");
}

void loop() {
  // put your main code here, to run repeatedly:
  boolean mag_dig = false;
  unsigned int mag_an = 0;
  mag_dig = digitalRead(7);
  mag_an = analogRead (A0);
  Serial.print ("Digital in: ");
  Serial.print (mag_dig);
  Serial.print (" - Analog in: ");
  Serial.println (mag_an);

}
