#define RED 8 
#define GREEN 9 
#define BLUE 10 
#define ANALOG A0

bool streaming = false;
bool raw_streaming = false;

union Result {
    uint8_t res8;
    uint16_t res16;
    uint32_t res32;
    uint64_t res64;
};

void setup_PWM()
{
  noInterrupts();
  DDRD |= (1 << PD3); // set D3 to output

  TCNT2 = 0;
  TCCR2A = 0;
  TCCR2A = 0;
  TIFR2 = 0;
  TIMSK2 = 0;
  
  TCCR2A |= (1 << COM2B1); // set to reset on top
  
  TCCR2A |= (1 << WGM21) | (1 << WGM20); // Fast PWM OCRA BOTTOM TOP
  TCCR2B |= (1 << WGM22);

   
  TCCR2B |= (1 << CS20) ; // set to 32 times prescale

  OCR2A = 49;
  OCR2B = 24; // set max and interupt time

  //1.25 kHz
  interrupts();
}

void setup() {
    setup_PWM();
  
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    
    digitalWrite(RED, HIGH);
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, HIGH);    
    Serial.begin(115200);
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

        Result result;

        switch (msg) {
            case '0':
                readValues(result, 8);
                Serial.write((uint8_t*)&result, 1);
                break;
            case '1':
                readValues(result, 16);
                Serial.write((uint8_t*)&result, 2);
                break;
            case '2':
                readValues(result, 32);
                Serial.write((uint8_t*)&result, 4);
                break;
            case '3':
                readValues(result, 64);
                Serial.write((uint8_t*)&result, 8);
                break;
            case '5':
                raw_streaming = !raw_streaming;
                break;
            case '4':
                streaming = true;
                break;
            case '8':
                streaming = false;
                break;
            default:
                blink();
                blink();
                blink();
        }
    }

    if (raw_streaming) {
        uint16_t _16bit_value = analogRead(ANALOG);
        Serial.write(reinterpret_cast<const uint8_t*>(&_16bit_value), sizeof(_16bit_value));
    }

    if (streaming) {
      uint8_t arr[1<<9]; //512 B
  
      for (int i = 0; i < 64; i++) {
          Result result;
          readValues(result, 8);
          arr[i] = result.res8;
      }
  
      Serial.write(arr, 1<<9);
    }

}
void readValues(Result &result, uint8_t bitSize) {
    uint8_t byteSize = bitSize / 8;

    result.res64 = 0;

    for (uint8_t i = 0; i < byteSize * 2; i++) {
        int read = analogRead(ANALOG);
        uint8_t nibble = (read >> 0) & 0xF;

        result.res64 |= ((uint64_t)(nibble) << (i * 4));
    }
}
