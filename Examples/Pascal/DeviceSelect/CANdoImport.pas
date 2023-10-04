//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// THIS HEADER FILE CONTAINS THE FUNCTION IMPORTS AND CONSTANTS FOR THE
// FUNCTIONS EXPORTED BY THE CANdo API LIBRARY -
// Windows CANdo.dll v4.1
// Linux libCANdo.so v2.0
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// TITLE : CANdo.dll/libCANdo.so import unit - CANdoImport.pas
//
// DESCRIPTION : Delphi/Lazarus import unit for interfacing to the CANdo API.
//
// DATE : 11/12/14
//
// MODIFICATIONS :
//    26/12/05 Created
//    28/07/10 1) Function return values updated
//             2) CANdoGetVersion function added
//    10/01/11 1) CANdoClose prototype return type modified
//             2) CANDO_INVALID_HANDLE return value added
//    24/02/11 1) CANdo H/W type constants added
//             2) CANdoDevice type added
//             3) CANdoGetDevices function added
//             4) CANdoOpenDevice function added
//             5) CANDO_MAX_CAN_BUFFER_LENGTH renamed to CANDO_CAN_BUFFER_LENGTH
//             6) CANDO_CLOSED return value renamed CANDO_CONNECTION_CLOSED
//    26/07/11 1) CANdoRequestDateStatus function added
//             2) CANdoRequestBusLoadStatus function added
//             3) TCANdoStatus.NewFlag type changed from Boolean to Byte
//             4) CANdo status type constants added
//    31/10/11 1) CANdoGetPID function added
//             2) CANdoClearStatus function added
//    27/08/13 1) CANdo AUTO support added
//             2) CANdoRequestSetupStatus function added
//             3) CANdoRequestAnalogInputStatus function added
//             4) CANdoAnalogStoreRead function added
//             5) CANdoAnalogStoreWrite function added
//             6) CANdoAnalogStoreClear function added
//             7) CANdoTransmitStoreRead function added
//             8) CANdoTransmitStoreWrite function added
//             9) CANdoTransmitStoreClear function added
//    11/12/14 Windows/Linux multi-platform support added
//
// LICENSE :-
// The SDK (Software Development Kit) provided for use with the CANdo device
// is issued as FREE software, meaning that it is free for personal,
// educational & commercial use, without restriction or time limit. The
// software is supplied "as is", with no implied warranties or guarantees.
//
// AUTHOR : Martyn Brown
//
// © 2005-14 Netronics Ltd. All rights reserved.
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
unit CANdoImport;

{$MODE Delphi}
{$MACRO On}

interface

uses
{$ifdef Windows}
  Windows;
  {$define CALL_TYPE := stdcall}
{$else if Unix}
  Dynlibs;
  {$define CALL_TYPE := cdecl}
{$endif}

