
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
}

void loop()
{
    String msg = "";
    // check for data byte on USB serial port
    if (Serial.available())
    {
        // get byte from USB serial port
        while(Serial.available())
        {
            msg = Serial.readString();// read the incoming data as string
        }
            uint8_t buffer = 67;
            Serial.write(&buffer, sizeof (int));
            Serial.write('\n');
    }
}
