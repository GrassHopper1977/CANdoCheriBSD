//------------------------------------------------------------------------------
//  TITLE :- CANdoAPI header file - CANdoAPI.h
//  AUTHOR :- Martyn Brown
//  DATE :- 28/08/14
//
//  DESCRIPTION :- CANdoAPI routines header file.
//
//  UPDATES :-
//  23/04/14 Created
//  28/08/14 CANdo API v4.1
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
#ifndef CANDO_API_H
#define CANDO_API_H
//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------
#include "SiUSBXp.h"
//------------------------------------------------------------------------------
// DEFINES
//------------------------------------------------------------------------------
// CANdo API version
#define CANDO_API_VERSION 41  // Version no. * 10
// CANdo H/W PIDs
#define CANDO_PID "8095"  // CANdo PID
#define CANDOISO_PID "8660"  // CANdoISO PID
#define CANDO_AUTO_PID "889B"  // CANdo AUTO PID
// CANdo H/W types
#define CANDO_TYPE_ANY 0x0000  // Any H/W type
#define CANDO_TYPE_CANDO 0x0001  // CANdo H/W type
#define CANDO_TYPE_CANDOISO 0x0002  // CANdoISO H/W type
#define CANDO_TYPE_CANDO_AUTO 0x0003  // CANdo AUTO H/W type
#define CANDO_TYPE_UNKNOWN 0x8000  // Unknown H/W type
// CANdo status types
#define CANDO_NO_STATUS 0
#define CANDO_DEVICE_STATUS 1
#define CANDO_DATE_STATUS 2
#define CANDO_BUS_LOAD_STATUS 3
#define CANDO_SETUP_STATUS 4
#define CANDO_ANALOG_INPUT_STATUS 5
// CANdo function return codes
#define CANDO_SUCCESS 0x0000  // All OK
#define CANDO_USB_DLL_ERROR 0x0001  // SiUSBXp DLL error
#define CANDO_USB_DRIVER_ERROR 0x0002  // SiUSBXp driver error
#define CANDO_NOT_FOUND 0x0004  // CANdo not found
#define CANDO_IO_FAILED 0x0008  // Failed to initialise USB UART parameters
#define CANDO_CONNECTION_CLOSED 0x0010  // No CANdo channel open
#define CANDO_READ_ERROR 0x0020  // USB UART read error
#define CANDO_WRITE_ERROR 0x0040  // USB UART write error
#define CANDO_WRITE_INCOMPLETE 0x0080  // Not all requested bytes written to CANdo
#define CANDO_BUFFER_OVERFLOW 0x0100  // Overflow in cyclic buffer
#define CANDO_RX_OVERRUN 0x0200  // Message received greater than max. message size
#define CANDO_RX_TYPE_UNKNOWN 0x0400  // Unknown message type received
#define CANDO_RX_CRC_ERROR 0x0800  // CRC mismatch
#define CANDO_RX_DECODE_ERROR 0x1000  // Error decoding message
#define CANDO_INVALID_HANDLE 0x2000  // Invalid device handle
#define CANDO_ERROR 0x8000  // Non specific error
// CANdo message types
#define CANDO_CAN_MESSAGE 0
#define CANDO_STATUS_MESSAGE 1
#define CANDO_DATE_STATUS_MESSAGE 2
#define CANDO_BUS_LOAD_STATUS_MESSAGE 3
#define CANDO_SETUP_STATUS_MESSAGE 4
#define CANDO_ANALOG_INPUT_STATUS_MESSAGE 5
// Transmission escape codes
#define STX 0x02
#define ETX 0x03
#define DLE 0x10
// Buffer sizes
#define CANDO_MAX_READ_BUFFER_LENGTH SI_MAX_READ_BUFFER_LENGTH * 2  // Size of raw data Rx cyclic buffer
#define CANDO_MAX_MESSAGE_SIZE 40  // Max. no. of bytes occupied by a message from CANdo in raw data cyclic buffer
#define CANDO_CAN_BUFFER_LENGTH 2048  // Size of CAN message receive cyclic buffer
//------------------------------------------------------------------------------
// TYPEDEFS
//------------------------------------------------------------------------------
typedef SI_DeviceStringType CANdoDeviceStringType;  // CANdo string type

// Struct type used to store device H/W type & S/N for CANdo
typedef struct CANdoDeviceType
{
  int HardwareType;  // H/W type of this CANdo
  SI_DeviceStringType SerialNo;  // USB S/N for this CANdo
} CANdoDeviceType;

typedef CANdoDeviceType * CANdoDevicePointerType;  // Pointer type to CANdoDeviceType

