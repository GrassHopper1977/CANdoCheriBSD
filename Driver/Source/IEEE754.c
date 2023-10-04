//------------------------------------------------------------------------------
//  TITLE :- IEEE754 floating point number routines - IEEE754.c
//  AUTHOR :- Martyn Brown
//  DATE :- 19/05/14
//
//  DESCRIPTION :- Routines to deal with encoding/decoding of floating point
//             		 numbers in a binary format.
//
//  UPDATES :-
//  19/05/14 IEEE754.pas v4.0 converted to 'C' for Linux library
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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "SiUSBXp.h"
//--------------------------------------------------------------------------
// FloatToIEEE754
//
// Encode a floating point number according to the truncated 24 bit IEEE754
// representation compatible with the Hi-Tech PICC18 compiler.
//
// Returns
//    IEEE754 representation of a floating point no. packed into the lower
//    24 bits of an int.
//--------------------------------------------------------------------------
int FloatToIEEE754(double Number)
{
  int Result, SignBit, Exponent;
  double Mantissa;

  // Sign bit
  if (Number < 0.0)
  {
    SignBit = 1;  // -ve
    Number = Number * (-1.0);
  }
  else
    SignBit = 0;  // +ve
  if (Number != 0.0)
  {
    // Exponent
    Exponent = trunc(log2(Number));
    if (Exponent > log2(Number))
      Exponent = Exponent - 1;  // Make sure that 2^Exponent is less than Number
    // Mantissa
    Mantissa = (Number / exp2(Exponent) - 1) * 32768.0;
    // Binary representation of floating point number
    Result = SignBit * 8388608 + (Exponent + 127) * 32768 + round(Mantissa);
  }
  else
    Result = 0;

  return Result;
}
//--------------------------------------------------------------------------
// IEEE754ToFloat
//
// Decode a floating point number encoded according to the truncated 24 bit
// IEEE754 representation compatible with the Hi-Tech PICC18 compiler.
//
// Returns
//    Float value decoded from IEEE754 binary representation
//--------------------------------------------------------------------------
double IEEE754ToFloat(int Number)
{
  double Result, Exponent, Mantissa;

  if (Number != 0)
  {
    Exponent = exp2(((Number >> 15) & 0xFF) - 127);
    // Sign bit
    if ((Number & 0x800000) > 0)
      Exponent = Exponent * (-1.0);  // -ve value
    Mantissa = ((Number & 0x7FFF) / 32768) + 1.0;
    Result = Exponent * Mantissa;
    if (isnan(Result))
      Result = NAN;  // Not a number
    else
    if (!isfinite(Result))
      Result = INFINITY;  // Infinity
  }
  else
    Result = 0.0;

  return Result;
}
//--------------------------------------------------------------------------
// CheckFloatingPointNumber
//
// Check that a string representing a floating point no. only contains the
// characters [0..9, '.', '-', 'e', 'E'] & is a valid float value.
//
// Returns
//    False = Invalid char. present
//    True = Valid floating point no.
//--------------------------------------------------------------------------
unsigned char CheckFloatingPointNumber(const char * Number)
{
  unsigned char ValidNumberFlag;
  double Value;

  if (strcmp(Number, "") != 0)
  {
    // Check string is a float
  	Value = strtod(Number, NULL);
  	if (Value == 0.0)
  		ValidNumberFlag = FALSE;  // Invalid
  	else
  		ValidNumberFlag = TRUE;  // Valid
  }
  else
    ValidNumberFlag = FALSE;  // Invalid

  return ValidNumberFlag;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
