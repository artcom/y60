//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
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

#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "uart64.h"
#include "defines.h"
#include <avr/pgmspace.h>

//=== Configuration ==========================================================

//=== Globals ================================================================
uint8_t  g_rowBuffer[MAX_MATRIX_WIDTH], g_colAverage[MAX_MATRIX_WIDTH];
uint8_t  g_RTB_Write_Pointer=0, g_RTB_Read_Pointer=0, g_RTB_Filled=0, g_PacketCounter=0;

#define TBUFFER_SIZE MAX_MATRIX_WIDTH+TX_PACKET_SIZE+LENGTH_STATUS_MSG //should be large enough to hold all that
uint8_t  g_rowTransmissionBuffer[TBUFFER_SIZE];

uint8_t  g_SPICounter, g_currentRow, g_rowReceptionComplete=0;

uint16_t g_scanTime = 1000000/SCAN_FREQUENCY; //(1000000us)
uint16_t g_deltaT   = (1000000/SCAN_FREQUENCY/(DEFAULT_MATRIX_HEIGTH+1))+1; //(1000000us)
uint16_t g_FrameNumber;

uint8_t  g_mainTimer2=0, g_readState=0;

uint8_t  g_SystemCalibrated=0, g_TaraRequest=0;

uint8_t  g_ConfigMode=0, g_NextCommand=0;

uint8_t  g_DIPSwitch;

uint16_t ADCValue_C0, ADCValue_C1;


FILE    g_uart0_str = FDEV_SETUP_STREAM(uart0_putchar, uart0_getchar, _FDEV_SETUP_RW);

const char configWelcomeMsg[] PROGMEM = "\nCommands:\nC01: Auto-calibrate transmission level \
\nC02: Tara and switch to relative mode \
\nC03: Print current error state \
\nC98: Just replies 'ok' \
\nC99: Resume normal operation\n@";//must be terminated with '@'!


void init(void) {
uint8_t i;
    //init LEDs
	DDR_LED |= _BV(LED_R);
	PORT_LED &= ~_BV(LED_R);
	DDR_LED |= _BV(LED_G);
	PORT_LED &= ~_BV(LED_G);

    //init DIP switch pins
    DDRD &= ~0xF0;
    DDRG &= ~0x03;
    DDRC &= ~0x03;
    PORTD |= 0xF0; //turn on pull-up resistors
    PORTG |= 0x03;
    PORTC |= 0x03;

    //init opto coupler
    DDRE |= _BV(SNC_OUT);
    PORTE &= ~_BV(SNC_OUT);
    DDRE &= ~_BV(SNC_IN);

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


    //===  init UART ==================
	uart0_init();
	stdout = &g_uart0_str;
    PORT_CTS |= _BV(CTS); //toggle CTS to force FTDI chip to flush buffer
    DDR_CTS |= _BV(CTS);


    //user timer1 for main system time
    TCCR1B = _BV(WGM12) | 1; //mode4: CTC, no prescaling
	OCR1A = (F_CPU/10000)-1; //approx. 10000Hz
    TIMSK |= _BV(OCIE1A);    //enable output compare match interrupt


	//=== init SPI ====
	DDRB |= _BV(PB2) | _BV(PB1) | _BV(PB0);              //MOSI, SCK, and #SS output (#SS must be configured as output pin!)
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPHA) | _BV(SPR0); //sample on falling edge, Speed: 1,8432MHz @ 14.745MHz
	SPSR |= _BV(SPI2X);


    //init TWI
    TWBR = 12; //f = 369KHz
    TWCR = _BV(TWEN);


    //=== init ADC ====
    ADMUX = _BV(REFS1) | _BV(REFS1) | 0; //internal 2.56V voltage reference; channel 0
    ADCSRA = _BV(ADEN) | 7;              //prescaler = 128


    //wait a couple of us to allow CPLD to start up
    for(i=0; i<20; i++){
        PAUSE_1_US()
    }


    g_ID = ID;
    g_matrixWidth = DEFAULT_MATRIX_WIDTH;
    g_matrixHeigth = DEFAULT_MATRIX_HEIGTH;
    g_mode = ABS_MODE;
    g_errorState = 0;

	//turn off all transmitters by shifting 0 to all transmitters
	for(i=0; i<=MAX_MATRIX_HEIGTH; i++){
		activateNextLine();
	}

    //config scan signal
    configScanOscillator();
    //ADD: handle CPLD error

    //=== fire ======================
    sei();
}


