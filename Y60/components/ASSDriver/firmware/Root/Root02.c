//============================================================================
// Copyright (C) 2008, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

//#define TEST_MODE
#define COL_CORRECTION
#define SEND_STATUS_BYTES

#define TESTBUFFERLENGTH 20

#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uart64.h"
#include "defines.h"
#include "global.h"
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

/*
 * Fuse settings
 *
 * Switch these fuses on:
 * - Preserve EEPROM memory through the Chip Erase cycle
 * - Boot Flash section size=4096 words
 * - CKOPT fuse
 * - Brown-out detection level at VCC=4.0 V
 * - Brown-out detection enabled
 * - Ext. Crystal/Resonator High Freq.; Start-up time: 1K CK + 64 ms
 *
 * Switch all other fuses off.
 *
 */

/*
 * DIP switches
 *
 * 1 On:  Perform automatic calibration on power-up after 4 sec.
 *   Off: Disable automatic calibration on power-up
 *
 * 2 On:
 *   Off:
 *
 * 3 On:
 *   Off:
 *
 * 4 On:
 *   Off:
 *
 * 5 On:  Use Com1/Xport for communication
 *   Off: Use Com0/USB for communication
 *
 * 6 On:  CTS line is ignored
 *   Off: System is forced in config mode when CTS is passive (only valid when UART0 is selected)
 *
 * 7 On:  UART runs at low speed (9600bps); Set up functions available only (via x[CR])
 *   Off: regular data at selected high speed on UART
 *
 * 8 On:  EEPROM parameters locked
 *   Off: EEPROM accessible for parameter write
 *
 */

//=== Globals ================================================================
FILE    g_uart0_str = FDEV_SETUP_STREAM(uart0_putchar, uart0_getchar, _FDEV_SETUP_RW);
FILE    g_uart1_str = FDEV_SETUP_STREAM(uart1_putchar, uart1_getchar, _FDEV_SETUP_RW);

const char configWelcomeMsg[] PROGMEM = "\nCommands:\
\nC01      Auto-calibrate transmission level\
\nC02      Tara and switch to relative mode\
\n\nC03      Print current error state\
\nC04      Clear error state\
\nC05      Print ID\
\nC06      Print (HS)Baud rate\
\nC07      Switch to absolute mode\
\n\nC11      Display max/min values\
\nC12 x,y  Display value of x/y-position (top left corner: 0, 0)\
\n\nC21 w    Set width\
\nC22 h    Set height\
\nC23 f    Set scan frequency (16-100Hz)\
\nC24 b    Set Baud rate factor (0-4)\
\nC25 s    Set grid spacing (in mm)\
\nC26 t    Set transmitter frequency (0-3)\
\n\nC98      Just replies 'ok'\
\nC99      Resume normal operation\n";

void init(void) {
uint8_t i, i2;
    //init LEDs
	DDR_LED |= _BV(LED_R);
	PORT_LED &= ~_BV(LED_R);
	DDR_LED |= _BV(LED_G);
	PORT_LED &= ~_BV(LED_G);

    //init AUX-ports
    DDR_AUX0 |= _BV(AUX0);
    DDR_AUX1 |= _BV(AUX1);

    //init DIP switch pins
    DDRD &= ~0xF0;
    DDRG &= ~0x03;
    DDRC &= ~0x03;
    PORTD |= 0xF0; //turn on pull-up resistors
    PORTG |= 0x03;
    PORTC |= 0x03;

    //init opto coupler
    DDR_SNC_OUT  |=  _BV(SNC_OUT);
    PORT_SNC_OUT &= ~_BV(SNC_OUT);
    DDR_SNC_IN   &= ~_BV(SNC_IN);
    PORT_SNC_IN  &= ~_BV(SNC_IN); //disable pull-up resistor

    //disable scan clock outputs
    PORT_CLK_EN &= ~_BV(CLK_EN); //CLK_EN ->low to disable outputs
    DDR_CLK_EN  |= _BV(CLK_EN);
    // #SS2
    PORT_SS2 |= _BV(SS2);
    DDR_SS2  |= _BV(SS2);
    // #SS3
    PORT_SS3 |= _BV(SS3);
    DDR_SS3  |= _BV(SS3);
    // SM_EN
    PORT_SM_EN &= ~_BV(SM_EN);
    DDR_SM_EN  |= _BV(SM_EN);
    DDR_SM_EN_FB  &= ~_BV(SM_EN_FB);
    PORT_SM_EN_FB |= _BV(SM_EN_FB);


	//=== init serial line for transmitters
	PORT_TR_CLK &= ~_BV(TR_CLK);  //CLK
	PORT_TR_DATA &= ~_BV(TR_DATA); //Data
	DDR_TR_CLK  |= _BV(TR_CLK);   //CLK
	DDR_TR_DATA  |= _BV(TR_DATA);  //Data


    //user timer1 for main system time
    TCCR1B = _BV(WGM12) | 1; //mode4: CTC, no prescaling
	OCR1A = (F_CPU/10000)-1; //approx. 10000Hz
    TIMSK |= _BV(OCIE1A);    //enable output compare match interrupt


	//=== init SPI ====
	DDRB |= _BV(PB2) | _BV(PB1) | _BV(PB0);              //MOSI, SCK, and #SS output (#SS must be configured as output pin!)
    SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPHA) | _BV(SPR0); //sample on falling edge, Speed: 3,686MHz @ 14.745MHz
    SPSR = _BV(SPI2X);

    //init TWI
    TWBR = 12; //f = 369KHz approx.
    TWCR = _BV(TWEN);


    //=== init ADC ====
    ADMUX = _BV(REFS1) | _BV(REFS1) | 0; //internal 2.56V voltage reference; channel 0
    ADCSRA = _BV(ADEN) | 7;              //prescaler = 128


    //===  prepare UART init ==================
    PORT_CTS |= _BV(CTS); //toggle CTS to force FTDI chip to flush buffer
    DDR_CTS |= _BV(CTS);
    DDR_RTS &= ~_BV(RTS); //set RTS to input (is used to turn on/off data transmission to host; this feature
                          //is controlled via DIP switch no. 6
    g_dataTransmitMode = 0; //normal operation


    //wait approx 4 ms to allow CPLD to start up
    for(i=0; i<15; i++){
        for(i2=0; i2<255; i2++){
            PAUSE_1_US()
        }
    }

    g_errorState = 0;

    //check if EEPROM contains stored values; if not write default values
    //check format entry
