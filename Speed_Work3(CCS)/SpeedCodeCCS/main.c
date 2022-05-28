#include "msp430.h"
#include "driverlib.h"
#include "milliSecond_timer.h"
//screen includes
//#include "ssd1306.h"
#include "i2c.h"
#include "clock.h"
//#include <stdio.h>
// Authors :
// Louis-Philippe St-Arnaud
// DESCRIPTION :
  // This is a basic vehicle speed calculator that uses seconds per revolution
  // to obtain angular velocity and converts it into translational velocity using the wheel radius.
  // Seconds per revolution are obtained continuously through a hall effect sensor and a magnetised bolt on the wheel.
//Version info :
  //V1 (22/02/2022) : Basic testing of angular velocity calculations using a push button, a digital pin and interrupts.
  //                  Reading accuracy still needs to be tested.
  /*V2 (26/02/2022) : Added oldspeed variable for keeping track of speed changes and filtering out unrealistic values.
                      Sometimes bad readings are NOT filetered out, and the solution for that has not been found.
  //                  However, the driver will know when a reading is bad because it jumps back to a good reading very
  //                  quickly. Tested with an Arduino Uno hooked up to the car's hall sensor. Readings were also obtained
  //                  with the TI board but not very successful.
   *V3 (25/03/2022) : Converted code to Code Composer Studio using driverlib, replacing millis function with timing function built by Camry Mabon.
                      Variable elasped_millis contains number of milliseconds since start of Timer A.*/

/*-------------------------------------------------------------------------------------------------------------------*/
//Constants and pin numbers :
#define TIMER_PORT GPIO_PORT_P1
#define TIMER_PIN GPIO_PIN4
#define HALL_PORT GPIO_PORT_P1
#define HALL_PIN  GPIO_PIN3 //Pin P1_3, which goes HIGH when the magnetized bolt passes in front of the sensor. Make sure it can use an interrupt.
const int bounce_duration = 50; //Estimated button bounce duration for software debouncing in milliseconds
volatile unsigned long bounceTime=0; //Variable for debouncing for storing the bounce duration plus the current milliseconds.
const double radius = 0.2667 ; //10.5 inches,  0.2667 meters
                            //More precision will be required to obtain accurate translational velocity results.
const double pi = 3.14159265358979323846264338327950; //You know you need help when your biggest flex is writing this without googling anything.
//Global variables :
  double speed = 0; //Current speed of the vehicle
  double oldspeed = 1; //Previous speed of the vehicle
  unsigned long time; //Time last recorded by the interrupt
  unsigned long initialTime; //Initial time for current revolution
  unsigned long finalTime; //Final time for current revolution
  const int numberOfBolts = 4;
  int counter = 0;
  int button_press;
  int pointInCycle = 0; //Indicates at which point in the speed computation cycle the program is.
                        //0 means no initial time reading has been made or car is immobile, 1 means an initial time reading has been made,
                        //2 means a final time reading has been made.
  int zeroDelay = 10000; //Time in milliseconds after which to reset the speed to zero.
//Function prototypes :
  void speedRead();
  void calculateSpeed();
  void recordTime();
  void displaySpeed();
  void zeroReset();
  void setup();
  void loop();
  //Initiate timer

//main
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	setup();
	//printf("wha\n");
	while(1){
	    loop();
	}
}
//setup
void setup() {
// Pin modes :
  GPIO_setAsInputPin(HALL_PORT, HALL_PIN);
//Interrupt for hall sensor pin
  __enable_interrupt();//Enable all interrupts
  GPIO_enableInterrupt(HALL_PORT, HALL_PIN);
  GPIO_selectInterruptEdge(HALL_PORT, HALL_PIN, GPIO_LOW_TO_HIGH_TRANSITION);//Set interrupt as falling
  //GPIO_clearInterrupt(HALL_PORT, HALL_PIN);
  setup_millisecond_timer();
}
//loop
void loop() {
  displaySpeed();
  zeroReset();
}
//Functions
/*--------Record time(ISR)-----------*/
//Main interrupt vector. This function uses the interrupt to record the times at which the magnetised bolt passes the hall sensor and determines how it should
// be stored (as initial or final time) based on the previous state.
/*#pragma vector=PORT1_VECTOR
__interrupt void time_record(void){
    if(){
    button_press = 1;
    if( elasped_millis > bounceTime) { //The time isn't recorded if the voltage hasn't had the time to stabilize since the last read.
    counter++;
    time = elasped_millis;
    if (pointInCycle == 2 || pointInCycle == 0){
      pointInCycle = 1;
    } else {
      pointInCycle = 2;
    }
    speedRead();
    bounceTime = elasped_millis + bounce_duration;//Add the bounce duration to the current milliseconds to prevent
                                            //recording a second button press too soon when looping back.
    }
    }
    if(){

    }
    //Clear flag?
}*/
/*-------Speed read----------*/
//This function checks for an input from the hall sensor and obtains the time until the next input, which is the time for one rotation.
  void speedRead(){
    //Initial read :
    if (pointInCycle == 1){
      initialTime = time;
    } else if (pointInCycle == 2){
      finalTime = time;
      calculateSpeed();
    }
  }
/*----Zero reset----*/
//Resets speed to zero after a certain delay without a hall effect read.
void zeroReset(){
  if (elasped_millis - initialTime >= zeroDelay){
        speed = 0;
        counter = 0;
  }
}
/*------Calculate speed --------*/
//This function uses the initial and final time to compute the speed and saves it in the speed variable for the display to use.
//It also initiates a new cycle based on the previous final time.
  void calculateSpeed(){
    int millisecondsPerRev; //Time taken for the current current revolution
    if(pointInCycle == 2){
      millisecondsPerRev = (int)(finalTime - initialTime);
      //Computation of speed :
      speed = (1.00/((millisecondsPerRev*numberOfBolts)/1000.00))*2*pi ; //This part gives the angular velocity in radians/second.
      speed = speed*radius; //Multiplying radians/second times meters/radian yields meters/second.
      speed = speed*3.6; //Convert to km/h
      //if (speed > 0 && counter > 2 && (speed >= 50 || speed >= (oldspeed*2))){
        //discard speed because of bounce
        pointInCycle=0;
      //} else {
        oldspeed = speed;
        initialTime = finalTime; //Start a new cycle based on the previous final reading
        pointInCycle = 1;
      //}
    }
  }
/*------Display speed-----*/
//This function prints the current speed to the serial monitor (and eventually the LCD).
  void displaySpeed(){

  }
  void timer(){
      //Include commands for starting a timer based on button presses and lap counts (Pin 1.4 is timer reset button)
      //Reset lap count when pressed down for 3 seconds
      int timerPinState;
      unsigned long startMillis;
      unsigned long displayMillis;
      unsigned long displaySeconds;
      timerPinState = GPIO_getInputPinValue(TIMER_PORT,TIMER_PIN);
      if (timerPinState == GPIO_INPUT_PIN_HIGH){
          startMillis = elasped_millis;
          //add some sort of lap keeping function and 3 second press reset
      }
      displayMillis = elasped_millis - startMillis;
      displaySeconds = displayMillis/1000;
      //fun screen functions
  }
