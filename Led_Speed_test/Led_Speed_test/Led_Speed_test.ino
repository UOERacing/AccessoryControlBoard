void setup () {
DDRD |= (1<<PD4);
    DIDR1 |= (1<<AIN0D); // Disable Digital Inputs at AIN0 and AIN1
ADCSRA &= ~(1<<ADEN);
    ADCSRB |= (1<<ACME);  //Set ACME bit in ADCSRB to use external analog input at AIN1 -ve input
ADMUX = 0x01; //select A1 as input 
ACSR = 
(0 << ACD) |    // Analog Comparator: Enabled
(0 << ACBG) |   // Clear ACBG to use external input to AIN0 +ve input
(0 << ACO) |    // Analog Comparator Output: OFF
(1 << ACI) |    // Analog Comparator Interrupt Flag: Clear Pending Interrupt by setting the bit
(1 << ACIE) |   // Analog Comparator Interrupt: Disabled 
(0 << ACIC) |   // Analog Comparator Input Capture: Disabled
(0 << ACIS1) | (0 << ACIS0);   // Analog Comparator Interrupt Mode: Comparator Interrupt on Output Toggle

sei();
}

void loop() {

}

ISR(ANALOG_COMP_vect)
{
if(ACSR & (1<<ACO))
PORTD |= (1<<PD4);
else
    PORTD &= ~(1<<PD4);

}
