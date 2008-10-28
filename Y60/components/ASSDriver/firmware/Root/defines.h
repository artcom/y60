//============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _ASS_ROOT_DEFINES_H_
#define _ASS_ROOT_DEFINES_H_

#include <avr/io.h>
#include <stdio.h>

#define F_CPU 14745600

//=== Configuration ==========================================================

#define VERSION    1
#define SUBVERSION 5//7 //kept 5 for be compatible to ASS Driver

#define DEFAULT_ID                    0
#define DEFAULT_MATRIX_WIDTH          20
#define DEFAULT_MATRIX_HEIGHT         10
#define DEFAULT_SCAN_FREQUENCY        20  //Hz (max 100Hz, min 16Hz)
#define DEFAULT_VOLTAGE_TRANSMITTER   106 //=5V *21,2(*1/V)
#define DEFAULT_GRID_SPACING          50  //mm
#define DEFAULT_BAUD_RATE             0   //0 = 57600bps
                                          //1 = 115200bps
                                          //2 = 230400bps
                                          //3 = 460800bps
                                          //4 = 921600bps

#define DEFAULT_T_FREQUENCY           2   //0 = 446.84kHz
                                          //1 = 460.80kHz
                                          //2 = 491.52kHz
                                          //3 = 546.13kHz


//=== Don't change anything below this line! =================================

#define MAX_MATRIX_WIDTH  120//40
#define MAX_MATRIX_HEIGHT 50

#define MIN_SCAN_FREQUENCY 16//Hz (lower frequencies don't work as g_scanPeriod is 16-bit integer)
#define MAX_SCAN_FREQUENCY 100//Hz (additional restrictions apply!)

#define UART_BAUD_RATE_LS 9600
#define UART_BAUD_RATE_HS 57600 //will be scaled by factors 2, 4, 8, 16 up to 921600 bps
#define UART_MAX_BAUD_RATE_FACTOR 4

#define MIN_GRID_SPACING   5 //mm
#define MAX_GRID_SPACING 200 //mm

#define MAX_T_FREQUENCY   3

#define TX_PACKET_SIZE 62 //matches to buffer size of FTDI chip

#define MIN_DELTA_T_ROW  750+350 //us

#define VZERO   10

#define LENGTH_STATUS_MSG 2+10*5 //adjust to actual number of transmitted status bytes per frame
//#define LENGTH_STATUS_MSG 2+3*5 //adjust to actual number of transmitted status bytes per frame

#define STANDBY_MODE 0
#define ABS_MODE 1
#define REL_MODE 2


#define ERROR_T_LEVEL1    _BV(0)
#define ERROR_T_LEVEL2    _BV(1)
#define ERROR_CPLD        _BV(2)
#define ERROR_IIC         _BV(3)
#define ERROR_TBUFFER     _BV(4)
#define ERROR_PARAMETERS  _BV(5)


//DIP Switch Functions
#define DIP_AUTO_SWITCH_TO_REL_MODE   0
#define DIP_UART_SELECT               4
#define DIP_RTS_CONTROL               5
#define DIP_UART_LOW_SPEED            6
#define DIP_EEPROM_LOCK               7


//EEPROM Map
#define EEPROM_LOC_FORMAT_ENTRY        0x00 //2 bytes

#define EEPROM_LOC_VOLTAGE_TRANSMITTER 0x02
#define EEPROM_LOC_MATRIX_WIDTH        0x03
#define EEPROM_LOC_MATRIX_HEIGHT       0x04
#define EEPROM_LOC_SCAN_FREQUENCY      0x05
#define EEPROM_LOC_BAUD_RATE           0x06
#define EEPROM_LOC_GRID_SPACING        0x07
#define EEPROM_LOC_ID                  0x08
#define EEPROM_LOC_T_FREQUENCY         0x09

#define EEPROM_FORMAT_KEY 0x77BB

#define TBUFFER_SIZE MAX_MATRIX_WIDTH+TX_PACKET_SIZE+LENGTH_STATUS_MSG //should be large enough to hold all that

#define LS 0
#define HS 1



//=== Pin Configuration =====================================================
//CPLD
#define    PORT_CLK_EN    PORTA
#define    DDR_CLK_EN     DDRA
#define    CLK_EN         PA3

#define    PORT_SS2       PORTA
#define    DDR_SS2        DDRA
#define    SS2            PA6

#define    PORT_SS3       PORTA
#define    DDR_SS3        DDRA
#define    SS3            PA7

#define    PORT_SM_EN     PORTA
#define    DDR_SM_EN      DDRA
#define    SM_EN          PA1

#define    PORT_SM_EN_FB  PORTA
#define    DDR_SM_EN_FB   DDRA
#define    PIN_SM_EN_FB   PINA
#define    SM_EN_FB       PA2

