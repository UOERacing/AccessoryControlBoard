void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(RED_LED, OUTPUT);
  pinMode(A1, INPUT);
    
}

void loop() {
  // put your main code here, to run repeatedly: 


  /*if (frein > 40){
    analogWrite(RED_LED, HIGH);
  }
  */
  
  Serial.println(analogRead(A1));
  
  
}
