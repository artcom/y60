//============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================


#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uart64.h"
#include "defines.h"


uint8_t detectNumberOfTransmitters(void){
    //not implemented yet!
    //  ...
    return 0;
}


uint8_t detectNumberOfReceivers(void){
    //not implemented yet!
    //  ...
    return 0;
}


void activateFirstLine(void){
	//set data high
	PORT_TR_DATA |= _BV(TR_DATA); //Data
	PAUSE_4_CYCLES()

	//issue clock pulse
	PORT_TR_CLK |= _BV(TR_CLK);  //CLK -> high
	PAUSE_4_CYCLES()

	PORT_TR_CLK &= ~_BV(TR_CLK);  //CLK -> low
	PAUSE_4_CYCLES()
}


void activateNextLine(void){
	//set data low
	PORT_TR_DATA &= ~_BV(TR_DATA); //Data
	PAUSE_4_CYCLES()

	//issue clock pulse
	PORT_TR_CLK |= _BV(TR_CLK);  //CLK -> high
	PAUSE_4_CYCLES()

	PORT_TR_CLK &= ~_BV(TR_CLK);  //CLK -> low
	PAUSE_4_CYCLES()
}


void activateTransmitter(uint8_t r){
    if(r == g_matrixHeight){
        //turn on 'special scan mode'
        PORT_SM_EN |= _BV(SM_EN); // SM_EN
    }else{
        //disable 'special scan mode'
        PORT_SM_EN &= ~_BV(SM_EN); // SM_EN->low
        //wait for mode to stop
        while((PIN_SM_EN_FB&_BV(SM_EN_FB)) != 0)
            ;

        if(r == 0){
            activateFirstLine();
        }else{
            activateNextLine();
        }
    }
}


void configScanOscillator(void){
uint8_t SREG_old, SPCR_old;
uint8_t cc0, cc1, cc2;
uint8_t CONF0, CONF1, CONF2;
uint8_t tries;

    switch(g_tFrequency){
        case 0:
            //settings for 446.84kHz
            CONF0 = 7;
            CONF1 = 21;
            CONF2 = 32;
            break;
        case 1:
            //settings for 460.80kHz
            CONF0 = 7;
            CONF1 = 21;
            CONF2 = 31;
            break;
        case 2:
            //settings for 491.52kHz
            CONF0 = 6;
            CONF1 = 19;
            CONF2 = 29;
            break;
        case 3:
            //settings for 546.13kHz
            CONF0 = 5;
            CONF1 = 16;
            CONF2 = 26;
            break;
        default:
            //settings for 491.52kHz
            CONF0 = 6;
            CONF1 = 19;
            CONF2 = 29;
    }


    SREG_old = SREG; //sample and restore I-bit at end of function
	cli();

    //set up SPI
    SPCR_old = SPCR;
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
    PORT_SS2 &= ~_BV(SS2); //#SS2->low to enable config shift register (make sure that all other #SSx are high!)

    //transfer config bytes (in reverse order)
    tries = 0;
    do{
        SPDR = CONF2;
        while(!(SPSR & (1<<SPIF))) ;
            cc2 = SPDR;
        SPDR = CONF1;
        while(!(SPSR & (1<<SPIF))) ;
            cc1 = SPDR;
        SPDR = CONF0;
        while(!(SPSR & (1<<SPIF))) ;
            cc0 = SPDR;
        tries++;
        if(tries == 50){
            g_errorState |= ERROR_CPLD;
            break;
        }
    }while(cc0!=CONF0 || cc1!=CONF1 || cc2!=CONF2); //repeat until correct read back of bytes

    PORT_SS2 |= _BV(SS2); //#SS2->high after data transfer completed

    //restore SPI setting
    SPCR = SPCR_old;

    if((SREG_old&_BV(7)) != 0){
        sei();
    }
}


