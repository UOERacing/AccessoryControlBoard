// Test code to get speed of vehicle, idea -> put the speed up on serial monitor
//every second update speed and print it on serial port

//const int hallSensor_NDInput = A0;     //bridge with P1_3? 
int hallSensor_NDState;
int vitesse;
int temps;
//int nombreDeTours;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(A0, INPUT);
  

}

void loop() {
  // put your main code here, to run repeatedly: 

  //Getting speed
  //Hall sensor when state is above 40? la bolt est devant le capteur hall

  //millis() or micros()

  //check la vitesse chaque seconde ?
  int hallSensor_NDState = analogRead(A0);

  Serial.println(hallSensor_NDState);

  //Serial.println(vitesse);
  
  temps = millis();
  int nombreDeTours = 0;
  
  while(millis()< temps+5000){

    if(hallSensor_NDState > 40){

      nombreDeTours++;
      
       
    }
    Serial.println(nombreDeTours);
    //Serial.println(vitesse);
    
    
    
  
  
 
  }
