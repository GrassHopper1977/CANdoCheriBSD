//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// TITLE : IEEE754 floating point number routines.
//
// DESCRIPTION : Routines to deal with encoding/decoding of floating point
//               numbers in a binary format.
//
// DATE : 27/05/14
//
// MODIFICATIONS :
//    30/06/13 Created
//    27/05/14 CharInSet function added for FPC
//
// LICENSE :
//  The SDK (Software Development Kit) provided for use with the CANdo
//  device is issued as FREE software, meaning that it is free for personal,
//  educational & commercial use, without restriction or time limit. The
//  software is supplied "as is", with no implied warranties or guarantees.
//
// AUTHOR : Martyn Brown
//
// © 2013-14 Netronics Ltd. All rights reserved.
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
unit IEEE754;

interface

uses
  SysUtils, Math;

  function FloatToIEEE754(Number : Double) : Integer;
  function IEEE754ToFloat(Number : Integer) : Double;
  function CheckFloatingPointNumber(Number : String) : Boolean;

implementation

type
TCharSet = set of Char;

{$IFDEF FPC}
function CharInSet(const Character : Char; const ASet : TCharSet): Boolean;
begin
  Result := Character in ASet;
end;
{$ENDIF}
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
function FloatToIEEE754(Number : Double) : Integer;
var
  SignBit, Exponent : Integer;
  Mantissa : Double;
begin
  // Sign bit
  if Number < 0.0 then
  begin
    SignBit := 1;  // -ve
    Number := Number * (-1.0);
  end
  else
    SignBit := 0;  // +ve
  if Number <> 0 then
  begin
    // Exponent
    Exponent := Trunc(Log2(Number));
    if Exponent > Log2(Number) then
      Exponent := Exponent - 1;  // Make sure that 2^Exponent is less than Number
    // Mantissa
    Mantissa := (Number / IntPower(2, Exponent) - 1) * 32768.0;
    // Binary representation of floating point number
    Result := SignBit * 8388608 + (Exponent + 127) * 32768 + Round(Mantissa);
  end
  else
    Result := 0;
end;
//--------------------------------------------------------------------------
// IEEE754ToFloat
//
// Decode a floating point number encoded according to the truncated 24 bit
// IEEE754 representation compatible with the Hi-Tech PICC18 compiler.
//
// Returns
//    Float value decoded from IEEE754 binary representation
//--------------------------------------------------------------------------
function IEEE754ToFloat(Number : Integer) : Double;
var
  Exponent, Mantissa : Double;
begin
  if Number <> 0 then
  begin
    Exponent := IntPower(2, (((Number shr 15) and $FF) - 127));
    // Sign bit
    if (Number and $800000) > 0 then
      Exponent := Exponent * (-1.0);  // -ve value
    Mantissa := 1.0 + (Number and $7FFF) / 32768.0;
    Result := Exponent * Mantissa;
    if IsNaN(Result) then
      Result := NaN  // Not a number
    else
    if IsInfinite(Result) then
      Result := Infinity;  // Infinity
  end
  else
    Result := 0.0;
end;
//--------------------------------------------------------------------------
// CheckFloatingPointNumber
//
// Check that a string representing a floating point no. only contains the
// characters [0..9, '.', '-', 'E'] & is a valid float value.
//
// Returns
//    False = Invalid char. present
//    True = Valid floating point no.
//--------------------------------------------------------------------------
function CheckFloatingPointNumber(Number : String) : Boolean;
var
  CharNo : Integer;
  ValidNumber : Boolean;
  Value : Double;
begin
  if (Number <> '') then
  begin
    // Check string is a float
    ValidNumber := True;  // Assume valid
    for CharNo := 1 to Length(Number) do
    begin
      if not CharInSet(Number[CharNo], ['0'..'9', '.', '-', 'E']) then
        ValidNumber := False;
    end;
    if ValidNumber then
      ValidNumber := TryStrToFloat(Number, Value);
  end
  else
    ValidNumber := False;  // Invalid
  Result := ValidNumber;
end;
//--------------------------------------------------------------------------
end.
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------