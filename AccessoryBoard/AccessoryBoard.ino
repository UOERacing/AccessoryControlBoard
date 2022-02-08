// Authors:
// Omar Hayat, Kian Bazarjani
//
// Hello Omar
// DESCRIPTION:
//    Accessory component functionality for prototype vehicle to be used in Shell Eco-Marathon Competition

// constants won't change. They're used here to 
// set pin numbers:
//const int leftInput = PUSH1; 
//const int rightInput = PUSH2;

const int leftInputWheel = 34;  //34 p4.3 on board // 
const int rightInputWheel = 35; // 35; // p1.2 on board // 

const int hazardInput = 31; //31 ; p8.2 on board // PUSH2; //
const int leftOut =   39; // p2.4 on board //  GREEN_LED; // 
const int rightOut =   40; // p2.5 on board // RED_LED; // 

// variables to read high/low input
//int leftState = 0;
//int rightState = 0;

int leftStateWheel = 0;
int rightStateWheel = 0;
int ledState;
int active = 0;
int hazardState = 0;

unsigned long previousMillis = 0;
unsigned long currentMillis;
const long interval = 300;
void Blink(int pin){
  if(millis() - previousMillis >= interval){
    previousMillis = millis();
    if (ledState == LOW){
        ledState = HIGH;
    }
    else {
      ledState = LOW;
    }
  }
  digitalWrite(pin,ledState);
}
void setup() {

  // pin outputs
  pinMode(leftOut,OUTPUT);
  pinMode(rightOut,OUTPUT);

  // pin inputs
  //pinMode(leftInput,INPUT_PULLUP);
  //pinMode(rightInput, INPUT_PULLUP);

  pinMode(leftInputWheel, INPUT);
  pinMode(rightInputWheel, INPUT);

  
  pinMode(hazardInput, INPUT);
 // Serial.begin(9600);
}

void loop(){
  
  //leftState  = digitalRead(leftInput);
  //rightState = digitalRead(rightInput);
  
  leftStateWheel = LOW; //digitalRead(leftInputWheel);
  rightStateWheel = LOW; //digitalRead(leftInputWheel);
  hazardState = LOW; //digitalRead(hazardInput);

  if (digitalRead(leftInputWheel)== LOW && digitalRead(rightInputWheel)== LOW && digitalRead(hazardInput)== LOW) //blinker not active
    ledState = LOW;
 
  if (digitalRead(hazardInput) == HIGH){
    hazardState = HIGH;
   // Serial.println("Left");
  }
  else if (digitalRead(rightInputWheel) == HIGH){
    rightStateWheel = HIGH;
   // Serial.println("Right");
  }
  else if (digitalRead(leftInputWheel) == HIGH){
    leftStateWheel = HIGH;
   // Serial.println("Hazard");
  }
// Hazard Lights - only pull low if both leftState and rightState are low as well
  if (hazardState == HIGH){
    // blink logic
    Blink(rightOut);
    Blink(leftOut);
   // Serial.println("Haz blink");
    //return;
  } else {
    if (leftStateWheel == HIGH){ //Turn left signal on
    // blink logic
    Blink(leftOut);
    //Serial.println("Left blink");
    //return;
  } else { //Turn left signal off
    digitalWrite(leftOut, LOW);
  }
      if (rightStateWheel == HIGH){ //Turn right signal on
      // blink logic
      Blink(rightOut);
      //Serial.println("Right blink");
    } else { //Turn right signal off
      digitalWrite(rightOut, LOW);
    }
  }
}