#define EEPROM_KEY_RETRIES 8
    for(i=0; i<EEPROM_KEY_RETRIES; i++){
        if(eeprom_read_byte(EEPROM_LOC_FORMAT_ENTRY) == (EEPROM_FORMAT_KEY&0xFF)  &&  eeprom_read_byte(EEPROM_LOC_FORMAT_ENTRY+1) == (EEPROM_FORMAT_KEY>>8)){
            //key found!
            break;
        }
    }
    if(i == EEPROM_KEY_RETRIES){
        eeprom_write_byte(EEPROM_LOC_FORMAT_ENTRY, (EEPROM_FORMAT_KEY&0xFF));
        eeprom_write_byte(EEPROM_LOC_FORMAT_ENTRY+1, (EEPROM_FORMAT_KEY>>8));

        eeprom_write_byte(EEPROM_LOC_VOLTAGE_TRANSMITTER, DEFAULT_VOLTAGE_TRANSMITTER);
        eeprom_write_byte(EEPROM_LOC_MATRIX_WIDTH, DEFAULT_MATRIX_WIDTH);
        eeprom_write_byte(EEPROM_LOC_MATRIX_HEIGHT, DEFAULT_MATRIX_HEIGHT);
        eeprom_write_byte(EEPROM_LOC_SCAN_FREQUENCY, DEFAULT_SCAN_FREQUENCY);
        eeprom_write_byte(EEPROM_LOC_BAUD_RATE, DEFAULT_BAUD_RATE);
        eeprom_write_byte(EEPROM_LOC_GRID_SPACING, DEFAULT_GRID_SPACING);
        eeprom_write_byte(EEPROM_LOC_ID, DEFAULT_ID);
        eeprom_write_byte(EEPROM_LOC_T_FREQUENCY, DEFAULT_T_FREQUENCY);
    }

    g_BaudRateFactor = eeprom_read_byte(EEPROM_LOC_BAUD_RATE);
    if( g_BaudRateFactor > UART_MAX_BAUD_RATE_FACTOR){
        eeprom_write_byte(EEPROM_LOC_BAUD_RATE, DEFAULT_BAUD_RATE);
        g_BaudRateFactor = DEFAULT_BAUD_RATE;
    }

    i = setScanParameter(eeprom_read_byte(EEPROM_LOC_MATRIX_WIDTH), \
                         eeprom_read_byte(EEPROM_LOC_MATRIX_HEIGHT), \
                         eeprom_read_byte(EEPROM_LOC_SCAN_FREQUENCY));
    if(i == 1){
        g_errorState |= ERROR_PARAMETERS;
    }
    g_gridSpacing = eeprom_read_byte(EEPROM_LOC_GRID_SPACING);
    g_ID = eeprom_read_byte(EEPROM_LOC_ID);
    g_tFrequency = eeprom_read_byte(EEPROM_LOC_T_FREQUENCY);


	//turn off all transmitters by shifting 0 to all transmitters
	for(i=0; i<=g_matrixHeight; i++){
		activateNextLine();
	}

    //config scan signal
    configScanOscillator();
    //ADD: handle CPLD error
    //  ...


    //=== fire ======================
    sei();
}


uint8_t setScanParameter(uint8_t w, uint8_t h, uint8_t f){
uint8_t  em, i;
uint32_t my_scanPeriod;
uint16_t my_deltaTRow;
uint32_t bw;

    em = 0;

    if(w < 1){
        w = 1;
        em = 1;
    }
    
    if(w > MAX_MATRIX_WIDTH){
        w = MAX_MATRIX_WIDTH;
        em = 1;
    }

    if(h < 1){
        h = 1;
        em = 1;
    }
    
    if(h > MAX_MATRIX_HEIGHT){
        h = MAX_MATRIX_HEIGHT;
        em = 1;
    }

    if(f < MIN_SCAN_FREQUENCY){
        f = MIN_SCAN_FREQUENCY;
        em = 1;
    }
    
    if(f > MAX_SCAN_FREQUENCY){
        f = MAX_SCAN_FREQUENCY;
        em = 1;
    }

    //check if sample time per row is sufficient
    my_scanPeriod = 1000000/(uint32_t)f; //(1000000us)
    my_deltaTRow  = (my_scanPeriod/(h+1))+1; //(1000000us)
    if(my_deltaTRow < MIN_DELTA_T_ROW){
        my_deltaTRow = MIN_DELTA_T_ROW;
        em = 1;
    }

    //check required bandwidth
    bw = (uint32_t)w*h*f*10 + LENGTH_STATUS_MSG*10;
    bw += bw/10; //10% increase to have some buffer
    if(bw > (((uint32_t)UART_BAUD_RATE_HS)<<g_BaudRateFactor)){
        em = 1;
    }

    //set new values
    g_scanPeriod = my_scanPeriod;
    g_deltaTRow = my_deltaTRow;
    g_matrixWidth = w;
    g_matrixHeight = h;
    g_scanFrequency = f;
    g_mode = ABS_MODE;

    for(i=0; i<255; i++){
        g_T1[i] = i/(50/g_matrixHeight);//exact weight might vary to g_matrixHeight (for now it's 50/g_matrixHeight)
    }
    g_T1[255] = 255/(50/g_matrixHeight);//exact weight might vary to g_matrixHeight (for now it's 50/g_matrixHeight)

    if(em == 0){
        g_errorState &= ~ERROR_PARAMETERS;
    }

    return em;
}



//=== interrupt routines ==============
// main system timer interrupt (@100us)
ISR( TIMER1_COMPA_vect ) {
static uint8_t sctr=0;
static uint16_t scanCounter=0, targetTime=0;
static uint8_t swBefore;
uint8_t v0;

#ifdef TASSI
    if(g_mode > STANDBY_MODE) {
#endif
	//move through read state according to following scheme:
	//read state = 0: reset
	//read state = 1: trigger read sequence
	//read state = 2: take measurement and transfer values
	if(g_readState == 0){
		scanCounter = 0;
		targetTime = g_deltaTRow;
		g_readState = 1;
	}else{
		if(scanCounter >= targetTime){
            //switch to next line
			if(scanCounter >= g_scanPeriod){
				scanCounter -= g_scanPeriod;
				targetTime = g_deltaTRow;
			}else{
				targetTime += g_deltaTRow;
			}
			g_readState = 1;
		}
		scanCounter += 100;//us
	}
#ifdef TASSI
    }
#endif


    //divide frequency by 10 -->rate 1ms
	sctr++;
	if(sctr >= 10){
		sctr = 0;

		if(g_mainTimer<255){
			g_mainTimer++;// is incremented every 1ms
        }

        //read in DIP switches
        v0 = ~((PIND&0xF0) | ((PING&0x01)<<3) | ((PING&0x02)<<1) | ((PINC&0x01)<<1) | ((PINC&0x02)>>1));
        //debounce
        if(v0 == swBefore){
            g_DIPSwitch = v0;
        }
        swBefore = v0;
	}
}


