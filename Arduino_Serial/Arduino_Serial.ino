#define RED 8 
#define GREEN 9 
#define BLUE 10 
#define ANALOG A0
void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, HIGH);    

    //randomSeed(analogRead(0));
    //long randNumber= random(1<<15 - 1);
    Serial.begin(9600);
}
void blink() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);  
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);      
}



void loop() {
    if (Serial.available() > 0) {
        char msg = Serial.read();
        Serial.print(msg);

        uint8_t res1 = 101;
        uint16_t res2 = 101;
        uint32_t res3 = 101;
        uint64_t res4 = 101;

        switch (msg) {
            case '0':  
                readValues(res1);
                Serial.write((uint8_t*)&res1, sizeof(res1));
                break;
            case '1':  
                readValues(res2);
                Serial.write((uint8_t*)&res2, sizeof(res2));
                break;
            case '2':  
                readValues(res3);
                Serial.write((uint8_t*)&res3, sizeof(res3));
                break;
            case '3':
                readValues(res4);
                Serial.write((uint8_t*)&res4, sizeof(res4));
                break;
            default:
                blink();
                blink();
                blink();
        }
        blink();
    }
}
void readValues(uint8_t &res)
{
  res = 0;
  int read = 0;
  for(uint8_t i = 0; i < 4; i++)
  {
    read = analogRead(ANALOG);
    read >>= 6;    
    res  |= read << (i * 2); 
  }
}

void readValues(uint16_t &res)
{
  res = 0;
  int read = 0;
  for(uint8_t i = 0; i < 8; i++)
  {
    read = analogRead(ANALOG);
    read >>= 6;    
    res  |= read << (i * 2); 
  }
}

void readValues(uint32_t &res)
{
  res = 0;
  int read = 0;
  for(uint8_t i = 0; i < 16; i++)
  {
    read = analogRead(ANALOG);
    read >>= 6;    
    res  |= read << (i * 2); 
  }
}

void readValues(uint64_t &res)
{
  res = 0;
  int read = 0;
  for(uint8_t i = 0; i < 32; i++)
  {
    read = analogRead(ANALOG);
    read >>= 6;    
    res  |= read << (i * 2); 
  }
}
