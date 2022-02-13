// Authors:
// Louis-Philippe St-Arnaud, Omar Hayat, Kian Bazarjani
//
// DESCRIPTION:
//    Accessory component functionality for prototype vehicle to be used in Shell Eco-Marathon Competition
//    This code is for Texas Instruments MSP-EXP430F5529LP boards, compiled in energia.

// These constants won't change. They're used here to 
// set pin numbers (see accessory board documentation):

const int leftInputWheel = P4_3; // Flasher input on steering, left
const int rightInputWheel = P1_2; // Flasher input on steering, right

const int hazardInput = P8_2; // Hazards button on steering
const int leftOut = P2_4; // Output pin for left flashers
const int rightOut = P2_5; // Output pin for right flashers

// State variables
int leftStateWheel = LOW;
int rightStateWheel = LOW;
int ledState = LOW; // Holds the current state of the blinkers
int hazardState = LOW; 

unsigned long previousMillis = 0;
unsigned long currentMillis;
const long interval = 300; // Time between blinks (milliseconds)

// Function prototypes for the blinking
void blink(int);
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
  
  // Serial.begin(9600);
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

  if (digitalRead(leftInputWheel)== LOW && digitalRead(rightInputWheel)== LOW && digitalRead(hazardInput)== LOW) //blinker not active
    ledState = LOW;

  // Hazard Lights - only pull low if both leftState and rightState are low as well
  if (hazardState == HIGH){
    // Blink logic
    // millis makes these two
    // run seemingly simultaneously
    blink(rightOut);
    blink(leftOut);
    //return;
  } else {
    if (leftStateWheel == HIGH) { //Turn left signal on
      // blink logic
      blink(leftOut);
      //Serial.println("Left blink");
      //return;
    } else { //Turn left signal off
      digitalWrite(leftOut, LOW);
    }

    if (rightStateWheel == HIGH) { //Turn right signal on
      // blink logic
      blink(rightOut);
      //Serial.println("Right blink");
    } else { //Turn right signal off
      digitalWrite(rightOut, LOW);
    }
  }

}

// This function does the actual blinking
void blink(int pin){
  if(millis() - previousMillis >= interval){
    previousMillis = millis();
    if (ledState == LOW){
        ledState = HIGH;
    } else {
      ledState = LOW;
    }
  }
  digitalWrite(pin,ledState);
}