// Struct type used to store USB info. for CANdo
typedef struct CANdoUSBType
{
  unsigned int TotalNo;  // Total no. of CANdo on USB bus
  unsigned int No;  // No. of this CANdo
  unsigned char OpenFlag;  // USB communications channel state
  SI_DeviceStringType Description;  // USB descriptor string for CANdo
  SI_DeviceStringType SerialNo;  // USB S/N for this CANdo
  SI_DeviceType * Handle;  // Handle to connected CANdo
} CANdoUSBType;

typedef CANdoUSBType * CANdoUSBPointerType;  // Pointer type to CANdoUSBType

// Struct type used as a cyclic buffer for raw data received from CANdo
typedef struct CANdoReadBufferType
{
  unsigned char Data[CANDO_MAX_READ_BUFFER_LENGTH];
  int WriteIndex;
  int ReadIndex;
} CANdoReadBufferType;

// Struct type used to store a CAN message
typedef struct CANdoCANType
{
  unsigned char IDE;
  unsigned char RTR;
  unsigned int ID;
  unsigned char DLC;
  unsigned char Data[8];
  unsigned char BusState;
  unsigned int TimeStamp;
} CANdoCANType;

// Struct type used as a cyclic buffer to store decoded CAN messages received from CANdo
typedef struct CANdoCANBufferType
{
  CANdoCANType CANMessage[CANDO_CAN_BUFFER_LENGTH];
  int WriteIndex;
  int ReadIndex;
  unsigned char FullFlag;
} CANdoCANBufferType;

typedef CANdoCANBufferType * CANdoCANBufferPointerType;  // Pointer type to CANdoCANBufferType

// Struct type used to store status information received from CANdo
typedef struct CANdoStatusType
{
  unsigned char HardwareVersion;
  unsigned char SoftwareVersion;
  unsigned char Status;
  unsigned char BusState;
  unsigned int TimeStamp;
  unsigned char NewFlag;
} CANdoStatusType;

typedef CANdoStatusType * CANdoStatusPointerType;  // Pointer type to CANdoStatusType
//------------------------------------------------------------------------------
// PROTOTYPES
//------------------------------------------------------------------------------
void __attribute__((constructor)) CANdoInitialise(void);
void __attribute__((destructor)) CANdoFinalise(void);
int CANdoGetType(int);
int CANdoRead(const CANdoUSBPointerType);
int CANdoWrite(const CANdoUSBPointerType, const unsigned char *, int);
int CANdoGetPID(int, CANdoDeviceStringType);
int CANdoGetDevices(const CANdoDevicePointerType, int *);
int CANdoOpen(const CANdoUSBPointerType);
int CANdoOpenDevice(const CANdoUSBPointerType, const CANdoDevicePointerType);
int CANdoClose(const CANdoUSBPointerType);
int CANdoFlushBuffers(const CANdoUSBPointerType);
int CANdoSetBaudRate(const CANdoUSBPointerType,
  unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
int CANdoSetMode(const CANdoUSBPointerType, unsigned char);
int CANdoSetFilters(const CANdoUSBPointerType,
  unsigned int,
  unsigned char, unsigned int,
  unsigned char, unsigned int,
  unsigned int,
  unsigned char, unsigned int,
  unsigned char, unsigned int,
  unsigned char, unsigned int,
  unsigned char, unsigned int);
int CANdoSetState(const CANdoUSBPointerType, unsigned char);
int CANdoReceive(const CANdoUSBPointerType,
  const CANdoCANBufferPointerType, const CANdoStatusPointerType);
int CANdoTransmit(const CANdoUSBPointerType,
  unsigned char, unsigned int, unsigned char, unsigned char,
  const unsigned char *, unsigned char, unsigned char);
int CANdoProgram(const CANdoUSBPointerType);
int CANdoRequestStatus(const CANdoUSBPointerType);
int CANdoRequestDateStatus(const CANdoUSBPointerType);
int CANdoRequestBusLoadStatus(const CANdoUSBPointerType);
int CANdoRequestSetupStatus(const CANdoUSBPointerType);
int CANdoRequestAnalogInputStatus(const CANdoUSBPointerType);
int CANdoClearStatus(const CANdoUSBPointerType);
int CANdoAnalogStoreRead(const CANdoUSBPointerType);
int CANdoAnalogStoreWrite(const CANdoUSBPointerType, unsigned char,
  unsigned char, unsigned int, unsigned char, unsigned char,
  double, double, unsigned char, unsigned char);
int CANdoAnalogStoreClear(const CANdoUSBPointerType);
int CANdoTransmitStoreRead(const CANdoUSBPointerType);
int CANdoTransmitStoreWrite(const CANdoUSBPointerType,
  unsigned char, unsigned int, unsigned char, unsigned char, const unsigned char *,
  unsigned char);
int CANdoTransmitStoreClear(const CANdoUSBPointerType);
void CANdoGetVersion(unsigned int *, unsigned int *, unsigned int *);
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#endif
