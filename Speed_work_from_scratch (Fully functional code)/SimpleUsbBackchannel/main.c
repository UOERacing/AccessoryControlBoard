/* --COPYRIGHT--,BSD
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//******************************************************************************
//   MSP430F5529LP:  simpleUsbBackchannel example
//
//   Description: 	Demonstrates simple sending over USB, as well as the F5529's
//                  backchannel UART.
//
//   Texas Instruments Inc.
//   August 2013
//******************************************************************************

// Basic MSP430 and driverLib #includes
//Modified by LP to include speed reading code
//Current issue: interrupt does not let program return to normal operation after triggering. Solved by introducing clear command at end of ISR.
//Code currently functions with triggering by a switch with pull down resistor. Zero reset has been tested. Next step is adding screen displaying functionality and testing it.
//Also need to add interrupt based timer functionality.
//Confirmed working with blinking lights, but speed seems to get "stuck" at zero after a while. Investigation required. Phenomenon seems to occur right after milliseconds overflow.
//It turned out to be the conditional statement checking for debouncing, which did not take into account the fact that the millisecond count can overflow and return to zero.
//Last steps: add screen and timer functionality.
//Screen commands will not work and halt code unless screen is connected (duh). Uncomment before proceeding.
//The clocks screen command seems to mess up serial communication by changing the baud rate or some other parameter.
//The timer now works after sorting out all the interferences. Any sort of attempt at i2c while the screen is not connected freezes the program.
/*
With the car, the seconds stop counting when the hall sensor is first triggered. Thankfully the screen works!
Commented out timer button code. It's now a simple running timer until the button on the steering wheel is wired.
 */
#include "msp430.h"
#include "driverlib/MSP430F5xx_6xx/wdt_a.h"
#include "driverlib/MSP430F5xx_6xx/ucs.h"
#include "driverlib/MSP430F5xx_6xx/pmm.h"
#include "driverlib/MSP430F5xx_6xx/sfr.h"
#include "milliSecond_timer.h"
#include "ssd1306.h"
#include "i2c.h"
#include "clock.h"
#include "Second_timer_B.h"

// USB API #includes
#include "USB_config/descriptors.h"
#include "USB_API/USB_Common/device.h"
#include "USB_API/USB_Common/types.h"
#include "USB_API/USB_Common/usb.h"

#include "USB_app/usbConstructs.h"

// Application #includes
#include "BCUart.h"           // Include the backchannel UART "library"
#include "hal.h"              // Modify hal.h to select your hardware
//Constants and pin numbers :
#define TIMER_PORT GPIO_PORT_P1
#define TIMER_PIN GPIO_PIN4
#define HALL_PORT GPIO_PORT_P1
#define HALL_PIN  GPIO_PIN3 //Pin P1_3, which goes HIGH when the magnetized bolt passes in front of the sensor. Make sure it can use an interrupt.
const int bounce_duration = 50; //Estimated button bounce duration for software debouncing in milliseconds
volatile uint16_t prevMillis=0; //Variable for debouncing for storing the bounce duration plus the current milliseconds.
const double radius = 0.2667 ; //10.5 inches,  0.2667 meters
                            //More precision will be required to obtain accurate translational velocity results.
const double pi = 3.14159265358979323846264338327950; //You know you need help when your biggest flex is writing this without googling anything.
//Global variables :
  double speed = 0; //Current speed of the vehicle
  uint32_t displaySpeed;
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
  int timerOn = 0;
  int speedRefresh;
  int carMove;
  uint16_t minutes;
  uint32_t displaySeconds;
  uint32_t startSeconds;
  uint32_t endSeconds;
  int zeroDelay = 5000; //Time in milliseconds after which to reset the speed to zero.
//Blinking functions:
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

uint16_t previousMillisHazards = 0;
uint16_t previousMillisLeft = 0;
uint16_t previousMillisRight = 0;
//uint16_t refrMillis;
//uint16_t refrRate = 500;
const uint16_t interval = 300; // Time between blinks (milliseconds)

//screen variables
extern unsigned char *PTxData;                  // Pointer to TX data, defined in i2c.h
extern unsigned char TXByteCtr;                 // number of bytes to transmit, defined in i2c.h

// Function prototypes for the blinking
void blinkLeft();
void blinkRight();
void updateblinkers();
void blinkHazards ();
void updateBlinkers();
void setup();
void loop();

  //Function prototypes :
  void speedRead();
  void calculateSpeed();
  void recordTime();
  void serialSpeed();
  void display();
  void zeroReset();