void sendByte(uint8_t b){
    if(g_UART_select == 0){
        //use UART0
        UCSR0B &= ~_BV(UDRIE0); // disable USART interrupt

        if(g_RTB_Filled >= (TBUFFER_SIZE-1)){
            g_errorState |= ERROR_TBUFFER;
        }else{
            g_RTB_Write_Pointer++;
            if(g_RTB_Write_Pointer >= TBUFFER_SIZE){
                g_RTB_Write_Pointer = 0;
            }
            g_rowTransmissionBuffer[g_RTB_Write_Pointer] = b;
            g_RTB_Filled++;

            if(g_PacketCounter == 0){ //currrently no transmission is going on
                //start transmission when buffer length is reached
                if(g_RTB_Filled >= TX_PACKET_SIZE){
                    PORT_CTS &= ~_BV(CTS);
                    UDR0 = g_rowTransmissionBuffer[g_RTB_Read_Pointer];
                    g_PacketCounter = TX_PACKET_SIZE;
                }
            }
        }

        if(g_PacketCounter > 0){ //transmit remaining/new bytes
            UCSR0B |= _BV(UDRIE0); // enable data register empty interrupt
        }
    }

    if(g_UART_select == 1){
        //use UART1
        UCSR1B &= ~_BV(UDRIE1); // disable USART interrupt

        if(g_RTB_Filled >= (TBUFFER_SIZE-1)){
            g_errorState |= ERROR_TBUFFER;
        }else{
            g_RTB_Write_Pointer++;
            if(g_RTB_Write_Pointer >= TBUFFER_SIZE){
                g_RTB_Write_Pointer = 0;
            }
            g_rowTransmissionBuffer[g_RTB_Write_Pointer] = b;
            g_RTB_Filled++;

            if(g_PacketCounter == 0){ //currrently no transmission is going on
                //start transmission when buffer length is reached
                if(g_RTB_Filled >= TX_PACKET_SIZE){
                    UDR1 = g_rowTransmissionBuffer[g_RTB_Read_Pointer];
                    g_PacketCounter = TX_PACKET_SIZE;
                }
            }
        }

        if(g_PacketCounter > 0){ //transmit remaining/new bytes
            UCSR1B |= _BV(UDRIE1); // enable data register empty interrupt
        }
    }
}


ISR( USART0_UDRE_vect ) {
    if(g_UART_select == 0){//UART0 is active
        g_RTB_Read_Pointer++;
        if(g_RTB_Read_Pointer >= TBUFFER_SIZE){
            g_RTB_Read_Pointer = 0;
        }
        g_RTB_Filled--;
        g_PacketCounter--;
        if(g_PacketCounter > 0){
		    //send next byte
    		UDR0 = g_rowTransmissionBuffer[g_RTB_Read_Pointer];
        }else{
            //check if still more bytes then package size, that is a full package, are left
            if(g_RTB_Filled >= TX_PACKET_SIZE){
                g_PacketCounter = TX_PACKET_SIZE;//start new package
                UDR0 = g_rowTransmissionBuffer[g_RTB_Read_Pointer];
            }else{
                UCSR0B &= ~_BV(UDRIE0); // disable USART interrupt
                PORT_CTS |= _BV(CTS);
            }
        }
	}
}


ISR( USART1_UDRE_vect ) {
    if(g_UART_select == 1){//UART1 is active
        g_RTB_Read_Pointer++;
        if(g_RTB_Read_Pointer >= TBUFFER_SIZE){
            g_RTB_Read_Pointer = 0;
        }
        g_RTB_Filled--;
        g_PacketCounter--;
        if(g_PacketCounter > 0){
		    //send next byte
    		UDR1 = g_rowTransmissionBuffer[g_RTB_Read_Pointer];
        }else{
            //check if still more bytes then package size, that is a full package, are left
            if(g_RTB_Filled >= TX_PACKET_SIZE){
                g_PacketCounter = TX_PACKET_SIZE;//start new package
                UDR1 = g_rowTransmissionBuffer[g_RTB_Read_Pointer];
            }else{
                UCSR1B &= ~_BV(UDRIE1); // disable USART interrupt
            }
        }
	}
}


void sendInt(uint8_t v){
uint8_t v2;

    v2 = v>>4;
    if(v2 < 10){
        sendByte('0'+v2);
    }else{
        sendByte('A'+v2-10);
    }

    v2 = v&15;
    if(v2 < 10){
        sendByte('0'+v2);
    }else{
        sendByte('A'+v2-10);
    }
}



//=== main code ==============
void SPIdummyRead(void){
    //send byte
    SPDR = 0;
    //wait for transmission complete
    while(!(SPSR & _BV(SPIF))); //add time-out functionality here later
}


void performReceiverTara(void){
uint8_t i, ix, UCSR0B_old;
int8_t  si;


    //activate last line to always start sequence from same state
    activateTransmitter(g_matrixHeight);


	//stop clock pulses for 250ms to indicate calibration request
	for(i=0,g_mainTimer=0; i<250; ){ //250ms
		if(g_mainTimer > 0){ // incremented every 1ms
			g_mainTimer = 0;
			i++;
		}
	}


	//scan once through all lines
	g_readState = 0;
	for(si=-1; si<(g_matrixHeight+1); ){ // +1: include additional average scan
		if(g_readState == 1){
			g_readState++;
            if(si >= 0){
    			activateTransmitter(si);
            }else{
    			activateTransmitter(g_matrixHeight);// one extra line as receivers ignore
            }                                       // first line after calibration request

			//read in current row
	        ix = 0;
            //temporarly turn off UART interrupts --> no transmission is going on during row read out time!!!
            cli();
            UCSR0B_old = UCSR0B;
            UCSR0B &= ~(_BV(RXCIE0)|_BV(UDRIE0));
            //wait for reception complete
            loop_until_bit_is_set(SPSR, SPIF);
 
           	SPDR = 0;//transmit/receive first byte
            do{
                //wait for reception complete
                loop_until_bit_is_set(SPSR, SPIF);
            	SPDR = 0;//transmit/receive next byte
                //briefly allow timer 1 interrupt
                sei();
asm (          "nop"     );
asm (          "nop"     );
                cli();
                ix++;
            }while(ix < g_matrixWidth);
            //turn all UART interrupts back on
            UCSR0B = UCSR0B_old;
            sei();
    		//all bytes received

			si++;
		}
	}


	//stop clock pulses for 20ms to terminate calibration
	for(i=0,g_mainTimer=0; i<20; ){ //20ms
		if(g_mainTimer > 0){ // incremented every 1ms
			g_mainTimer = 0;
			i++;
		}
	}


	g_SystemCalibrated = 1;

    g_mode = REL_MODE;

    return;
}


