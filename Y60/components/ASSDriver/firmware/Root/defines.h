//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//#define OLDBOARD


#ifndef _DEFINES_H_
#define _DEFINES_H_

#ifndef OLDBOARD
#define F_CPU 14745600
#else
#define F_CPU 16000000
#endif

//=== Configuration ==========================================================

#define MAX_MATRIX_WIDTH  40
#define MAX_MATRIX_HEIGTH 25

#define DEFAULT_MATRIX_WIDTH  20 //user definable: range 1-MAX_MATRIX_WIDTH
#define DEFAULT_MATRIX_HEIGTH 10 //user definable: range 1-MAX_MATRIX_HEIGTH

#define VERSION    1
#define SUBVERSION 4

#define ID 1

#define TX_PACKET_SIZE 62 //matches to buffer size of FTDI chip


#define UART0_BAUD_RATE 57600


#define SCAN_FREQUENCY    21//Hz (max 30Hz, min 16Hz)


#define VZERO   10 //don't change!

#define ABS_MODE 1
#define REL_MODE 2

#define ERROR_T_LEVEL1 1
#define ERROR_T_LEVEL2 2
#define ERROR_CPLD     4
#define ERROR_IIC      8
#define ERROR_TBUFFER  16


uint8_t g_matrixWidth;
uint8_t g_matrixHeigth;
uint8_t g_ID;
uint8_t g_mode;
uint8_t g_status;
uint8_t g_errorState;
uint8_t g_mainTimer;

#define LENGTH_STATUS_MSG 2+9*5


//=== Pin Configuration =====================================================
#ifndef OLDBOARD
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

//LEDs
#define    PORT_LED       PORTB
#define    DDR_LED        DDRB
#define    LED_R          PB0
#define    LED_G          PB6

//AUX
#define    PORT_AUX0      PORTC
#define    DDR_AUX0       DDRC
#define    AUX0           PC3

#define    PORT_AUX1      PORTC
#define    DDR_AUX1       DDRC
#define    AUX1           PC4

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
#define    DDR_SNC_IN    DDRE
#define    SNC_IN        PE6


#else
//CPLD
#define    PORT_CLK_EN    PORTC
#define    DDR_CLK_EN     DDRC
#define    CLK_EN         PC0

#define    PORT_SS2       PORTB
#define    DDR_SS2        DDRB
#define    SS2            PB6

#define    PORT_SS3       PORTA
#define    DDR_SS3        DDRA
#define    SS3            PA0

#define    PORT_SM_EN     PORTA
#define    DDR_SM_EN      DDRA
#define    SM_EN          PA1

#define    PORT_SM_EN_FB  PORTA
#define    DDR_SM_EN_FB   DDRA
#define    PIN_SM_EN_FB   PINA
#define    SM_EN_FB       PA2

//LEDs
#define    PORT_LED       PORTG
#define    DDR_LED        DDRG
#define    LED_R          PG4
#define    LED_G          PG3

//AUX
#define    PORT_AUX0      PORTC
#define    DDR_AUX0       DDRC
#define    AUX0           PC7

#define    PORT_AUX1      PORTC
#define    DDR_AUX1       DDRC
#define    AUX1           PC6

//PC1 is CLK of line shift register
//PC2 is Data of line shift register
#define    PORT_TR_DATA   PORTC
#define    DDR_TR_DATA    DDRC
#define    TR_DATA        PC2
#define    PORT_TR_CLK    PORTC
#define    DDR_TR_CLK     DDRC
#define    TR_CLK         PC1

//12V Generator
#define    PORT_HVSHDN    PORTF
#define    DDR_HVSHDN     DDRF
#define    HVSHDN         PF2

#endif


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

void    init(void);
void    configScanOscillator(void);
void    activateFirstLine(void);
void    activateNextLine(void);
void    activateTransmitter(uint8_t);
void    triggerRowReadout(void);
void    SPIdummyRead(void);
void    performReceiverCalibration(void);
void    configSpecialScanMode(uint8_t);
int8_t  setDACValue(uint8_t);
void    pause1us(void);
void    EEPROM_write(uint16_t uiAddress, uint8_t ucData);
uint8_t EEPROM_read(uint16_t uiAddress);
uint8_t detectNumberOfTransmitters(void);
uint8_t detectNumberOfReceivers(void);
void    handleConfigRequests(void);
void    sendByte(uint8_t);
void    sendInt(uint8_t);

#endif
