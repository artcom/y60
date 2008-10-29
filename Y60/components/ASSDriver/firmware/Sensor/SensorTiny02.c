//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

/*
control functions:
calibration sequence:
   1. turn off CLK signal for 250ms to indicate calibration request
   3. issue one scan cycle starting with line -1
   4. turn off CLK signal for 20 ms --> calibrated

turn on absolute mode:
   1. turn off CLK signal for 150 ms

turn on relative mode:
   2. turn off CLK signal for 200 ms


FUSE SETTINGS:
- Brown-out detection level at VCC=2.7 V
- Int. RC Osc. 8MHz; Start-up time PWRDWN/RESET: 6 CK/14 CK + 0 ms
all others off
*/


#define OFFSET_ADJUST


#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//=== Configuration ==========================================================

#define F_CPU 8000000 //+-10% calibration accuracy (MCU is using internal RC oscillator)

#define TICKS_PER_SECOND  1000

#define ZERO_VALUE 10 //don't change

#define MAX_LINES 50+1


#define ABS_MODE 1
#define REL_MODE 2
#define CAL_MODE 3
#define RESET_REQUEST 4



//=== Macros ================================================================

#define ADC_CONFIGURATION1()\
	ADMUX = _BV(REFS1) | 6; // 1.1 V reference; channel 2+, Channel 3-, gain=1


//=== Globals ================================================================

uint8_t  g_USImark = 0; // XXX reset
uint8_t  g_nextModeRequest = 0;
uint8_t  g_mode = ABS_MODE;
uint8_t  g_requestCalibration = 0;
uint8_t  g_triggerSample = 0;
uint8_t  g_ADC_Calibrated = 0;
uint8_t  g_inhibitModeChange = 1;

uint16_t g_ADCOffset[MAX_LINES];
int16_t  g_ADCOffsetFollow[MAX_LINES];
uint8_t  g_NumberOfLines;
uint8_t  g_lineCounter;

//uint8_t  g_EEPROMState = 0; // XXX reset

uint16_t g_ADCLH; // XXX reset
uint8_t  g_ADCLHAvailable=0; // XXX reset


//=== Prototypes =============================================================
void    init(void);
void    resetBuffersAndVariables(void);
void    EEPROM_write(uint8_t ucAddress, uint8_t ucData);
uint8_t EEPROM_read(uint8_t ucAddress);


//=== Code =============================================================
void init(void) {
	//turn on timer0 @ 1ms/1000 Hz (8MHz/(64*125) = 1000Hz)
	OCR0A  = 125-1;
	TCNT0  = 0;
    TCCR0A = _BV(WGM01); // CTC mode
    TCCR0B = 3; //prescaler=64

    TIMSK |= _BV(OCIE0A); //enable output compare match interrupt


	//=== turn on ADC           =====
	ADC_CONFIGURATION1();
	ADCSRA = _BV(ADEN) | _BV(ADIE) | 5; //turn ADC on; pre-scaler = 32->fs=250kHz -> conversion takes 52us, 4 conversions take 208us
	DIDR0 = _BV(ADC2D) | _BV(ADC3D);
	//DDRB |= _BV(PB3); //for debuggubg: use PB3 as monitor pin (if so, also inhibit changing the DIDR0 register)


	//=== prepare timer1
	TCCR1 = 0; //stop timer1 for now
//	OCR1A = 175; //350us time out (pre-scaler=16 --> 0.5us base) 
	OCR1A = 250; //500us time out (pre-scaler=16 --> 0.5us base) 
    TIMSK |= _BV(OCIE1A); //enable output compare match interrupt


	//=== turn on USI
	DDRB &= ~(_BV(PB2)|_BV(PB0));                                  //set CLK and MOSI to input
	DDRB |= _BV(PB1);
	USICR = _BV(USIOIE) | _BV(USIWM0) | _BV(USICS1) | _BV(USICS0); //three-wire mode, shift on positive edge


    resetBuffersAndVariables();


    //=== fire ======================
    sei();
}


