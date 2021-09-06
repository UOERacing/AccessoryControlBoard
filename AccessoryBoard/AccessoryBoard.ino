// Authors:
// Omar Hayat, Kian Bazarjani
//
// DESCRIPTION:
//    Accessory component functionality for prototype vehicle to be used in Shell Eco-Marathon Competition

// constants won't change. They're used here to 
// set pin numbers:
const int leftInput = PUSH1; // 34; // p4.3 on board // 
const int rightInput = PUSH2; // 35; // p1.2 on board // 
const int hazardInput =  31; // p8.2 on board // PUSH2; //
const int leftOut =   39; // p2.4 on board //  GREEN_LED; // 
const int rightOut =   40; // p2.5 on board // RED_LED; // 

// variables to read high/low input
int leftState = 0;
int rightState = 0;
int hazardState = 0;

void setup() {

  // pin outputs
  pinMode(leftOut,OUTPUT);
  pinMode(rightOut,OUTPUT);

  // pin inputs
  pinMode(leftInput,INPUT_PULLUP);
  pinMode(rightInput, INPUT_PULLUP);
  pinMode(hazardInput, INPUT);
   
}

void loop(){
  
  leftState  = digitalRead(leftInput);
  rightState = digitalRead(rightInput);
  hazardState = digitalRead(hazardInput);
  //digitalWrite(rightOut,LOW);
  
// Hazard Lights - only pull low if both leftState and rightState are low as well
  if (hazardState == LOW && leftState == HIGH && rightState  == HIGH){
    digitalWrite(leftOut,LOW);
    digitalWrite(rightOut,LOW);
    return;
  }else if (hazardState == HIGH){
    // blink logic
    digitalWrite(leftOut,HIGH);
    digitalWrite(rightOut,HIGH);
    delay(300);
    digitalWrite(leftOut,LOW);
    digitalWrite(rightOut,LOW);
    delay(300);
    return;
  }

  // Still need to get left + right signal working on car

  // Left Signal
  if (leftState == HIGH){
    digitalWrite(leftOut,LOW);
  }else if (leftState == LOW){
    // blink logic
    digitalWrite(leftOut,HIGH);
    delay(300);
    digitalWrite(leftOut,LOW);
    delay(300);
  }

  // Right Signal
  if (rightState == HIGH){
    digitalWrite(rightOut,LOW);
  }else if (rightState == LOW){
    // blink logic
    digitalWrite(rightOut,HIGH);
    delay(300);
    digitalWrite(rightOut,LOW);
    delay(300);
  }
  
}
