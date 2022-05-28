/*
 * milliSecond_timer.c
 *
 *  Created on: Mar 25, 2022
 *      Author: Camry (the GOAT <3 from LP)
 *
 *      Version -> V 1.0
 *
 *  Purpose -> This little bit of code will use the ACLK to generate a 1Khz signal and update a count of milliseconds
 *              seconds since it was started.
 *
 *              Please Note the following:
 *              - This can not give you a count of the number of milliseconds since the program started, only since the timer started
 *              - This will not be accurate at all if you change the frequency of ACKLI away from the 32.XX Khz that it is by defualt
 *              - Interrupts need to be enabled for this to function correctly, if they arn't it is likely that you will start to drop seconds
 *              - Take this as a rough solution only! if you need perfect timing then you should be looking else where
 *              - This will use up timer A1
 */

#include "milliSecond_timer.h"

    //Storage variable for elapsed milliseconds, will be updated by the interrupt service routine
    volatile uint16_t elasped_millis = 0;
    int timerStart;
    /*---------------------setup_millisecond_timer(void)------------
     *------------ Setups up one of the internal timers to count millisecond based off of the slow ACLK
     *------------ Uses Timer A1
     *------------ Input frequency -> 32768Hz
     *------------ Divider frequency -> 1
     *------------ Count too -> 125
     *------------ Output Frequency -> 1Khz
     */
    void setup_millisecond_timer(void){
        Timer_A_initUpModeParam setup_parm = {.clockSource = TIMER_A_CLOCKSOURCE_ACLK, .clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1,
                                              .timerPeriod = 32.768, .captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE, .startTimer = true};
        Timer_A_initUpMode(TIMER_A1_BASE, &setup_parm);
    }

#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_interupt(void){
    //enable_interrupt();
    timerStart=1;
    elasped_millis++;
}