void resetBuffersAndVariables(void){
uint8_t i;
//uint16_t o;

	for(i=0; i<MAX_LINES; i++){
		g_ADCOffset[i] = 1023;
		g_ADCOffsetFollow[i] = 0;
	}
    g_ADC_Calibrated = 0;
    g_requestCalibration = 0;
    g_NumberOfLines = 0;
    g_lineCounter = 0;
    g_triggerSample = 0;

    g_nextModeRequest = 0;
    g_mode = ABS_MODE;
    g_inhibitModeChange = 1;

//  //check if calibration values are available in EEPROM
//  i = EEPROM_read(0);
//  if(i <= MAX_LINES){
//      g_NumberOfLines = i;
//      for(i=0; i<g_NumberOfLines; i++){
//          o = EEPROM_read((i<<1)+1);
//          o <<= 8;//high byte first
//          o |= EEPROM_read((i<<1)+1+1);
//          if(o>1023){
//              o = 1023; //max. offset
//          }
//  		g_ADCOffset[i] = o;
//      }
//      g_lineCounter = g_NumberOfLines-1;
//      g_ADC_Calibrated = 1;
//  }
}


/* USI overflow interrupt */
ISR( USI_OVF_vect ) {
	//turn off USI-interrupt and clear USIOIF flag
   	USICR &= ~_BV(USIOIE);
	USISR |= _BV(USIOIF);

	//clear timer 1 and let it run
	TCNT1 = 0; //ISR will be called when counter reaches 175 --> 350us time out
    TCCR1 = 5; //prescaler=16 -> 0.5us base

    g_USImark = 2;
}


/* timer1 interrupt */
//is called 500us(/350us depending on settings) after first clocked through byte
ISR( TIM1_COMPA_vect ) {
	//stop timer1
	TCCR1 = 0;

	//reset USI-counter (this ensures that next transfer starts with bit 0)
	USISR &= 240; //=0b11110000  -->clear USI-counter
    //turn USI interrupt back on
   	USICR |= _BV(USIOIE);

    if(g_nextModeRequest == 0){
        g_triggerSample = 1; //trigger sampling if no modeRequest is pending
    }else{
        if(g_mode != g_nextModeRequest){ //new mode differs from current mode
            g_mode = g_nextModeRequest;
            if(g_mode == CAL_MODE){
                g_requestCalibration = 1;
            }
        }
        g_nextModeRequest = 0;
    }
}




// timer0 interrupt: main timer interrupt (1ms/1000 Hz)
ISR( TIM0_COMPA_vect ) {
static uint8_t  USIbreakCounter = 0, USIbreakSubCounter = 0;
static uint8_t  USImarkCounter = 0;

    //clear USImark after more than 1ms of no clock signal
    if(g_USImark > 0){
        g_USImark--;
    }


	if(g_USImark == 0){
        USImarkCounter = 0;
        USIbreakSubCounter++;
        if(USIbreakSubCounter >= 10){
            USIbreakSubCounter = 0;
            //10ms base
            if(USIbreakCounter < 255){
                USIbreakCounter++;
            }
            switch(USIbreakCounter){
                case  1:  //20ms-10%
                    if(g_mode == CAL_MODE){//finish calibration
                        g_ADC_Calibrated = 1;
                        g_requestCalibration = 0;
                        g_lineCounter = 0; //after calibration read out sequence starts with line 0
                        g_mode = REL_MODE;
//                      // start storage of new calibration values in EEPROM
//                      g_EEPROMState = 1;
//                      // wait for completion of previous write (actually there should be no write going on!)
//                      while(EECR & _BV(EEPE))
//                          ;
//                      EECR = (0<<EEPM1) | (0<<EEPM0) | _BV(EERIE);//set programming mode and turn on interrupt
//                      EEAR = 0; //ucAddress
//                      EEDR = g_NumberOfLines; //ucData
//                      asm volatile ("sbi 0x1C,2");//sbi EECR,EEMPE
//                      asm volatile ("sbi 0x1C,1");//sbi EECR,EEPE
                    }
                    break;
                case  13:  //150ms-10%
                    g_nextModeRequest = ABS_MODE;
                    break;
                case  18:  //200ms-10%
                    g_nextModeRequest = REL_MODE;
                    break;
                case  23:  //250ms-10%
                    g_nextModeRequest = CAL_MODE;
                    break;
                case  27:  //300ms-10%
                    g_nextModeRequest = RESET_REQUEST;
                    break;
                case  33:  //360ms-10%
                    resetBuffersAndVariables();
                    break;
                default:
                    ;
            }
        }
	}

    if(g_USImark > 0){
        USIbreakCounter = 0;
        USIbreakSubCounter = 0;
        if(USImarkCounter < 255){
            USImarkCounter++;
        }
        if(USImarkCounter == 10){//after 10 ms
            g_inhibitModeChange = 0;
        }
	}
}


