/* 
tooth brushing timer
 */

const int LED_PIN_0 = 0;
const int LED_PIN_1 = 1;
const int LED_PIN_2 = 2;
const int LED_PIN_3 = 3;

int ledPin = LED_PIN_0;
int endPin = LED_PIN_0;

int counter = 0;
boolean stop = false;
int ledState = LOW;             // ledState used to set the LED
long previousMillis = 0;        // will store last time LED was updated

long interval = 1000;           // interval at which to blink (milliseconds)

void setup() {
    DDRB = B11111111;  // set all pins as OUTPUT
  
    pinMode(LED_PIN_0, OUTPUT);
    digitalWrite(LED_PIN_0, LOW);
    pinMode(LED_PIN_1, OUTPUT);
    digitalWrite(LED_PIN_1, LOW);
    pinMode(LED_PIN_2, OUTPUT);
    digitalWrite(LED_PIN_2, LOW);
    pinMode(LED_PIN_3, OUTPUT);
    digitalWrite(LED_PIN_3, LOW);
}

void loop()
{
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > interval) {
    
    if (!stop) {
    
      // save the last time you blinked the LED 
      previousMillis = currentMillis;   
  
      // if the LED is off turn it on and vice-versa:
      if (ledState == LOW)
        ledState = HIGH;
      else
        ledState = LOW;
  
      // set the LED with the ledState of the variable:
      digitalWrite(ledPin, ledState);
      counter++;
      
      if (counter >= 30) {
        counter = 0;
        digitalWrite(ledPin, LOW);
        ledPin++;
        if (ledPin > LED_PIN_3) {
          stop = true;
          interval = interval / 4;
        }
      }
    } else if (stop) {
      previousMillis = currentMillis;

      digitalWrite(LED_PIN_0, LOW);
      digitalWrite(LED_PIN_1, LOW);
      digitalWrite(LED_PIN_2, LOW);
      digitalWrite(LED_PIN_3, LOW);

      digitalWrite(endPin, HIGH);
      endPin++;
      if (endPin > LED_PIN_3) {
        endPin = LED_PIN_0;
      }
    }
  }
}