void switchToAbsoluteMode(void){
uint8_t i;

	//stop clock pulses for 150ms to request absolute mode
	for(i=0,g_mainTimer=0; i<150; ){ //150ms
		if(g_mainTimer > 0){ // incremented every 1ms
			g_mainTimer = 0;
			i++;
		}
	}

    g_mode = ABS_MODE;

    return;
}

void performScanningOuter(void);
void performScanning(void);

int
main(void) {
  init();

#ifdef TASSI
  TSC_init();
  g_mode = STANDBY_MODE;
#else
  g_mode = ABS_MODE;
#endif



  //make sure timer interrupt has been called at least once (to get secure DIP-Switch readings)
  g_mainTimer = 0;
  while(g_mainTimer < 2);

  uart0_init();
  uart1_init();
  
  while(1) {
#ifdef TASSI
      g_mode = STANDBY_MODE;
      while(1) {
          while(g_mainTimer > 0) {
              g_mainTimer--;
              TSC_update();
          }
          if(g_power > 0) {
              PORT_HVSHDN |= _BV(HVSHDN);
              uint16_t x;
              for(x = 0; x < 1000; x++) {
                  pause1us();
              }
              break;
          }
      }
#endif
      
      init();
      
      performScanningOuter();
      
      init();
  }

  return 0;
}

