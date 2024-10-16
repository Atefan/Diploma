
void setup()
{
    // initialize serial ports
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);    // USB serial port 0
}
void blink() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);  // wait for a second
  delay(100);      
}

void loop()
{
    char * msg = 0;
    // check for data byte on USB serial port
    uint8_t buffer = 67;
    Serial.write(&buffer, sizeof (int));
    Serial.write('\n');

    // check if received messages
    if (Serial.available()>0){
      //clear the buffer
      while(Serial.available())msg = Serial.read();
      //then indicate
      blink();
    }
}
