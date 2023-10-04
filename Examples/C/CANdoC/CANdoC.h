//------------------------------------------------------------------------------
//  TITLE :- CANdoC header file - CANdoC.h
//  AUTHOR :- Martyn Brown
//  DATE :- 15/12/14
//
//  DESCRIPTION :- Main routines header file.
//
//  UPDATES :-
//  08/05/14 Created
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
#ifndef CANDOC_H
#define CANDOC_H
//------------------------------------------------------------------------------
// DEFINES
//------------------------------------------------------------------------------
#define VERSION_NO 1.2
#define FALSE 0
#define TRUE 1
#define SLEEP_TIME 10000  // Sleep time in us
#define RX_DISPLAY_TIME 20  // Receive poll time in multiples of SLEEP_TIME
#define BUS_LOAD_REQUEST_TIME 100  // Bus load request time in multiples of SLEEP_TIME
#define MAX_NO_OF_DEVICES 10  // Max. no. of CANdo devices to enumerate
//------------------------------------------------------------------------------
// PROTOTYPES
//------------------------------------------------------------------------------
unsigned char CANdoInitialise(void);
void CANdoFinalise(void);
int CANdoMapFunctionPointers(void);
void CANdoUnmapFunctionPointers(void);
void CANdoConnect(void);
void CANdoStart(void);
void CANdoStop(void);
void CANdoGetStatus(unsigned char);
void CANdoPID(void);
void CANdoVersion(void);
void CANdoRx(void);
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#endif
