//------------------------------------------------------------------------------
//  TITLE :- IEEE754 routines header file - IEEE754.h
//  AUTHOR :- Martyn Brown
//  DATE :- 19/05/14
//
//  DESCRIPTION :- IEEE754 routines header file.
//
//  UPDATES :-
//  19/05/14 Created
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
#ifndef IEEE754_H
#define IEEE754_H
//------------------------------------------------------------------------------
// PROTOTYPES
//------------------------------------------------------------------------------
int FloatToIEEE754(double);
double IEEE754ToFloat(int);
unsigned char CheckFloatingPointNumber(const char *);
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#endif
