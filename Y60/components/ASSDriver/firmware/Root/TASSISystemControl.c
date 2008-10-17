
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "defines.h"
#include "global.h"
#include "uart64.h"

#ifdef TASSI

uint8_t g_TSC_TCTR, g_TSC_TB;
#define TSC_lengthCommandBuffer 32
uint8_t g_TSC_commandBuffer[TSC_lengthCommandBuffer];
uint8_t g_TSC_commandBufferCTR;
uint8_t g_TSC_State=0;

const char g_projectorCommand1[] PROGMEM = "REMOTE\n@"; //delimiter is @
const char g_projectorCommand2[] PROGMEM = "POWER ON\n@";
const char g_projectorCommand3[] PROGMEM = "POWER OFF\n@";
const char g_projectorCommand4[] PROGMEM = "LOCAL\n@";

void TSC_init(void){
    //set IO-Ports
    //AUX0: used for input main switch
    DDR_AUX0 &= ~_BV(AUX0);
    PORT_AUX0 |= _BV(AUX0); //turn on pull-up resistor

    //AUX1: used for main LED
    DDR_AUX1 |= _BV(AUX1);
    PORT_AUX1 &= ~_BV(AUX1);

    //AUX2: used drives relais 1 (projector, amplifier, fans)
    DDR_AUX2 |= _BV(AUX2);
    PORT_AUX2 &= ~_BV(AUX2);

    //AUX3: used drives relais 2 (PC)
    DDR_AUX3 |= _BV(AUX3);
    PORT_AUX3 &= ~_BV(AUX3);

    //AUX4: used to power IO-Board
    DDR_AUX4 |= _BV(AUX4);
    PORT_AUX4 |= _BV(AUX4);


    g_TSC_State=0;


    //use sync port for projector UART
    PORT_SNC_OUT |= _BV(SNC_OUT);
    DDR_SNC_OUT |= _BV(SNC_OUT);
    DDR_SNC_IN &= _BV(SNC_IN);
    g_TSC_commandBufferCTR = 0;

    //prepare timer 2 for software UART
    TCCR2 = _BV(WGM21) | 2; //CTC, prescaler = 8;
    OCR2 = ((F_CPU/8)/19200)-1; //19200bps
}


