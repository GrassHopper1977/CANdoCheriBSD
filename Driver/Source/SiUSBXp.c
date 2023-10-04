//------------------------------------------------------------------------------
//  TITLE :- SiLabs USBXpress wrapper for Linux - SiUSBXp.c
//  AUTHOR :- Martyn Brown
//  DATE :- 01/09/14
//
//  DESCRIPTION :- Linux equivalent of USBXpress based on libusb-1.0 written
//  specifically for the CANdo devices.
//
//  UPDATES :-
//  20/04/14 Created
//  07/07/14 NULL replaced with Context in relevant libusb function calls
//  29/08/14 SI_GetDriverVersion(...) function modified to work with older
//           versions of libusb-1.0
//  01/09/14 Changed to asynchronous I/O for bulk read from CP2102 device
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
#include <pthread.h>
#include "SiUSBXp.h"
//------------------------------------------------------------------------------
// GLOBALS
//------------------------------------------------------------------------------
static libusb_context * Context = NULL; // Context for libusb in application
static unsigned int InTimeout, OutTimeout;  // Timeouts for I/O transfers
static struct libusb_transfer * ReadTransferBlock = NULL;  // libusb read transfer block
static unsigned char ReadTransferBuffer[SI_BUFFER_SIZE];  // libusb read transfer buffer
static pthread_mutex_t ReadTransferMutex;  // Mutex to control threaded access to read data buffer
static pthread_t EventThreadID;  // ID of libusb event thread
static unsigned char EventRunFlag;  // Flag to control libusb event handler function
//------------------------------------------------------------------------------
// IsDeviceCANdoInterface
//
// Checks to see if USB device is a CANdo I/F based on VID & PID.
//
// (Note : This is an internal function, not exported to the outside world.)
//
// Returns -
//    FALSE = Not a CANdo I/F
//    TRUE = Device is a CANdo I/F
//------------------------------------------------------------------------------
static unsigned char IsDeviceCANdoInterface(libusb_device * Device)
{
  struct libusb_device_descriptor Descriptor;

  if (libusb_get_device_descriptor(Device, &Descriptor) == LIBUSB_SUCCESS)
  {
    if (Descriptor.idVendor == SI_VID)
    {
      // SiLabs device found
      if ((Descriptor.idProduct == SI_CANDO_PID) ||
          (Descriptor.idProduct == SI_CANDOISO_PID) ||
          (Descriptor.idProduct == SI_CANDO_AUTO_PID))
        return TRUE;  // CANdo device found
    }
  }

  return FALSE;
}
//--------------------------------------------------------------------------
// EventHandler
//
// This handler function deals with the background tasks required by libusb
// for asynchronous I/O. The function sleeps within the
// libusb_handle_events(...) function.
//
// (Note : This is an internal function, not exported to the outside world.)
//
// Returns -
//    Nothing
//--------------------------------------------------------------------------
static void * EventHandler(void * EventContext)
{
  while (EventRunFlag)
    libusb_handle_events(EventContext);

  pthread_exit(NULL);  // Terminate thread
}
//------------------------------------------------------------------------------
// ReadTransferHandler
//
// libusb call back function for asynchronous bulk reads from the CP2102. The
// data read from the CANdo device is stored in the local buffer.
//
// (Note : This is an internal function, not exported to the outside world.)
//
// Returns -
//    Nothing
//------------------------------------------------------------------------------
static void ReadTransferHandler(struct libusb_transfer * TransferBlock)
{
  int SpaceInBuffer;
  SI_DeviceType * SI_Device = (SI_DeviceType *)TransferBlock->user_data;

  switch (TransferBlock->status)
  {
    case LIBUSB_TRANSFER_COMPLETED :
    {
      // Read transfer complete
      if (TransferBlock->actual_length > 0)
      {
        // Data in buffer, so copy across to local store
        pthread_mutex_lock(&ReadTransferMutex);  // Lock local data buffer
        SpaceInBuffer = SI_BUFFER_SIZE - SI_Device->NoOfBytes;  // Space left in local data buffer
        if (TransferBlock->actual_length > SpaceInBuffer)
        {
          // Insufficient room in local buffer, so copy as much as possible
          memcpy(&SI_Device->Buffer[SI_Device->NoOfBytes], ReadTransferBuffer, SpaceInBuffer);
          SI_Device->NoOfBytes = SI_BUFFER_SIZE;  // Buffer full
        }
        else
        {
          // Enough room in local buffer, so append data
          memcpy(&SI_Device->Buffer[SI_Device->NoOfBytes], ReadTransferBuffer, TransferBlock->actual_length);
          SI_Device->NoOfBytes += TransferBlock->actual_length;
        }
        pthread_mutex_unlock(&ReadTransferMutex);  // Unlock local data buffer
      }
      // Re-submit read transfer
      libusb_submit_transfer(ReadTransferBlock);
    }
    break;

    case LIBUSB_TRANSFER_ERROR :
    case LIBUSB_TRANSFER_TIMED_OUT :
    case LIBUSB_TRANSFER_CANCELLED :
    case LIBUSB_TRANSFER_STALL :
    case LIBUSB_TRANSFER_NO_DEVICE :
    case LIBUSB_TRANSFER_OVERFLOW :
    break;
  }
}
//------------------------------------------------------------------------------
// GetBuffer
//
// Get data read from CANdo device stored in local buffer.
//
// (Note : This is an internal function, not exported to the outside world.)
//
// Returns -
//    No.of bytes retrieved
//------------------------------------------------------------------------------
static int GetBuffer(SI_DeviceType * SI_Device, SI_ReadBufferPointerType Buffer, unsigned int BytesToGet)
{
  int NoOfBytes = 0;

  pthread_mutex_lock(&ReadTransferMutex);  // Lock local data buffer
  if (SI_Device->NoOfBytes >= BytesToGet)
  {
    // Buffer contains all data required
    NoOfBytes = BytesToGet;  // Return no. of bytes requested
    SI_Device->NoOfBytes -= NoOfBytes;  // Remove from buffer
    memcpy(Buffer, SI_Device->Buffer, NoOfBytes);  // Copy data to buffer
    memmove(SI_Device->Buffer, SI_Device->Buffer + NoOfBytes, SI_Device->NoOfBytes);  // Move remaining data in buffer to start
  }
  else
  if (SI_Device->NoOfBytes > 0)
  {
    // Copy all data present in the buffer
    NoOfBytes = SI_Device->NoOfBytes;  // Return all data in buffer
    memcpy(Buffer, SI_Device->Buffer, NoOfBytes);
    SI_Device->NoOfBytes = 0;  // Buffer empty
  }
  pthread_mutex_unlock(&ReadTransferMutex);  // Unlock local data buffer

  return NoOfBytes;
}
//------------------------------------------------------------------------------
// SI_Initialise
//
// Initialise libusb.
//
// Returns -
//    SI function return code
//------------------------------------------------------------------------------
int SI_Initialise(void)
{
  // Set default timeouts for R/W operations
  InTimeout = SI_TIMEOUT_100MS;
  OutTimeout = SI_TIMEOUT_100MS;
  // Init. libusb
  if (libusb_init(&Context) == LIBUSB_SUCCESS)
    return SI_SUCCESS;
  else
    return SI_SYSTEM_ERROR_CODE;
}
//------------------------------------------------------------------------------
// SI_Finalise
//
// Finalise libusb.
//
// Returns -
//    Nothing
//------------------------------------------------------------------------------
void SI_Finalise(void)
{
  libusb_exit(Context);
}
//------------------------------------------------------------------------------
// SI_GetNumDevices
//
// Get the no. of CANdo devices connected.
//
// Returns -
//    SI function return code
//------------------------------------------------------------------------------
int SI_GetNumDevices(unsigned int * NumDevices)
{
  int Status = SI_DEVICE_NOT_FOUND;
  ssize_t DevNo, NoOfDevs;
  libusb_device ** List, * Device;

  *NumDevices = 0;

  // Get a list of all USB devices connected
  NoOfDevs = libusb_get_device_list(Context, &List);

  if (NoOfDevs > 0)
  {
    // Iterate list for CANdo devices
    for (DevNo = 0; DevNo < NoOfDevs; DevNo++)
    {
      Device = List[DevNo];
      if (IsDeviceCANdoInterface(Device))
        (*NumDevices)++;  // CANdo device found
    }
    Status = SI_SUCCESS;
  }

  libusb_free_device_list(List, 1);

  return Status;
}
//------------------------------------------------------------------------------
// SI_GetProductString
//
// Get the S/N or description of CANdo device connected.
//
// Returns
//    SI function return code
//------------------------------------------------------------------------------
int SI_GetProductString(unsigned int DeviceNum, SI_DeviceStringType DeviceString, unsigned int Options)
{
  int Status = SI_DEVICE_NOT_FOUND;
  ssize_t DevNo, NoOfDevs, CANdoNo;
  libusb_device ** List, * Device = NULL;
  struct libusb_device_descriptor Descriptor;
  libusb_device_handle * Handle;
  SI_DeviceStringType Buffer;

  strcpy((char *)DeviceString, "");

  if ((DeviceNum < 0) || (DeviceString == NULL))
    Status = SI_INVALID_PARAMETER;
  else
  {
    // Get a list of all USB devices connected
    NoOfDevs = libusb_get_device_list(Context, &List);

    if (NoOfDevs < 0)
      Status = SI_DEVICE_NOT_FOUND;
    else
    {
      // Find CANdo device specified
      DevNo = 0;
      CANdoNo = -1;
      while ((DevNo < NoOfDevs) && (CANdoNo != DeviceNum))
      {
        Device = List[DevNo];
        if (IsDeviceCANdoInterface(Device))
          CANdoNo++;  // CANdo device found
        DevNo++;
      }

      if ((CANdoNo == DeviceNum) && (Device != NULL))
      {
        // CANdo device found, so get requested info.
        if (libusb_get_device_descriptor(Device, &Descriptor) == LIBUSB_SUCCESS)
        {
          switch(Options)
          {
            case SI_RETURN_SERIAL_NUMBER :
              // Open device
              if (libusb_open(Device, &Handle) == LIBUSB_SUCCESS)
              {
                // Device open, so retrieve S/N
                if (libusb_get_string_descriptor_ascii(Handle, Descriptor.iSerialNumber, (unsigned char *)Buffer, SI_MAX_DEVICE_STRLEN) > 0)
                {
                  strcpy(DeviceString, (const char *)Buffer);
                  Status = SI_SUCCESS;
                }
                // Close device
                libusb_close(Handle);
              }
              break;

            case SI_RETURN_DESCRIPTION :
              // Open device
              if (libusb_open(Device, &Handle) == LIBUSB_SUCCESS)
              {
                // Device open, so retrieve description
                if (libusb_get_string_descriptor_ascii(Handle, Descriptor.iProduct, (unsigned char *)Buffer, SI_MAX_DEVICE_STRLEN) > 0)
                {
                  strcpy(DeviceString, (const char *)Buffer);
                  CANdoNo++;  // CANdo device found
                  Status = SI_SUCCESS;
                }
                // Close device
                libusb_close(Handle);
              }
              break;

            case SI_RETURN_LINK_NAME :
              // Open device
              if (libusb_open(Device, &Handle) == LIBUSB_SUCCESS)
              {
                // Device open, so retrieve manufacturer
                if (libusb_get_string_descriptor_ascii(Handle, Descriptor.iManufacturer, (unsigned char *)Buffer, SI_MAX_DEVICE_STRLEN) > 0)
                {
                  strcpy(DeviceString, (const char *)Buffer);
                  Status = SI_SUCCESS;
                }
                // Close device
                libusb_close(Handle);
              }
              break;

            case SI_RETURN_VID :
              sprintf(DeviceString, "%X", SI_VID);
              Status = SI_SUCCESS;
              break;

            case SI_RETURN_PID :
              sprintf(DeviceString, "%X", Descriptor.idProduct);
              Status = SI_SUCCESS;
              break;

            default :
              Status = SI_INVALID_PARAMETER;
          }
        }
      }

      // Free list of USB devices
      libusb_free_device_list(List, 1);
    }
  }

  return Status;
}
//------------------------------------------------------------------------------
// SI_Open
//
// Open connection to specified CANdo device.
//
// (Note : The CP2102 contains one config. descriptor, that contains one I/F,
// that contains one alternate setting with two bulk end points, one IN (0x81) &
// one OUT (0x01).)
//
// Returns -
//    SI function return code
//------------------------------------------------------------------------------
int SI_Open(unsigned int DeviceNum, SI_DeviceType ** SI_Device)
{
  int Status = SI_DEVICE_NOT_FOUND, EPNo;
  ssize_t DevNo, NoOfDevs, CANdoNo = -1;
  libusb_device ** List, * Device = NULL;
  SI_DeviceType * SI_DeviceInfo = NULL;
  struct libusb_config_descriptor * Configuration;
  const struct libusb_interface * Interface;
  const struct libusb_interface_descriptor * InterfaceDescriptor;
  const struct libusb_endpoint_descriptor * EPDescriptor;

  if (DeviceNum < 0)
    Status = SI_INVALID_PARAMETER;
  else
  {
    // Get a list of all USB devices connected
    NoOfDevs = libusb_get_device_list(Context, &List);

    if (NoOfDevs > 0)
    {
      // Find CANdo device specified
      DevNo = 0;
      while ((DevNo < NoOfDevs) && (CANdoNo != DeviceNum))
      {
        Device = List[DevNo];
        if (IsDeviceCANdoInterface(Device))
          CANdoNo++;  // CANdo device found
        DevNo++;
      }
    }
  }

  if ((CANdoNo == DeviceNum) && (Device != NULL))
  {
    // CANdo device found, so get USB config.
    SI_DeviceInfo = (SI_DeviceType *) malloc(sizeof(SI_DeviceType));  // Allocate some mem. to store device info.
    if (SI_DeviceInfo != NULL)
    {
      SI_DeviceInfo->EPIn = -1;
      SI_DeviceInfo->EPOut = -1;
      if (libusb_get_config_descriptor(Device, SI_DEFAULT_CONFIG_NO, &Configuration) == LIBUSB_SUCCESS)
      {
        // Check there is at least one I/F
        if ((int)Configuration->bNumInterfaces > 0)
        {
          // Get the 1st I/F
          Interface = &Configuration->interface[SI_DEFAULT_INTERFACE_NO];
          if (Interface->num_altsetting > 0)
          {
            // At least 1 alternate setting present, so get the end points for the 1st one
            InterfaceDescriptor = &Interface->altsetting[0];
            for (EPNo = 0; EPNo < InterfaceDescriptor->bNumEndpoints; EPNo++)
            {
              EPDescriptor = &InterfaceDescriptor->endpoint[EPNo];
              if ((EPDescriptor->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_BULK)
              {
                // Bulk EP found
                if ((EPDescriptor->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_IN)
                  SI_DeviceInfo->EPIn = EPDescriptor->bEndpointAddress;  // In EP found
                else
                  SI_DeviceInfo->EPOut = EPDescriptor->bEndpointAddress;  // Out EP found
              }
            }
          }
        }
      }
      // Check the EPs were found
      if ((SI_DeviceInfo->EPIn < 0) || (SI_DeviceInfo->EPOut < 0))
      {
        // EPs not mapped, so de-allocate mem.
        free(SI_DeviceInfo);
        SI_DeviceInfo = NULL;
      }
    }

    if (SI_DeviceInfo != NULL)
    {
      // Device EPs found, so open device
      if (libusb_open(Device, &SI_DeviceInfo->Handle) == LIBUSB_SUCCESS)
      {
        // Device open
        if (SI_DeviceInfo->Handle != NULL)
        {
          // Check for kernel driver attached 1st
          if (libusb_kernel_driver_active(SI_DeviceInfo->Handle, SI_DEFAULT_INTERFACE_NO) == 1)
            // Kernel driver attached, so detach 1st
            libusb_detach_kernel_driver(SI_DeviceInfo->Handle, SI_DEFAULT_INTERFACE_NO);

          // Enable CP2102 UART
          if (libusb_control_transfer(SI_DeviceInfo->Handle,
            LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE, SI_UART_CTRL,
            SI_UART_ENABLE, 0x0000, NULL, 0, OutTimeout) == 0)
          {
            // Claim I/F
            if (libusb_claim_interface(SI_DeviceInfo->Handle, SI_DEFAULT_INTERFACE_NO) == LIBUSB_SUCCESS)
            {
              // Interface successfully claimed, so configure for asynchronous read transfers
              ReadTransferBlock = libusb_alloc_transfer(0);  // Allocate a bulk transfer block
              if (ReadTransferBlock != NULL)
              {
                // Transfer block allocated, so populate & submit the transfer
                SI_DeviceInfo->NoOfBytes = 0;  // Buffer empty
                libusb_fill_bulk_transfer(ReadTransferBlock, SI_DeviceInfo->Handle, SI_DeviceInfo->EPIn,
                                          ReadTransferBuffer, SI_BUFFER_SIZE, ReadTransferHandler,
                                          (void *)SI_DeviceInfo, 0);  // Initialise ReadTransferBlock
                // Create libusb event thread
                EventRunFlag = TRUE;  // Run event handler function loop
                if (pthread_create(&EventThreadID, NULL, EventHandler, (void *)Context) == 0)
                {
                  // Event thread created successfully, so init. mutex & submit the transfer
                  pthread_mutex_init(&ReadTransferMutex, NULL);  // Init. mutex
                  if (libusb_submit_transfer(ReadTransferBlock) == LIBUSB_SUCCESS)
                  {
                    // Transfer successfully submitted
                    SI_DeviceInfo->OpenFlag = TRUE;  // USB conn. to CANdo device open
                    *SI_Device = SI_DeviceInfo;  // Return device info.
                    Status = SI_SUCCESS;
                  }
                  else
                  {
                    EventRunFlag = FALSE;  // Terminate event handler function loop
                    libusb_close(SI_DeviceInfo->Handle);  // Close conn. & wake up event handler function
                    pthread_join(EventThreadID, NULL);  // Wait for event handler thread to terminate
                    free(SI_DeviceInfo);
                    SI_DeviceInfo = NULL;
                    libusb_free_transfer(ReadTransferBlock);
                  }
                }
                else
                {
                  libusb_close(SI_DeviceInfo->Handle);
                  free(SI_DeviceInfo);
                  SI_DeviceInfo = NULL;
                  libusb_free_transfer(ReadTransferBlock);
                }
              }
              else
              {
                libusb_close(SI_DeviceInfo->Handle);
                free(SI_DeviceInfo);
                SI_DeviceInfo = NULL;
              }
            }
            else
            {
              libusb_close(SI_DeviceInfo->Handle);
              free(SI_DeviceInfo);
              SI_DeviceInfo = NULL;
            }
          }
          else
          {
            libusb_close(SI_DeviceInfo->Handle);
            free(SI_DeviceInfo);
            SI_DeviceInfo = NULL;
          }
        }
        else
        {
          // Device not opened, so clear allocated mem.
          free(SI_DeviceInfo);
          SI_DeviceInfo = NULL;
        }
      }
      else
      {
        // Device not opened, so clear allocated mem.
        free(SI_DeviceInfo);
        SI_DeviceInfo = NULL;
      }
    }
  }

  // Free list of USB devices
  libusb_free_device_list(List, 1);

  return Status;
}
//------------------------------------------------------------------------------
// SI_Close
//
// Close connection to CANdo device.
//
// Returns -
//    SI function return code
//------------------------------------------------------------------------------
int SI_Close(SI_DeviceType * SI_Device)
{
  int Status = SI_SYSTEM_ERROR_CODE;

  if ((SI_Device->Handle != NULL) && SI_Device->OpenFlag)
  {
    // Disable CP2102 UART
    libusb_control_transfer(SI_Device->Handle,
      LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE, SI_UART_CTRL,
      SI_UART_DISABLE, 0x0000, NULL, 0, OutTimeout);
    // Release I/F
    libusb_release_interface(SI_Device->Handle, SI_DEFAULT_INTERFACE_NO);
    // Close device connection
    EventRunFlag = FALSE;  // Terminate event handler function loop
    libusb_close(SI_Device->Handle);  // Close conn. & wake up event handler function
    pthread_join(EventThreadID, NULL);  // Wait for event handler thread to terminate
    pthread_mutex_destroy(&ReadTransferMutex);  // Destroy read transfer mutex
    SI_Device->OpenFlag = FALSE;
    // Free the read transfer block
    libusb_free_transfer(ReadTransferBlock);

    Status = SI_SUCCESS;
  }
  else
    Status = SI_INVALID_HANDLE;

  // Free the allocated mem. for CANdo
  free(SI_Device);
  SI_Device = NULL;

  return Status;
}
//------------------------------------------------------------------------------
// SI_Read
//
// Read data from CANdo device.
//
// Returns -
//    SI function return code
//------------------------------------------------------------------------------
int SI_Read(SI_DeviceType * SI_Device, SI_ReadBufferPointerType Buffer, unsigned int BytesToRead, unsigned int * BytesRead)
{
  int Status = SI_SYSTEM_ERROR_CODE;

  if ((Buffer == NULL) || (BytesRead == NULL) || (BytesToRead < 0) || (BytesToRead > SI_MAX_READ_BUFFER_LENGTH))
    Status = SI_INVALID_PARAMETER;
  else
  if ((SI_Device->Handle != NULL) && SI_Device->OpenFlag)
  {
    // Device is open, so read data
    *BytesRead = GetBuffer(SI_Device, Buffer, BytesToRead);  // Copy data in local buffer
    Status = SI_SUCCESS;
  }
  else
    Status = SI_INVALID_HANDLE;

  return Status;
}
//------------------------------------------------------------------------------
// SI_Write
//
// Write data to CANdo device.
//
// Returns -
//    SI function return code
//------------------------------------------------------------------------------
int SI_Write(SI_DeviceType * SI_Device, SI_WriteBufferPointerType Buffer, unsigned int BytesToWrite, unsigned int * BytesWritten)
{
  int Status = SI_SYSTEM_ERROR_CODE, BytesActuallyWritten = 0;

  if ((Buffer == NULL) || (BytesWritten == NULL) || (BytesToWrite == 0) || (BytesToWrite > SI_BUFFER_SIZE))
    Status = SI_INVALID_PARAMETER;
  else
  if ((SI_Device->Handle != NULL) && SI_Device->OpenFlag)
  {
    // Device is open, so write data
    if (libusb_bulk_transfer(SI_Device->Handle, SI_Device->EPOut, Buffer, (int)BytesToWrite, &BytesActuallyWritten, OutTimeout) == LIBUSB_SUCCESS)
    {
      // Data written successfully
      *BytesWritten = BytesActuallyWritten;
      Status = SI_SUCCESS;
    }
    else
      Status = SI_WRITE_ERROR;
  }
  else
    Status = SI_INVALID_HANDLE;

  return Status;
}
//------------------------------------------------------------------------------
// SI_FlushBuffers
//
// Flush the local data buffers.
//
// Returns -
//    SI function return code
//------------------------------------------------------------------------------
int SI_FlushBuffers(SI_DeviceType * SI_Device, unsigned char TransmitBufferFlag, unsigned char ReceiveBufferFlag)
{
  int Status = SI_SYSTEM_ERROR_CODE;
  unsigned int BufferFlags = 0x00;

  if ((SI_Device->Handle != NULL) && SI_Device->OpenFlag)
  {
    // Device is open, so flush specified buffers
    if (ReceiveBufferFlag)
    {
      // Clear local receive buffer
      pthread_mutex_lock(&ReadTransferMutex);  // Lock local data buffer
      SI_Device->NoOfBytes = 0;  // Clear receive buffer
      pthread_mutex_unlock(&ReadTransferMutex);  // Unlock local data buffer
      BufferFlags |= SI_FLUSH_RX;
    }

    if (TransmitBufferFlag)
      BufferFlags |= SI_FLUSH_TX;

    // Clear CANdo UART buffers
    BufferFlags = libusb_cpu_to_le16(BufferFlags);  // Convert to host endian byte order
    // Send purge buffer command to CANdo
    if (libusb_control_transfer(SI_Device->Handle,
      LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE, SI_PURGE,
      BufferFlags, 0, NULL, 0, OutTimeout) == 0)
      Status = SI_SUCCESS;
  }
  else
    Status = SI_INVALID_HANDLE;

  return Status;
}
//------------------------------------------------------------------------------
// SI_SetTimeouts
//
// Set the global R/W timeouts for synchronous transfers.
//
// Returns -
//    SI function return code
//------------------------------------------------------------------------------
int SI_SetTimeouts(unsigned int ReadTimeout, unsigned int WriteTimeout)
{
  InTimeout = ReadTimeout;
  OutTimeout = WriteTimeout;

  return SI_SUCCESS;
}
//------------------------------------------------------------------------------
// SI_CheckRXQueue
//
// Checks the receive queue for data or an overrun.
//
// Returns -
//    SI function return code
//------------------------------------------------------------------------------
int SI_CheckRXQueue(SI_DeviceType * SI_Device, unsigned int * NumBytesInQueue, unsigned int * QueueStatus)
{
  int Status = SI_DEVICE_IO_FAILED;

  if ((NumBytesInQueue == NULL) || (QueueStatus == NULL))
    Status = SI_INVALID_PARAMETER;
  else
  if ((SI_Device->Handle != NULL) && SI_Device->OpenFlag)
  {
    // Device is open, so return bytes in queue & status
    pthread_mutex_lock(&ReadTransferMutex);  // Lock local data buffer
    *NumBytesInQueue = SI_Device->NoOfBytes;
    *QueueStatus = SI_RX_NO_OVERRUN | (SI_Device->NoOfBytes ? SI_RX_READY : SI_RX_EMPTY);
    pthread_mutex_unlock(&ReadTransferMutex);  // Unlock local data buffer
    Status = SI_SUCCESS;
  }
  else
    Status = SI_INVALID_HANDLE;

  return Status;
}
//------------------------------------------------------------------------------
// SI_SetBaudRate
//
// Set the CANdo UART baud rate.
//
// Returns -
//    SI function return code
//------------------------------------------------------------------------------
int SI_SetBaudRate(SI_DeviceType * SI_Device, unsigned int BaudRate)
{
  int Status = SI_DEVICE_IO_FAILED;
  unsigned char Buffer[4];
  unsigned int BufferSize = 4;

  if ((SI_Device->Handle != NULL) && SI_Device->OpenFlag)
  {
    // Device is open, so send baud rate command
    Buffer[0] = (unsigned char)BaudRate;
    Buffer[1] = (unsigned char)(BaudRate >> 8);
    Buffer[2] = (unsigned char)(BaudRate >> 16);
    Buffer[3] = (unsigned char)(BaudRate >> 24);
    BufferSize = libusb_cpu_to_le16(BufferSize);  // Convert to host endian byte order
    // Send UART baud rate command to CANdo
    if (libusb_control_transfer(SI_Device->Handle,
      LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE, SI_SET_BAUDRATE,
      0, 0, Buffer, BufferSize, OutTimeout) == BufferSize)
      Status = SI_SUCCESS;
  }
  else
    Status = SI_INVALID_HANDLE;

  return Status;
}
//------------------------------------------------------------------------------
// SI_SetLineControl
//
// Configure the CANdo UART.
//
// Returns -
//    SI function return code
//------------------------------------------------------------------------------
int SI_SetLineControl(SI_DeviceType * SI_Device, unsigned int LineControl)
{
  int Status = SI_DEVICE_IO_FAILED;
  unsigned int LineCtrl;

  if ((SI_Device->Handle != NULL) && SI_Device->OpenFlag)
  {
    // Device is open, so send UART config. command
    LineCtrl = libusb_cpu_to_le16(LineControl);  // Convert to host endian byte order
    // Send UART line control command to CANdo
    if (libusb_control_transfer(SI_Device->Handle,
      LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE, SI_SET_LINE_CTRL,
      LineCtrl, 0, NULL, 0, OutTimeout) == 0)
      Status = SI_SUCCESS;
  }
  else
    Status = SI_INVALID_HANDLE;

  return Status;
}
//------------------------------------------------------------------------------
// SI_SetFlowControl
//
// Set the flow control settings for the CANdo UART.
//
// Returns -
//    SI function return code
//------------------------------------------------------------------------------
int SI_SetFlowControl(SI_DeviceType * SI_Device,
  unsigned char CTS, unsigned char RTS, unsigned char DTR, unsigned char DSR,
  unsigned char DCD, unsigned char XonXoff)
{
  int Status = SI_DEVICE_IO_FAILED;
  unsigned char Buffer[8];
  unsigned int BufferSize = 8;


  if ((SI_Device->Handle != NULL) && SI_Device->OpenFlag)
  {
    // Device is open, so send UART flow control command
    Buffer[0] = DTR;
    Buffer[0] |= CTS << 3;
    Buffer[0] |= DSR << 4;
    Buffer[0] |= DCD << 5;
    Buffer[0] |= DSR << 6;
    Buffer[1] = 0;
    Buffer[2] = 0;
    Buffer[3] = 0;
    Buffer[4] = XonXoff;
    Buffer[4] |= XonXoff << 1;
    Buffer[4] |= RTS << 6;
    Buffer[5] = 0;
    Buffer[6] = 0;
    Buffer[7] = 0;

    BufferSize = libusb_cpu_to_le16(BufferSize);  // Convert to host endian byte order
    // Send UART flow control command to CANdo
    if (libusb_control_transfer(SI_Device->Handle,
      LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE, SI_SET_FLOW,
      0, 0, Buffer, BufferSize, OutTimeout) == BufferSize)
      Status = SI_SUCCESS;
  }
  else
    Status = SI_INVALID_HANDLE;

  return Status;
}
//------------------------------------------------------------------------------
// SI_GetDLLVersion
//
// Return the version of the SiUSBXp API.
//
// Returns -
//    SI function return code
//------------------------------------------------------------------------------
int SI_GetDLLVersion(unsigned int * HighVersion, unsigned int * LowVersion)
{
  *HighVersion = SI_API_VERSION / 10;
  *LowVersion = SI_API_VERSION % 10;

  return SI_SUCCESS;
}
//------------------------------------------------------------------------------
// SI_GetDriverVersion
//
// Return the version of the libusb driver.
//
// Returns -
//    SI function return code
//------------------------------------------------------------------------------
int SI_GetDriverVersion(unsigned int * HighVersion, unsigned int * LowVersion)
{
#ifdef LIBUSB_API_VERSION
  // libusb_get_version(...) function defined
  const struct libusb_version * Version = libusb_get_version();

  *HighVersion = Version->major;
  *LowVersion = Version->minor;
#else
  *HighVersion = 1;
  *LowVersion = 0;
#endif

  return SI_SUCCESS;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------



