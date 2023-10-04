//------------------------------------------------------------------------------
//  TITLE :- CANdoAUTOC header file - CANdoAUTOC.h
//  AUTHOR :- Martyn Brown
//  DATE :- 15/12/14
//
//  DESCRIPTION :- Main routines header file.
//
//  UPDATES :-
//  27/05/14 Created
//  15/12/14 Functions added to support dynamic loading of libCANdo.so
//
//  LICENSE :-
//  The SDK (Software Development Kit) provided for use with the CANdo device
//  is issued as FREE software, meaning that it is free for personal,
//  educational & commercial use, without restriction or time limit. The
//  software is supplied "as is", with no implied warranties or guarantees.
//
//  (c) 2014 Netronics Ltd. All rights reserved.
//------------------------------------------------------------------------------
#ifndef CANDOAUTOC_H
#define CANDOAUTOC_H
//------------------------------------------------------------------------------
// DEFINES
//------------------------------------------------------------------------------
#define VERSION_NO 1.2
#define FALSE 0
#define TRUE 1
#define SLEEP_TIME 100000  // Sleep time in us
// CAN transmit repeat time
#define REPEAT_TIME_OFF 0
#define REPEAT_TIME_10MS 1
#define REPEAT_TIME_20MS 2
#define REPEAT_TIME_50MS 3
#define REPEAT_TIME_100MS 4
#define REPEAT_TIME_200MS 5
#define REPEAT_TIME_500MS 6
#define REPEAT_TIME_1000MS 7
#define REPEAT_TIME_2000MS 8
#define REPEAT_TIME_5000MS 9
#define REPEAT_TIME_10000MS 10
//------------------------------------------------------------------------------
// PROTOTYPES
//------------------------------------------------------------------------------
unsigned char CANdoInitialise(void);
void CANdoFinalise(void);
int CANdoMapFunctionPointers(void);
void CANdoUnmapFunctionPointers(void);
void CANdoConnect(void);
void CANdoGetStatus(unsigned int);
void CANdoVersion(void);
double CANdoIEEE754ToFloat(int);
void CANdoWriteAnalogInputStore(void);
void CANdoReadAnalogInputStore(void);
void CANdoWriteCANTransmitStore(void);
void CANdoReadCANTransmitStore(void);
void CANdoMenu(void);
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#endif