const
  // CANdo constants
  CANDO_STOP = 0;  // Stop Rx/Tx of CAN messages
  CANDO_RUN = 1;  // Start Rx/Tx of CAN messages
  CANDO_NORMAL_MODE = 0;  // Rx/Tx CAN mode
  CANDO_LISTEN_ONLY_MODE = 1;  // Rx only mode, no ACKs
  CANDO_LOOPBACK_MODE = 2;  // Tx internally looped back to Rx
  CANDO_CLK_FREQ = 20000;  // CANdo clk. freq. in kHz for baud rate calc.
  CANDO_CLK_FREQ_HIGH = 40000;  // CANdoISO & CANdo AUTO clk. freq. in kHz for baud rate calc.
  CANDO_BRP_ENHANCED_OFFSET = 63;  // BRP enhanced baud rate setting offset, CANdoISO & CANdo AUTO only
  // CANdo AUTO constants
  CANDO_AUTO_V1_INPUT = 1;  // V1 analogue I/P
  CANDO_AUTO_V2_INPUT = 2;  // V2 analogue I/P
  CANDO_AUTO_MAX_NO_OF_TX_ITEMS = 10;  // Max. no. of items in CAN Transmit store
  // CAN message constants
  CANDO_ID_11_BIT = 0;  // Standard 11 bit ID
  CANDO_ID_29_BIT = 1;  // Extended 29 bit ID
  CANDO_DATA_FRAME = 0;  // CAN data frame
  CANDO_REMOTE_FRAME = 1;  // CAN remote frame
  // CAN receive cyclic buffer size
  CANDO_CAN_BUFFER_LENGTH = 2048;
  // CANdo string type length
  CANDO_STRING_LENGTH = 256;
  // CANdo H/W types
  CANDO_TYPE_ANY = $0000;  // Any H/W type
  CANDO_TYPE_CANDO = $0001;  // CANdo H/W type
  CANDO_TYPE_CANDOISO = $0002;  // CANdoISO H/W type
  CANDO_TYPE_CANDO_AUTO = $0003;  // CANdo AUTO H/W type
  CANDO_TYPE_UNKNOWN = $8000;  // Unknown H/W type
  // CANdo status types
  CANDO_NO_STATUS = 0;  // No new status received
  CANDO_DEVICE_STATUS = 1;  // Device status received
  CANDO_DATE_STATUS = 2;  // Date status received
  CANDO_BUS_LOAD_STATUS = 3;  // Bus load status received
  CANDO_SETUP_STATUS = 4;  // CAN setup status received
  CANDO_ANALOG_INPUT_STATUS = 5;  // Analogue I/P status received
  // CANdo USB PIDs
  CANDO_PID = '8095';  // CANdo PID
  CANDOISO_PID = '8660';  // CANdoISO PID
  CANDO_AUTO_PID = '889B';  // CANdo AUTO PID
  // Function return values
  CANDO_SUCCESS = $0000;  // All OK
  CANDO_USB_DLL_ERROR = $0001;  // SiUSBXp DLL error
  CANDO_USB_DRIVER_ERROR = $0002;  // SiUSBXp driver error
  CANDO_NOT_FOUND = $0004;  // CANdo not found
  CANDO_IO_FAILED = $0008;  // Failed to initialise USB UART parameters
  CANDO_CONNECTION_CLOSED = $0010;  // No CANdo channel open
  CANDO_READ_ERROR = $0020;  // USB UART read error
  CANDO_WRITE_ERROR = $0040;  // USB UART write error
  CANDO_WRITE_INCOMPLETE = $0080;  // Not all requested bytes written to CANdo
  CANDO_BUFFER_OVERFLOW = $0100;  // Overflow in cyclic buffer
  CANDO_RX_OVERRUN = $0200;  // Message received greater than max. message size
  CANDO_RX_TYPE_UNKNOWN = $0400;  // Unknown message type received
  CANDO_RX_CRC_ERROR = $0800;  // CRC mismatch
  CANDO_RX_DECODE_ERROR = $1000;  // Error decoding message
  CANDO_INVALID_HANDLE = $2000;  // Invalid device handle
  CANDO_ERROR = $8000;  // Non specific error