void TSC_update(void){
static uint8_t  buttonStateBefore = 0, buttonPressed=0, buttonReleased=0;
static uint8_t  subState=0;
static uint32_t myDelayTimer=0;

    //system control state machine
    switch(g_TSC_State){
        case 0:
            g_TSC_State++;
            break;

        case 1://all off
            PORT_AUX1 &= ~_BV(AUX1);
            PORT_AUX2 &= ~_BV(AUX2);
            PORT_AUX3 &= ~_BV(AUX3);
            PORT_AUX4 &= ~_BV(AUX4);
            //wait for button press
            if(buttonPressed == 1){
                buttonPressed = 0;
                subState=0;
                g_TSC_State++;
            }
            break;

        case 2://power system up
            if(subState == 0){
                PORT_AUX1 |= _BV(AUX1); //LED
                PORT_AUX2 |= _BV(AUX2); //relais 1 (projector, amplifier, fans)
                PORT_AUX3 |= _BV(AUX3); //relais 2 (PC)
                PORT_AUX4 |= _BV(AUX4); //power IO board
                uart1_putchar(0x0D, &g_uart1_str); //CR
                uart1_putchar(0x0A, &g_uart1_str); //LF
                myDelayTimer = 1000;//ms
                subState++;
            }else if(subState == 1){
                if(buttonPressed == 1){
                    buttonPressed = 0;
                    g_TSC_State = 1;//go to off state when button pressed within one second
                }
                myDelayTimer--;
                if(myDelayTimer == 0){
                    subState++;
                }
            }else if(subState == 2){
                //turn projector on
                TSC_transferCommand((uint16_t)&g_projectorCommand1);//switch to remote mode
                TSC_transferCommand((uint16_t)&g_projectorCommand2);//power on command
//                TSC_transferCommand((uint16_t)&g_projectorCommand4);//switch to local mode
                //turn PC on
                //ADD WAKE BY LAN...
                //turn sensor on
                g_power = 1;
                myDelayTimer = 55000;//ms
                subState++;
            }else if(subState == 3){
                //repeat on-commands two times to make sure projector is powered up
                myDelayTimer--;
                if(myDelayTimer == (55000-1000)){
                    TSC_transferCommand((uint16_t)&g_projectorCommand1);//switch to remote mode
                    TSC_transferCommand((uint16_t)&g_projectorCommand2);//power on command
//                    TSC_transferCommand((uint16_t)&g_projectorCommand4);//switch to local mode
                }
                if(myDelayTimer == (55000-2000)){
                    TSC_transferCommand((uint16_t)&g_projectorCommand1);//switch to remote mode
                    TSC_transferCommand((uint16_t)&g_projectorCommand2);//power on command
                    TSC_transferCommand((uint16_t)&g_projectorCommand4);//switch to local mode
                      TSC_transferCommand((uint16_t)&g_projectorCommand4);//switch to local mode
                }
                if(myDelayTimer == 0){
                    buttonPressed = 0;
                    g_TSC_State++;
                }
            }
            break;

        case 3://all system on
            PORT_AUX1 |= _BV(AUX1); //LED
            DDR_AUX1 |= _BV(AUX1); //for some reason DDR_AUX1 is reset -> hint for a bug: explore!
            if(buttonPressed == 1){
                buttonPressed = 0;
                myDelayTimer = 1000;//ms
                g_TSC_State++;
            }
            break;

        case 4://check if button is held for more than 1 sec
            if(buttonPressed == 1){
                buttonPressed = 0;
                myDelayTimer = 1000;//ms
     //           g_TSC_State = 10;//perform PC reset -->still to be implemented!
            } 
            myDelayTimer--;
            if(myDelayTimer == 0){
                if(buttonReleased == 1){
                    g_TSC_State = 3;//go to state 'all on'
                }else{
                    subState=0;
                    g_TSC_State++;
                }
            }

        case 5://power down sequence
            if(subState == 0){
                //turn projector off
                TSC_transferCommand((uint16_t)&g_projectorCommand1);//switch to remote mode
                TSC_transferCommand((uint16_t)&g_projectorCommand3);//power off command
//                TSC_transferCommand((uint16_t)&g_projectorCommand4);//switch to local mode
                //turn PC off
                //send power down message
                uart1_putchar(0x0D, &g_uart1_str); //CR
                uart1_putchar(0x0A, &g_uart1_str); //LF
                uart1_putchar('S', &g_uart1_str);
                uart1_putchar('H', &g_uart1_str);
                uart1_putchar('U', &g_uart1_str);
                uart1_putchar('T', &g_uart1_str);
                uart1_putchar(' ', &g_uart1_str);
                uart1_putchar('D', &g_uart1_str);
                uart1_putchar('O', &g_uart1_str);
                uart1_putchar('W', &g_uart1_str);
                uart1_putchar('N', &g_uart1_str);
                uart1_putchar(0x0D, &g_uart1_str); //CR
                uart1_putchar(0x0A, &g_uart1_str); //LF
                myDelayTimer = 15000;//ms
                subState++;
            }else if(subState == 1){
                //repeat off-commands two times to make sure devices power down
                myDelayTimer--;
                if(myDelayTimer == (15000-1000)){
                    TSC_transferCommand((uint16_t)&g_projectorCommand1);//switch to remote mode
                    TSC_transferCommand((uint16_t)&g_projectorCommand3);//power off command
//                    TSC_transferCommand((uint16_t)&g_projectorCommand4);//switch to local mode
                    //turn PC off
                    //send power down message
                    uart1_putchar(0x0D, &g_uart1_str); //CR
                    uart1_putchar(0x0A, &g_uart1_str); //LF
                    uart1_putchar('S', &g_uart1_str);
                    uart1_putchar('H', &g_uart1_str);
                    uart1_putchar('U', &g_uart1_str);
                    uart1_putchar('T', &g_uart1_str);
                    uart1_putchar(' ', &g_uart1_str);
                    uart1_putchar('D', &g_uart1_str);
                    uart1_putchar('O', &g_uart1_str);
                    uart1_putchar('W', &g_uart1_str);
                    uart1_putchar('N', &g_uart1_str);
                    uart1_putchar(0x0D, &g_uart1_str); //CR
                    uart1_putchar(0x0A, &g_uart1_str); //LF
                }
                if(myDelayTimer == (15000-2000)){
                    TSC_transferCommand((uint16_t)&g_projectorCommand1);//switch to remote mode
                    TSC_transferCommand((uint16_t)&g_projectorCommand3);//power off command
                    TSC_transferCommand((uint16_t)&g_projectorCommand4);//switch to local mode
                      TSC_transferCommand((uint16_t)&g_projectorCommand4);//switch to local mode
                    //turn PC off
                    //send power down message
                    uart1_putchar(0x0D, &g_uart1_str); //CR
                    uart1_putchar(0x0A, &g_uart1_str); //LF
                    uart1_putchar('S', &g_uart1_str);
                    uart1_putchar('H', &g_uart1_str);
                    uart1_putchar('U', &g_uart1_str);
                    uart1_putchar('T', &g_uart1_str);
                    uart1_putchar(' ', &g_uart1_str);
                    uart1_putchar('D', &g_uart1_str);
                    uart1_putchar('O', &g_uart1_str);
                    uart1_putchar('W', &g_uart1_str);
                    uart1_putchar('N', &g_uart1_str);
                    uart1_putchar(0x0D, &g_uart1_str); //CR
                    uart1_putchar(0x0A, &g_uart1_str); //LF
                }
                if(myDelayTimer == 0){
                    //turn off relais 2
                    PORT_AUX3 &= ~_BV(AUX3); //relais 2 (PC)
                    myDelayTimer = 90000;//ms
                    subState++;
                }
                //blink LED
                if((myDelayTimer&256) != 0){
                    PORT_AUX1 |= _BV(AUX1); //LED
                }else{
                    PORT_AUX1 &= ~_BV(AUX1); //LED
                }
            }else if(subState == 2){
                myDelayTimer--;
                if(myDelayTimer == 0){
                    //turn off relais 1
                    PORT_AUX2 &= ~_BV(AUX2); //relais 1 (projector, amplifier, fans)
                    g_power = 0;
                    buttonPressed = 0;
                    g_TSC_State = 1;//go to off state
                }
                //blink LED
                if((myDelayTimer&256) != 0){
                    PORT_AUX1 |= _BV(AUX1); //LED
                }else{
                    PORT_AUX1 &= ~_BV(AUX1); //LED
                }
            }
            break;

        case 10://perform PC reset
            //ADD ...
            buttonPressed = 0;
            g_TSC_State = 3;//go to state 'all on'
            break;

        default:
            g_TSC_State = 0;
    }


    //handle main power switch
    if((PIN_AUX0&_BV(AUX0)) == 0){
        //pressed!
        if(buttonStateBefore == 0){
            buttonStateBefore = 1;
            buttonPressed = 1;
            buttonReleased = 0;
        }
    }else{
        buttonStateBefore = 0;
        buttonReleased = 1;
        buttonPressed = 0;
    }


    return;
}


