#include "msp430.h"
#include "driverlib.h"
#include "milliSecond_timer.h"
#define leftInputWheelPort GPIO_PORT_P4
#define leftInputWheelPin  GPIO_PIN3

#define rightInputWheelPort GPIO_PORT_P1
#define rightInputWheelPin GPIO_PIN2

#define hazardInputPort GPIO_PORT_P8
#define hazardInputPin GPIO_PIN2

#define leftOutPort GPIO_PORT_P2
#define leftOutPin GPIO_PIN4

#define rightOutPort GPIO_PORT_P2
#define rightOutPin GPIO_PIN5

//int HIGH = 1;
//int LOW = 0;


// State variables
int leftStateWheel = 0;
int rightStateWheel = 0;
int ledStateLeft = 0; // Holds the current state of the blinkers
int ledStateRight = 0;
int ledStateHazards = 0;
int hazardState = 0;
int sumRight;
int sumLeft;
int sumHazard;

unsigned long previousMillisHazards = 0;
unsigned long previousMillisLeft = 0;
unsigned long previousMillisRight = 0;
const long interval = 100; // Time between blinks (milliseconds)

// Function prototypes for the blinking
void blinkLeft();
void blinkRight();
void updateblinkers();
void blinkHazards ();
void updateBlinkers();
void setup();
void loop();

////////////////////////////////////////////
//             SETUP FUNCTION             //
////////////////////////////////////////////
int main(void)
{
    WDT_A_hold(WDT_A_BASE);
    setup();
    while(1){
        loop();
    }

}
    void setup()
    {

      // pin outputs
      GPIO_setAsOutputPin(leftOutPort,leftOutPin);
      GPIO_setAsOutputPin(rightOutPort,rightOutPin);

      // pin inputs
      GPIO_setAsInputPin(leftInputWheelPort,leftInputWheelPin);
      GPIO_setAsInputPin(rightInputWheelPort,rightInputWheelPin);
      GPIO_setAsInputPin(hazardInputPort,hazardInputPin);

      __enable_interrupt();
      setup_millisecond_timer();

    }


    ////////////////////////////////////////////
    //            LOOPING FUNCTION            //
    ////////////////////////////////////////////
    void loop(){
      updateBlinkers();
    }

    // This function reads the blinker state and
    void updateBlinkers() {

        //get many readings from pins
        int ix;
        sumLeft = 0;
        sumRight = 0;
        sumHazard = 0;
        for (ix =0; ix<100; ix++){
            sumLeft = sumLeft + GPIO_getInputPinValue(leftInputWheelPort,leftInputWheelPin);
            sumRight = sumRight + GPIO_getInputPinValue(rightInputWheelPort,rightInputWheelPin);
            sumHazard = sumHazard + GPIO_getInputPinValue(hazardInputPort,hazardInputPin);
        }

        if(sumLeft/100 > 0.5){
                        leftStateWheel= GPIO_INPUT_PIN_HIGH;
                    } else {
                        leftStateWheel = GPIO_INPUT_PIN_LOW;
                    }
        if (sumRight/100 > 0.5){
                        rightStateWheel = GPIO_INPUT_PIN_HIGH;
                    } else {
                        rightStateWheel = GPIO_INPUT_PIN_LOW;
                    }
        if (sumHazard/100 > 0.5){
                        hazardState = GPIO_INPUT_PIN_HIGH;
                    } else {
                        hazardState = GPIO_INPUT_PIN_LOW;
                    }

            /*if (leftStateWheel== GPIO_INPUT_PIN_LOW && rightStateWheel== GPIO_INPUT_PIN_LOW && hazardState== GPIO_INPUT_PIN_LOW){ //blinker not active
               ledStateLeft = GPIO_INPUT_PIN_LOW;
               ledStateRight = GPIO_INPUT_PIN_LOW;
               ledStateHazards = GPIO_INPUT_PIN_LOW;
            }*/
            // Hazard Lights - only pull low if both leftState and rightState are low as well
             if (hazardState == GPIO_INPUT_PIN_HIGH){
               // Blink logic
               // millis makes these two
               // run seemingly simultaneously
               blinkHazards();

             } else {
               if (leftStateWheel == GPIO_INPUT_PIN_HIGH) { //Turn left signal on
                 // blink logic
                 blinkLeft();

               } else { //Turn left signal off
                   GPIO_setOutputLowOnPin(leftOutPort,leftOutPin);

               }

               if (rightStateWheel == GPIO_INPUT_PIN_HIGH) { //Turn right signal on
                 // blink logic
                 blinkRight();
               } else { //Turn right signal off
                 GPIO_setOutputLowOnPin(rightOutPort,rightOutPin);

               }
             }

           }

      // This function does the actual blinking
      void blinkLeft(){
          if (elasped_millis - previousMillisLeft >= interval){
          previousMillisLeft = elasped_millis;
          if (ledStateLeft == GPIO_INPUT_PIN_LOW){
              ledStateLeft = GPIO_INPUT_PIN_HIGH;
              GPIO_setOutputHighOnPin(leftOutPort,leftOutPin);
          } else {
              ledStateLeft = GPIO_INPUT_PIN_LOW;
              GPIO_setOutputLowOnPin(leftOutPort,leftOutPin);
          }
        }
      }


      void blinkRight (){

            if(elasped_millis - previousMillisRight >= interval){
            previousMillisRight = elasped_millis;
            if (ledStateRight == GPIO_INPUT_PIN_LOW){
                ledStateRight = GPIO_INPUT_PIN_HIGH;
                GPIO_setOutputHighOnPin(rightOutPort,rightOutPin);
            } else {
              ledStateRight = GPIO_INPUT_PIN_LOW;
              GPIO_setOutputLowOnPin(rightOutPort,rightOutPin);
            }
          }
      }

      void blinkHazards (){
        if(elasped_millis - previousMillisHazards >= interval){
            previousMillisHazards = elasped_millis;
            if (ledStateHazards == GPIO_INPUT_PIN_LOW){
                ledStateHazards = GPIO_INPUT_PIN_HIGH;
                GPIO_setOutputHighOnPin(rightOutPort,rightOutPin);
                GPIO_setOutputHighOnPin(leftOutPort,leftOutPin);
            } else {
              ledStateHazards = GPIO_INPUT_PIN_LOW;
              GPIO_setOutputLowOnPin(rightOutPort,rightOutPin);
              GPIO_setOutputLowOnPin(leftOutPort,leftOutPin);
            }
          }
     }