type
  TCANdoDeviceString = array [0..CANDO_STRING_LENGTH - 1] of AnsiChar;  // CANdo string type
  PCANdoDeviceString = ^TCANdoDeviceString;  // Pointer type to TCANdoDeviceString

  // Record type used to store device H/W type & S/N for CANdo
  TCANdoDevice = record
    HardwareType : Integer;  // H/W type of this CANdo
    SerialNo : TCANdoDeviceString;  // USB S/N for this CANdo
  end;

  PCANdoDevice = ^TCANdoDevice;  // Pointer type to TCANdoDevice

  // Record type used to store info. relating to connected CANdo
  TCANdoUSB = record
    TotalNo : Integer;  // Total no. of CANdo on USB bus
    No : Integer;  // No. of this CANdo
    OpenFlag : Boolean;  // USB communications channel state
    Description : TCANdoDeviceString;  // USB decriptor string for CANdo
    SerialNo : TCANdoDeviceString;  // USB S/N for this CANdo
    {$ifdef Windows}
    Handle : THandle;  // Handle to connected CANdo
    {$else if Unix}
    Handle : TLibHandle;  // Handle to connected CANdo
    {$endif}
  end;

  PCANdoUSB = ^TCANdoUSB;  // Pointer type to TCANdoUSB

  // Record type used to store a CAN message
  TCANdoCAN = record
    IDE : Byte;
    RTR : Byte;
    ID : Cardinal;
    DLC : Byte;
    Data : array [0..7] of Byte;
    BusState : Byte;
    TimeStamp : Cardinal;
  end;

  // Record type used as a cyclic buffer to store decoded CAN messages received from CANdo
  TCANdoCANBuffer = record
    CANMessage : array [0..CANDO_CAN_BUFFER_LENGTH - 1] of TCANdoCAN;
    WriteIndex : Integer;
    ReadIndex : Integer;
    FullFlag : Boolean;
  end;

  PCANdoCANBuffer = ^TCANdoCANBuffer;  // Pointer type to TCANdoCANBuffer

  // Record type used to store status information received from CANdo
  TCANdoStatus = record
    HardwareVersion : Byte;
    SoftwareVersion : Byte;
    Status : Byte;
    BusState : Byte;
    TimeStamp : Cardinal;
    NewFlag : Byte;
  end;

  PCANdoStatus = ^TCANdoStatus;  // Pointer type to TCANdoStatus

  // Function pointer types
  TCANdoGetPID = function (CANdoNo : Integer; CANdoPID : PCANdoDeviceString) : Integer; CALL_TYPE;
  TCANdoGetDevices = function (const CANdoDevices : Pointer; NoOfDevices : PInteger) : Integer; CALL_TYPE;
  TCANdoOpen = function (const CANdoUSBPointer : PCANdoUSB) : Integer; CALL_TYPE;
  TCANdoOpenDevice = function (const CANdoUSBPointer : PCANdoUSB; const CANdoDevicePointer : PCANdoDevice) : Integer; CALL_TYPE;
  TCANdoClose = function (const CANdoUSBPointer : PCANdoUSB) : Integer; CALL_TYPE;
  TCANdoFlushBuffers = function (const CANdoUSBPointer : PCANdoUSB) : Integer; CALL_TYPE;
  TCANdoSetBaudRate = function (const CANdoUSBPointer : PCANdoUSB;
    SJW : Byte; BRP : Byte; PHSEG1 : Byte; PHSEG2 : Byte; PROPSEG : Byte; SAM : Byte) : Integer; CALL_TYPE;
  TCANdoSetMode = function (const CANdoUSBPointer : PCANdoUSB; Mode : Byte) : Integer; CALL_TYPE;
  TCANdoSetFilters = function (const CANdoUSBPointer : PCANdoUSB;
    Rx1Mask : Cardinal;
    Rx1IDE1 : Byte; Rx1Filter1 : Cardinal;
    Rx1IDE2 : Byte; Rx1Filter2 : Cardinal;
    Rx2Mask : Cardinal;
    Rx2IDE1 : Byte; Rx2Filter1 : Cardinal;
    Rx2IDE2 : Byte; Rx2Filter2 : Cardinal;
    Rx2IDE3 : Byte; Rx2Filter3 : Cardinal;
    Rx2IDE4 : Byte; Rx2Filter4 : Cardinal) : Integer; CALL_TYPE;
  TCANdoSetState = function (const CANdoUSBPointer : PCANdoUSB; State : Byte) : Integer; CALL_TYPE;
  TCANdoReceive = function (const CANdoUSBPointer : PCANdoUSB;
    const CANdoCANBufferPointer : PCANdoCANBuffer; const CANdoStatusPointer : PCANdoStatus) : Integer; CALL_TYPE;
  TCANdoTransmit = function (const CANdoUSBPointer : PCANdoUSB; IDExtended : Byte;
    ID : Cardinal; RTR : Byte; DLC : Byte; const Data : PByte; BufferNo : Byte; RepeatTime : Byte) : Integer; CALL_TYPE;
  TCANdoRequestStatus = function (const CANdoUSBPointer : PCANdoUSB) : Integer; CALL_TYPE;
  TCANdoRequestDateStatus = function (const CANdoUSBPointer : PCANdoUSB) : Integer; CALL_TYPE;
  TCANdoRequestBusLoadStatus = function (const CANdoUSBPointer : PCANdoUSB) : Integer; CALL_TYPE;
  TCANdoRequestSetupStatus = function (const CANdoUSBPointer : PCANdoUSB) : Integer; CALL_TYPE;
  TCANdoRequestAnalogInputStatus = function (const CANdoUSBPointer : PCANdoUSB) : Integer; CALL_TYPE;
  TCANdoClearStatus = function (const CANdoUSBPointer : PCANdoUSB) : Integer; CALL_TYPE;
  TCANdoGetVersion = procedure (APIVersion : PCardinal; DLLVersion : PCardinal; DriverVersion : PCardinal); CALL_TYPE;
  TCANdoAnalogStoreRead = function (const CANdoUSBPointer : PCANdoUSB) : Integer; CALL_TYPE;
  TCANdoAnalogStoreWrite = function (const CANdoUSBPointer : PCANdoUSB; InputNo : Byte;
    IDExtended : Byte; ID : Cardinal; Start : Byte; Length : Byte;
    ScalingFactor : Double; Offset : Double; Padding : Byte; RepeatTime : Byte) : Integer; CALL_TYPE;
  TCANdoAnalogStoreClear = function (const CANdoUSBPointer : PCANdoUSB) : Integer; CALL_TYPE;
  TCANdoTransmitStoreRead = function (const CANdoUSBPointer : PCANdoUSB) : Integer; CALL_TYPE;
  TCANdoTransmitStoreWrite = function (const CANdoUSBPointer : PCANdoUSB; IDExtended : Byte;
    ID : Cardinal; RTR : Byte; DLC : Byte; const Data : PByte; RepeatTime : Byte) : Integer; CALL_TYPE;
  TCANdoTransmitStoreClear = function (const CANdoUSBPointer : PCANdoUSB) : Integer; CALL_TYPE;

