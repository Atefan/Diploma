#define RED 8 
#define GREEN 9 
#define BLUE 10 
void setup()
{
    // initialize serial ports
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, HIGH);    
    // if analog input pin 0 is unconnected, random analog
    // noise will cause the call to randomSeed() to generate
    // different seed numbers each time the sketch runs.
    // randomSeed() will then shuffle the random function.
    randomSeed(analogRead(0));
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
  char *msg;
  long randNumber= random(1<<15 - 1);
  Serial.println(randNumber, BIN);
  // check if received messages
  if (Serial.available()>0){
    //clear the buffer
    while(Serial.available())msg = Serial.read();
    //then indicate
    blink();
  }
}