void
performScanningOuter(void) {
uint8_t  ix, next_row, SPICounter, rowReceptionComplete;
uint8_t  v0, v1, e;
uint16_t autoCalTimer;
uint16_t checksum;
uint8_t  tVoltage;
uint8_t  ADCcounter=0;
uint8_t  sc2=0, sc3=0;
//uint8_t  statusBytesRingCounter=0;
uint8_t  UCSRnB_old;

#ifdef TEST_MODE
uint8_t  testBuffer[TESTBUFFERLENGTH];
#endif


    //decide which UART to use depending on DIP switch no. 5
    if((g_DIPSwitch&_BV(DIP_UART_SELECT)) == 0){
        g_UART_select = 0; //use UART0
    }else{
        g_UART_select = 1; //use UART1
    }

    if(g_UART_select == 0){
        stdout = &g_uart0_str;

    }else{
        stdout = &g_uart1_str;
        g_ConfigMode = 1;//in COM1-Version always start in config mode as there is no CTS control
    }


#ifdef TEST_MODE
   	fprintf(stdout, "\nHello!\n");
#endif


    //turn on 12V generator
    DDR_HVSHDN |= _BV(HVSHDN);
    PORT_HVSHDN &= ~_BV(HVSHDN);
    //set DAC value
    //e = setDACValue(106); //5V*21,2(*1/V)
    v1 = eeprom_read_byte(EEPROM_LOC_VOLTAGE_TRANSMITTER);
    e = setDACValue(v1);
  	if((g_errorState&ERROR_IIC) == 0){
#ifdef TEST_MODE
        fprintf(stdout, "Scan voltage set.\n");
#endif
    }else{
        //ADD: error handling: high voltage couldn't be turned on
        //  [...]
    }


	//initialize scanning
	autoCalTimer = 4*1000; //4sec; automated calibration 4 seconds after power up
	g_mainTimer = 0;

	g_currentRow = 0; //start scan with row 0
	g_readState = 0;

    configSpecialScanMode(g_matrixHeight);

    PORT_CLK_EN |= _BV(CLK_EN); //CLK_EN ->high to enable outputs



    //MAIN SCAN LOOP
	while(1){
		if(g_readState == 1){
        //begin of row sampling
			g_readState++;

			//switch transmitter to next row
			next_row = g_currentRow + 1;
			if(next_row >= (g_matrixHeight+1)){ // +1: there is one extra row sampling to determine average line load
				next_row = 0;
            }
			activateTransmitter(next_row);

			//read in current row
	        SPICounter = 0;
            //temporarly turn off UART RX interrupts during row read in
            cli();
            if(g_UART_select == 0){
                UCSRnB_old = UCSR0B;
                UCSR0B &= ~(_BV(RXCIE0)|_BV(UDRIE0));
            }else{
                UCSRnB_old = UCSR1B;
                UCSR1B &= ~(_BV(RXCIE1)|_BV(UDRIE1));
            }
            //wait for reception complete
            loop_until_bit_is_set(SPSR, SPIF);
           	SPDR = 0;//transmit/receive first byte
            do{
                //wait for reception complete
                loop_until_bit_is_set(SPSR, SPIF);
                //store received byte
                v1 = SPDR;
            	SPDR = 0;//transmit/receive next byte
                //briefly allow UART TX interrupts
                sei();
                g_rowBuffer[SPICounter] = v1;
                cli();
                SPICounter++;
            }while(SPICounter < g_matrixWidth);
            //turn all UART interrupts back on
            if(g_UART_select == 0){
                UCSR0B = UCSRnB_old;
            }else{
                UCSR1B = UCSRnB_old;
            }
            sei();
    		//all bytes received
    		rowReceptionComplete = 1;
		}


		//wait until new row is available
		if(rowReceptionComplete){
			rowReceptionComplete = 0;

            //search for grid points with max/min levels when requested
            if(g_maxMinLevelRequest == 2){
                if(g_currentRow < g_matrixHeight){
    	    	    for(ix=0; ix<g_matrixWidth; ix++){
                        v1 = g_rowBuffer[ix];
                        if(v1 > g_maxLevel){
                            g_maxLevel = v1;
                            g_PMaxX = ix;
                            g_PMaxY = g_currentRow;
                        }
                        if(v1 < g_minLevel){
                            g_minLevel = v1;
                            g_PMinX = ix;
                            g_PMinY = g_currentRow;
                        }
                    }
                }else{//don't include last row (average scan) in max/min levels
                    g_maxMinLevelRequest = 0;
                }
            }

            //grab level at designated coordinates when requested
            if(g_XYLevelRequest == 1){
                if(g_currentRow < g_matrixHeight){
                    if(g_currentRow == g_arg2){
                        g_XYLevel = g_rowBuffer[g_arg1];
                    }
                }else{//don't include last row (average scan)
                    g_XYLevelRequest = 0;
                }
            }


#ifdef COL_CORRECTION
            if(g_SystemCalibrated == 1){
 			    //correct values to minimize crosstalk between lines dueto weak receiver line
                if(g_currentRow < g_matrixHeight){
   	    		    for(ix=0; ix<g_matrixWidth; ix++){
       	    		    //adjust values of new row
                        v1 = g_colAverage[ix];
                        if(g_rowBuffer[ix] > v1){
                            g_rowBuffer[ix] -= v1;
                        }else{
                            g_rowBuffer[ix] = 0;
                        }
        		    }
                }else{
        	    	for(ix=0; ix<g_matrixWidth; ix++){
                        v1 = g_rowBuffer[ix];
                        if(v1 > VZERO){
                            v1 -= VZERO;
                        }else{
                            v1 = 0;
                        }
	  	        		g_colAverage[ix] = g_T1[v1];
		            }
                }
            }
#endif


            //print or transmit values unless in config mode
            if(g_ConfigMode == 0){
#ifdef TEST_MODE
	        	//print current ADC value
    	    	if(g_currentRow == 0){
		        	fprintf(stdout, "\n");
                    //fprintf(stdout, "%4.u ", ADCValue_C0);
    	    	}
        		if(g_currentRow < TESTBUFFERLENGTH){
                    //copy values
                    if(1){//if true sample column
                        if(g_currentRow<g_matrixHeight)
                            testBuffer[g_currentRow] = g_rowBuffer[12]; //sample column 12
                        else
                            testBuffer[g_currentRow] = 0;
                    }else{
                        if(g_currentRow == 3){ //sample row 3
                            for(i=0; i<TESTBUFFERLENGTH; i++){
                                if(i<g_matrixWidth)
                                    testBuffer[i] = g_rowBuffer[i];
                                else
                                    testBuffer[i] = 0;
                            }
                        }
                    }
                    //print test values one at a time
    		  		v1 = testBuffer[g_currentRow];
	        		fprintf(stdout, "%3.u ", v1);
    	    	}
#endif
#ifndef TEST_MODE
                //if enabled by corresponding DIP switch transmit only when RTS is passive
                if((g_dataTransmitMode&(1+4)) != (1+4)){
        		   	//line is ready to send to host
                    //send data only if UART high speed mode is enabled
                    if(g_UART_mode == HS){
                        if(g_currentRow < g_matrixHeight){//don't send average sample
                            //copy bytes to TX buffer
                            sendByte(255);
                            sendByte(g_currentRow+1);
                            for(ix=0; ix<g_matrixWidth; ix++){
                                //limit byte value to range 0-254
                                v1 = g_rowBuffer[ix];
                                if(v1 == 255){//substitute 255 by 254 because 255 is a reserved special character
                                    v1 = 254;
                                }
                                sendByte(v1);
                                checksum += v1;
                            }
                        }else{//send info bytes
                            g_FrameNumber++;//increment frame number
  #ifdef SEND_STATUS_BYTES
                            //send header bytes
                            sendByte(255);//reserved byte to indicate start of header bytes/start of frame
                            sendByte(0);
//                            switch(statusBytesRingCounter){
//                                case 0:
                                    //send version
                                    sendByte('V');
                                    sendInt(VERSION);
                                    sendInt(SUBVERSION);
//                                    statusBytesRingCounter++;
//                                    break;
//                                case 1:
                                    //send status
                                    sendByte('S');
                                    sendInt(0);
                                    sendInt(g_status);
//                                    statusBytesRingCounter++;
//                                    break;
//                                case 2:
                                    //send ID
                                    sendByte('I');
                                    sendInt(0);
                                    sendInt(g_ID);
//                                    statusBytesRingCounter++;
//                                    break;
//                                case 3:
                                    //send mode
                                    sendByte('M');
                                    sendInt(0);
                                    sendInt(g_mode);
//                                    statusBytesRingCounter++;
//                                    break;
//                                case 4:
                                    //send scan frequency
                                    sendByte('F');
                                    sendInt(0);
                                    sendInt(g_scanFrequency);
//                                    statusBytesRingCounter++;
//                                    break;
//                                case 5:
                                    //send width
                                    sendByte('W');
                                    sendInt(0);
                                    sendInt(g_matrixWidth);
//                                    statusBytesRingCounter++;
//                                    break;
//                                case 6:
                                    //send height
                                    sendByte('H');
                                    sendInt(0);
                                    sendInt(g_matrixHeight);
//                                    statusBytesRingCounter++;
//                                    break;
//                                case 7:
                                    //send grid size
                                    sendByte('G');
                                    sendInt(0);
                                    sendInt(g_gridSpacing);
//                                    statusBytesRingCounter++;
//                                    break;
//                                case 8:
//                                default:
//                                    statusBytesRingCounter = 0;
//                            }
                            //send frame number
                            sendByte('N');
                            sendInt(g_FrameNumber>>8);
                            sendInt(g_FrameNumber&255);
                            //send check sum
                            sendByte('C');
                            sendInt(checksum>>8);
                            sendInt(checksum&255);
                            checksum = 0;
                            //make sure that correct number of bytes is in header file (LENGTH_STATUS_MSG)
  #endif
                         }
                    }
                }
#endif
            }
            //switch to next line
        	g_currentRow = next_row;
        	if(g_currentRow == 0){
                if(g_maxMinLevelRequest == 1){
                    g_maxMinLevelRequest = 2;
                    g_maxLevel = 0;
                    g_minLevel = 255;
                }
            }
            //end of row sampling
            //quite some time is lost in sendByte(); optimizing this function will speed up things tremendously
	    }

        //check if system should switch to config mode because of RTS active
        if((g_dataTransmitMode&1) != 0){
            if((g_dataTransmitMode&4) != 0){//RTS-line
                g_ConfigMode = 2;//skip config mode welcome message and directly go to step 2
            }
        }

        handleConfigRequests();


        if(g_TaraRequest == 1){
            performReceiverTara();
            //reset read cycle
            g_currentRow = g_matrixHeight; //by this next row will be row 0
            g_readState = 0;
            checksum = 0;
            g_TaraRequest = 0;
        }

        if(g_TaraRequest == 2){
            //find optimal transmitter level
            //first turn receivers to abs mode
        	switchToAbsoluteMode();
            //then turn transmit level to maximum
            tVoltage = 255;
            setDACValue(tVoltage);
            //get maximum levels
            g_maxMinLevelRequest = 1;
            g_TaraRequest = 3; //switch to next state
        }else if(g_TaraRequest == 3){
            if(g_maxMinLevelRequest == 0){
                //check if level is below 250
                if(g_maxLevel < 250){
                    if(g_maxLevel > 200  &&  g_minLevel > 100){
                        //-->voltage found
                        //store value in EEPROM
                        eeprom_write_byte(EEPROM_LOC_VOLTAGE_TRANSMITTER, tVoltage);
                        g_errorState &= ~(ERROR_T_LEVEL2 | ERROR_T_LEVEL1);
                        g_TaraRequest = 4; //switch to next state
                    }else{ //if g_maxLevel is close to 0: search failed!
                        g_errorState |= ERROR_T_LEVEL1;
                        g_TaraRequest = 4; //switch to next state
                    }
                }else{
                    //reduce voltage and try again
                    if(tVoltage > 70){ //minium level is 70 (3V)
                        tVoltage -= 2; //search in -2 steps
                        setDACValue(tVoltage);
                        //get maximum levels
                        g_maxMinLevelRequest = 1;
                    }else{
                        //failed!
                        g_errorState |= ERROR_T_LEVEL2;
                        g_TaraRequest = 4; //switch to next state
                    }
                }
            }
        }else if(g_TaraRequest == 4){
            //reset read cycle
            g_currentRow = g_matrixHeight; //by this next row will be row 0
            g_readState = 0;
            checksum = 0;
            g_TaraRequest = 0;
        }

        if(g_TaraRequest == 10){
            switchToAbsoluteMode();
            //reset read cycle
            g_currentRow = g_matrixHeight; //by this next row will be row 0
            g_readState = 0;
            checksum = 0;
            g_TaraRequest = 0;
        }


        if(g_mainTimer > 0){
            //read ADC conversion result
            //check if conversion is finished
            if((ADCSRA&_BV(ADSC)) == 0){
                switch(ADCcounter){
                    case 0:
                        //channel 0:
                        v0 = ADCL;
                        v1 = ADCH;
                        ADCValue_C0 = (v1<<8)|v0; //Vtransmitter (82,82/V)
                        break;
                    case 2:
                        //channnel 1:
                        v0 = ADCL;
                        v1 = ADCH;
                        ADCValue_C1 = (v1<<8)|v0;//VCC (163,48/V)
                        break;
                    default:
                        ;
                }                    
                //sample 0V between each channel to minimize crosstalk
                ADCcounter++;
                if(ADCcounter >= 4){
                    ADCcounter = 0;
                }
                if((ADCcounter&1) == 0){
                    ADMUX = (ADMUX&0xE0) | (ADCcounter>>1); //channel 0 or channel 1
                }else{
                    ADMUX = (ADMUX&0xE0) | 0x1F; //0V
                }
                //start next conversion
                ADCSRA |= _BV(ADSC);
            }

            //process DIP switch no. 6
            if((g_DIPSwitch&_BV(DIP_RTS_CONTROL)) == 0  &&  g_UART_select == 0){//only applicable when uart0 is selected
                g_dataTransmitMode |= 1; //set flag: data transmission is controlled by RTS
            }else{
                g_dataTransmitMode &= ~1;
            }
            //process RTS-line
            if((PIN_RTS&_BV(RTS)) != 0){
                g_dataTransmitMode |= 4;
            }else{
                g_dataTransmitMode &= ~4;
            }

            //toggle LEDs
            //RED LED
            if(g_errorState == 0){
                //turn off red LED
                PORT_LED &= ~_BV(LED_R);
                sc2 = 0;
            }else{
                if(sc2 == 0){
                    //toggle red LED
                    PORT_LED ^= _BV(LED_R);
                }
            sc2++;
            }
            if(g_mode == REL_MODE){
                //turn on green LED
                PORT_LED |= _BV(LED_G);
                sc3 = 0;
            } else if (g_mode == ABS_MODE) {
                if(sc3 == 0){
                    //toggle green LED
                    PORT_LED ^= _BV(LED_G);
                    if((PORT_LED&_BV(LED_G)) == 0){
                        sc3=180; //off period is shorter
                    }
                }
                sc3++;
            } else {
                PORT_LED ^= ~_BV(LED_G);
            }


            //trigger automatic calibration a few seconds after start-up
            while(g_mainTimer > 0){
                g_mainTimer--;

#ifdef TASSI
                TSC_update();
#endif

                if(autoCalTimer > 0){
                    autoCalTimer--;
                    if(autoCalTimer == 1){
                        if((g_DIPSwitch&_BV(DIP_AUTO_SWITCH_TO_REL_MODE)) != 0){
                            g_TaraRequest = 1;
                        }
                        autoCalTimer = 0;
                    }
                }
            }
        }

#ifdef TASSI
        if(g_power == 0) {
            break;
        }
#endif
    }
}


