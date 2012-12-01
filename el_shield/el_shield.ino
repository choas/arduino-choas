/*
 * //////////////////////////////////////////////////
 * //making sense of the Parallax PIR sensor's output
 * //////////////////////////////////////////////////
 *
 * Switches a LED according to the state of the sensors output pin.
 * Determines the beginning and end of continuous motion sequences.
 * Handle two sensors.
 * Two active sensors will start a running session.
 *
 * @author: Kristian Gohlke / krigoo (_) gmail (_) com / http://krx.at
 *          Lars Gregori / lars (_) gregori (_) gmail (_) com / http://crystal-lab.blogger.com
 * @date:   3. September 2006
 *          1. December 2012
 *
 * kr1 (cleft) 2006, 2012
 * released under a creative commons "Attribution-NonCommercial-ShareAlike 2.0" license
 * http://creativecommons.org/licenses/by-nc-sa/2.0/de/
 *
 *
 * The Parallax PIR Sensor is an easy to use digital infrared motion sensor module.
 * (http://www.parallax.com/detail.asp?product_id=555-28027)
 *
 * The sensor's output pin goes to HIGH if motion is present.
 * However, even if motion is present it goes to LOW from time to time,
 * which might give the impression no motion is present.
 * This program deals with this issue by ignoring LOW-phases shorter than a given time,
 * assuming continuous motion is present during these phases.
 *
 */

/////////////////////////////
//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;

//the time when the sensor outputs a low impulse
long unsigned int lowIns[2];

//the amount of milliseconds the sensor has to be low
//before we assume all motion has stopped
long unsigned int pause = 5000;

boolean lockLows[2] = {
  true, true};
boolean takeLowTimes[2];

int pirPins[] = {
  8, 9};    //the digital pin connected to the PIR sensor's output
int runningPin = 10;
int ledPins[] = {
  11, 12};


boolean activePirs[] = {
  false, false};
long unsigned int detectedTimes[2];
boolean running = false;
long int millisecondsPerMeter;

long unsigned int startRunTime;
long unsigned int endRunTime;

int startMeters = 1;
int endMeters = 4;


/////////////////////////////
//SETUP
void setup(){
  Serial.begin(9600);
  pinMode(pirPins[0], INPUT);
  digitalWrite(pirPins[0], LOW);
  pinMode(pirPins[1], INPUT);
  digitalWrite(pirPins[1], LOW);

  pinMode(ledPins[0], OUTPUT);
  digitalWrite(ledPins[0], HIGH);
  pinMode(ledPins[1], OUTPUT);
  digitalWrite(ledPins[1], HIGH);

  pinMode(runningPin, OUTPUT);

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    delay(1000);
  }

  digitalWrite(ledPins[0], LOW);
  digitalWrite(ledPins[1], LOW);

  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}

////////////////////////////
//LOOP
void loop(){

  pirHigh(0);
  pirHigh(1);

  if(activePirs[0] && activePirs[1] && !running) {
    millisecondsPerMeter = detectedTimes[0] - detectedTimes[1];
    startRunTime = abs(millisecondsPerMeter) * startMeters + millis();
    endRunTime = abs(millisecondsPerMeter) * (startMeters + endMeters) + millis();
    running = true;

    Serial.println("####");
    Serial.print("delay 0:");

    Serial.print(detectedTimes[0]);
    Serial.print(" 1:");
    Serial.print(detectedTimes[1]);
    Serial.print(" => ");
    Serial.print(millisecondsPerMeter);

    Serial.print(" ... start in ");
    Serial.print(startRunTime);
    Serial.println("ms ");
  }
  else {

  }

  if (running) {
    if (millis() >= startRunTime && millis() <= endRunTime) {
      digitalWrite(runningPin, HIGH);
      Serial.print("R");
    } else if (millis() > endRunTime) {  // && (!activePirs[0] || !activePirs[1])) {
     
      // reset running flag if at least one PIR isn't active, otherwise a re-run starts
      if (!activePirs[0] || !activePirs[1]) {
        running = false;
      } else {
        Serial.print("r");
      }
    }
    
  }
  else {
    digitalWrite(runningPin, LOW);
  }

  delay(50);

  pirLow(0);
  pirLow(1);
}

void pirHigh(int index) {
  if(digitalRead(pirPins[index]) == HIGH){
    digitalWrite(ledPins[index], HIGH);   //the led visualizes the sensors output pin state
     
    if(lockLows[index]){
      //makes sure we wait for a transition to LOW before any further output is made:
      lockLows[index] = false;
      Serial.println("---");
      Serial.print(index);
      Serial.print(":motion detected at ");
      Serial.print(millis()/1000);
      Serial.println(" sec");
      //delay(50);
    }
    takeLowTimes[index] = true;
    if (!activePirs[index]) {
      detectedTimes[index] = millis();
    }
    activePirs[index] = true;
  }
}

void pirLow(int index) {

  if(digitalRead(pirPins[index]) == LOW){
    digitalWrite(ledPins[index], LOW);  //the led visualizes the sensors output pin state

    if(takeLowTimes[index]){
      lowIns[index] = millis();          //save the time of the transition from high to LOW
      takeLowTimes[index] = false;       //make sure this is only done at the start of a LOW phase
    }
    //if the sensor is low for more than the given pause,
    //we assume that no more motion is going to happen
    if(!lockLows[index] && millis() - lowIns[index] > pause){
      //makes sure this block of code is only executed again after
      //a new motion sequence has been detected

      activePirs[index] = false;
      lockLows[index] = true;
      Serial.print(index);
      Serial.print(":motion ended at ");      //output
      Serial.print((millis() - pause)/1000);
      Serial.println(" sec");
      //delay(50);
    }
  }
}