var
  // Global function pointers
  CANdoGetPID : TCANdoGetPID = nil;
  CANdoGetDevices : TCANdoGetDevices = nil;
  CANdoOpen : TCANdoOpen = nil;
  CANdoOpenDevice : TCANdoOpenDevice = nil;
  CANdoClose : TCANdoClose = nil;
  CANdoFlushBuffers : TCANdoFlushBuffers = nil;
  CANdoSetBaudRate : TCANdoSetBaudRate = nil;
  CANdoSetMode : TCANdoSetMode = nil;
  CANdoSetFilters : TCANdoSetFilters = nil;
  CANdoSetState : TCANdoSetState = nil;
  CANdoReceive : TCANdoReceive = nil;
  CANdoTransmit : TCANdoTransmit = nil;
  CANdoRequestStatus : TCANdoRequestStatus = nil;
  CANdoRequestDateStatus : TCANdoRequestDateStatus = nil;
  CANdoRequestBusLoadStatus : TCANdoRequestBusLoadStatus = nil;
  CANdoRequestSetupStatus : TCANdoRequestSetupStatus = nil;
  CANdoRequestAnalogInputStatus : TCANdoRequestAnalogInputStatus = nil;
  CANdoClearStatus : TCANdoClearStatus = nil;
  CANdoGetVersion : TCANdoGetVersion = nil;
  CANdoAnalogStoreRead : TCANdoAnalogStoreRead = nil;
  CANdoAnalogStoreWrite : TCANdoAnalogStoreWrite = nil;
  CANdoAnalogStoreClear : TCANdoAnalogStoreClear = nil;
  CANdoTransmitStoreRead : TCANdoTransmitStoreRead = nil;
  CANdoTransmitStoreWrite : TCANdoTransmitStoreWrite = nil;
  CANdoTransmitStoreClear : TCANdoTransmitStoreClear = nil;

  // Global functions
  function CANdoLoadDLL : Boolean;
  procedure CANdoUnloadDLL;
  function CANdoMapFunctionPointers : Integer;
  procedure CANdoUnmapFunctionPointers;
  function CANdoInitialise : Boolean;
  procedure CANdoFinalise;  

var  
  // Global variables
  {$ifdef Windows}
  DLLHandle : THandle = 0;
  {$else if Unix}
  DLLHandle : TLibHandle = 0;
  {$endif}

implementation

//--------------------------------------------------------------------------
// CANdoLoadDLL
//
// Load CANdo.dll.
//
// Returns
//    False = Error
//    True = OK
//--------------------------------------------------------------------------
function CANdoLoadDLL : Boolean;
begin
  if DLLHandle = 0 then
    {$ifdef Windows}
    DLLHandle := LoadLibrary('CANdo');
    {$else if Unix}
    DLLHandle := LoadLibrary('libCANdo.so');
    {$endif}

  if DLLHandle > 32 then
    Result := True
  else
    Result := False;