/*************************************************************************
*****    serial command reception processing                           ***
*************************************************************************/
ISR( USART0_RX_vect ) {
uint8_t c;

    if(g_UART_select == 0){ //UART0 is selected
        //check error bits first
        c = UCSR0A;
        if((c&_BV(FE0)) != 0){
            //framing error occured; ignore byte
            //clear receive buffer
            g_UARTBytesReceived=0;
            //dummy read to clear RXC0
            c = UDR0;
        }else{
            //read in byte
            c = UDR0;
            processReceivedByte(c);
        }
    }else{
        //dummy read to clear RXC0
        c = UDR0;
    }

    return;
}


ISR( USART1_RX_vect ) {
uint8_t c;

    if(g_UART_select == 1){ //UART1 is selected
        //check error bits first
        c = UCSR1A;
        if((c&_BV(FE1)) != 0){
            //framing error occured; ignore byte
            //clear receive buffer
            g_UARTBytesReceived=0;
            //dummy read to clear RXC1
            c = UDR1;
        }else{
            //read in byte
            c = UDR1;
            processReceivedByte(c);
        }
    }else{
        //dummy read to clear RXC1
        c = UDR1;
    }

    return;
}

void
parseInt(char *buf, uint8_t len, uint8_t *res) {
    uint8_t r = 0;
    uint8_t i = 0;
    char c;

    while(i < len) {
        c = buf[i];
        if(c == ',') {
            goto ret;
        } else if(c >= '0' && c <= '9') {
            r = r*10 + c-'0';
        } else {
            goto err;
        }
        i++;
    }

  ret:
    *res = r;
    return;
    
  err:
    *res = 0; // XXX: report error somehow
    return;    
}