/* You have a choice between implementing this as a CDC USB device, or a HID-
 * Datapipe device.  With CDC, the USB device presents a COM port on the host;
 * you interact with it with a terminal application, like Hyperterminal or
 * Docklight.  With HID-Datapipe, you interact with it using the Java HID Demo
 * App available within the MSP430 USB Developers Package.
 *
 * By default, this app uses CDC.  The HID calls are included, but commented
 * out.
 *
 * See the F5529 LaunchPad User's Guide for simple instructions to convert
 * this demo to HID-Datapipe.  For deeper information on CDC and HID-Datapipe,
 * see the USB API Programmer's Guide in the USB Developers Package.
 */


void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;		// Halt the dog

    // MSP430 USB requires a Vcore setting of at least 2.  2 is high enough
	// for 8MHz MCLK, below.
    PMM_setVCore(PMM_CORE_LEVEL_2);
    initPorts();           // Config all the GPIOS for low-power (output low)
    initClocks(8000000);   // Config clocks. MCLK=SMCLK=FLL=8MHz; ACLK=REFO=32kHz
    bcUartInit();          // Init the back-channel UART
    USB_setup(TRUE,TRUE);  // Init USB; if a USB host (PC) is present, connect
    __enable_interrupt();  // Enable interrupts globally
    setup_millisecond_timer();
    //For accessory screen
    clock_init();

        P1DIR |= BIT5;                              // P1.5 output
        P1OUT |= BIT5;                              // P1.5 high to turn on power to display

     i2c_init();                                 // initialize UCB1 I2C, port 4 pins 1, 2

     ssd1306_init();                             // initialize SSD1306 OLED
     ssd1306_clearDisplay();                     // clear garbage data

     ssd1306_printText(0,4,"       Run time:");
     ssd1306_printText(0,0, "Vehicle speed (km/h):");
    setup_seconds_timer();
    timerOn = 1;
    //Blinkers pins
    // pin outputs
          GPIO_setAsOutputPin(leftOutPort,leftOutPin);
          GPIO_setAsOutputPin(rightOutPort,rightOutPin);

          // pin inputs
          GPIO_setAsInputPinWithPullDownResistor(leftInputWheelPort,leftInputWheelPin);
          GPIO_setAsInputPinWithPullDownResistor(rightInputWheelPort,rightInputWheelPin);
          GPIO_setAsInputPinWithPullDownResistor(hazardInputPort,hazardInputPin);
    // Pin modes :
          GPIO_setAsInputPinWithPullDownResistor(HALL_PORT, HALL_PIN);
        //Interrupt for hall sensor pin
          GPIO_enableInterrupt(HALL_PORT, HALL_PIN);
          GPIO_selectInterruptEdge(HALL_PORT, HALL_PIN, GPIO_HIGH_TO_LOW_TRANSITION);//Set interrupt as falling for actual sensor, rising for tests
          GPIO_clearInterrupt(HALL_PORT, HALL_PIN);
          //Interrupt for timer pin
          //GPIO_enableInterrupt(TIMER_PORT,TIMER_PIN);
          //GPIO_selectInterruptEdge(TIMER_PORT, TIMER_PIN, GPIO_LOW_TO_HIGH_TRANSITION);//Set interrupt as falling for actual sensor, rising for tests
          //GPIO_clearInterrupt(TIMER_PORT, TIMER_PIN);

    while(1)
    {
       // Look for rcv'ed bytes on the backchannel UART. If any, send over USB.
       zeroReset();
       display();
       updateBlinkers();
    }
    //For screen
    //__bis_SR_register(LPM0_bits + GIE);         // Enter LPM0, enable interrupts
    //__no_operation();
}
    //Functions
    /*--------Record time(ISR)-----------*/
    //Main interrupt vector. This function uses the interrupt to record the times at which the magnetised bolt passes the hall sensor and determines how it should
    // be stored (as initial or final time) based on the previous state.
    #pragma vector=PORT1_VECTOR
    __interrupt void time_record(void){
        button_press = 1;
        carMove=1;
       // if(GPIO_getInputPinValue(TIMER_PORT,TIMER_PIN)){
            //For checking which pin triggered what.
           /*if(timerOn){
               timerOn=0;
               displaySeconds = seconds - startSeconds;
           } else {
               timerOn=1;
               startSeconds = seconds;
           }
           GPIO_clearInterrupt(TIMER_PORT,TIMER_PIN);*/
       //} else {

        if( (elasped_millis - prevMillis) > bounce_duration)
        { //The time isn't recorded if the voltage hasn't had the time to stabilize since the last read.
        counter++;
            time = elasped_millis;
            if (pointInCycle == 2 || pointInCycle == 0)
            {
              pointInCycle = 1;
            }
            else
            {
              pointInCycle = 2;
            }
            speedRead();
            prevMillis = elasped_millis;//Add the bounce duration to the current milliseconds to prevent
                                                    //recording a second button press too soon when looping back.
        }
        //serialSpeed();
        GPIO_clearInterrupt(HALL_PORT,HALL_PIN);
        //}
    }
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
            pointInCycle=0;
            initialTime=0;
            if(carMove){
                ssd1306_printText(0,2,"                    ");
                ssd1306_printUI32(0,2,(uint32_t)speed,HCENTERUL_ON);
            }
            carMove=0;
      }
    }
    /*------Calculate speed --------*/
    //This function uses the initial and final time to compute the speed and saves it in the speed variable for the display to use.
    //It also initiates a new cycle based on the previous final time.
      void calculateSpeed(){
        int millisecondsPerRev; //Time taken for the current current revolution
          millisecondsPerRev = (int)(finalTime - initialTime);
          oldspeed=speed;
          //Computation of speed :
          speed = (1.00/((millisecondsPerRev*numberOfBolts)/1000.00))*2*pi ; //This part gives the angular velocity in radians/second.
          speed = speed*radius; //Multiplying radians/second times meters/radian yields meters/second.
          speed = speed*3.6; //Convert to km/h
          //if (speed > 0 && counter > 2 && (speed >= 50 || speed >= (oldspeed*2))){
            //discard speed because of bounce
            //pointInCycle=0;
          initialTime = finalTime; //Start a new cycle based on the previous final reading
          pointInCycle = 1;
          speedRefresh=1;
        }
    /*------Display speed-----*/
    //This function prints the current speed to the serial monitor (and eventually the LCD).
      void serialSpeed(){
          uint16_t displayspeed = (uint16_t)speed;
          bcUartSend((&displayspeed),sizeof(displayspeed));
          bcUartSend(&(elasped_millis),sizeof(elasped_millis));
      }
      void display(){
          //Timer display
          //if(timerOn){
          if( (speed - (uint32_t)speed) >= 0.5)
          {
              displaySpeed = (uint32_t)speed + 1;
          } else {
              displaySpeed = (uint32_t)speed;
          }
          if (speedRefresh && carMove) {
          ssd1306_printText(0,2,"                    ");
          ssd1306_printUI32(0,2,(uint32_t)speed,HCENTERUL_ON);
          speedRefresh=0;
          }
           minutes = seconds/60;
           displaySeconds = seconds - minutes*60; //- startSeconds;
          //}
          if (displaySeconds>=10){
           ssd1306_printText(62,6,":");
           ssd1306_printUI32(67,6,displaySeconds,0);
          }
          if (displaySeconds<10){

             ssd1306_printText(62,6,":0");
             ssd1306_printUI32(74,6,displaySeconds,0);
          }
          if(minutes>9){
              ssd1306_printUI32(50,6,minutes,0);
          } else {
              ssd1306_printUI32(57,6,minutes,0);
          }

           //bcUartSend(&displaySeconds,sizeof(displaySeconds));
      }
      /*void timer(){
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
      }*/
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
          //bcUartSend(&elasped_millis,sizeof(elasped_millis));
       }