end;
//--------------------------------------------------------------------------
// CANdoUnloadDLL
//
// Unload CANdo.dll.
//
// Returns
//    Nothing
//--------------------------------------------------------------------------
procedure CANdoUnloadDLL;
begin
  if DLLHandle > 0 then
  begin
    FreeLibrary(DLLHandle);
    DLLHandle := 0;
  end;
end;
//--------------------------------------------------------------------------
// CANdoMapFunctionPointers
//
// Map function pointers to functions within CANdo.dll.
//
// Returns
//    0 = OK
//    >0 = At least one function not mapped to DLL
//--------------------------------------------------------------------------
function CANdoMapFunctionPointers : Integer;
begin
  if DLLHandle <> 0 then
  begin
    Result := $00000000;

    CANdoGetPID := GetProcAddress(DLLHandle, 'CANdoGetPID');
    if @CANdoGetPID = nil then
      Result := $00000001;  // Function not mapped

    CANdoGetDevices := GetProcAddress(DLLHandle, 'CANdoGetDevices');
    if @CANdoGetDevices = nil then
      Result := Result or $00000002;  // Function not mapped

    CANdoOpen := GetProcAddress(DLLHandle, 'CANdoOpen');
    if @CANdoOpen = nil then
      Result := Result or $00000004;  // Function not mapped

    CANdoOpenDevice := GetProcAddress(DLLHandle, 'CANdoOpenDevice');
    if @CANdoOpenDevice = nil then
      Result := Result or $00000008;  // Function not mapped

    CANdoClose := GetProcAddress(DLLHandle, 'CANdoClose');
    if @CANdoClose = nil then
      Result := Result or $00000010;  // Function not mapped

    CANdoFlushBuffers := GetProcAddress(DLLHandle, 'CANdoFlushBuffers');
    if @CANdoFlushBuffers = nil then
      Result := Result or $00000020;  // Function not mapped

    CANdoSetBaudRate := GetProcAddress(DLLHandle, 'CANdoSetBaudRate');
    if @CANdoSetBaudRate = nil then
      Result := Result or $00000040;  // Function not mapped

    CANdoSetMode := GetProcAddress(DLLHandle, 'CANdoSetMode');
    if @CANdoSetMode = nil then
      Result := Result or $00000080;  // Function not mapped

    CANdoSetFilters := GetProcAddress(DLLHandle, 'CANdoSetFilters');
    if @CANdoSetFilters = nil then
      Result := Result or $00000100;  // Function not mapped

    CANdoSetState := GetProcAddress(DLLHandle, 'CANdoSetState');
    if @CANdoSetState = nil then
      Result := Result or $00000200;  // Function not mapped

    CANdoReceive := GetProcAddress(DLLHandle, 'CANdoReceive');
    if @CANdoReceive = nil then
      Result := Result or $00000400;  // Function not mapped

    CANdoTransmit := GetProcAddress(DLLHandle, 'CANdoTransmit');
    if @CANdoTransmit = nil then
      Result := Result or $00000800;  // Function not mapped

    CANdoRequestStatus := GetProcAddress(DLLHandle, 'CANdoRequestStatus');
    if @CANdoRequestStatus = nil then
      Result := Result or $00001000;  // Function not mapped

    CANdoRequestDateStatus := GetProcAddress(DLLHandle, 'CANdoRequestDateStatus');
    if @CANdoRequestDateStatus = nil then
      Result := Result or $00002000;  // Function not mapped

    CANdoRequestBusLoadStatus := GetProcAddress(DLLHandle, 'CANdoRequestBusLoadStatus');
    if @CANdoRequestBusLoadStatus = nil then
      Result := Result or $00004000;  // Function not mapped

    CANdoRequestSetupStatus := GetProcAddress(DLLHandle, 'CANdoRequestSetupStatus');
    if @CANdoRequestSetupStatus = nil then
      Result := Result or $00008000;  // Function not mapped

    CANdoRequestAnalogInputStatus := GetProcAddress(DLLHandle, 'CANdoRequestAnalogInputStatus');
    if @CANdoRequestAnalogInputStatus = nil then
      Result := Result or $00010000;  // Function not mapped

    CANdoClearStatus := GetProcAddress(DLLHandle, 'CANdoClearStatus');
    if @CANdoClearStatus = nil then
      Result := Result or $00020000;  // Function not mapped

    CANdoGetVersion := GetProcAddress(DLLHandle, 'CANdoGetVersion');
    if @CANdoGetVersion = nil then
      Result := Result or $00040000;  // Function not mapped

    CANdoAnalogStoreRead := GetProcAddress(DLLHandle, 'CANdoAnalogStoreRead');
    if @CANdoAnalogStoreRead = nil then
      Result := Result or $00080000;  // Function not mapped

    CANdoAnalogStoreWrite := GetProcAddress(DLLHandle, 'CANdoAnalogStoreWrite');
    if @CANdoAnalogStoreWrite = nil then
      Result := Result or $00100000;  // Function not mapped

    CANdoAnalogStoreClear := GetProcAddress(DLLHandle, 'CANdoAnalogStoreClear');
    if @CANdoAnalogStoreClear = nil then
      Result := Result or $00200000;  // Function not mapped

    CANdoTransmitStoreRead := GetProcAddress(DLLHandle, 'CANdoTransmitStoreRead');
    if @CANdoTransmitStoreRead = nil then
      Result := Result or $00400000;  // Function not mapped

    CANdoTransmitStoreWrite := GetProcAddress(DLLHandle, 'CANdoTransmitStoreWrite');
    if @CANdoTransmitStoreWrite = nil then
      Result := Result or $00800000;  // Function not mapped

    CANdoTransmitStoreClear := GetProcAddress(DLLHandle, 'CANdoTransmitStoreClear');
    if @CANdoTransmitStoreClear = nil then
      Result := Result or $01000000;  // Function not mapped
  end
  else
    Result := $7FFFFFFF;