void processReceivedByte(uint8_t c){
uint8_t i, j;

#define CR 13
#define LF 10

#define recBufferSize 10
static char recBuffer[recBufferSize];

#define C01  1
#define C02  2
#define C03  3
#define C04  4
#define C05  5
#define C06  6
#define C07  7
#define C11  8
#define C12  9
#define C21  10
#define C22  11
#define C23  12
#define C24  13
#define C25  14
#define C26  15
#define C98  16
#define C99  17
#define NrOfCMDs 17
#define CMDLength 3 //for now, all commands have to be 3 characters long!

//das sollte noch in den Programm-Speicher um RAM zu sparen...
uint8_t commandList[NrOfCMDs][CMDLength] = {       \
                             {"C01"},      \
                             {"C02"},      \
                             {"C03"},      \
                             {"C04"},      \
                             {"C05"},      \
                             {"C06"},      \
                             {"C07"},      \
                             {"C11"},      \
                             {"C12"},      \
                             {"C21"},      \
                             {"C22"},      \
                             {"C23"},      \
                             {"C24"},      \
                             {"C25"},      \
                             {"C26"},      \
                             {"C98"},      \
                             {"C99"}};

#ifdef TASSI
    if(g_power == 0) {
        goto skipProcessing;
    }
#endif

    //ignore LF
    if(c == LF){
        return;
    }

    //ignore space
    if(c == ' '){
        return;
    }

    if(c != CR){
        //convert all to capital characters
        if(c>'a' && c<'z'){
            c -= 'a'-'A';
        }
        //add to receive buffer if in valid range, and there is no buffer overrun
        if(c >= 32  &&  c <= 90  &&  g_UARTBytesReceived < recBufferSize){
            recBuffer[g_UARTBytesReceived] = c;
            g_UARTBytesReceived++;
        }else{
            //clear receive buffer
            g_UARTBytesReceived=0;
        }
        return;
    }

    //CR received; parse received bytes
    if(g_ConfigMode == 0  ||  g_ConfigMode == 2){
        //wait for start command ('x')
        if(g_UARTBytesReceived == 1){
            //check received byte is an 'x'
            if(recBuffer[0] == 'X'){
                //change to config mode
                g_ConfigMode = 1;
            }
        }
    }
    
    if(g_ConfigMode >= 1){
        if(g_UARTBytesReceived >= CMDLength){
            //check if buffer matches one command of command list
            for(j=0; j<NrOfCMDs; j++){
                for(i=0; i<CMDLength; i++){
                    if(commandList[j][i] != recBuffer[i]){
                        break;
                    }
                }
                if(i == CMDLength){
                    break; //match found!
                }
            }
            if(j<NrOfCMDs){
                j++; //adjust offset 1
                if(g_NextCommand == 0){//begin executing new command

                    g_arg1 = 0;
                    g_arg2 = 0;

                    switch(j) {
                    // commands with two arguments
                    case C12:
                        parseInt(recBuffer + i, g_UARTBytesReceived - i, &g_arg2);
                    // commands with one argument
                    case C21:
                    case C22:
                    case C23:
                    case C24:
                    case C25:
                    case C26:
                        parseInt(recBuffer + i, g_UARTBytesReceived - i, &g_arg1);
                        break;

                    default:
                        break;
                    }

                    g_NextCommand = j;
                }else{
                    //previous command is still pending, so ignore new command
                    //ADD err msg. here later
                }
            }
        }
    }


#ifdef TASSI
 skipProcessing:
#endif

    //clear receive buffer
    g_UARTBytesReceived=0;
}


