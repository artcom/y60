
#include <stdint.h>

#include "defines.h"
#include "global.h"

// config values (saved in EEPROM)
uint8_t g_matrixWidth;
uint8_t g_matrixHeight;
uint8_t g_scanFrequency;
uint8_t g_gridSpacing;
uint8_t g_ID;
uint8_t g_tFrequency;
uint8_t g_BaudRateFactor;
//txVoltage is handled as static but also save in EEPROM

// max/min value
uint8_t g_maxMinLevelRequest=0;
uint8_t g_maxLevel;
uint8_t g_minLevel;
uint8_t g_PMinX;
uint8_t g_PMinY;
uint8_t g_PMaxX;
uint8_t g_PMaxY;

// point measurement (coordinates in g_arg1 and g_arg2)
uint8_t g_XYLevelRequest=0;
uint8_t g_XYLevel;

// buffered dip switch state
// updated by Timer1 compare interrupt handler
uint8_t g_DIPSwitch;

// row transmission buffer
uint8_t g_rowTransmissionBuffer[TBUFFER_SIZE+1];
uint8_t g_RTB_Write_Pointer=0;
uint8_t g_RTB_Read_Pointer=0;
uint8_t g_RTB_Filled=0;
uint8_t g_PacketCounter=0;

// "high speed" or "low speed" (see LS and HS)
uint8_t g_UART_mode;
// which UART should be used?
uint8_t g_UART_select;

// number of bytes in (static) UART receive buffer
uint8_t g_UARTBytesReceived;

// buffer for current command
uint8_t g_NextCommand=0;
// parsed command arguments
uint8_t g_arg1;
uint8_t g_arg2;

// duration of full-scan
uint16_t g_scanPeriod;
// duration of row-scan
uint16_t g_deltaTRow;

// global frame counter
uint16_t g_FrameNumber;

// determines flow control behaviour.
// only bits 0 and 2 seem relevant.
uint8_t g_dataTransmitMode;

// row buffer during readout
uint8_t g_rowBuffer[MAX_MATRIX_WIDTH];

// column correction
uint8_t g_colAverage[MAX_MATRIX_WIDTH];
uint8_t g_T1[256];

// REL_MODE or ABS_MODE
uint8_t g_mode;

// global error flags
uint8_t g_errorState;

// "system timer" - fiddled with
// in acrobatic ways all over the place
uint8_t g_mainTimer;

// while scanning contains the
// current row offset.
uint8_t g_currentRow;
// some magic flag used in readout timing
uint8_t g_readState=0;

// request tara operation
uint8_t g_TaraRequest=0;
// set TRUE on tara completion. never reset!?
uint8_t g_SystemCalibrated=0;

// configuration mode state machine
uint8_t g_ConfigMode=0;

// AD measurements of Vtx and Vcc.
// seemingly only used for debug.
uint16_t ADCValue_C0;
uint16_t ADCValue_C1;

// never used. only sent to host. always 0.
uint8_t g_status;

#ifdef TASSI
// tassi power state
uint8_t g_power;
#endif