void configSpecialScanMode(uint8_t nrOfTransmitters){
uint8_t SREG_old, SPCR_old;

    if(nrOfTransmitters < 2){
        //ADD: special case: number of transmitter is less than 2
        //  [...]
        return;
    }

    SREG_old = SREG; //sample and restore I-bit at end of function
	cli();

    //set up SPI
    SPCR_old = SPCR;
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
    PORT_SS3 &= ~_BV(SS3); //#SS3->low to enable shift register (make sure that all other #SSx are high!)


    //transfer byte
    SPDR = nrOfTransmitters-2;
    while(!(SPSR & (1<<SPIF))) ;

    //for now there is no read back of the byte; so simply
    //transfer it twice to make sure it is successfully transmitted
    SPDR = nrOfTransmitters-2;
    while(!(SPSR & (1<<SPIF))) ;


    PORT_SS3 |= _BV(SS3); //#SS3->high after data transfer completed

    //restore SPI setting
    SPCR = SPCR_old;

    if((SREG_old&_BV(7)) != 0){
        sei();
    }
}


int8_t setDACValue(uint8_t val){
#define START       0x08
#define MT_SLA_ACK  0x18
#define MT_DATA_ACK 0x28

    //initiate transmission of START
    TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
    //wait for TWINT flag set
    g_mainTimer = 0;
    while(!(TWCR&_BV(TWINT))){
        //time out
        if(g_mainTimer >= 100){ //100ms
            g_errorState |= ERROR_IIC;
            return -1;
        }
    }
    //check error state
    if((TWSR & 0xF8) != START){
        g_errorState |= ERROR_IIC;
        return -1;
    }

    //send address
    TWDR = 0x18;
    TWCR = _BV(TWINT) | _BV(TWEN);
    //wait for TWINT flag set
    g_mainTimer = 0;
    while(!(TWCR&_BV(TWINT))){
        //time out
        if(g_mainTimer >= 100){ //100ms
            g_errorState |= ERROR_IIC;
            return -1;
        }
    }
    //check error state
    if((TWSR & 0xF8) != MT_SLA_ACK){
        g_errorState |= ERROR_IIC;
        return -1;
    }

    //send data (high byte)
    TWDR = (uint8_t)(val>>4);
    TWCR = _BV(TWINT) | _BV(TWEN);
    //wait for TWINT flag set
    g_mainTimer = 0;
    while(!(TWCR&_BV(TWINT))){
        //time out
        if(g_mainTimer >= 100){ //100ms
            g_errorState |= ERROR_IIC;
            return -1;
        }
    }
    //check error state
    if((TWSR & 0xF8) != MT_DATA_ACK){
        g_errorState |= ERROR_IIC;
        return -1;
    }

    //send data (low byte)
    TWDR = (uint8_t)(val<<4);
    TWCR = _BV(TWINT) | _BV(TWEN);
    //wait for TWINT flag set
    g_mainTimer = 0;
    while(!(TWCR&_BV(TWINT))){
        //time out
        if(g_mainTimer >= 100){ //100ms
            g_errorState |= ERROR_IIC;
            return -1;
        }
    }
    //check error state
    if((TWSR & 0xF8) != MT_DATA_ACK){
        g_errorState |= ERROR_IIC;
        return -1;
    }

    //transmit STOP condition
    TWCR = _BV(TWINT) |_BV(TWSTO) | _BV(TWEN);

    g_errorState &= ~ERROR_IIC;

    return 0;
}


void pause1us(void){
uint8_t i;

    for(i=0; i<8; i++){
        PAUSE_4_CYCLES()
    }

    return;
}


/////////////////////
void EEPROM_write(uint16_t uiAddress, uint8_t ucData){
    if((g_DIPSwitch&_BV(DIP_EEPROM_LOCK)) != 0){//EEPROM is locked by DIP switch 8
        return;
    }

    /* Wait for completion of previous write */
    while(EECR & (1<<EEWE))
        ;
    /* Set up address and data registers */
    EEAR = uiAddress;
    EEDR = ucData;
    asm volatile ("sbi 0x1C,2");//sbi EECR,EEMWE
    asm volatile ("sbi 0x1C,1");//sbi EECR,EEWE
    
    return;
}


uint8_t EEPROM_read(uint16_t uiAddress){
    /* Wait for completion of previous write */
    while((EECR & _BV(EEWE)) != 0)
        ;
    /* Set up address register */
    EEAR = uiAddress;
    /* Start eeprom read by writing EERE */
   asm volatile ("sbi 0x1C,0");//EECR |= (1<<EERE);
    /* Return data from data register */
    return EEDR;
}