end;
//--------------------------------------------------------------------------
// CANdoUnmapFunctionPointers
//
// Unmap function pointers to functions within CANdo.dll.
//
// Returns
//    Nothing
//--------------------------------------------------------------------------
procedure CANdoUnmapFunctionPointers;
begin
  CANdoGetPID := nil;
  CANdoGetDevices := nil;
  CANdoOpen := nil;
  CANdoOpenDevice := nil;
  CANdoClose := nil;
  CANdoFlushBuffers := nil;
  CANdoSetBaudRate := nil;
  CANdoSetMode := nil;
  CANdoSetFilters := nil;
  CANdoSetState := nil;
  CANdoReceive := nil;
  CANdoTransmit := nil;
  CANdoRequestStatus := nil;
  CANdoRequestDateStatus := nil;
  CANdoRequestBusLoadStatus := nil;
  CANdoRequestSetupStatus := nil;
  CANdoRequestAnalogInputStatus := nil;
  CANdoClearStatus := nil;
  CANdoGetVersion := nil;
  CANdoAnalogStoreRead := nil;
  CANdoAnalogStoreWrite := nil;
  CANdoAnalogStoreClear := nil;
  CANdoTransmitStoreRead := nil;
  CANdoTransmitStoreWrite := nil;
  CANdoTransmitStoreClear := nil;
end;
//--------------------------------------------------------------------------
// CANdoInitialise
//
// Load the CANdo.dll & map the functions.
//
// Returns
//    False = Error loading DLL or mapping functions
//    True = DLL loaded & functions all mapped
//--------------------------------------------------------------------------
function CANdoInitialise : Boolean;
var
  Status : Integer;
begin
  // Load the DLL
  if CANdoLoadDLL then
  begin
    // Now map all the functions within the DLL
    Status := CANdoMapFunctionPointers;
    if Status > 0 then
    begin
      // One or more functions not mapped correctly, so deallocate all resources
      CANdoFinalise;
      Result := False;  // Error
    end
    else
      Result := True;  // OK
  end
  else
    // Unable to load DLL
    Result := False;  // Error
end;
//--------------------------------------------------------------------------
// CANdoFinalise
//
// Unmap the functions & unload the CANdo.dll.
//
// Returns
//    Nothing
//--------------------------------------------------------------------------
procedure CANdoFinalise;
begin
  // Unmap function pointers
  CANdoUnmapFunctionPointers;
  // Unload the DLL
  CANdoUnloadDLL;
end;
//--------------------------------------------------------------------------
end.
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------