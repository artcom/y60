//============================================================================
// Copyright (C) 2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

/*control functions:
calibration sequence:
   1. turn on CLK signal w/o. interruption for 40ms to indicate calibration request
   2. turn off CLK signal for 2ms
   3. issue one scan cycle starting with line 0
   4. turn off CLK signal for 80 ms --> calibrated

turn on absolute mode:
   Method 1
   1. turn on CLK signal w/o. interruption for 10ms to switch to absolute mode
   2. turn off CLK signal for 80 ms

   Method 2
   1. turn off CLK signal for 200 ms

turn on relative mode:
   1. turn on CLK signal w/o. interruption for 20ms to switch to relative mode
   2. turn off CLK signal for 80 ms


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

#define RESET_REQUEST 0
#define IDLE_STATE    1
#define ACTIVE        2


//=== Macros ================================================================

#define ADC_CONFIGURATION1()\
	ADMUX = _BV(REFS1) | 6; // 1.1 V reference; channel 2+, Channel 3-, gain=1


//=== Globals ================================================================

uint8_t  g_USIactive = IDLE_STATE;
uint8_t  g_USImark = 0;
uint8_t  g_USImarkCounter = 0;
uint8_t  g_nextModeRequest = 0;
uint8_t  g_mode = ABS_MODE;
uint8_t  g_USIbreakCounter = 0;
uint8_t  g_requestCalibration = 0;
uint8_t  g_sampleState = 0;
uint8_t  g_readOutState = 0;
uint8_t  g_ADCvalueAvailable = 0;
uint8_t  g_ADC_Calibrated = 0;

uint16_t g_ADCOffset[MAX_LINES];
int16_t  g_ADCOffsetFollow[MAX_LINES];
uint8_t  g_NumberOfLines;
uint8_t  g_lineCounter;

uint8_t  g_EEPROMState = 0;

uint16_t g_ADCLH;
uint8_t  g_ADCLHAvailable=0;


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
	OCR1A = 40;        //40us time out (pre-scaler=8 --> 1us base) 
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
    g_sampleState = 0;

    g_mode = ABS_MODE;

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
	//clear USIOIF flag
	USISR |= _BV(USIOIF);

	//clear timer counter 1
	//turn on timer 1
	TCNT1 = 0; //ISR will be called when counter reaches 40 --> 40us time out
    TCCR1 = 4; //prescaler=8 -> 1us base

    g_USIactive = ACTIVE; //will be set to RESET_REQUEST after 200ms of no USI activity
    g_USImark = 1;
}


/* timer1 interrupt */
ISR( TIM1_COMPA_vect ) {
	//is called when a gap on SCLK is detected (OCR1A is set to 40, which corresponds to 40us of no received byte)
	//stop timer1
	TCCR1 = 0;

    g_sampleState = 2; //trigger sampling

	//reset USI-counter (this ensures that next transfer starts with bit 0)
	USISR &= 240; //=0b11110000  -->clear USI-counter

    //check if mode request is pending (by having sent in continuous clock pulses for several ms...)
    if(g_nextModeRequest > 0){
        if(g_mode != g_nextModeRequest){
            g_mode = g_nextModeRequest;
            if(g_mode == CAL_MODE){
                g_requestCalibration = 1;
            }
        }
    g_nextModeRequest = 0;
    }

    g_USImark = 0;
}




// timer0 interrupt: main timer interrupt (1ms/1000 Hz)
ISR( TIM0_COMPA_vect ) {

	if(g_USImark == 1){
        g_USIbreakCounter = 0;
        if(g_USImarkCounter != 255){
            g_USImarkCounter++;
        }

        switch(g_USImarkCounter){
            case   7:   //10ms-10%
                g_nextModeRequest = ABS_MODE;
                break;
            case  15:   //20ms-10%
                g_nextModeRequest = REL_MODE;
                break;
            case  32:   //40ms-10%
                g_nextModeRequest = CAL_MODE;
                break;
            case  60:   //80ms-10%
                g_nextModeRequest = g_mode;//keep current mode
                break;
            default:
                ;
        }
	}

	if(g_USImark == 0){
        g_USImarkCounter = 0;
        if(g_USIbreakCounter < 255){
            g_USIbreakCounter++;
        }

        switch(g_USIbreakCounter){
            case   65:   //80ms-10%
                if(g_mode == CAL_MODE){
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
            case   170:   //200ms-10%
                g_USIactive = RESET_REQUEST;
                break;
            default:
                ;
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


    init();


	//main loop
	while(1){
		//check if new sample is requested
		if(g_sampleState == 2){
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

            g_sampleState = 3;
		}



		if(g_sampleState == 3){//process new sample
			//calibration stuff
			if(g_requestCalibration > 0){
				switch(g_requestCalibration){
				case 1:
					g_NumberOfLines = 0;
					g_requestCalibration = 2;
				case 2:
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
				g_sampleState = 4; //wait for gap in USI-SCK
			}else{
				//transmit currrent value
                if(g_mode == REL_MODE){
    				USIDR = ADCvalueREL;
                }else{
    				USIDR = ADCvalueABS;
                }

				//change to next line
				g_lineCounter++;
				if(g_lineCounter >= g_NumberOfLines){
					g_lineCounter = 0;
				}
				g_sampleState = 4; //wait for gap in USI-SCK
			}
		}

		if(g_sampleState == 4){//process new sample
            if(g_USImark == 0){
				g_sampleState = 0; //await next sample
            }
        }

        if(g_USIactive == RESET_REQUEST){
            resetBuffersAndVariables();
            g_USIactive = IDLE_STATE;
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

// EEPROM Ready interrupt
ISR( EE_RDY_vect ) {
uint8_t jk;
    if(g_EEPROMState >= 1){//g_EEPROMState counts from 1 (number of lines) up to 2*g_NumberOfLines+1
        if(g_EEPROMState <= (g_NumberOfLines<<1)){
            //write next byte
            EECR = (0<<EEPM1) | (0<<EEPM0) | _BV(EERIE);//set programming mode and turn on interrupt
            EEAR = g_EEPROMState; //ucAddress
            //ucData
            g_EEPROMState--;
            if((g_EEPROMState&1) == 0){
                EEDR = jk = (uint8_t)(g_ADCOffset[(g_EEPROMState>>1)]>>8);
            }else{
                EEDR = jk = (uint8_t)(g_ADCOffset[(g_EEPROMState>>1)]&255);
            }
            //start EEPROM write by setting EEPE
            asm volatile ("sbi 0x1C,2");//sbi EECR,EEMPE
            asm volatile ("sbi 0x1C,1");//sbi EECR,EEPE
            g_EEPROMState += 2;
        }else{
            g_EEPROMState = 0;
            EECR &= ~_BV(EERIE);
        }
    }else{
        EECR &= ~_BV(EERIE);
    }
}