ISR( ADC_vect ) {
uint8_t  th, tl;

	tl = ADCL;
	th = ADCH;
	g_ADCLH = (((uint16_t)th)<<8) | tl;
	g_ADCLHAvailable = 1;
}



int main(void){
uint8_t  i;
uint16_t ADCsum, ADCreadOut, j;
uint8_t  ADCvalueABS;
uint8_t  ADCvalueREL;

uint8_t  secondCal = 0;

    init();

    g_triggerSample = 0;

	//main loop
	while(1){
		//check if new sample is requested
		if(g_triggerSample == 1){
			//start conversion (average 4 samples)
			//make sure no conversion is going on
			while((ADCSRA&_BV(ADSC)) != 0){;}
			//turn off timer0 interrupt call during sampling
    		TIMSK &= ~_BV(OCIE0A);
			//set up ADC noise reduction mode
			cli();
			MCUCR &= ~_BV(SM1);
			MCUCR |= _BV(SE) | _BV(SM0);
			sei();
			for(i=0,ADCsum=0; i<4; i++){
				//start conversion
				g_ADCLHAvailable = 0;
				//start sampling by entering ADC noise reduction mode
				asm volatile ("sleep"::);
				//->system is back up because ADC-conversion is finished
				//but make sure ADC sampling is really finished by checking g_ADCLHAvailable
				while(g_ADCLHAvailable == 0){;}
				ADCsum += g_ADCLH;
			}
			cli();
			MCUCR &= ~_BV(SE);
			sei();
			//turn timer0 interrupt back on
    		TIMSK |= _BV(OCIE0A);

			ADCreadOut = (ADCsum>>2); //divide by 4 (normalize the 4 added samples)

            ADCvalueABS = (ADCreadOut>>2); //use top byte for storing absolute value
            //calculate ADCvalueREL
			if(g_ADC_Calibrated == 1){
				//add in offset
				j = g_ADCOffset[g_lineCounter];
				if(j >= ADCreadOut){
					j = j - ADCreadOut;
                //j >>= 1;//divide by two to avoid saturation
					//limit to 8 bits
					if(j > 255){
						j = 255;
					}
				}else{
					j = 0;
				}
#ifdef OFFSET_ADJUST
				//adjust ADC offset (extreme lowpass)
				//check if sensor value differs from ZERO_VALUE
				if(j > ZERO_VALUE){
					g_ADCOffsetFollow[g_lineCounter] += 1;
					if(g_ADCOffsetFollow[g_lineCounter] >= 3000){ //2min. * 25Hz = 3000
						if(g_ADCOffset[g_lineCounter] > 0){
							g_ADCOffset[g_lineCounter]--;
						}
						g_ADCOffsetFollow[g_lineCounter] = 0;
					}
				}else if(j < ZERO_VALUE){
					g_ADCOffsetFollow[g_lineCounter] -= 2;
					if(g_ADCOffsetFollow[g_lineCounter] <= -3000){
						if(g_ADCOffset[g_lineCounter] < (1024+ZERO_VALUE)){
							g_ADCOffset[g_lineCounter]++;
						}
						g_ADCOffsetFollow[g_lineCounter] = 0;
					}
				}
#endif
			}else{
                j = 0;
			}

			ADCvalueREL = j; //8 bit value


			//calibration stuff
			if(g_requestCalibration > 0){
				switch(g_requestCalibration){
				case 1:
					g_NumberOfLines = 0;
					g_requestCalibration = 2;
                    secondCal = 4;                            //XX
				case 2:
                    if(ADCreadOut > (1023-ZERO_VALUE)){
                        ADCreadOut = 1023-ZERO_VALUE;
                    }
                    j = ADCreadOut + ZERO_VALUE;
					g_ADCOffset[g_NumberOfLines] = j;
                    g_ADCOffsetFollow[g_NumberOfLines] = 0;
					if(g_NumberOfLines < MAX_LINES){
						g_NumberOfLines++;
					}else{
						g_requestCalibration = 3;
					}
				default:
					;
				}
				g_triggerSample = 0; //wait for next sampling
			}else{
				//transmit currrent value
                if(g_mode == REL_MODE){
    				USIDR = ADCvalueREL;
                }else{
    				USIDR = ADCvalueABS;
                }

                //do second calibration                      //XX
                if(secondCal != 0){                          //XX
                    if(secondCal == 1){                      //XX
                        if(ADCreadOut > (1023-ZERO_VALUE)){  //XX
                            ADCreadOut = 1023-ZERO_VALUE;    //XX
                        }                                    //XX
                        j = ADCreadOut + ZERO_VALUE;         //XX
	    				g_ADCOffset[g_lineCounter] = j;      //XX
                    }                                        //XX
                    if(g_lineCounter == g_NumberOfLines-1){  //XX
                        secondCal--;                         //XX
                    }                                        //XX
                }                                            //XX

				//change to next line
				g_lineCounter++;
				if(g_lineCounter >= g_NumberOfLines){
					g_lineCounter = 0;
				}
				g_triggerSample = 0; //wait for next sampling
			}
		}


        if(g_mode == RESET_REQUEST){
            resetBuffersAndVariables();
        }
	}

    return 0;
}