//=== interrupt routines ==============
// main system timer interrupt (@100us) (duration: appr. 5us)
ISR( TIMER1_COMPA_vect ) {
static uint8_t sc=0, sc2, sc3;
static uint16_t scanCounter=0, targetTime=0;
static uint8_t ADCcounter=0;
static uint8_t swBefore;
uint8_t v0, v1;

	g_mainTimer2 = 1;// is set every 100us

	//move through read state according to following scheme:
	//read state = 0: reset
	//read state = 1: trigger read sequence
	//read state = 2: take measurement and transfer values
	if(g_readState == 0){
		scanCounter = 0;
		targetTime = g_deltaT;
		g_readState = 1;
	}else{
		if(scanCounter >= targetTime){
			if(scanCounter >= g_scanTime){
				scanCounter = 0;
				targetTime = g_deltaT;
			}else{
				targetTime += g_deltaT;
			}
			g_readState = 1;
		}
		scanCounter += 100;//us
	}


    //divide frequency by 10 -->rate 1ms
	sc++;
	if(sc >= 10){
		sc = 0;

		if(g_mainTimer<255)
			g_mainTimer++;// is incremented every 1ms

        //read ADC conversion result
        //check if conversion is finished
        if((ADCSRA&_BV(ADSC)) == 0){
            switch(ADCcounter){
                case 0:
                    //channel 0: 12V*0,32
                    v0 = ADCL;
                    v1 = ADCH;
                    ADCValue_C0 = (v1<<8)|v0;
                    break;
                case 2:
                    //channnel 1:
                    v0 = ADCL;
                    v1 = ADCH;
                    ADCValue_C1 = (v1<<8)|v0;
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


            //read in DIP switches
            v0 = ~((PIND&0xF0) | ((PING&0x01)<<3) | ((PING&0x02)<<1) | ((PINC&0x01)<<1) | ((PINC&0x02)>>1));
            //debounce
            if(v0 == swBefore){
                g_DIPSwitch = v0;
            }
            swBefore = v0;

            //toggle LEDs
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
            }else{
                if(sc3 == 0){
                    //toggle green LED
                    PORT_LED ^= _BV(LED_G);
                    if((PORT_LED&_BV(LED_G)) == 0){
                        sc3=180; //make off period shorter
                    }
                }
            sc3++;
            }
        }
	}
}


