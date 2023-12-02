//------------------------------------------------------------------------------
//  TITLE :- CANdo API unit - CANdoAPI.c
//  AUTHOR :- Martyn Brown
//  DATE :- 28/08/14
//
//  DESCRIPTION :- CANdo API.
//
//  UPDATES :-
//  23/04/14 CANdoAPI.pas v4.0 converted to 'C' for Linux library
//  28/08/14 1) CANdo API v4.1
//           2) Delay added to CANdoClose(...) function to allow any pending
//              command to complete before closing conn.
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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "CRC.h"
#include "IEEE754.h"
#include "CANdoAPI.h"
//------------------------------------------------------------------------------
// GLOBALS
//------------------------------------------------------------------------------
int SiUSBXpStatus = SI_SYSTEM_ERROR_CODE;  // libusb status, uninitialised
CANdoReadBufferType CANdoReadBuffer;  // Cyclic buffer for raw messages received from CANdo
//--------------------------------------------------------------------------
// CANdoInitialise
//
// Init. libusb.
//
// (Note : This is an internal function, not exported to the outside world.)
//
// Returns
//    Nothing
//--------------------------------------------------------------------------
void __attribute__((constructor)) CANdoInitialise(void)
{
  SiUSBXpStatus = SI_Initialise();
}
//--------------------------------------------------------------------------
// CANdoFinalise
//
// Finalise libusb.
//
// (Note : This is an internal function, not exported to the outside world.)
//
// Returns
//    Nothing
//--------------------------------------------------------------------------
void __attribute__((destructor)) CANdoFinalise(void)
{
  SI_Finalise();
}
//--------------------------------------------------------------------------
// CANdoGetType
//
// Get the H/W type of the specified CANdo by reading the USB PID.
//
// (Note : This is an internal function, not exported to the outside world.)
//
// Returns
//    CANdo H/W type
//--------------------------------------------------------------------------
int CANdoGetType(int CANdoNo)
{
  int Status = CANDO_TYPE_UNKNOWN;
  CANdoDeviceStringType CANdoPID;

  if (SiUSBXpStatus == SI_SUCCESS)
  {
    if (SI_GetProductString(CANdoNo, CANdoPID, SI_RETURN_PID) == SI_SUCCESS)
    {
      // PID returned, so compare with CANdo PID constants
      if (strncmp((const char *)CANDO_PID, (const char *)CANdoPID, 4) == 0)
        Status = CANDO_TYPE_CANDO;
      else
      if (strncmp((const char *)CANDOISO_PID, (const char *)CANdoPID, 4) == 0)
        Status = CANDO_TYPE_CANDOISO;
      else
      if (strncmp((const char *)CANDO_AUTO_PID, (const char *)CANdoPID, 4) == 0)
        Status = CANDO_TYPE_CANDO_AUTO;
    }
  }

  return Status;
}
//--------------------------------------------------------------------------
// CANdoRead
//
// Read raw data from the CP2102 inside CANdo & store in CANdoReadBuffer.
//
// (Note : The CANdoReadBuffer is operated as a cyclic buffer, but the
// SI_Read function only understands a linear buffer, so in order to wrap
// the data back to the start of the local buffer, two calls are required to
// the SI_Read function.)
//
// (Note : This is an internal function, not exported to the outside world.)
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoRead(const CANdoUSBPointerType CANdoUSBPointer)
{
  unsigned int NoOfBytesInQueue, QueueStatus, NoOfBytesToRead, NoOfBytesRead;
  SI_ReadBufferPointerType ReadBufferWritePointer;
  int Status = CANDO_CONNECTION_CLOSED, SpaceInBuffer;

  // 1st check to see if there is enough space in cyclic buffer for data from CP2102
  if (CANdoReadBuffer.WriteIndex < CANdoReadBuffer.ReadIndex)
    SpaceInBuffer = CANdoReadBuffer.ReadIndex - CANdoReadBuffer.WriteIndex;
  else
    SpaceInBuffer = CANDO_MAX_READ_BUFFER_LENGTH - CANdoReadBuffer.WriteIndex + CANdoReadBuffer.ReadIndex;

  if (SpaceInBuffer < SI_MAX_READ_BUFFER_LENGTH)
    // Potentially insufficient space in cyclic buffer for data from CP2102, so set buffer overflow flag
    Status = CANDO_BUFFER_OVERFLOW;
  else
  if (CANdoUSBPointer->OpenFlag)
  {
    // Check to see if any data pending in CP2102
    if (SI_CheckRXQueue(CANdoUSBPointer->Handle, &NoOfBytesInQueue, &QueueStatus) == SI_SUCCESS)
    {
      // Check for CP2102 buffer overrun
      if (QueueStatus == SI_RX_OVERRUN)
      {
        // Overrun in CP2102 buffer, so flush buffers & discard data
        SI_FlushBuffers(CANdoUSBPointer->Handle, SI_FLUSH, SI_FLUSH);
        Status = CANDO_BUFFER_OVERFLOW;
      }
      else
      if (NoOfBytesInQueue == 0)
        // No data pending in CP2102 buffer, so skip data read
        Status = CANDO_SUCCESS;
      else
      {
        // Read data from CP2102 into local cyclic buffer
        ReadBufferWritePointer = &CANdoReadBuffer.Data[CANdoReadBuffer.WriteIndex];  // Set pointer to next free slot
        if ((CANdoReadBuffer.WriteIndex + SI_MAX_READ_BUFFER_LENGTH) > CANDO_MAX_READ_BUFFER_LENGTH)
        {
          // Could be too many bytes of data in CP2102, so need to reduce no. of bytes read to prevent running off end of buffer
          NoOfBytesToRead = CANDO_MAX_READ_BUFFER_LENGTH - CANdoReadBuffer.WriteIndex;
          Status = SI_Read(CANdoUSBPointer->Handle, ReadBufferWritePointer, NoOfBytesToRead, &NoOfBytesRead);
          if ((Status == SI_SUCCESS) || (Status == SI_READ_TIMED_OUT))
          {
            if (NoOfBytesRead == NoOfBytesToRead)
            {
              // Might be more data pending, so read again
              CANdoReadBuffer.WriteIndex = 0;  // Reset write index back to start of cyclic buffer
              ReadBufferWritePointer = &CANdoReadBuffer.Data[CANdoReadBuffer.WriteIndex];  // Set pointer to next free slot
              Status = SI_Read(CANdoUSBPointer->Handle, ReadBufferWritePointer, SI_MAX_READ_BUFFER_LENGTH, &NoOfBytesRead);
              if ((Status == SI_SUCCESS) || (Status == SI_READ_TIMED_OUT))
              {
                CANdoReadBuffer.WriteIndex += NoOfBytesRead;  // Increment write index to next free space in cyclic buffer
                Status = CANDO_SUCCESS;
              }
              else
                Status = CANDO_READ_ERROR;
            }
            else
            {
              CANdoReadBuffer.WriteIndex += NoOfBytesRead;  // Increment write index to next free space in cyclic buffer
              Status = CANDO_SUCCESS;
            }
          }
          else
            Status = CANDO_READ_ERROR;
        }
        else
        {
          // Plenty of room in cyclic buffer to store raw data
          Status = SI_Read(CANdoUSBPointer->Handle, ReadBufferWritePointer, SI_MAX_READ_BUFFER_LENGTH, &NoOfBytesRead);
          if ((Status == SI_SUCCESS) || (Status == SI_READ_TIMED_OUT))
          {
            if ((CANdoReadBuffer.WriteIndex + NoOfBytesRead) == CANDO_MAX_READ_BUFFER_LENGTH)
              CANdoReadBuffer.WriteIndex = 0;  // Wrap back to start
            else
              CANdoReadBuffer.WriteIndex += NoOfBytesRead;  // Increment write index to next free space in cyclic buffer
            Status = CANDO_SUCCESS;
          }
          else
            Status = CANDO_READ_ERROR;
        }
      }
    }
    else
      Status = CANDO_READ_ERROR;
  }

  return Status;
}
//--------------------------------------------------------------------------
// CANdoWrite
//
// Format data, add start/end codes & send to CP2102 in CANdo.
//
// (Note : This is an internal function, not exported to the outside world.)
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoWrite(const CANdoUSBPointerType CANdoUSBPointer, const unsigned char * DataPointer, int NoOfBytes)
{
  int Status = SI_WRITE_ERROR;
  SI_WriteBufferType DataBuffer;
  unsigned int BufferNo, ByteNo, BytesWritten, CRC;
  const unsigned char * BufferPointer;

  // First check there aren't too many bytes
  if (NoOfBytes < SI_MAX_WRITE_BUFFER_LENGTH)
  {
    // Copy data buffer pointer to local var.
    BufferPointer = DataPointer;
    // Start of packet delimiter
    DataBuffer[0] = DLE;
    DataBuffer[1] = STX;

    // Copy & encode binary data into data buffer for transmission
    BufferNo = 2;
    for (ByteNo = 0; ByteNo < NoOfBytes; ByteNo++)
    {
      if (*BufferPointer != DLE)
        DataBuffer[BufferNo] = *BufferPointer;  // Copy data
      else
      {
        // Byte is a DLE character, so need to escape with another preceding DLE
        DataBuffer[BufferNo] = DLE;  // Escape character
        DataBuffer[BufferNo + 1] = DLE;
        BufferNo++;
      }
      BufferNo++;
      BufferPointer++;
    }

    // Append CRC
    CRC = 0;  // Init. to 0
    CalculateCRC(DataPointer, NoOfBytes, &CRC);
    // CRC low byte
    DataBuffer[BufferNo] = (unsigned char)(CRC & 0xFF);  // Low byte of CRC
    if (DataBuffer[BufferNo] == DLE)
    {
      // Byte is a DLE character, so need to escape with another preceding DLE
      DataBuffer[BufferNo] = DLE;  // Preceding DLE
      DataBuffer[BufferNo + 1] = DLE;
      BufferNo++;
    }
    BufferNo++;
    // CRC high byte
    DataBuffer[BufferNo] = (unsigned char)((CRC >> 8) & 0xFF);  // High byte of CRC
    if (DataBuffer[BufferNo] == DLE)
    {
      // Byte is a DLE character, so need to escape with another preceding DLE
      DataBuffer[BufferNo] = DLE;  // Preceding DLE
      DataBuffer[BufferNo + 1] = DLE;
      BufferNo++;
    }
    BufferNo++;

    // End of packet delimiter
    DataBuffer[BufferNo] = DLE;
    DataBuffer[BufferNo + 1] = ETX;
    BufferNo = BufferNo + 2;

    // Transmit data to CP2102
    if (CANdoUSBPointer->OpenFlag)
    {
      if (SI_Write(CANdoUSBPointer->Handle, DataBuffer, BufferNo, &BytesWritten) == SI_SUCCESS)
      {
        if (BytesWritten == BufferNo)
          Status = CANDO_SUCCESS;  // OK
        else
          Status = CANDO_WRITE_INCOMPLETE;  // Not all bytes written
      }
      else
        Status = CANDO_WRITE_ERROR;  // Write error
    }
    else
      Status = CANDO_CONNECTION_CLOSED;  // No USB port open to CANdo
  }
  else
    Status = CANDO_ERROR;  // Data block too large

  return Status;
}
//--------------------------------------------------------------------------
// CANdoGetPID
//
// Get the USB PID of the specified CANdo.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoGetPID(int CANdoNo, CANdoDeviceStringType CANdoPIDPointer)
{
  int Status = CANDO_NOT_FOUND;

  if (SiUSBXpStatus == SI_SUCCESS)
  {
    if (SI_GetProductString(CANdoNo, CANdoPIDPointer, SI_RETURN_PID) == SI_SUCCESS)
      Status = CANDO_SUCCESS;
  }

  return Status;
}
//--------------------------------------------------------------------------
// CANdoGetDevices
//
// Retrieve a list of the CANdo devices connected to the PC & return the
// H/W type & S/N of each one.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoGetDevices(const CANdoDevicePointerType CANdoDevices, int * NoOfDevices)
{
  int Status = CANDO_NOT_FOUND;
  unsigned int NoOfDevicesFound, NoOfValidDevicesFound, DeviceNo;
  CANdoDevicePointerType CANdoDevicePointer;

  if (SiUSBXpStatus == SI_SUCCESS)
  {
    // libusb initialised, so determine no. of devices present
    if (SI_GetNumDevices(&NoOfDevicesFound) == SI_SUCCESS)
    {
      if (*NoOfDevices < NoOfDevicesFound)
        NoOfDevicesFound = *NoOfDevices;  // Clip to size of array
      // Interrogate each device in turn
      CANdoDevicePointer = CANdoDevices;  // Load pointer with base addr. of array
      NoOfValidDevicesFound = 0;
      for (DeviceNo = 0; DeviceNo < NoOfDevicesFound; DeviceNo++)
      {
        // Get the H/W type of this device
        CANdoDevicePointer->HardwareType = CANdoGetType(DeviceNo);
        if (CANdoDevicePointer->HardwareType != CANDO_TYPE_UNKNOWN)
        {
          // Get the USB S/N of this device
          if (SI_GetProductString(DeviceNo, CANdoDevicePointer->SerialNo, SI_RETURN_SERIAL_NUMBER) == SI_SUCCESS)
          {
            // Device valid, so move onto next entry in array
            CANdoDevicePointer++;
            NoOfValidDevicesFound++;
          }
        }
      }

      // Clear other device entries in array
      for (DeviceNo = NoOfValidDevicesFound; DeviceNo < *NoOfDevices; DeviceNo++)
      {
        // Mark remaining entries in array as device unknown
        CANdoDevicePointer->HardwareType = CANDO_TYPE_UNKNOWN;
        strcpy(CANdoDevicePointer->SerialNo, "");  // Empty string
        CANdoDevicePointer++;
      }
      *NoOfDevices = NoOfValidDevicesFound;  // Return no. of devices found
      Status = CANDO_SUCCESS;
    }
    else
      *NoOfDevices = 0;
  }
  else
  {
    *NoOfDevices = 0;
    Status = CANDO_USB_DRIVER_ERROR;  // libusb driver error
  }

  return Status;
}
//--------------------------------------------------------------------------
// CANdoOpen
//
// Open up a USB port to a CANdo device & return the USB device details.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoOpen(const CANdoUSBPointerType CANdoUSBPointer)
{
  CANdoDeviceType CANdoDevice;

  CANdoDevice.HardwareType = CANDO_TYPE_ANY;  // Any H/W type
  strcpy(CANdoDevice.SerialNo, "");  // Any S/N

  return CANdoOpenDevice(CANdoUSBPointer, &CANdoDevice);
}
//--------------------------------------------------------------------------
// CANdoFlushBuffersInner
//
// Flush the read/write buffers in the CP2102 & the local read buffer.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
static int CANdoFlushBuffersInner(const CANdoUSBPointerType CANdoUSBPointer)
{
  int Status = CANDO_IO_FAILED;

  // Flush CP2102 read/write buffers within device
  // printf("CANdoFlushBuffersInner()\n");
  if (CANdoUSBPointer->OpenFlag)
  {
    if (SI_FlushBuffers(CANdoUSBPointer->Handle, SI_FLUSH, SI_FLUSH) == SI_SUCCESS)
      Status = CANDO_SUCCESS;
  }
  else
    Status = CANDO_CONNECTION_CLOSED;

  // Flush local read buffer
  CANdoReadBuffer.WriteIndex = 0;
  CANdoReadBuffer.ReadIndex = 0;

  return Status;
}
//--------------------------------------------------------------------------
// CANdoOpenDevice
//
// Open up a USB port to a specific CANdo device based on the H/W type + S/N
// & return the USB device details.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoOpenDevice(const CANdoUSBPointerType CANdoUSBPointer, const CANdoDevicePointerType CANdoDevicePointer)
{
  int Status = CANDO_CONNECTION_CLOSED, CANdoType;

  if (SiUSBXpStatus == SI_SUCCESS)
  {
    // libusb initialised
    if (SI_GetNumDevices(&(CANdoUSBPointer->TotalNo)) == SI_SUCCESS)
    {
      // At least one CANdo found, so find next one free
      CANdoUSBPointer->No = 0;
      CANdoUSBPointer->OpenFlag = FALSE;
      CANdoUSBPointer->Handle = NULL;
      do
      {
        CANdoType = CANdoGetType(CANdoUSBPointer->No);
        if (((CANdoDevicePointer->HardwareType == CANDO_TYPE_ANY) || (CANdoDevicePointer->HardwareType == CANdoType)) && (CANdoType != CANDO_TYPE_UNKNOWN))
        {
          // The CANdo selected is of the correct H/W type, so read the S/N
          if (SI_GetProductString(CANdoUSBPointer->No, CANdoUSBPointer->SerialNo, SI_RETURN_SERIAL_NUMBER) == SI_SUCCESS)
          {
            // Check the S/N
            if ((strcmp(CANdoDevicePointer->SerialNo, "") == 0) || (strncmp(CANdoDevicePointer->SerialNo, CANdoUSBPointer->SerialNo, 4) == 0))
            {
              // The S/N matches the one requested, so attempt to open a connection
              if (SI_Open(CANdoUSBPointer->No, &(CANdoUSBPointer->Handle)) == SI_SUCCESS)
              {
                // Attempt to set baud rate for this CANdo
                if (SI_SetBaudRate(CANdoUSBPointer->Handle, 460800) == SI_SUCCESS)
                {
                  // Baud rate successfully set to 500k/1M (CANdo/CANdoISO/CANdo AUTO), so CANdo is free
                  CANdoDevicePointer->HardwareType = CANdoType;  // Return the H/W type
                  strcpy(CANdoDevicePointer->SerialNo, CANdoUSBPointer->SerialNo);  // Copy the S/N
                  CANdoUSBPointer->OpenFlag = TRUE;  // Set flag to indicate channel to CANdo open
                }
                else
                  CANdoUSBPointer->No++;  // Skip onto next CANdo
              }
              else
                CANdoUSBPointer->No++;  // Skip onto next CANdo
            }
            else
              CANdoUSBPointer->No++;  // Skip onto next CANdo
          }
          else
            CANdoUSBPointer->No++;  // Skip onto next CANdo
        }
        else
          CANdoUSBPointer->No++;  // Skip onto next CANdo
      }
      while (!CANdoUSBPointer->OpenFlag && (CANdoUSBPointer->No < CANdoUSBPointer->TotalNo));

      if (CANdoUSBPointer->OpenFlag)
      {
        // CANdo channel open, so read USB description
        if (SI_GetProductString(CANdoUSBPointer->No, CANdoUSBPointer->Description, SI_RETURN_DESCRIPTION) == SI_SUCCESS)
        {
          // Set USB UART parameters
          if ((SI_SetLineControl(CANdoUSBPointer->Handle, SI_ONE_STOP | SI_NO_PARITY | SI_8_DATA_BITS) == SI_SUCCESS) &&
            (SI_SetTimeouts(SI_TIMEOUT_100MS, SI_TIMEOUT_100MS) == SI_SUCCESS)) {
            // Synchronous, blocking read & write with timeouts set to 100ms
            Status = CANdoFlushBuffersInner(CANdoUSBPointer);  // Flush CP2102 buffers & local read buffer
            } else {
              Status = CANDO_IO_FAILED;
            }
        }
        else
          Status = CANDO_IO_FAILED;
      }
      else
        Status = CANDO_CONNECTION_CLOSED;  // No connections open
    }
    else
      Status = CANDO_NOT_FOUND;  // No CANdo found
  }
  else
    Status = CANDO_USB_DRIVER_ERROR;  // libusb driver error

  return Status;
}
//--------------------------------------------------------------------------
// CANdoClose
//
// Close the USB port to the CANdo device.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoClose(const CANdoUSBPointerType CANdoUSBPointer)
{
  int Status = CANDO_CONNECTION_CLOSED;

  if (CANdoUSBPointer->OpenFlag)
  {
    usleep(20000);  // 20ms delay to allow any pending command to complete before closing conn.
    Status = SI_Close(CANdoUSBPointer->Handle);  // Close USB port

    CANdoUSBPointer->OpenFlag = FALSE;  // Closed
    CANdoUSBPointer->Handle = NULL;

    switch (Status)
    {
      case SI_SUCCESS : Status = CANDO_SUCCESS; break;
      case SI_INVALID_HANDLE : Status = CANDO_INVALID_HANDLE; break;
      default : Status = CANDO_ERROR;
    }
  }

  return Status;
}
//--------------------------------------------------------------------------
// CANdoFlushBuffers
//
// Flush the read/write buffers in the CP2102 & the local read buffer.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoFlushBuffers(const CANdoUSBPointerType CANdoUSBPointer)
{
  // printf("CANdoFlushBuffers()\n");
  return CANdoFlushBuffersInner(CANdoUSBPointer);
}
//--------------------------------------------------------------------------
// CANdoTransmit
//
// Transmit a CAN message to CANdo.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoTransmit(const CANdoUSBPointerType CANdoUSBPointer,
  unsigned char IDExtended, unsigned int ID, unsigned char RTR, unsigned char DLC,
  const unsigned char * Data, unsigned char BufferNo, unsigned char RepeatTime)
{
  int Status;
  unsigned char MessageBuffer[16];
  unsigned int DataNo;
  const unsigned char * DataPointer;

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer[0] = 0;  // CAN transmit message
  // IDE & RTR flags
  MessageBuffer[1] = ((IDExtended << 1) & 0x02) | (RTR & 0x01);
  // ID
  MessageBuffer[2] = (unsigned char)ID;  // LSB
  MessageBuffer[3] = (unsigned char)(ID >> 8);
  MessageBuffer[4] = (unsigned char)(ID >> 16);
  MessageBuffer[5] = (unsigned char)(ID >> 24);  // MSB
  // DLC
  if (DLC > 8)
    DLC = 8;  // Clip DLC to max. value
  MessageBuffer[6] = DLC;
  // Data
  if (Data != NULL)
  {
    // Data supplied, so use
    DataPointer = Data;  // Copy pointer to local variable
    for (DataNo = 0; DataNo < DLC; DataNo++)
    {
      MessageBuffer[DataNo + 7] = *DataPointer;  // Copy data
      DataPointer++;
    }
  }
  else
  {
    // Data null, so ignore
    for (DataNo = 0; DataNo < DLC; DataNo++)
      MessageBuffer[DataNo + 7] = 0;  // Clear data
  }
  // Buffer no. & interval
  MessageBuffer[DLC + 7] = ((RepeatTime << 4) & 0xF0) | (BufferNo & 0x0F);
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, MessageBuffer, DLC + 8);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoRequestStatus
//
// Request the status from CANdo including the H/W + S/W versions & the
// current time.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoRequestStatus(const CANdoUSBPointerType CANdoUSBPointer)
{
  int Status;
  unsigned char MessageBuffer;

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer = 1;  // CANdo status request
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, &MessageBuffer, 1);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoSetBaudRate
//
// Set the CAN bus baud rate for CANdo.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoSetBaudRate(const CANdoUSBPointerType CANdoUSBPointer,
  unsigned char SJW, unsigned char BRP, unsigned char PHSEG1, unsigned char PHSEG2, unsigned char PROPSEG, unsigned char SAM)
{
  int Status;
  unsigned char MessageBuffer[7];

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer[0] = 2;  // CAN baud rate message
  // SJW
  MessageBuffer[1] = SJW;
  // BRP
  MessageBuffer[2] = BRP;
  // PHSEG1
  MessageBuffer[3] = PHSEG1;
  // PHSEG2
  MessageBuffer[4] = PHSEG2;
  // PROPSEG
  MessageBuffer[5] = PROPSEG;
  // SAM
  MessageBuffer[6] = SAM;
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, MessageBuffer, 7);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoSetMode
//
// Set CAN operating mode of CANdo.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoSetMode(const CANdoUSBPointerType CANdoUSBPointer, unsigned char Mode)
{
  int Status;
  unsigned char MessageBuffer[2];

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer[0] = 3;  // CAN operating mode message
  // Operating mode
  MessageBuffer[1] = Mode;
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, MessageBuffer, 2);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoSetFilters
//
// Set the CAN receive acceptance filters for CANdo.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoSetFilters(const CANdoUSBPointerType CANdoUSBPointer,
  unsigned int Rx1Mask,
  unsigned char Rx1IDE1, unsigned int Rx1Filter1,
  unsigned char Rx1IDE2, unsigned int Rx1Filter2,
  unsigned int Rx2Mask,
  unsigned char Rx2IDE1, unsigned int Rx2Filter1,
  unsigned char Rx2IDE2, unsigned int Rx2Filter2,
  unsigned char Rx2IDE3, unsigned int Rx2Filter3,
  unsigned char Rx2IDE4, unsigned int Rx2Filter4)
{
  int Status;
  unsigned char MessageBuffer[39];

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer[0] = 4;  // CAN receive acceptance filter message

  // Rx1Mask
  MessageBuffer[1] = (unsigned char)Rx1Mask;  // LSB
  MessageBuffer[2] = (unsigned char)(Rx1Mask >> 8);
  MessageBuffer[3] = (unsigned char)(Rx1Mask >> 16);
  MessageBuffer[4] = (unsigned char)(Rx1Mask >> 24);  // MSB
  // Rx1Filter1
  MessageBuffer[5] = Rx1IDE1;  // 11 or 29 bit filter
  MessageBuffer[6] = (unsigned char)Rx1Filter1;  // LSB
  MessageBuffer[7] = (unsigned char)(Rx1Filter1 >> 8);
  MessageBuffer[8] = (unsigned char)(Rx1Filter1 >> 16);
  MessageBuffer[9] = (unsigned char)(Rx1Filter1 >> 24);  // MSB
  // Rx1Filter2
  MessageBuffer[10] = Rx1IDE2;  // 11 or 29 bit filter
  MessageBuffer[11] = (unsigned char)Rx1Filter2;  // LSB
  MessageBuffer[12] = (unsigned char)(Rx1Filter2 >> 8);
  MessageBuffer[13] = (unsigned char)(Rx1Filter2 >> 16);
  MessageBuffer[14] = (unsigned char)(Rx1Filter2 >> 24);  // MSB

  // Rx2Mask
  MessageBuffer[15] = (unsigned char)Rx2Mask;  // LSB
  MessageBuffer[16] = (unsigned char)(Rx2Mask >> 8);
  MessageBuffer[17] = (unsigned char)(Rx2Mask >> 16);
  MessageBuffer[18] = (unsigned char)(Rx2Mask >> 24);  // MSB
  // Rx2Filter1
  MessageBuffer[19] = Rx2IDE1;  // 11 or 29 bit filter
  MessageBuffer[20] = (unsigned char)Rx2Filter1;  // LSB
  MessageBuffer[21] = (unsigned char)(Rx2Filter1 >> 8);
  MessageBuffer[22] = (unsigned char)(Rx2Filter1 >> 16);
  MessageBuffer[23] = (unsigned char)(Rx2Filter1 >> 24);  // MSB
  // Rx2Filter2
  MessageBuffer[24] = Rx2IDE2;  // 11 or 29 bit filter
  MessageBuffer[25] = (unsigned char)Rx2Filter2;  // LSB
  MessageBuffer[26] = (unsigned char)(Rx2Filter2 >> 8);
  MessageBuffer[27] = (unsigned char)(Rx2Filter2 >> 16);
  MessageBuffer[28] = (unsigned char)(Rx2Filter2 >> 24);  // MSB
  // Rx2Filter3
  MessageBuffer[29] = Rx2IDE3;  // 11 or 29 bit filter
  MessageBuffer[30] = (unsigned char)Rx2Filter3;  // LSB
  MessageBuffer[31] = (unsigned char)(Rx2Filter3 >> 8);
  MessageBuffer[32] = (unsigned char)(Rx2Filter3 >> 16);
  MessageBuffer[33] = (unsigned char)(Rx2Filter3 >> 24);  // MSB
  // Rx2Filter4
  MessageBuffer[34] = Rx2IDE4;  // 11 or 29 bit filter
  MessageBuffer[35] = (unsigned char)Rx2Filter4;  // LSB
  MessageBuffer[36] = (unsigned char)(Rx2Filter4 >> 8);
  MessageBuffer[37] = (unsigned char)(Rx2Filter4 >> 16);
  MessageBuffer[38] = (unsigned char)(Rx2Filter4 >> 24);  // MSB

  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, MessageBuffer, 39);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoSetState
//
// Set CAN run state for CANdo.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoSetState(const CANdoUSBPointerType CANdoUSBPointer, unsigned char State)
{
  int Status;
  unsigned char MessageBuffer[2];

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer[0] = 5;  // CAN run state message
  // Run state
  MessageBuffer[1] = State;
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, MessageBuffer, 2);
  // Pause execution if state set to run, to allow CANdoISO VISO supply to
  // settle before allowing further commands
  if (State == 1)
    usleep(20000);  // 20ms delay

  return Status;
}
//--------------------------------------------------------------------------
// CANdoProgram
//
// Request CANdo goes into flash memory program mode.
//
// (Note : This is a special function, not exported to the outside world.)
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoProgram(const CANdoUSBPointerType CANdoUSBPointer)
{
  int Status;
  unsigned char MessageBuffer[3];

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer[0] = 6;  // CANdo program mode
  // Program check code 1
  MessageBuffer[1] = 0x55;
  // Program check code 1
  MessageBuffer[2] = 0xAA;
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, MessageBuffer, 3);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoClearStatus
//
// Clear the sys. status within CANdo.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoClearStatus(const CANdoUSBPointerType CANdoUSBPointer)
{
  int Status;
  unsigned char MessageBuffer;

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer = 7;  // CANdo clear status command
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, &MessageBuffer, 1);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoRequestDateStatus
//
// Request the date status from CANdo including the H/W version & the
// current time.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoRequestDateStatus(const CANdoUSBPointerType CANdoUSBPointer)
{
  int Status;
  unsigned char MessageBuffer;

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer = 8;  // CANdo date status request
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, &MessageBuffer, 1);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoRequestBusLoadStatus
//
// Request the CAN bus load status from CANdo including the CAN module
// receive/transmit error counters & the current time.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoRequestBusLoadStatus(const CANdoUSBPointerType CANdoUSBPointer)
{
  int Status;
  unsigned char MessageBuffer;

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer = 9;  // CANdo bus load status request
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, &MessageBuffer, 1);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoRequestSetupStatus
//
// Request the CAN baud rate & operating mode status for the CANdo device.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoRequestSetupStatus(const CANdoUSBPointerType CANdoUSBPointer)
{
	int Status;
  unsigned char MessageBuffer;

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer = 10;  // CANdo setup status request
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, &MessageBuffer, 1);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoRequestAnalogInputStatus
//
// Request a voltage reading from each of the analog I/Ps.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoRequestAnalogInputStatus(const CANdoUSBPointerType CANdoUSBPointer)
{
	int Status;
  unsigned char MessageBuffer;

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer = 11;  // CANdo analog I/Ps status request
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, &MessageBuffer, 1);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoAnalogStoreRead CANdoUSBPointer,
//
// Read the input config. stored in the CANdo analog EEPROM.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoAnalogStoreRead(const CANdoUSBPointerType CANdoUSBPointer)
{
	int Status;
  unsigned char MessageBuffer;

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer = 16;  // Read the analog store
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, &MessageBuffer, 1);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoAnalogStoreWrite
//
// Write the input config. to the CANdo analog EEPROM.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoAnalogStoreWrite(const CANdoUSBPointerType  CANdoUSBPointer, unsigned char InputNo,
  unsigned char IDExtended, unsigned int ID, unsigned char Start, unsigned char Length,
  double ScalingFactor, double Offset, unsigned char Padding, unsigned char RepeatTime)
{
  int Status, Value;
  unsigned char MessageBuffer[17];

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer[0] = 17;  // Write to analog store
  // Input no.
  MessageBuffer[1] = InputNo;
  // IDE
  MessageBuffer[2] = IDExtended;
  // ID
  MessageBuffer[3] = (unsigned char)ID;  // LSB
  MessageBuffer[4] = (unsigned char)(ID >> 8);
  MessageBuffer[5] = (unsigned char)(ID >> 16);
  MessageBuffer[6] = (unsigned char)(ID >> 24);  // MSB
  // Start
  MessageBuffer[7] = Start;
  // Length
  MessageBuffer[8] = Length;
  // Padding
  MessageBuffer[9] = Padding;
  // Scaling factor
  Value = FloatToIEEE754(ScalingFactor);
  MessageBuffer[10] = (unsigned char)(Value & 0xFF);
  MessageBuffer[11] = (unsigned char)((Value >> 8) & 0xFF);
  MessageBuffer[12] = (unsigned char)((Value >> 16) & 0xFF);
  // Offset
  Value = FloatToIEEE754(Offset);
  MessageBuffer[13] = (unsigned char)(Value & 0xFF);
  MessageBuffer[14] = (unsigned char)((Value >> 8) & 0xFF);
  MessageBuffer[15] = (unsigned char)((Value >> 16) & 0xFF);
  // Rate
  MessageBuffer[16] = RepeatTime;
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, MessageBuffer, 17);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoAnalogStoreClear
//
// Clear the CANdo AUTO analog EEPROM store.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoAnalogStoreClear(const CANdoUSBPointerType CANdoUSBPointer)
{
	int Status;
  unsigned char MessageBuffer;

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer = 18;  // Clear the analog store
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, &MessageBuffer, 1);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoTransmitStoreRead CANdoUSBPointer,
//
// Read the CAN transmit messages stored in the CANdo AUTO EEPROM.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoTransmitStoreRead(const CANdoUSBPointerType CANdoUSBPointer)
{
	int Status;
  unsigned char MessageBuffer;

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer = 32;  // Read the CAN transmit message store
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, &MessageBuffer, 1);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoTransmitStoreWrite
//
// Write a CAN transmit message to the CANdo AUTO EEPROM store.
// CANdoUSBPointer,
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoTransmitStoreWrite(const CANdoUSBPointerType CANdoUSBPointer,
  unsigned char IDExtended, unsigned int ID, unsigned char RTR, unsigned char DLC, const unsigned char * Data,
  unsigned char RepeatTime)
{
	int Status, DataNo;
  unsigned char MessageBuffer[16];
  const unsigned char * DataPointer;

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer[0] = 33;  // Write to CAN transmit mess CANdoUSBPointer,age store
  // IDE & RTR flags
  MessageBuffer[1] = ((IDExtended << 1) & 0x02) | (RTR & 0x01);
  // ID
  MessageBuffer[2] = (unsigned char)ID;  // LSB
  MessageBuffer[3] = (unsigned char)(ID >> 8);
  MessageBuffer[4] = (unsigned char)(ID >> 16);
  MessageBuffer[5] = (unsigned char)(ID >> 24);  // MSB
  // DLC
  if (DLC > 8)
    DLC = 8;  // Clip DLC to max. value
  MessageBuffer[6] = DLC;
  // Data
  if (Data != NULL)
  {
    // Data supplied, so use
    DataPointer = Data;  // Copy pointer to local variable
    for (DataNo = 0; DataNo < DLC; DataNo++)
    {
      MessageBuffer[DataNo + 7] = *DataPointer;  // Copy data
      DataPointer++;
    }
  }
  else
  {
    // Data null, so ignore
    for (DataNo = 0; DataNo < DLC; DataNo++)
      MessageBuffer[DataNo + 7] = 0;  // Clear data
  }
  // Buffer no. & interval
  MessageBuffer[DLC + 7] = RepeatTime;
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, MessageBuffer, DLC + 8);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoTransmitStoreClear
//
// Clear the CANdo AUTO EEPROM transmit store.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoTransmitStoreClear(const CANdoUSBPointerType CANdoUSBPointer)
{
	int Status;
  unsigned char MessageBuffer;

  // Load message into local buffer ready for writing to CANdo
  // Message type
  MessageBuffer = 34;  // Clear the CAN transmit message store
  // Now write message to CANdo
  Status = CANdoWrite(CANdoUSBPointer, &MessageBuffer, 1);

  return Status;
}
//--------------------------------------------------------------------------
// CANdoReceive
//
// Decode raw data in CANdoReadBuffer & store decoded CAN & status messages.
//
// Returns
//    CANdo comms. status
//--------------------------------------------------------------------------
int CANdoReceive(const CANdoUSBPointerType CANdoUSBPointer,
  const CANdoCANBufferPointerType CANdoCANBufferPointer, const CANdoStatusPointerType CANdoStatusPointer)
{
  unsigned char ErrorFlag, DLEFoundFlag, StartFoundFlag;
  unsigned char MessageBuffer[CANDO_MAX_MESSAGE_SIZE];
  int Status, MessageStartIndex, MessageIndex, DLC, DataNo;
  unsigned int CalculatedCRC, ReceivedCRC;

  Status = CANdoRead(CANdoUSBPointer);  // Read raw data from CANdo & store in local cyclic buffer
  if (Status == CANDO_SUCCESS)
  {
    // Process new messages in cyclic buffer
    ErrorFlag = FALSE;
    DLEFoundFlag = FALSE;
    StartFoundFlag = FALSE;
    MessageIndex = 0;
    MessageStartIndex = 0;
    while (CANdoReadBuffer.ReadIndex != CANdoReadBuffer.WriteIndex)
    {
      if (!DLEFoundFlag && (CANdoReadBuffer.Data[CANdoReadBuffer.ReadIndex] == DLE))
        DLEFoundFlag = TRUE;  // DLE found
      else
      if (DLEFoundFlag)
      {
        // Previous byte was a DLE, so process this one accordingly
        if (CANdoReadBuffer.Data[CANdoReadBuffer.ReadIndex] == STX)
        {
          // Start of new message found
          ErrorFlag = FALSE;  // Clear error flag
          StartFoundFlag = TRUE;  // Set flag to indicate start found
          MessageIndex = 0;  // Reset index to start of buffer
          // Store start index of mess. in raw data buffer in case need to
          // backtrack, if complete mess. not in buffer yet
          if (CANdoReadBuffer.ReadIndex == 0)
            MessageStartIndex = CANDO_MAX_READ_BUFFER_LENGTH - 1;
          else
            MessageStartIndex = CANdoReadBuffer.ReadIndex - 1;
        }
        else
        if ((CANdoReadBuffer.Data[CANdoReadBuffer.ReadIndex] == DLE) && StartFoundFlag)
        {
          // DLE embedded in the data
          MessageBuffer[MessageIndex] = DLE;  // Insert DLE in local buffer
          MessageIndex++;  // Increment onto next free slot
          if (MessageIndex == CANDO_MAX_MESSAGE_SIZE)
          {
            // Message larger than max. allowed
            ErrorFlag = TRUE;  // Flag an error as we have reached the limit on message size
            Status |= CANDO_RX_OVERRUN;
          }
        }
        else
        if ((CANdoReadBuffer.Data[CANdoReadBuffer.ReadIndex] == ETX) && StartFoundFlag)
        {
          // End of message found
          StartFoundFlag = FALSE;  // Clear start found flag as complete message collected
          CalculatedCRC = 0;  // Init. to 0
          CalculateCRC(MessageBuffer, MessageIndex - 2, &CalculatedCRC);  // Calculate CRC of message
          ReceivedCRC = (MessageBuffer[MessageIndex - 1] << 8) + MessageBuffer[MessageIndex - 2];  // CRC in message
          if (CalculatedCRC == ReceivedCRC)
          {
            // CRC correct, so decode message according to type specified in 1st byte
            switch (MessageBuffer[0])
            {
              // CAN message
              case CANDO_CAN_MESSAGE :
              {
                if (!CANdoCANBufferPointer->FullFlag)
                {
                  // IDE/RTR
                  CANdoCANBufferPointer->CANMessage[CANdoCANBufferPointer->WriteIndex].RTR = MessageBuffer[1] & 0x01;
                  CANdoCANBufferPointer->CANMessage[CANdoCANBufferPointer->WriteIndex].IDE = (MessageBuffer[1] >> 1) & 0x01;
                  // ID
                  CANdoCANBufferPointer->CANMessage[CANdoCANBufferPointer->WriteIndex].ID =
                    MessageBuffer[2] + (MessageBuffer[3] << 8) + (MessageBuffer[4] << 16) + (MessageBuffer[5] << 24);
                  // DLC
                  DLC = MessageBuffer[6];
                  CANdoCANBufferPointer->CANMessage[CANdoCANBufferPointer->WriteIndex].DLC = DLC;
                  // Data
                  for (DataNo = 0; DataNo < 8; DataNo++)
                  {
                    if (DataNo < DLC)
                      CANdoCANBufferPointer->CANMessage[CANdoCANBufferPointer->WriteIndex].Data[DataNo] = MessageBuffer[DataNo + 7];
                    else
                      CANdoCANBufferPointer->CANMessage[CANdoCANBufferPointer->WriteIndex].Data[DataNo] = 0;
                  }
                  // Status
                  CANdoCANBufferPointer->CANMessage[CANdoCANBufferPointer->WriteIndex].BusState = MessageBuffer[DLC + 7];
                  // Time stamp
                  CANdoCANBufferPointer->CANMessage[CANdoCANBufferPointer->WriteIndex].TimeStamp =
                    MessageBuffer[DLC + 8] + (MessageBuffer[DLC + 9] << 8) + (MessageBuffer[DLC + 10] << 16) + (MessageBuffer[DLC + 11] << 24);
                  // Increment CAN message cyclic buffer write pointer
                  if ((CANdoCANBufferPointer->WriteIndex + 1) < CANDO_CAN_BUFFER_LENGTH)
                    CANdoCANBufferPointer->WriteIndex++;  // Increment index onto next free slot
                  else
                    CANdoCANBufferPointer->WriteIndex = 0;  // Wrap back to start
                  if (CANdoCANBufferPointer->WriteIndex == CANdoCANBufferPointer->ReadIndex)
                    CANdoCANBufferPointer->FullFlag = TRUE;  // Set flag to indicate cyclic buffer full
                }
              }
              break;

              // Status message
              case CANDO_STATUS_MESSAGE :
              case CANDO_DATE_STATUS_MESSAGE :
              case CANDO_BUS_LOAD_STATUS_MESSAGE :
              case CANDO_SETUP_STATUS_MESSAGE :
              case CANDO_ANALOG_INPUT_STATUS_MESSAGE :
              {
                // Hardware version
                CANdoStatusPointer->HardwareVersion = MessageBuffer[1];
                // Software version
                CANdoStatusPointer->SoftwareVersion = MessageBuffer[2];
                // Status
                CANdoStatusPointer->Status = MessageBuffer[3];
                // Bus state
                CANdoStatusPointer->BusState = MessageBuffer[4];
                // Time stamp
                CANdoStatusPointer->TimeStamp =
                  MessageBuffer[5] + (MessageBuffer[6] << 8) + (MessageBuffer[7] << 16) + (MessageBuffer[8] << 24);
                // Set flag to indicate new status message arrived
                CANdoStatusPointer->NewFlag = MessageBuffer[0];
              }
              break;

              default :
              {
                // Message type unknown
                ErrorFlag = TRUE;  // Flag an error as type of message unknown
                Status |= CANDO_RX_TYPE_UNKNOWN;
              }
            }
          }
          else
          {
            ErrorFlag = TRUE;  // Flag an error as CRCs don't match
            Status |= CANDO_RX_CRC_ERROR;
          }
        }
        else
        {
          ErrorFlag = TRUE;  // Flag an error as lone DLE found in data
          Status |= CANDO_RX_DECODE_ERROR;
        }
        DLEFoundFlag = FALSE;  // Reset DLE found flag
      }
      else
      if (StartFoundFlag)
      {
        // Copy data to local buffer
        MessageBuffer[MessageIndex] = CANdoReadBuffer.Data[CANdoReadBuffer.ReadIndex];
        MessageIndex++;  // Increment onto next free slot
        if (MessageIndex == CANDO_MAX_MESSAGE_SIZE)
        {
          // Message larger than max. allowed
          ErrorFlag = TRUE;  // Flag an error as we have reached the limit on message size
          Status |= CANDO_RX_OVERRUN;
        }
      }

      // Check to see if any errors found
      if (ErrorFlag)
      {
        // Error found in message, so reset all flags to dispose of data collected so far
        ErrorFlag = FALSE;
        DLEFoundFlag = FALSE;
        StartFoundFlag = FALSE;
      }
      // Increment cyclic buffer read pointer onto next slot
      if ((CANdoReadBuffer.ReadIndex + 1) < CANDO_MAX_READ_BUFFER_LENGTH)
        CANdoReadBuffer.ReadIndex++;  // Increment index onto next slot
      else
        CANdoReadBuffer.ReadIndex = 0;  // Wrap back to start
    }
    if (StartFoundFlag)
      // Incomplete mess. not decoded, so backtrack pointer to start of mess. ready for next time
      CANdoReadBuffer.ReadIndex = MessageStartIndex;
  }
  else
    // Error reading raw data from CP2102
    Status |= CANDO_READ_ERROR;

  return Status;
}
//--------------------------------------------------------------------------
// CANdoGetVersion
//
// Get the version of the CANdo API & libusb driver.
//
// Returns
//    CANdo API & driver version no.s
//--------------------------------------------------------------------------
void CANdoGetVersion(unsigned int * APIVersion, unsigned int * DLLVersion, unsigned int * DriverVersion)
{
  unsigned int LowVersion, HighVersion;

  // CANdo API version no.
  *APIVersion = CANDO_API_VERSION;
  // Get DLL version no.
  SI_GetDLLVersion(&HighVersion, &LowVersion);
  *DLLVersion = HighVersion * 10 + LowVersion;
  // Get driver version no.
  SI_GetDriverVersion(&HighVersion, &LowVersion);
  *DriverVersion = HighVersion * 10 + LowVersion;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