////////////////////////////////
void EEPROM_write(uint8_t ucAddress, uint8_t ucData){
    //wait for completion of previous write
    while(EECR & _BV(EEPE))
        ;

    cli();
    //set programming mode
    EECR = (0<<EEPM1) | (0<<EEPM0);
    //set up address and data registers
    EEAR = ucAddress;
    EEDR = ucData;
    //start EEPROM write by setting EEPE
    asm volatile ("sbi 0x1C,2");//sbi EECR,EEMPE
    asm volatile ("sbi 0x1C,1");//sbi EECR,EEPE
    sei();

    return;
}

uint8_t EEPROM_read(uint8_t ucAddress){
    //wait for completion of previous write
    while(EECR & _BV(EEPE))
        ;

    //set up address register
    EEAR = ucAddress;
    //start EEPROM read by writing EERE
    EECR |= _BV(EERE);
    //return data from data register
    return EEDR;
}

/* // EEPROM Ready interrupt */
/* ISR( EE_RDY_vect ) { */
/* uint8_t jk; */
/*     if(g_EEPROMState >= 1){//g_EEPROMState counts from 1 (number of lines) up to 2*g_NumberOfLines+1 */
/*         if(g_EEPROMState <= (g_NumberOfLines<<1)){ */
/*             //write next byte */
/*             EECR = (0<<EEPM1) | (0<<EEPM0) | _BV(EERIE);//set programming mode and turn on interrupt */
/*             EEAR = g_EEPROMState; //ucAddress */
/*             //ucData */
/*             g_EEPROMState--; */
/*             if((g_EEPROMState&1) == 0){ */
/*                 EEDR = jk = (uint8_t)(g_ADCOffset[(g_EEPROMState>>1)]>>8); */
/*             }else{ */
/*                 EEDR = jk = (uint8_t)(g_ADCOffset[(g_EEPROMState>>1)]&255); */
/*             } */
/*             //start EEPROM write by setting EEPE */
/*             asm volatile ("sbi 0x1C,2");//sbi EECR,EEMPE */
/*             asm volatile ("sbi 0x1C,1");//sbi EECR,EEPE */
/*             g_EEPROMState += 2; */
/*         }else{ */
/*             g_EEPROMState = 0; */
/*             EECR &= ~_BV(EERIE); */
/*         } */
/*     }else{ */
/*         EECR &= ~_BV(EERIE); */
/*     } */
/* } */