void handleConfigRequests(void){
uint8_t         e;
static uint16_t pointer1=0;

    //config mode stuff
    if(g_ConfigMode > 0){
        //system is in config mode
        if(g_ConfigMode == 1){//while g_ConfigMode=1 print welcome message
            if(pointer1 == 0){
                //clear current transmission
                if(g_UART_select == 0){
                    UCSR0B &= ~ _BV(UDRIE0); // disable USART interrupt
                }else{
                    UCSR1B &= ~ _BV(UDRIE1); // disable USART interrupt
                }
                g_RTB_Filled = g_RTB_Write_Pointer = g_RTB_Read_Pointer = g_PacketCounter = 0;

                pointer1 = &configWelcomeMsg;
            }
            e = pgm_read_byte(pointer1++);//get next byte from FLASH;
            if(e == '\0'){
                fprintf(stdout, "OK\n");
                g_ConfigMode = 2;
                pointer1 = 0;
            }else{
                if(g_UART_select == 0){
                    uart0_putchar(e, stdout);
                }else{
                    uart1_putchar(e, stdout);
                }
            }
        }else if(g_ConfigMode == 2){//wait for command
            switch(g_NextCommand){
                case C01: //optimize transmitter level command
                    g_TaraRequest = 2;
                    g_ConfigMode = 5;//wait until calibration done
                    break;
                case C02: //tara command
                    g_TaraRequest = 1;
                    g_ConfigMode = 5;//wait until calibration done
                    break;
                case C03: //print current error state
                    fprintf(stdout, "\nErr%02X\n", g_errorState);
                    g_NextCommand = 0;
                    break;
                case C04: //clear error state
                    g_errorState = 0;
                    fprintf(stdout, "\nOK\n");
                    g_NextCommand = 0;
                    break;
                case C05: //print current ID
                    fprintf(stdout, "\n%u\n", g_ID);
                    g_NextCommand = 0;
                    break;
                case C06: //print current (HS)Baud rate
                    fprintf(stdout, "\n%u (", g_BaudRateFactor);
                    switch(g_BaudRateFactor){
                        case 0: fprintf(stdout, "57600bps)\n"); break;
                        case 1: fprintf(stdout, "115200bps)\n"); break;
                        case 2: fprintf(stdout, "230400bps)\n"); break;
                        case 3: fprintf(stdout, "460800bps)\n"); break;
                        case 4: fprintf(stdout, "921600bps)\n"); break;
                    }
                    g_NextCommand = 0;
                    break;
                case C07: //switch to absolute mode
                    g_TaraRequest = 10;
                    g_ConfigMode = 5;//wait until done
                    break;
                case C11: //print max/min values
                    g_maxMinLevelRequest = 1;
                    g_ConfigMode = 6;
                    break;
                case C12: //print x/y values
                    if(g_arg1 >= g_matrixWidth  ||  g_arg2 >= g_matrixHeight){
                        fprintf(stdout, "\nOut of range\n");
                        g_NextCommand = 0;
                    }else{
                        g_XYLevelRequest = 1; // g_arg1: x coordinate, g_arg2: y coordinate
                        g_ConfigMode = 7;
                    }
                    break;
                case C21: //set width/number of columns
                    //first check if locked (switch 8 on)
                    if((g_DIPSwitch&_BV(DIP_EEPROM_LOCK)) != 0){
                        fprintf(stdout, "\nDevice is locked!\n");
                    }else{
                        if(g_arg1 == 0  ||  g_arg1 > MAX_MATRIX_WIDTH){
                            fprintf(stdout, "\nOut of range\n");
                        }else{
                            if(setScanParameter(g_arg1, g_matrixHeight, g_scanFrequency) == 0){
                                eeprom_write_byte(EEPROM_LOC_MATRIX_WIDTH, g_arg1);
                                switchToAbsoluteMode();
                                fprintf(stdout, "\nOK\n");
                            }else{
                                fprintf(stdout, "\nOut of range\n");
                            }
                        }
                    }
                    g_NextCommand = 0;
                    break;
                case C22: //set height/number of rows
                    //first check if locked (switch 8 on)
                    if((g_DIPSwitch&_BV(DIP_EEPROM_LOCK)) != 0){
                        fprintf(stdout, "\nDevice is locked!\n");
                    }else{
                        if(g_arg1 == 0  ||  g_arg1 > MAX_MATRIX_HEIGHT){
                            fprintf(stdout, "\nOut of range\n");
                        }else{
                            if(setScanParameter(g_matrixWidth, g_arg1, g_scanFrequency) == 0){
                                eeprom_write_byte(EEPROM_LOC_MATRIX_HEIGHT, g_arg1);
                                switchToAbsoluteMode();
                                fprintf(stdout, "\nOK\n");
                            }else{
                                fprintf(stdout, "\nOut of range\n");
                            }
                        }
                    }
                    g_NextCommand = 0;
                    break;
                case C23: //set scan frequency
                    //first check if locked (switch 8 on)
                    if((g_DIPSwitch&_BV(DIP_EEPROM_LOCK)) != 0){
                        fprintf(stdout, "\nDevice is locked!\n");
                    }else{
                        if(g_arg1 < MIN_SCAN_FREQUENCY  ||  g_arg1 > MAX_SCAN_FREQUENCY){
                            fprintf(stdout, "\nOut of range\n");
                        }else{
                            if(setScanParameter(g_matrixWidth, g_matrixHeight, g_arg1) == 0){
                                eeprom_write_byte(EEPROM_LOC_SCAN_FREQUENCY, g_arg1);
                                fprintf(stdout, "\nOK\n");
                            }else{
                                fprintf(stdout, "\nOut of range\n");
                            }
                        }
                    }
                    g_NextCommand = 0;
                    break;
                case C24: //set baud rate factor
                    //first check if locked (switch 8 on)
                    if((g_DIPSwitch&_BV(DIP_EEPROM_LOCK)) != 0){
                        fprintf(stdout, "\nDevice is locked!\n");
                    }else{
                        if(g_arg1 > UART_MAX_BAUD_RATE_FACTOR){
                            fprintf(stdout, "\nOut of range\n");
                        }else{
                            e = g_BaudRateFactor;
                            g_BaudRateFactor = g_arg1;
                            if(setScanParameter(g_matrixWidth, g_matrixHeight, g_scanFrequency) != 0){
                                g_BaudRateFactor = e;
                                fprintf(stdout, "\nOut of range\n");
                            }else{
                                eeprom_write_byte(EEPROM_LOC_BAUD_RATE, g_arg1);
                                //g_BaudRateFactor = g_arg1;
                                fprintf(stdout, "\nWill be applied after restart.\n");
                                fprintf(stdout, "OK\n");
                            }
                        }
                    }
                    g_NextCommand = 0;
                    break;
                case C25: //set grid spacing (in mm)
                    //first check if locked (switch 8 on)
                    if((g_DIPSwitch&_BV(DIP_EEPROM_LOCK)) != 0){
                        fprintf(stdout, "\nDevice is locked!\n");
                    }else{
                        if(g_arg1 < MIN_GRID_SPACING  ||  g_arg1 > MAX_GRID_SPACING){
                            fprintf(stdout, "\nOut of range\n");
                        }else{
                            eeprom_write_byte(EEPROM_LOC_GRID_SPACING, g_arg1);
                            g_gridSpacing = g_arg1;
                            fprintf(stdout, "OK\n");
                        }
                    }
                    g_NextCommand = 0;
                    break;
                case C26: //select transmitter frequency
                    //first check if locked (switch 8 on)
                    if((g_DIPSwitch&_BV(DIP_EEPROM_LOCK)) != 0){
                        fprintf(stdout, "\nDevice is locked!\n");
                    }else{
                        if(g_arg1 > MAX_T_FREQUENCY){
                            fprintf(stdout, "\nOut of range\n");
                        }else{
                            eeprom_write_byte(EEPROM_LOC_T_FREQUENCY, g_arg1);
                            fprintf(stdout, "\nWill be applied after restart.\n");
                            fprintf(stdout, "OK\n");
                        }
                    }
                    g_NextCommand = 0;
                    break;
                case C98: //reply with "OK"
                    fprintf(stdout, "\nOK\n");
                    g_NextCommand = 0;
                    break;
                case C99: //exit config mode
                    fprintf(stdout, "\nOK\n");
                    g_NextCommand = 0;
                    g_ConfigMode = 0;// exit config mode
                    break;
                default:
                    ;
             }
        }else if(g_ConfigMode == 5){//wait for calibration to be done
            if(g_TaraRequest == 0){
                if((g_errorState&(ERROR_T_LEVEL2|ERROR_T_LEVEL1)) == 0){
                    fprintf(stdout, "\nOK\n");
                }else{
                    fprintf(stdout, "\nErr%02X\n", g_errorState);
                }
                g_NextCommand = 0;
                g_ConfigMode = 2;// return to config mode
            }
        }else if(g_ConfigMode == 6){//print min/max values
            if(g_maxMinLevelRequest == 0){
                fprintf(stdout, "%3.u (%2.u,%2.u)  %3.u (%2.u,%2.u)\n", g_maxLevel, g_PMaxX, g_PMaxY, g_minLevel, g_PMinX, g_PMinY);
                g_maxMinLevelRequest = 1;
            }
            //stop if next byte is received
            if(g_UARTBytesReceived != 0){
                g_NextCommand = 0;
                g_ConfigMode = 2;// return to config mode
            }
        }else if(g_ConfigMode == 7){//print x/y value
            if(g_XYLevelRequest == 0){
                fprintf(stdout, "%3.u\n", g_XYLevel);
                g_XYLevelRequest = 1;
            }
            //stop if next byte is received
            if(g_UARTBytesReceived != 0){
                g_NextCommand = 0;
                g_ConfigMode = 2;// return to config mode
            }
        }
    }
}
