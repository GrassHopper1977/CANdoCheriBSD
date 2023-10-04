//------------------------------------------------------------------------------
//  TITLE :- SiLabs USBXpress header file - SiUSBXp.h
//  AUTHOR :- Martyn Brown
//  DATE :- 20/04/14
//
//  DESCRIPTION :- Linux SiUSBXp routines header file.
//
//  UPDATES :-
//  20/04/14 Created
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
#ifndef SIUSBXP_H
#define SIUSBXP_H
//------------------------------------------------------------------------------
// INCLUDES
//------------------------------------------------------------------------------
#include <libusb-1.0/libusb.h>
//------------------------------------------------------------------------------
// DEFINES
//------------------------------------------------------------------------------
// SiUSBXp API version
#define SI_API_VERSION 20  // Version no. * 10
// Flag constants
#define FALSE 0
#define TRUE 1
// CANdo USB IDs
#define SI_VID 0x10C4
#define SI_CANDO_PID 0x8095
#define SI_CANDOISO_PID 0x8660
#define SI_CANDO_AUTO_PID 0x889B
// Function return codes
#define SI_SUCCESS 0x00
#define	SI_DEVICE_NOT_FOUND 0xFF
#define	SI_INVALID_HANDLE 0x01
#define	SI_READ_ERROR 0x02
#define	SI_RX_QUEUE_NOT_READY 0x03
#define	SI_WRITE_ERROR 0x04
#define	SI_RESET_ERROR 0x05
#define	SI_INVALID_PARAMETER 0x06
#define	SI_INVALID_REQUEST_LENGTH 0x07
#define	SI_DEVICE_IO_FAILED 0x08
#define	SI_INVALID_BAUDRATE 0x09
#define	SI_FUNCTION_NOT_SUPPORTED 0x0A
#define	SI_GLOBAL_DATA_ERROR 0x0B
#define	SI_SYSTEM_ERROR_CODE 0x0C
#define	SI_READ_TIMED_OUT 0x0D
#define	SI_WRITE_TIMED_OUT 0x0E
#define	SI_IO_PENDING 0x0F
// GetProductString function flags
#define	SI_RETURN_SERIAL_NUMBER 0x00
#define	SI_RETURN_DESCRIPTION 0x01
#define	SI_RETURN_LINK_NAME 0x02
#define	SI_RETURN_VID 0x03
#define	SI_RETURN_PID 0x04
// FlushBuffers function flags
#define SI_DO_NOT_FLUSH 0x00
#define SI_FLUSH 0x01
// SetLineControl function flags
#define SI_ONE_STOP 0x0000
#define SI_NO_PARITY 0x0000
#define SI_8_DATA_BITS 0x0800
// SetFlowControl function flags
#define SI_HELD_INACTIVE 0x00
#define SI_HELD_ACTIVE 0x01
#define SI_FIRMWARE_CONTROLLED 0x02
#define SI_RECEIVE_FLOW_CONTROL 0x02
#define SI_TRANSMIT_ACTIVE_SIGNAL 0x03
#define SI_STATUS_INPUT 0x00
#define SI_HANDSHAKE_LINE 0x01
// Rx queue status codes
#define SI_RX_NO_OVERRUN 0x00
#define	SI_RX_EMPTY 0x00
#define	SI_RX_OVERRUN 0x01
#define	SI_RX_READY 0x02
// Default device descriptors
#define SI_DEFAULT_CONFIG_NO 0
#define SI_DEFAULT_INTERFACE_NO 0
// Timeouts
#define SI_TIMEOUT_100MS 100
#define SI_TIMEOUT_1000MS 1000
// USB control flags
#define SI_FLUSH_RX 0x08  // Flush receive buffer flag
#define SI_FLUSH_TX 0x04  // Flush transmit buffer flag
#define SI_UART_DISABLE 0x0000  // Disable UART I/F flag
#define SI_UART_ENABLE 0xFFFF  // Enable UART I/F flag
// USB control command codes
#define SI_UART_CTRL 0x00  // Enable/disable UART command code
#define SI_SET_LINE_CTRL 0x03  // Set UART config. command code
#define SI_PURGE 0x12  // Flush UART buffers command code
#define SI_SET_FLOW 0x13  // Set UART flow control command code
#define SI_SET_BAUDRATE 0x1E  // Set UART baud rate command code
// Max. buffer sizes
#define	SI_MAX_DEVICE_STRLEN 256
#define SI_BUFFER_SIZE 4096
#define SI_MAX_READ_BUFFER_LENGTH 65536  // Read buffer length (64K max.)
#define SI_MAX_WRITE_BUFFER_LENGTH 4096  // Write buffer length (4K max.)
//------------------------------------------------------------------------------
// TYPEDEFS
//------------------------------------------------------------------------------
typedef struct SI_DeviceType
{
  unsigned char OpenFlag;  // USB conn. open flag
  libusb_device_handle * Handle;  // USB handle to CANdo device
  int EPOut;  // Host -> CANdo bulk end point
  int EPIn;  // CANdo -> Host bulk end point
  int NoOfBytes;  // No. of bytes stored in buffer
  unsigned char Buffer[SI_BUFFER_SIZE];  // Buffer to store data from EPIn
} SI_DeviceType;

typedef char SI_DeviceStringType[SI_MAX_DEVICE_STRLEN];
typedef unsigned char SI_ReadBufferType[SI_MAX_READ_BUFFER_LENGTH];
typedef unsigned char * SI_ReadBufferPointerType;
typedef unsigned char SI_WriteBufferType[SI_MAX_WRITE_BUFFER_LENGTH];
typedef unsigned char * SI_WriteBufferPointerType;
//------------------------------------------------------------------------------
// PROTOTYPES
//------------------------------------------------------------------------------
int SI_Initialise(void);
void SI_Finalise(void);
int SI_GetNumDevices(unsigned int *);
int SI_GetProductString(unsigned int, SI_DeviceStringType, unsigned int);
int SI_Open(unsigned int, SI_DeviceType **);
int SI_Close(SI_DeviceType *);
int SI_Read(SI_DeviceType *, SI_ReadBufferPointerType, unsigned int, unsigned int *);
int SI_Write(SI_DeviceType *, SI_WriteBufferPointerType, unsigned int, unsigned int *);
int SI_FlushBuffers(SI_DeviceType *, unsigned char, unsigned char);
int SI_SetTimeouts(unsigned int, unsigned int);
int SI_CheckRXQueue(SI_DeviceType *, unsigned int *, unsigned int *);
int SI_SetBaudRate(SI_DeviceType *, unsigned int);
int SI_SetLineControl(SI_DeviceType *, unsigned int);
int SI_SetFlowControl(SI_DeviceType *,
  unsigned char, unsigned char, unsigned char, unsigned char,
  unsigned char, unsigned char);
int SI_GetDLLVersion(unsigned int *, unsigned int *);
int SI_GetDriverVersion(unsigned int *, unsigned int *);
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#endif
