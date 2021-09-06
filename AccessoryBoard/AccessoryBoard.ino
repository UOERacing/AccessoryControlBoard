// Authors:
// Omar Hayat, Kian Bazarjani
//
// DESCRIPTION:
//    Accessory component functionality for prototype vehicle to be used in Shell Eco-Marathon Competition

// constants won't change. They're used here to 
// set pin numbers:
const int leftInput = 34; // p4.3 on board
const int rightInput = 35; // p1.2 on board
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
  pinMode(leftInput,INPUT);
  pinMode(rightInput, INPUT);
  pinMode(hazardInput, INPUT);
   
}

void loop(){
  
  leftState  = digitalRead(leftInput);
  rightState = digitalRead(rightInput);
  hazardState = digitalRead(hazardInput);
  //digitalWrite(rightOut,LOW);
  
// Hazard Lights
  if (hazardState == LOW){
    digitalWrite(leftOut,LOW);
    digitalWrite(rightOut,LOW);
  }else if (hazardState == HIGH){
    // blink logic
    digitalWrite(leftOut,HIGH);
    digitalWrite(rightOut,HIGH);
    delay(300);
    digitalWrite(leftOut,LOW);
    digitalWrite(rightOut,LOW);
    delay(300);
  }

  // Still need to get left + right signal working on car

  // Left Signal
//  if (leftState == HIGH){
//    digitalWrite(leftOut,LOW);
//  }else if (leftState == LOW){
//    // blink logic
//    digitalWrite(leftOut,HIGH);
//  }

  // Right Signal
//  if (rightState == HIGH){
//    digitalWrite(rightOut,LOW);
//  }else if (rightState == LOW){
//    // blink logic
//    digitalWrite(rightOut,HIGH);
//  }
  
    
//  else if (leftState == HIGH){
//    digitalWrite(leftOut,HIGH);
//  }else if (rightState == HIGH){
//    digitalWrite(rightOut, HIGH);
//  }

}
