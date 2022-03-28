#include "Energia.h"

#line 1 "C:/Users/louis/workspace_v11/CSS-Energia_SpeedCode/CSS-Energia_SpeedCode.ino"



  
  
  

  
  
  






void setup();
void loop();

#line 17
const int hallPin = P1_3; 
const int bounce_duration = 20; 
volatile unsigned long bounceTime=0; 
const double radius = 0.2667 ; 
                            
const double pi = 3.14159265358979323846264338327950; 

  double speed = 0; 
  double oldspeed = 1; 
  unsigned long time; 
  unsigned long initialTime; 
  unsigned long finalTime; 
  const int numberOfBolts = 4;
  int counter = 0;
  int pointInCycle = 0; 
                        
                        
  int zeroDelay = 10000; 

  void speedRead();
  void calculateSpeed();
  void recordTime();
  void displaySpeed();
  void zeroReset();

void setup() {
  Serial.begin(9600); 

  pinMode(hallPin, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(hallPin),recordTime,FALLING);
}
void loop() {
  displaySpeed();
  zeroReset();
}




  void recordTime(){
    
    Serial.println("Interrupt read!");
    counter++;
    
    time = millis();
    if (pointInCycle == 2 || pointInCycle == 0){
      pointInCycle = 1;
      Serial.println(pointInCycle);
    } else {
      pointInCycle = 2;
      Serial.println(pointInCycle);
    }
    speedRead();
    
                                            
    }



  void speedRead(){
    
    if (pointInCycle == 1){
      initialTime = time;
      Serial.println("Initial time:");
      Serial.println(initialTime);
    } else if (pointInCycle == 2){
      finalTime = time;
      Serial.println("Final time:");
      Serial.println(finalTime);
      calculateSpeed();
    }
  }


void zeroReset(){
  if (millis() - initialTime >= zeroDelay){
        speed = 0;
        counter = 0;
  }
}



  void calculateSpeed(){
    int millisecondsPerRev; 
    if(pointInCycle == 2){
      millisecondsPerRev = (int)(finalTime - initialTime);
      
      
      speed = (1.00/((millisecondsPerRev*numberOfBolts)/1000.00))*2*pi ; 
      speed = speed*radius; 
      speed = speed*3.6; 
      if (speed > 0 && counter > 2 && (speed >= 50 || speed >= (oldspeed*2))){
        
        Serial.print("Bad : ");
        Serial.print(speed);
        Serial.print("\n");
        pointInCycle=0;
      } else {
        oldspeed = speed;
        initialTime = finalTime; 
        pointInCycle = 1;
        Serial.print(speed);
        Serial.print(" km/h \n");
      }
    }
  }


  void displaySpeed(){
    Serial.print(speed);
    Serial.print(" km/h \n");
    delay(10);
  }