//Screen interrupt
        //------------------------------------------------------------------------------
        // The USCIAB1TX_ISR is structured such that it can be used to transmit any
        // number of bytes by pre-loading TXByteCtr with the byte count. Also, TXData
        // points to the next byte to transmit.
        //------------------------------------------------------------------------------
       #if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
        #pragma vector = USCI_B1_VECTOR
        __interrupt void USCI_B1_ISR(void)
        #elif defined(__GNUC__)
        void __attribute__ ((interrupt(USCI_B0_VECTOR))) USCI_B0_ISR (void)
        #else
        #error Compiler not supported!
        #endif
        {
          switch(__even_in_range(UCB1IV,12))
          {
          case  0: break;                           // Vector  0: No interrupts
          case  2: break;                           // Vector  2: ALIFG
          case  4: break;                           // Vector  4: NACKIFG
          case  6: break;                           // Vector  6: STTIFG
          case  8: break;                           // Vector  8: STPIFG
          case 10: break;                           // Vector 10: RXIFG
          case 12:                                  // Vector 12: TXIFG
            if (TXByteCtr)                          // Check TX byte counter
            {
              UCB1TXBUF = *PTxData++;               // Load TX buffer
              TXByteCtr--;                          // Decrement TX byte counter
            }
            else
            {
              UCB1CTL1 |= UCTXSTP;                  // I2C stop condition
              UCB1IFG &= ~UCTXIFG;                  // Clear USCI_B1 TX int flag
              __bic_SR_register_on_exit(LPM0_bits); // Exit LPM0
            }
          default: break;
          }
        }