void sendByte(uint8_t b){

    UCSR0B &= ~_BV(UDRIE0); // disable USART interrupt

    if(g_RTB_Filled >= TBUFFER_SIZE){
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

ISR( USART0_UDRE_vect ) {

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
        //check if still more bytes then package size are left
        if(g_RTB_Filled >= TX_PACKET_SIZE){
            g_PacketCounter = TX_PACKET_SIZE;
            UDR0 = g_rowTransmissionBuffer[g_RTB_Read_Pointer];
        }else{
            UCSR0B &= ~ _BV(UDRIE0); // disable USART interrupt
            PORT_CTS |= _BV(CTS);
        }
	}
}


//=== main code ==============
void SPIdummyRead(void){
    //make sure that no interrupt driven SPI-communication is going on right now
    if(g_SPICounter != 0)
        return;

    //send byte
    SPDR = 0;
    //wait for transmission complete
    while(!(SPSR & _BV(SPIF))); //add time-out functionality here later
}


void triggerRowReadout(void){
	//start first transmission (and reception at the same time)
	g_SPICounter = 1;
	SPCR |= _BV(SPIE); //enable interrupt
	SPDR = 0;
	//rest of process is performed by interrupt calls
}


// receive one byte via SPI (reception complete call)
ISR( SPI_STC_vect ) {
uint8_t rec;

	//read byte
	rec = SPDR;

	//start reading next byte
	if(g_SPICounter < g_matrixWidth){
		SPDR = 0;
	}

	//store received byte
	g_SPICounter--;
	g_rowBuffer[g_SPICounter] = rec;
	g_SPICounter++;

	g_SPICounter++;
	if(g_SPICounter > g_matrixWidth){
		//all bytes received
		SPCR &= ~_BV(SPIE); //disable interrupt
		g_SPICounter = 0;
		g_rowReceptionComplete = 1;
	}
}


void performReceiverCalibration(void){
uint8_t i, ix;

	//select last line
	activateFirstLine();
	for(i=1; i<g_matrixHeigth; i++)
		activateNextLine();


	//issue continuous clock pulses for 40ms to request calibration
	for(i=0,g_mainTimer=0; i<40; ){ //40ms
		SPIdummyRead();
		if(g_mainTimer > 0){ // incremented every 1ms
			g_mainTimer = 0;
			i++;
		}
	}


	//stop clock pulses for more than 1ms
	for(i=0,g_mainTimer=0; i<2; ){ //2ms
		if(g_mainTimer > 0){ // incremented every 1ms
			g_mainTimer = 0;
			i++;
		}
	}


	//scan once through all lines
	g_readState = 0;
	for(i=0; i<(g_matrixHeigth+1); ){ // +1: include additional average scan
		if(g_readState == 1){
			g_readState++;
            activateTransmitter(i);

			// read line
			for(ix=0; ix<g_matrixWidth; ix++)
				SPIdummyRead();

			i++;
		}
	}


	//stop clock pulses for 80ms to terminate calibration
	for(i=0,g_mainTimer=0; i<80; ){ //80ms
		if(g_mainTimer > 0){ // incremented every 1ms
			g_mainTimer = 0;
			i++;
		}
	}


	g_SystemCalibrated = 1;

    g_mode = REL_MODE;

    return;
}


int main(void) {
uint8_t  ix, next_row;
uint8_t  v1, i, F1;
uint16_t autoCalTimer;
uint16_t checksum;
uint8_t  tVoltage;
uint8_t  maxLevelRequest=0, maxLevel;
#ifdef TEST_MODE
uint8_t  testBuffer[10];
#endif


    init();


#ifdef TEST_MODE
   	fprintf(stdout, "\nHello!\n");
#endif


    //turn on 12V generator
    DDR_HVSHDN |= _BV(HVSHDN);
    PORT_HVSHDN &= ~_BV(HVSHDN);
    //set DAC value
    //e = setDACValue(106); //5V*21,2*1(V)
    v1 = EEPROM_read(0x00);
    if(v1 == 0xFF){
        v1 = 106;
    }
    setDACValue(v1);
  	if((g_errorState&ERROR_IIC) == 0){
#ifdef TEST_MODE
        fprintf(stdout, "Scan voltage set.\n");
#endif
    }else{
        //ADD: error handling: high voltage couldn't be turned on
        //  [...]
    }


	//initialize scanning
	autoCalTimer = 2*1000; //2sec; automated calibration 2 seconds after power up
	g_mainTimer = 0;

	g_currentRow = 0; //start scan with row 0
	g_readState = 0;

    configSpecialScanMode(g_matrixHeigth);

    F1 = 50/g_matrixHeigth;//exact weight might vary to g_matrixHeigth (for now it's g_matrixHeigth/50)

    PORT_CLK_EN |= _BV(CLK_EN); //CLK_EN ->high to enable outputs

    //main scan loop
	while(1){
		if(g_readState == 1){
			g_readState++;

			//switch to next row
			next_row = g_currentRow + 1;
			if(next_row >= (g_matrixHeigth+1)) // +1: ther is one extra row sample that determines average values
				next_row = 0;
			activateTransmitter(next_row);

//DDR_AUX0 |= _BV(AUX0); //AUX0
//PORT_AUX0 |= _BV(AUX0); //AUX0
			// start reading current row
			triggerRowReadout();
		}


		//wait until new row is available (duration: up to 300us depending on number of bytes to be read)
		if(g_rowReceptionComplete){
			g_rowReceptionComplete = 0;

            if(maxLevelRequest == 2){
                if(g_currentRow < g_matrixHeigth){
    	    	    for(ix=0; ix<g_matrixWidth; ix++){
                        v1 = g_rowBuffer[ix];
                        if(v1 > maxLevel){
                            maxLevel = v1;
                        }
                    }
                }else{
                    maxLevelRequest = 0;
                }
            }

#ifdef COL_CORRECTION
            if(g_SystemCalibrated == 1){
 			    //correct values to minimize crosstalk between lines dueto weak receiver line
                if(g_currentRow < g_matrixHeigth){
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
	  	        		g_colAverage[ix] = v1/F1;
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
    	    	}
        		if(g_currentRow < 10){
                    //copy values
                    if(1){
                        if(g_currentRow<g_matrixHeigth)
                            testBuffer[g_currentRow] = g_rowBuffer[0];
                        else
                            testBuffer[g_currentRow] = 0;
                    }
                    else if(g_currentRow == 0){
                        for(i=0; i<10; i++){
                            if(i<g_matrixWidth)
                                testBuffer[i] = g_rowBuffer[i];
                            else
                                testBuffer[i] = 0;
                        }
                    }
    		  		v1 = testBuffer[g_currentRow];
	        		fprintf(stdout, "%3.u ", v1);
    	    	}
#endif
#ifndef TEST_MODE
    		   	//line is ready to send to host
                if(g_currentRow < g_matrixHeigth){//don't send average sample
                    //copy bytes to TX buffer
                    sendByte(255);
                    sendByte(g_currentRow+1);
                    for(ix=0; ix<g_matrixWidth; ix++){
                        //limit byte value to range 0-254
                        v1 = g_rowBuffer[ix];
                        if(v1 > 254){
                            v1 = 254;
                        }
                        sendByte(v1);
                        checksum += v1;
                    }
                }else{//send info bytes
                    g_FrameNumber++;//increment frame number
#ifdef SEND_STATUS_BYTES
                    //send header bytes
                    sendByte(255);
                    sendByte(0);
                    //send version
                    sendByte('V');
                    sendInt(VERSION);
                    sendInt(SUBVERSION);
                    //send status
                    sendByte('S');
                    sendInt(0);
                    sendInt(g_status);
                    //send ID
                    sendByte('I');
                    sendInt(0);
                    sendInt(g_ID);
                    //send mode
                    sendByte('M');
                    sendInt(0);
                    sendInt(g_mode);
                    //send scan frequency
                    sendByte('F');
                    sendInt(0);
                    sendInt(SCAN_FREQUENCY);
                    //send width
                    sendByte('W');
                    sendInt(0);
                    sendInt(g_matrixWidth);
                    //send heigth
                    sendByte('H');
                    sendInt(0);
                    sendInt(g_matrixHeigth);
                    //send frame number
                    sendByte('N');
                    sendInt(g_FrameNumber>>8);
                    sendInt(g_FrameNumber&255);
                    //send check sum
                    sendByte('C');
                    sendInt(checksum>>8);
                    sendInt(checksum&255);
                    checksum = 0;
#endif
                }
#endif
            }
            //switch to next line
        	g_currentRow++;
        	if(g_currentRow >= (g_matrixHeigth+1)){
    	    	g_currentRow = 0;

                if(maxLevelRequest == 1){
                    maxLevelRequest = 2;
                    maxLevel = 0;
                }
            }

//PORT_AUX0 &= ~_BV(AUX0); //AUX0
	    }


        handleConfigRequests();


        if(g_TaraRequest == 1){
            performReceiverCalibration();
            //reset read cycle
            g_currentRow = g_matrixHeigth; //by this next row will be row 0
            g_readState = 0;
            checksum = 0;
            g_TaraRequest = 0;
        }

        if(g_TaraRequest == 2){
            //find optimal transmitter level
            //first turn receivers to abs mode
        	//issue continuous clock pulses for 10ms to switch to absolute mode
            for(i=0,g_mainTimer=0; i<10; ){ //10ms
                SPIdummyRead();
                if(g_mainTimer > 0){ // incremented every 1ms
                    g_mainTimer = 0;
                    i++;
                }
            g_mode = ABS_MODE;
            }

            //then turn transmit level to maximum
            tVoltage = 255;
            setDACValue(tVoltage);
            //get maximum levels
            maxLevelRequest = 1;
            g_TaraRequest = 3; //switch to next state
        }else if(g_TaraRequest == 3){
            if(maxLevelRequest == 0){
                //check if level is below 250
                if(maxLevel < 250){
                    if(maxLevel > 200){
                        //-->voltage found
                        //store value in EEPROM
                        EEPROM_write(0x0000, tVoltage);
                        g_errorState &= ~(ERROR_T_LEVEL2 | ERROR_T_LEVEL1);
                        g_TaraRequest = 4; //switch to next state
                    }else{ //if maxLevel is close to 0: search failed!
                        g_errorState |= ERROR_T_LEVEL1;
                        g_TaraRequest = 4; //switch to next state
                    }
                }else{
                    //reduce voltage and try again
                    if(tVoltage > 70){ //minium level is 65 (3V)
                        tVoltage -= 5;
                        setDACValue(tVoltage);
                        //get maximum levels
                        maxLevelRequest = 1;
                    }else{
                        //failed!
                        g_errorState |= ERROR_T_LEVEL2;
                        g_TaraRequest = 4; //switch to next state
                    }
                }
            }
        }else if(g_TaraRequest == 4){
            //reset read cycle
            g_currentRow = g_matrixHeigth; //by this next row will be row 0
            g_readState = 0;
            checksum = 0;
            g_TaraRequest = 0;
        }

//        //trigger automatic calibration a few seconds after start-up
//        while(g_mainTimer > 0){
//            g_mainTimer--;
//            if(autoCalTimer > 0){
//                autoCalTimer--;
//                if(autoCalTimer == 1){
//                    g_TaraRequest = 1;
//                    autoCalTimer = 0;
//                }
//            }
//        }
	}

    return 0;
}


/*************************************************************************
*****    serial command reception processing                           ***
*************************************************************************/
ISR( USART0_RX_vect ) {
uint8_t i, j, c;

#define CR 13
#define LF 10

#define recBufferSize 10
static uint8_t recBuffer[recBufferSize], recBufferFilled=0;

//das muss noch in den Programm-Speicher!!
#define C01  1
#define C02  2
#define C03  3
#define C98  4
#define C99  5
#define NrOfCMDs 5
#define CMDLength 3 //for now, all commands have to be 3 characters long!

uint8_t commandList[NrOfCMDs][CMDLength] = {       \
                             {"C01"},      \
                             {"C02"},      \
                             {"C03"},      \
                             {"C98"},      \
                             {"C99"}};

    //check error bits first
    c = UCSR0A;
    if((c&_BV(FE0)) != 0){
        //framing error occured; ignore byte
        //clear receive buffer
        recBufferFilled=0;
        //dummy read to clear RXC0
        c = UDR0;
        return;
    }

    //read byte
    c = UDR0;

    //ignore LF
    if(c == LF){
        return;
    }

    if(c != CR){
        //convert all to capital characters
        if(c>'a' && c<'z'){
            c -= 'a'-'A';
        }
        //add to receive buffer if in valid range, and there is no buffer overrun
        if(c>0x20 && c<=90 && recBufferFilled<recBufferSize){
            recBuffer[recBufferFilled] = c;
            recBufferFilled++;
        }else{
            //clear receive buffer
            recBufferFilled=0;
        }
        return;
    }

    //CR received; parse received bytes
    if(g_ConfigMode == 0){
        //wait for start command ('x')
        if(recBufferFilled >= 1){
            //check if previous byte was an 'x'
            if(recBuffer[recBufferFilled-1] == 'X'){
                //change to config mode
                g_ConfigMode = 1;
            }
        }
    }else{
        if(recBufferFilled == CMDLength){
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
                if(g_NextCommand == 0){//there is nothing pending; ADD err msg. here later
                    g_NextCommand = j+1;
                }
            }
        }
    }

    //clear receive buffer
    recBufferFilled=0;
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
                UCSR0B &= ~ _BV(UDRIE0); // disable USART interrupt
                g_RTB_Filled = g_RTB_Write_Pointer = g_RTB_Read_Pointer = g_PacketCounter = 0;

                pointer1 = &configWelcomeMsg;
            }
            e = pgm_read_byte(pointer1++);//get next byte from FLASH;
            if(e == '@'){
                fprintf(stdout, "OK\n");
                g_ConfigMode = 2;
                pointer1 = 0;
            }else{
                uart0_putchar(e, stdout);
                //g_H++;
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
                    fprintf(stdout, "Err%02X\n", g_errorState);
                    g_NextCommand = 0;
                    break;
                case C98: //reply with "OK"
                    fprintf(stdout, "OK\n");
                    g_NextCommand = 0;
                    break;
                case C99: //exit config mode
                    fprintf(stdout, "OK\n");
                    g_NextCommand = 0;
                    g_ConfigMode = 0;// exit config mode
                    break;
                default:
                    ;
             }
        }else if(g_ConfigMode == 5){//wait for calibration to be done
            if(g_TaraRequest == 0){
                if((g_errorState&(ERROR_T_LEVEL2|ERROR_T_LEVEL1)) == 0){
                    fprintf(stdout, "OK\n");
                }else{
                    fprintf(stdout, "Err%02X\n", g_errorState);
                }
                g_NextCommand = 0;
                g_ConfigMode = 2;// return to config mode
            }
        }
    }
}