#define    PORT_CTS       PORTE
#define    DDR_CTS        DDRE
#define    CTS            PE4

#define    PORT_RTS       PORTE
#define    PIN_RTS        PINE
#define    DDR_RTS        DDRE
#define    RTS            PE5

//LEDs
#define    PORT_LED       PORTB
#define    DDR_LED        DDRB
#define    LED_R          PB0
#define    LED_G          PB6

//AUX
#define    PORT_AUX0      PORTC
#define    DDR_AUX0       DDRC
#define    PIN_AUX0       PINC
#define    AUX0           PC3

#define    PORT_AUX1      PORTC
#define    DDR_AUX1       DDRC
#define    PIN_AUX1       PINC
#define    AUX1           PC4

#define    PORT_AUX2      PORTC
#define    DDR_AUX2       DDRC
#define    PIN_AUX2       PINC
#define    AUX2           PC5

#define    PORT_AUX3      PORTC
#define    DDR_AUX3       DDRC
#define    PIN_AUX3       PINC
#define    AUX3           PC6

#define    PORT_AUX4      PORTC
#define    DDR_AUX4       DDRC
#define    PIN_AUX4       PINC
#define    AUX4           PC7


#define    PORT_AUX2      PORTC
#define    DDR_AUX2       DDRC
#define    AUX2           PC5

#define    PORT_AUX3      PORTC
#define    DDR_AUX3       DDRC
#define    AUX3           PC6

#define    PORT_AUX4      PORTC
#define    DDR_AUX4       DDRC
#define    AUX4           PC7


//PC1 is CLK of line shift register
//PC2 is Data of line shift register
#define    PORT_TR_DATA   PORTC
#define    DDR_TR_DATA    DDRC
#define    TR_DATA        PC2
#define    PORT_TR_CLK    PORTA
#define    DDR_TR_CLK     DDRA
#define    TR_CLK         PA4

//12V Generator
#define    PORT_HVSHDN    PORTF
#define    DDR_HVSHDN     DDRF
#define    HVSHDN         PF2


//opto coupler
#define    PORT_SNC_OUT  PORTE
#define    DDR_SNC_OUT   DDRE
#define    SNC_OUT       PE7
#define    PORT_SNC_IN   PORTE
#define    PIN_SNC_IN    PINE
#define    DDR_SNC_IN    DDRE
#define    SNC_IN        PE6


//=== Macros ================================================================
#define UART_BAUD_CALC( theBaudRate, theClock) \
    ( (theClock) / ((theBaudRate) * 16l) - 1)

#define DISABLE_OC1A() TIMSK &= ~ (1 << OCIE1A);
#define ENABLE_OC1A() TIMSK |= (1 << OCIE1A);

#define DISABLE_RX0() UCSR0B &= ~ (1 << RXCIE0);
#define ENABLE_RX0() UCSR0B |= (1 << RXCIE0);

#define DISABLE_TX0() UCSR0B &= ~ (1 << UDRIE0);
#define ENABLE_TX0() UCSR0B |= (1 << UDRIE0);

#define PAUSE_4_CYCLES()   \
		asm volatile (     \
		"nop" "\n\t"       \
		"nop" "\n\t"       \
		"nop" "\n\t"       \
		"nop" "\n\t"       \
		);

#define PAUSE_1_US()   \
		asm volatile (     \
		"nop" "\n\t"       \
		"nop" "\n\t"       \
		"nop" "\n\t"       \
		"nop" "\n\t"       \
		"nop" "\n\t"       \
		"nop" "\n\t"       \
		"nop" "\n\t"       \
		"nop" "\n\t"       \
		"nop" "\n\t"       \
		"nop" "\n\t"       \
		"nop" "\n\t"       \
		"nop" "\n\t"       \
		"nop" "\n\t"       \
		"nop" "\n\t"       \
		);


//=== Prototypes =============================================================

extern FILE g_uart0_str;
extern FILE g_uart1_str;

void    init(void);
void    configScanOscillator(void);
void    activateFirstLine(void);
void    activateNextLine(void);
void    activateTransmitter(uint8_t);
void    triggerRowReadout(void);
void    SPIdummyRead(void);
void    performReceiverTara(void);
void    switchToAbsoluteMode(void);
void    configSpecialScanMode(uint8_t);
uint8_t setScanParameter(uint8_t, uint8_t, uint8_t);
int8_t  setDACValue(uint8_t);
void    pause1us(void);
uint8_t detectNumberOfTransmitters(void);
uint8_t detectNumberOfReceivers(void);
void    handleConfigRequests(void);
void    sendByte(uint8_t);
void    sendInt(uint8_t);
void    processReceivedByte(uint8_t c);

void    TSC_init(void);
void    TSC_update(void);
void    TSC_transferCommand(uint16_t pointer);

#endif
