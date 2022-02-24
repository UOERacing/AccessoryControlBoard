// Authors:
// Louis-Philippe St-Arnaud, Omar Hayat, Kian Bazarjani
// This is a test file used by Louis-Philippe on an Arduino breadboard test platform with LEDs, so pin numbers will not match
// DESCRIPTION:
//    Accessory component functionality for prototype vehicle to be used in Shell Eco-Marathon Competition
//    This code is for Texas Instruments MSP-EXP430F5529LP boards, compiled in energia.

// These constants won't change. They're used here to 
// set pin numbers (see accessory board documentation):

const int leftInputWheel = 4; // Flasher input on steering, left
const int rightInputWheel = 3; // Flasher input on steering, right

const int hazardInput = 8; // Hazards button on steering
const int leftOut = 12; // Output pin for left flashers
const int rightOut = 11; // Output pin for right flashers

// State variables
int leftStateWheel = LOW;
int rightStateWheel = LOW;
int ledStateLeft = LOW; // Holds the current state of the blinkers
int ledStateRight = LOW;
int ledStateHazards = LOW;
int hazardState = LOW; 

unsigned long previousMillisHazards = 0;
unsigned long previousMillisLeft = 0;
unsigned long previousMillisRight = 0;
const long interval = 300; // Time between blinks (milliseconds)

// Function prototypes for the blinking
void blinkLeft(int);
void blinkRight(int);
void updateblinkers();

////////////////////////////////////////////
//             SETUP FUNCTION             //
////////////////////////////////////////////
void setup() {

  // pin outputs
  pinMode(leftOut,OUTPUT);
  pinMode(rightOut,OUTPUT);

  // pin inputs
  pinMode(leftInputWheel, INPUT);
  pinMode(rightInputWheel, INPUT);
  pinMode(hazardInput, INPUT);
  
  Serial.begin(9600);
}

////////////////////////////////////////////
//            LOOPING FUNCTION            //
////////////////////////////////////////////
void loop(){
  updateBlinkers();
}

// This function reads the blinker state and 
void updateBlinkers() {

  leftStateWheel = digitalRead(leftInputWheel);
  rightStateWheel = digitalRead(rightInputWheel);
  hazardState = digitalRead(hazardInput);

  if (digitalRead(leftInputWheel)== LOW && digitalRead(rightInputWheel)== LOW && digitalRead(hazardInput)== LOW){ //blinker not active
    ledStateLeft = LOW;
    ledStateRight = LOW;
    ledStateHazards = LOW;
}

  // Hazard Lights - only pull low if both leftState and rightState are low as well
  if (hazardState == HIGH){
    // Blink logic
    // millis makes these two
    // run seemingly simultaneously
    blinkHazards();
    Serial.println("Haz blink");
  } else {
    if (leftStateWheel == HIGH) { //Turn left signal on
      // blink logic
      blinkLeft();
      Serial.println("Left blink");
      //return;
    } else { //Turn left signal off
      digitalWrite(leftOut, LOW);
    }

    if (rightStateWheel == HIGH) { //Turn right signal on
      // blink logic
      blinkRight();
      Serial.println("Right blink");
    } else { //Turn right signal off
      digitalWrite(rightOut, LOW);
    }
  }

}

// This function does the actual blinking
void blinkLeft(){
      if(millis() - previousMillisLeft >= interval){
      previousMillisLeft = millis();
      if (ledStateLeft == LOW){
          ledStateLeft = HIGH;
      } else {
        ledStateLeft = LOW;
      }
    }
    digitalWrite(leftOut,ledStateLeft);
}

void blinkRight (){
  
      if(millis() - previousMillisRight >= interval){
      previousMillisRight = millis();
      if (ledStateRight == LOW){
          ledStateRight = HIGH;
      } else {
        ledStateRight = LOW;
      }
    }
    digitalWrite(rightOut,ledStateRight);
}
void blinkHazards (){
  if(millis() - previousMillisHazards >= interval){
      previousMillisHazards = millis();
      if (ledStateHazards == LOW){
          ledStateHazards = HIGH;
      } else {
        ledStateHazards = LOW;
      }
    }
    digitalWrite(rightOut,ledStateHazards);
    digitalWrite(leftOut,ledStateHazards);
}
