//------------------------------------------------------------------------------
//  TITLE :- CRC Calculation header file - CRC.h
//  AUTHOR :- Martyn Brown
//  DATE :- 25/04/14
//
//  DESCRIPTION :- CRC routines header file.
//
//  UPDATES :-
//  25/04/14 Created
//
//  LICENSE :-
//  The CANdo API library source code is proprietary software & must not be
//  copied, modified or re-distributed without prior permission from the
//  author. The source code is provided to allow compilation of the CANdo API
//  library on any compatible platform. The resulting binary library file
//  'libCANdo.so' maybe freely copied & distributed without any restriction or
//  time limit, for personal, educational or commercial use. The software is
//  supplied "as is", with no implied warranties or guarantees.
//
//  (c) 2014 Netronics Ltd. All rights reserved.
//------------------------------------------------------------------------------
#ifndef CRC_H
#define CRC_H
//------------------------------------------------------------------------------
// PROTOTYPES
//------------------------------------------------------------------------------
void CalculateCRC(const unsigned char *, unsigned int, unsigned int *);
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#endif
