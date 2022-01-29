// Authors:
// Omar Hayat, Kian Bazarjani
//
// DESCRIPTION:
//    Accessory component functionality for prototype vehicle to be used in Shell Eco-Marathon Competition

// constants won't change. They're used here to 
// set pin numbers:
const int leftInput = PUSH1; //S1 //p2_1 on board //
const int rightInput = PUSH2; //S2 //p1_1 on board //

const int leftInputWheel = 34; // 34; // p4.3 on board // 
const int rightInputWheel = 35; // 35; // p1.2 on board // 

const int hazardInput =  31; // p8.2 on board // PUSH2; //
const int leftOut =   39; // p2.4 on board //  GREEN_LED; // 
const int rightOut =   40; // p2.5 on board // RED_LED; // 

const int hallSensor_NDInput = A0; // 77 // P6_0 on board //

// variables to read high/low input
int leftState = 0;
int rightState = 0;

int leftStateWheel = 0;
int rightStateWheel = 0;


int hazardState = 0;

int hallSensor_NDState;

int currentTime;
int ctr;

void setup() {

  // pin outputs
  pinMode(leftOut,OUTPUT);
  pinMode(rightOut,OUTPUT);

  // pin inputs
  pinMode(leftInput,INPUT_PULLUP);
  pinMode(rightInput, INPUT_PULLUP);

  pinMode(leftInputWheel, INPUT);
  pinMode(rightInputWheel, INPUT);

  
  pinMode(hazardInput, INPUT);
  Serial.begin(9600);
  currentTime = millis();
  ctr = 0;
}

void loop(){
 /* 
  int temp_time = millis();
  
  if (temp_time - currentTime > 1000){
    ctr++;
    currentTime = temp_time;
    Serial.print(ctr);
    Serial.println(" seconds have elapsed.");
    */
  }
  leftState  = digitalRead(leftInput);
  rightState = digitalRead(rightInput);

  leftStateWheel = digitalRead(leftInputWheel);
  rightStateWheel = digitalRead(leftInputWheel);
  
  hazardState = digitalRead(hazardInput);

  hallSensor_NDState = analogRead(hallSensor_NDInput);
  
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

  // Left Signal
  if (leftState == HIGH || leftStateWheel == LOW){
    digitalWrite(leftOut,LOW);
  }else if (leftState == LOW || leftStateWheel == HIGH){
    // blink logic
    digitalWrite(leftOut,HIGH);
    delay(300);
    digitalWrite(leftOut,LOW);
    delay(300);
  }

  // Right Signal 
  if (rightState == HIGH || rightStateWheel == LOW){
    digitalWrite(rightOut,LOW);
  }else if (rightState == LOW || rightStateWheel == HIGH){
    // blink logic
    digitalWrite(rightOut,HIGH);
    delay(300);
    digitalWrite(rightOut,LOW);
    delay(300);
  }

  // Hall sensor, when state is above 40??? the steel bolt is infront of sensor.
  // Getting speed
  if(hallSensor_NDState > 40){

  }
  
}