void TSC_transferCommand(uint16_t pointer){
uint8_t  e;

    do{
        e = pgm_read_byte(pointer++);//get next byte from FLASH;
        if(g_TSC_commandBufferCTR < TSC_lengthCommandBuffer){
            TIMSK &= ~_BV(OCIE2); //disable compare match interrupt
            g_TSC_commandBuffer[g_TSC_commandBufferCTR] = e;
            g_TSC_commandBufferCTR++;
            TIMSK |= _BV(OCIE2); //enable compare match interrupt
        }else{
            //buffer full!!
            break;
        }
    }while(e != '@');

    return;
}


ISR( TIMER2_COMP_vect ) {
//Data format: 19200bps, 8bits, 2stop bits, no parity
uint8_t i;
static uint8_t nextState=1, delay=0, subDelay=0;

    //do pin change right after timer call
DDR_SNC_OUT |= _BV(SNC_OUT); //debug: where is DDR reset???
    if(nextState == 0){
        PORT_SNC_OUT &= ~_BV(SNC_OUT); //low
    }else{
        PORT_SNC_OUT |= _BV(SNC_OUT);  //high
    }

    if(delay > 0){
        if(subDelay < 192){
            subDelay++;
        }else{
            subDelay = 0;
            delay--;
        }
    }else{

        if(g_TSC_TCTR == 0){
            if(g_TSC_commandBufferCTR > 0){
                //transmit next byte
                g_TSC_TB = g_TSC_commandBuffer[0];
                //shift buffer
                i = 0;
                do{
                    g_TSC_commandBuffer[i] = g_TSC_commandBuffer[i+1];
                    i++;
                }while(i < g_TSC_commandBufferCTR);
                g_TSC_commandBufferCTR--;
                if(g_TSC_TB != '@'){
                    g_TSC_TCTR++;
                }else{
                    delay = 20; //initiate small pause (0,2 sec)
                }
            }else{
                TIMSK &= ~_BV(OCIE2); //terminate transmission/disable compare match interrupt
            }

        }else{
            if(g_TSC_TCTR == 1){
                //issue start bit
                nextState = 0; //low
                g_TSC_TCTR++;
            }else if(g_TSC_TCTR < 10){
                //issue data bits
                if((g_TSC_TB&1) != 0){
                    nextState = 1; //high
                }else{
                    nextState = 0; //low
                }
                g_TSC_TB >>= 1;
                g_TSC_TCTR++;
            }else if(g_TSC_TCTR < 12){
                //two stop bits
                nextState = 1; //high
                g_TSC_TCTR++;
            }else{
                g_TSC_TCTR = 0;
            }
        }
    } 
}

#endif /* TASSI */
