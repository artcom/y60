
#ifndef _ASS_ROOT_GLOBAL_H_
#define _ASS_ROOT_GLOBAL_H_

extern uint8_t g_matrixWidth;
extern uint8_t g_matrixHeight;
extern uint8_t g_scanFrequency;
extern uint8_t g_gridSpacing;
extern uint8_t g_ID;
extern uint8_t g_tFrequency;
extern uint8_t g_BaudRateFactor;
// txVoltage

extern uint8_t g_maxMinLevelRequest;
extern uint8_t g_maxLevel;
extern uint8_t g_minLevel;
extern uint8_t g_PMinX;
extern uint8_t g_PMinY;
extern uint8_t g_PMaxX;
extern uint8_t g_PMaxY;

extern uint8_t  g_XYLevelRequest;
extern uint8_t  g_XYLevel;

extern uint8_t g_DIPSwitch;

extern uint8_t g_rowTransmissionBuffer[];
extern uint8_t g_RTB_Write_Pointer;
extern uint8_t g_RTB_Read_Pointer;
extern uint8_t g_RTB_Filled;
extern uint8_t g_PacketCounter;

extern uint8_t g_UART_mode;
extern uint8_t g_UART_select;
extern uint8_t g_UARTBytesReceived;

extern uint8_t g_NextCommand;
extern uint8_t g_arg1;
extern uint8_t g_arg2;

extern uint16_t g_scanPeriod;
extern uint16_t g_deltaTRow;

extern uint16_t g_FrameNumber;

extern uint8_t g_dataTransmitMode;

extern uint8_t g_rowBuffer[];

extern uint8_t g_colAverage[];
extern uint8_t g_T1[];

extern uint8_t g_mode;

extern uint8_t g_errorState;

extern uint8_t g_mainTimer;

extern uint8_t g_currentRow;
extern uint8_t g_readState;

extern uint8_t g_TaraRequest;
extern uint8_t g_SystemCalibrated;

extern uint8_t g_ConfigMode;

extern uint16_t ADCValue_C0;
extern uint16_t ADCValue_C1;

extern uint8_t g_status;

#ifdef TASSI
extern uint8_t g_power;
#endif

#endif
