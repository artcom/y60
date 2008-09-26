
#include <stdint.h>

#include "defines.h"
#include "global.h"

uint8_t g_matrixWidth;
uint8_t g_matrixHeight;
uint8_t g_scanFrequency;
uint8_t g_gridSpacing;
uint8_t g_ID;
uint8_t g_tFrequency;
uint8_t g_mode;
uint8_t g_status;
uint8_t g_errorState;
uint8_t g_mainTimer;
uint8_t g_DIPSwitch;
uint8_t g_UARTBytesReceived;
uint8_t g_arg1;
uint8_t g_arg2;
uint8_t g_PMinX;
uint8_t g_PMinY;
uint8_t g_PMaxX;
uint8_t g_PMaxY;
uint8_t g_BaudRateFactor;
uint8_t g_UART_mode;
uint8_t g_UART_select;
uint8_t g_dataTransmitMode;
uint8_t g_T1[256];

uint8_t g_rowBuffer[MAX_MATRIX_WIDTH];
uint8_t g_colAverage[MAX_MATRIX_WIDTH];
uint8_t g_RTB_Write_Pointer=0;
uint8_t g_RTB_Read_Pointer=0;
uint8_t g_RTB_Filled=0;
uint8_t g_PacketCounter=0;

uint8_t g_rowTransmissionBuffer[TBUFFER_SIZE+1];

uint8_t g_currentRow;

uint16_t g_scanPeriod;
uint16_t g_deltaTRow;
uint16_t g_FrameNumber;

uint8_t g_readState=0;

uint8_t g_SystemCalibrated=0;
uint8_t g_TaraRequest=0;

uint8_t g_ConfigMode=0;
uint8_t g_NextCommand=0;

uint8_t g_maxMinLevelRequest=0;
uint8_t g_XYLevelRequest=0;
uint8_t g_maxLevel;
uint8_t g_minLevel;
uint8_t g_XYLevel;

uint16_t ADCValue_C0;
uint16_t ADCValue_C1;
