//------------------------------------------------------------------------------
//  TITLE :- CANdoAUTOC main routines - CANdoAUTOC.c
//  AUTHOR :- Martyn Brown
//  DATE :- 02/09/14
//
//  DESCRIPTION :- 'C' example program to demonstrate configuring the CANdo
//  AUTO device via the CANdo API.
//
//  UPDATES :-
//  27/05/14 Created
//  02/09/14 GetKey(...) function added
//  15/12/14 Modified to load libCANdo.so dynamically
//
//  LICENSE :-
//  The SDK (Software Development Kit) provided for use with the CANdo device
//  is issued as FREE software, meaning that it is free for personal,
//  educational & commercial use, without restriction or time limit. The
//  software is supplied "as is", with no implied warranties or guarantees.
//
//  (c) 2014 Netronics Ltd. All rights reserved.
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <termios.h>
#include <dlfcn.h>
#include "CANdoImport.h"
#include "CANdoAUTOC.h"
//------------------------------------------------------------------------------
// GLOBALS
//------------------------------------------------------------------------------
// Library handle
#ifdef _WIN32
  HINSTANCE DLLHandle = NULL;
#define GET_ADDRESS GetProcAddress
#elif __unix
  void * DLLHandle = NULL;
#define GET_ADDRESS dlsym
#endif
// Pointers to functions exported by library
PCANdoGetPID CANdoGetPID;
PCANdoGetDevices CANdoGetDevices;
PCANdoOpen CANdoOpen;
PCANdoOpenDevice CANdoOpenDevice;
PCANdoClose CANdoClose;
PCANdoFlushBuffers CANdoFlushBuffers;
PCANdoSetBaudRate CANdoSetBaudRate;
PCANdoSetMode CANdoSetMode;
PCANdoSetFilters CANdoSetFilters;
PCANdoSetState CANdoSetState;
PCANdoReceive CANdoReceive;
PCANdoTransmit CANdoTransmit;
PCANdoRequestStatus CANdoRequestStatus;
PCANdoRequestDateStatus CANdoRequestDateStatus;
PCANdoRequestBusLoadStatus CANdoRequestBusLoadStatus;
PCANdoRequestSetupStatus CANdoRequestSetupStatus;
PCANdoRequestAnalogInputStatus CANdoRequestAnalogInputStatus;
PCANdoClearStatus CANdoClearStatus;
PCANdoGetVersion CANdoGetVersion;
PCANdoAnalogStoreRead CANdoAnalogStoreRead;
PCANdoAnalogStoreWrite CANdoAnalogStoreWrite;
PCANdoAnalogStoreClear CANdoAnalogStoreClear;
PCANdoTransmitStoreRead CANdoTransmitStoreRead;
PCANdoTransmitStoreWrite CANdoTransmitStoreWrite;
PCANdoTransmitStoreClear CANdoTransmitStoreClear;

TCANdoUSB CANdoUSB;  // Store for parameters relating to connected CANdo
TCANdoCANBuffer CANdoCANBuffer;  // Cyclic store for CAN messages collected from CANdo
TCANdoStatus CANdoStatus;  // Store for status message collected from CANdo

const char * REPEAT_TIME[] = { ">>>> OFF - NO TRANSMISSION <<<<", "10ms", "20ms", "50ms", "100ms", "200ms", "500ms", "1000ms", "2000ms", "5000ms", "10000ms" };
//------------------------------------------------------------------------------
// CANdoInitialise
//
// Load the CANdo.dll & map the functions.
//
// Returns -
//    FALSE = Error loading DLL or mapping functions
//    TRUE = DLL loaded & functions all mapped
//------------------------------------------------------------------------------
unsigned char CANdoInitialise(void)
{
  unsigned char Status;

  if (DLLHandle == NULL)
#ifdef _WIN32
    DLLHandle = LoadLibrary("CANdo.dll");
#elif __unix
    DLLHandle = dlopen("libCANdo.so", RTLD_LAZY);
#endif

  if (DLLHandle != NULL)
  {
    // DLL loaded, so map functions
    if (CANdoMapFunctionPointers())
    {
      // One or more functions not mapped correctly, so deallocate all resources
      CANdoFinalise();
      Status = FALSE;  // Error
    }
    else
      Status = TRUE;  // OK
  }
  else
    Status = FALSE;  // Error

  return Status;
}
//--------------------------------------------------------------------------
// CANdoFinalise
//
// Unmap the functions & unload the CANdo.dll.
//
// Returns -
//    Nothing
//--------------------------------------------------------------------------
void CANdoFinalise(void)
{
  // Unmap the function pointers to the DLL
  CANdoUnmapFunctionPointers();
  // Unload the library
#ifdef _WIN32
  FreeLibrary((HMODULE)DLLHandle);
#elif __unix
  if (DLLHandle != NULL)
    dlclose((void *)DLLHandle);
#endif
}
//--------------------------------------------------------------------------
// CANdoMapFunctionPointers
//
// Map function pointers to functions within CANdo.dll.
//
// Returns -
//    0 = OK
//    >0 = At least one function not mapped to DLL
//--------------------------------------------------------------------------
int CANdoMapFunctionPointers(void)
{
  int MapState;

  if (DLLHandle != NULL)
  {
    MapState = 0x00000000;

    CANdoGetPID = (PCANdoGetPID)GET_ADDRESS(DLLHandle, "CANdoGetPID");
    if (CANdoGetPID == NULL)
      MapState = 0x00000001;  // Function not mapped

    CANdoGetDevices = (PCANdoGetDevices)GET_ADDRESS(DLLHandle, "CANdoGetDevices");
    if (CANdoGetDevices == NULL)
      MapState |= 0x00000002;  // Function not mapped

    CANdoOpen = (PCANdoOpen)GET_ADDRESS(DLLHandle, "CANdoOpen");
    if (CANdoOpen == NULL)
      MapState |= 0x00000004;  // Function not mapped

    CANdoOpenDevice = (PCANdoOpenDevice)GET_ADDRESS(DLLHandle, "CANdoOpenDevice");
    if (CANdoOpenDevice == NULL)
      MapState |= 0x00000008;  // Function not mapped

    CANdoClose = (PCANdoClose)GET_ADDRESS(DLLHandle, "CANdoClose");
    if (CANdoClose == NULL)
      MapState |= 0x00000010;  // Function not mapped

    CANdoFlushBuffers = (PCANdoFlushBuffers)GET_ADDRESS(DLLHandle, "CANdoFlushBuffers");
    if (CANdoFlushBuffers == NULL)
      MapState |= 0x00000020;  // Function not mapped

    CANdoSetBaudRate = (PCANdoSetBaudRate)GET_ADDRESS(DLLHandle, "CANdoSetBaudRate");
    if (CANdoSetBaudRate == NULL)
      MapState |= 0x00000040;  // Function not mapped

    CANdoSetMode = (PCANdoSetMode)GET_ADDRESS(DLLHandle, "CANdoSetMode");
    if (CANdoSetMode == NULL)
      MapState |= 0x00000080;  // Function not mapped

    CANdoSetFilters = (PCANdoSetFilters)GET_ADDRESS(DLLHandle, "CANdoSetFilters");
    if (CANdoSetFilters == NULL)
      MapState |= 0x00000100;  // Function not mapped

    CANdoSetState = (PCANdoSetState)GET_ADDRESS(DLLHandle, "CANdoSetState");
    if (CANdoSetState == NULL)
      MapState |= 0x00000200;  // Function not mapped

    CANdoReceive = (PCANdoReceive)GET_ADDRESS(DLLHandle, "CANdoReceive");
    if (CANdoReceive == NULL)
      MapState |= 0x00000400;  // Function not mapped

    CANdoTransmit = (PCANdoTransmit)GET_ADDRESS(DLLHandle, "CANdoTransmit");
    if (CANdoTransmit == NULL)
      MapState |= 0x00000800;  // Function not mapped

    CANdoRequestStatus = (PCANdoRequestStatus)GET_ADDRESS(DLLHandle, "CANdoRequestStatus");
    if (CANdoRequestStatus == NULL)
      MapState |= 0x00001000;  // Function not mapped

    CANdoRequestDateStatus = (PCANdoRequestDateStatus)GET_ADDRESS(DLLHandle, "CANdoRequestDateStatus");
    if (CANdoRequestDateStatus == NULL)
      MapState |= 0x00002000;  // Function not mapped

    CANdoRequestBusLoadStatus = (PCANdoRequestBusLoadStatus)GET_ADDRESS(DLLHandle, "CANdoRequestBusLoadStatus");
    if (CANdoRequestBusLoadStatus == NULL)
      MapState |= 0x00004000;  // Function not mapped

    CANdoRequestSetupStatus = (PCANdoRequestSetupStatus)GET_ADDRESS(DLLHandle, "CANdoRequestSetupStatus");
    if (CANdoRequestSetupStatus == NULL)
      MapState |= 0x00008000;  // Function not mapped

    CANdoRequestAnalogInputStatus = (PCANdoRequestAnalogInputStatus)GET_ADDRESS(DLLHandle, "CANdoRequestAnalogInputStatus");
    if (CANdoRequestAnalogInputStatus == NULL)
      MapState |= 0x00010000;  // Function not mapped

    CANdoClearStatus = (PCANdoClearStatus)GET_ADDRESS(DLLHandle, "CANdoClearStatus");
    if (CANdoClearStatus == NULL)
      MapState |= 0x00020000;  // Function not mapped

    CANdoGetVersion = (PCANdoGetVersion)GET_ADDRESS(DLLHandle, "CANdoGetVersion");
    if (CANdoGetVersion == NULL)
      MapState |= 0x00040000;  // Function not mapped

    CANdoAnalogStoreRead = (PCANdoAnalogStoreRead)GET_ADDRESS(DLLHandle, "CANdoAnalogStoreRead");
    if (CANdoAnalogStoreRead == NULL)
      MapState |= 0x00080000;  // Function not mapped

    CANdoAnalogStoreWrite = (PCANdoAnalogStoreWrite)GET_ADDRESS(DLLHandle, "CANdoAnalogStoreWrite");
    if (CANdoAnalogStoreWrite == NULL)
      MapState |= 0x00100000;  // Function not mapped

    CANdoAnalogStoreClear = (PCANdoAnalogStoreClear)GET_ADDRESS(DLLHandle, "CANdoAnalogStoreClear");
    if (CANdoAnalogStoreClear == NULL)
      MapState |= 0x00200000;  // Function not mapped

    CANdoTransmitStoreRead = (PCANdoTransmitStoreRead)GET_ADDRESS(DLLHandle, "CANdoTransmitStoreRead");
    if (CANdoTransmitStoreRead == NULL)
      MapState |= 0x00400000;  // Function not mapped

    CANdoTransmitStoreWrite = (PCANdoTransmitStoreWrite)GET_ADDRESS(DLLHandle, "CANdoTransmitStoreWrite");
    if (CANdoTransmitStoreWrite == NULL)
      MapState |= 0x00800000;  // Function not mapped

    CANdoTransmitStoreClear = (PCANdoTransmitStoreClear)GET_ADDRESS(DLLHandle, "CANdoTransmitStoreClear");
    if (CANdoTransmitStoreClear == NULL)
      MapState |= 0x01000000;  // Function not mapped
  }
  else
    MapState = 0x7FFFFFFF;

  return MapState;
}
//--------------------------------------------------------------------------
// CANdoUnmapFunctionPointers
//
// Unmap function pointers to functions within CANdo.dll.
//
// Returns -
//    Nothing
//--------------------------------------------------------------------------
void CANdoUnmapFunctionPointers(void)
{
  CANdoGetPID = NULL;
  CANdoGetDevices = NULL;
  CANdoOpen = NULL;
  CANdoOpenDevice = NULL;
  CANdoClose = NULL;
  CANdoFlushBuffers = NULL;
  CANdoSetBaudRate = NULL;
  CANdoSetMode = NULL;
  CANdoSetFilters = NULL;
  CANdoSetState = NULL;
  CANdoReceive = NULL;
  CANdoTransmit = NULL;
  CANdoRequestStatus = NULL;
  CANdoRequestDateStatus = NULL;
  CANdoRequestBusLoadStatus = NULL;
  CANdoRequestSetupStatus = NULL;
  CANdoRequestAnalogInputStatus = NULL;
  CANdoClearStatus = NULL;
  CANdoGetVersion = NULL;
  CANdoAnalogStoreRead = NULL;
  CANdoAnalogStoreWrite = NULL;
  CANdoAnalogStoreClear = NULL;
  CANdoTransmitStoreRead = NULL;
  CANdoTransmitStoreWrite = NULL;
  CANdoTransmitStoreClear = NULL;
}
//--------------------------------------------------------------------------
// CANdoConnect
//
// This function scans the PC for CANdo AUTO devices & connects to the 1st
// one available.
//
// Returns -
//    Nothing
//--------------------------------------------------------------------------
void CANdoConnect(void)
{
  int Status;
  TCANdoDevice CANdoDevice;
  TCANdoDeviceString Description;

  CANdoDevice.HardwareType = CANDO_TYPE_CANDO_AUTO;  // CANdo AUTO device only
  strcpy((char *)CANdoDevice.SerialNo, "");  // Any S/N
  Status = CANdoOpenDevice(&CANdoUSB, &CANdoDevice);  // Open connection
  if (Status == CANDO_SUCCESS)
  {
    // Connection open
    strcpy((char *)Description, (char *)CANdoUSB.Description);
    strcat((char *)Description, " S/N ");
    strcat((char *)Description, (char *)CANdoUSB.SerialNo);
    printf("\n %s - CONNECTED\n", (char *)Description);
    // Stop all transmissions to allow configuration
    CANdoSetState(&CANdoUSB, CANDO_STOP);
  }
  else
  if (Status == CANDO_USB_DLL_ERROR)
    printf("\n CANdo USB DLL not found");
  else
  if (Status == CANDO_USB_DRIVER_ERROR)
    printf("\n CANdo driver not found");
  else
    printf("\n CANdo AUTO not found");
}
//--------------------------------------------------------------------------
// CANdoGetStatus
//
// Request the CANdo status.
//
// Returns -
//    Nothing
//--------------------------------------------------------------------------
void CANdoGetStatus(unsigned int StatusType)
{
  unsigned char BRP, PROPSEG, PHSEG1, PHSEG2;

  // Send status request to CANdo
  switch (StatusType)
  {
    case CANDO_DEVICE_STATUS : CANdoRequestStatus(&CANdoUSB); break;
    case CANDO_SETUP_STATUS : CANdoRequestSetupStatus(&CANdoUSB); break;
  }
  usleep(5000);  // Wait for reply
  // Check for status message
  if (CANdoReceive(&CANdoUSB, &CANdoCANBuffer, &CANdoStatus) == CANDO_SUCCESS)
  {
    // Message received
    switch (CANdoStatus.NewFlag)
    {
      case CANDO_DEVICE_STATUS :
        printf("\n H/W v.  S/W v.  Status  BusState  Timestamp");
        printf("\n %.1f     %.1f     %.2X      %.2X        %d\n >",
          ((float)CANdoStatus.HardwareVersion / 10),
          ((float)CANdoStatus.SoftwareVersion / 10),
          CANdoStatus.Status,
          CANdoStatus.BusState,
          CANdoStatus.TimeStamp);
        break;

      case CANDO_SETUP_STATUS :
        BRP = CANdoStatus.HardwareVersion;
        BRP++;  // Add 1 as value is stored 0 based
        PROPSEG = CANdoStatus.Status;
        PROPSEG >>= 4;
        PROPSEG++;  // Add 1 as value is stored 0 based
        PHSEG1 = CANdoStatus.SoftwareVersion;
        PHSEG1 &= 0x0F;
        PHSEG1++;  // Add 1 as value is stored 0 based
        PHSEG2 = CANdoStatus.SoftwareVersion;
        PHSEG2 >>= 4;
        PHSEG2++;  // Add 1 as value is stored 0 based
        printf("\n CAN baud rate settings -");
        printf("\n BRP = %d", BRP);
        printf("\n PHROPSEG = %d", PROPSEG);
        printf("\n PHSEG1 = %d", PHSEG1);
        printf("\n PHSEG2 = %d", PHSEG2);
        printf("\n SAM = %d", (CANdoStatus.Status & 0x01) + 1);
        printf("\n SJW = %d", ((CANdoStatus.Status >> 1) & 0x01) + 1);
        printf("\n Baud Rate = %.2fk", CANDO_CLK_FREQ_HIGH / (float)(2 * BRP * (1 + PROPSEG + PHSEG1 + PHSEG2)));
        printf("\n Sample point = %.1f%%\n >", ((float)(1 + PROPSEG + PHSEG1) * 100.0) / (float)(1 + PROPSEG + PHSEG1 + PHSEG2));
        break;
    }

    CANdoStatus.NewFlag = CANDO_NO_STATUS;  // Clear flag to indicate status read
  }
}
//--------------------------------------------------------------------------
// CANdoVersion
//
// Display the versions of the CANdo API, USB DLL & driver.
//
// Returns -
//    Nothing
//--------------------------------------------------------------------------
void CANdoVersion(void)
{
  unsigned int APIVersion, DLLVersion, DriverVersion;

  CANdoGetVersion(&APIVersion, &DLLVersion, &DriverVersion);

  printf("\n CANdo API DLL v%.1f\n CANdo USB DLL v%.1f\n CANdo driver v%.1f\n >",
    (float)APIVersion / 10, (float)DLLVersion / 10, (float)DriverVersion / 10);
}
//--------------------------------------------------------------------------
// CANdoIEEE754ToFloat
//
// Decode a floating point number encoded according to the truncated 24 bit
// IEEE754 representation compatible with the Hi-Tech PICC18 compiler.
//
// Returns
//    Float value decoded from IEEE754 binary representation
//--------------------------------------------------------------------------
double CANdoIEEE754ToFloat(int Number)
{
  double Exponent, Mantissa;

  if (Number != 0)
  {
    // Exponent
    Exponent = pow(2.0, ((Number >> 15) & 0xFF) - 127);
    // Sign bit
    if ((Number & 0x800000) > 0)
      Exponent *= -1;  // -ve value
    // Mantissa
    Mantissa = 1 + ((double)(Number & 0x7FFF) / 32768.0);
    // Decoded floating point value
    return (Exponent * Mantissa);
  }

  return 0.0;
}
//--------------------------------------------------------------------------
// CANdoWriteAnalogInputStore
//
// This procedure configures the 2 analogue input channels within the CANdo
// AUTO device by writing to the non-volatile analogue input store. The CAN
// baud rate for the device is also set to 250k bits/s.
//
// The 'V1 Input' is configured to accept a voltage in the range 0 to 10V &
// generate a corresponding J1939 EEC1 CAN message with the engine speed
// in rpm. The transfer function is 1 rpm per mV, eg. 3V applied to the
// input results in a value of 3000rpm in the transmitted CAN message.
//
// The 'V2 Input' is configured to measure the voltage O/P of a linear
// temperature sensor & generate a corresponding J1939 ET1 CAN message with
// the engine coolant temperature in deg. C. The transfer function is
// 1 deg. C per 100mV, eg. 1V applied to the input results in a value of
// 10 deg. C in the transmitted CAN message.
//
// Returns
//    Nothing
//--------------------------------------------------------------------------
void CANdoWriteAnalogInputStore(void)
{
  const unsigned int EEC1 = 0x18F00400;  // J1939 EEC1 ID, source address = 0
  const unsigned int ET1 = 0x18FEEE00;  // J1939 ET1 ID, source address = 0
  const unsigned char PADDING = 0xFF;  // J1939 padding value
  double ScalingFactor, Offset;

  if (CANdoUSB.OpenFlag == CANDO_OPEN)
  {
    if (CANdoAnalogStoreClear(&CANdoUSB) == CANDO_SUCCESS)
    {
      usleep(50000);  // Wait 50ms to allow CANdo AUTO to clear analog store

      // Configure V1 Input
      ScalingFactor = 8.0;
      Offset = 0.0;
      if (CANdoAnalogStoreWrite(&CANdoUSB, CANDO_AUTO_V1_INPUT, CANDO_ID_29_BIT, EEC1,
        4, 2, ScalingFactor, Offset, PADDING, REPEAT_TIME_500MS) == CANDO_SUCCESS)
        printf("\n V1 Input configured ID = 0x18F00400 with 500ms period");
      else
        printf("\n ERROR - V1 Input not configured!");
      usleep(100000);  // Wait 100ms to allow CANdo AUTO to store settings

      // Configure V2 Input
      ScalingFactor = 0.01;
      Offset = 40.0;
      if (CANdoAnalogStoreWrite(&CANdoUSB, CANDO_AUTO_V2_INPUT, CANDO_ID_29_BIT, ET1,
        1, 1, ScalingFactor, Offset, PADDING, REPEAT_TIME_1000MS) == CANDO_SUCCESS)
        printf("\n V2 Input configured ID = 0x18FEEE00 with 1s period");
      else
        printf("\n ERROR - V2 Input not configured!");
      usleep(100000);  // Wait 100ms to allow CANdo AUTO to store setttings

      // Configure CAN baud rate
      if (CANdoSetBaudRate(&CANdoUSB, 0, 4 + CANDO_BRP_ENHANCED_OFFSET, 7, 5, 4, 0) == CANDO_SUCCESS)
        printf("\n CAN bus baud rate set to 250k bits/s");
      else
        printf("\n ERROR - Unable to set CAN baud rate!");

      printf("\n Cycle power to CANdo AUTO device for settings to take effect\n >");
    }
  }
}
//--------------------------------------------------------------------------
// CANdoReadAnalogInputStore
//
// This function reads the configuration contained within the analogue input
// store & displays the settings. This can be useful to verify the contents
// of the store following a write.
//
// Returns
//    Nothing
//--------------------------------------------------------------------------
void CANdoReadAnalogInputStore(void)
{
  int InputNo, FloatValue;
  unsigned char BRP, PROPSEG, PHSEG1, PHSEG2;

  if (CANdoUSB.OpenFlag == CANDO_OPEN)
  {
    // Clear receive buffer ready to read analog store config.
    if (CANdoFlushBuffers(&CANdoUSB) == CANDO_SUCCESS)
    {
      // Clear CAN cyclic buffer
      CANdoCANBuffer.ReadIndex = CANdoCANBuffer.WriteIndex;
      CANdoCANBuffer.FullFlag = FALSE;
      // Issue analog store read & setup status commands
      if ((CANdoAnalogStoreRead(&CANdoUSB) == CANDO_SUCCESS) &&
        (CANdoRequestSetupStatus(&CANdoUSB) == CANDO_SUCCESS))
      {
        usleep(50000);  // Wait 50ms for CANdo AUTO to reply
        InputNo = CANDO_AUTO_V1_INPUT;
        CANdoStatus.NewFlag = CANDO_NO_STATUS;
        // Collect analog store configuration sent by CANdo AUTO
        if (CANdoReceive(&CANdoUSB, &CANdoCANBuffer, &CANdoStatus) == CANDO_SUCCESS)
        {
          while ((CANdoCANBuffer.ReadIndex != CANdoCANBuffer.WriteIndex) || CANdoCANBuffer.FullFlag)
          {
            // Display I/P config.
            printf("\n V%d Analog I/P configuration -", InputNo);
            // ID
            if (CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].IDE == CANDO_ID_11_BIT)
              printf("\n 11 bit ID = 0x%.3X", CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].ID);  // 11 bit ID
            else
              printf("\n 29 bit ID = 0x%.8X", CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].ID);  // 29 bit ID
            // Start, length & padding bytes
            printf("\n Start byte no. = %d", CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[0]);  // Start
            printf("\n No. of bytes = %d", CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[1]);  // Length
            printf("\n Padding byte = 0x%.2X", CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].BusState);  // Padding
            // Scaling factor & offset
            FloatValue = CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[2] + (CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[3] << 8) + (CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[4] << 16);
            printf("\n Scaling factor = %f", CANdoIEEE754ToFloat(FloatValue));  // Scaling factor
            FloatValue = CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[5] + (CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[6] << 8) + (CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[7] << 16);
            printf("\n Offset = %f", CANdoIEEE754ToFloat(FloatValue));  // Offset
            // Repeat time
            if (CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].TimeStamp < sizeof(REPEAT_TIME))
              printf("\n Repeat time = %s\n >", REPEAT_TIME[CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].TimeStamp]);

            InputNo++;  // Next I/P

            // Move read pointer onto next slot in cyclic buffer
            if ((CANdoCANBuffer.ReadIndex + 1) < CANDO_CAN_BUFFER_LENGTH)
              CANdoCANBuffer.ReadIndex++;  // Increment index onto next free slot
            else
              CANdoCANBuffer.ReadIndex = 0;  // Wrap back to start

            CANdoCANBuffer.FullFlag = FALSE;  // Clear flag as buffer is not full
          }

          // CAN baud rate
          if (CANdoStatus.NewFlag == CANDO_SETUP_STATUS)
          {
            // Display CAN baud rate settings
            BRP = CANdoStatus.HardwareVersion;
            BRP++;  // Add 1 as value is stored 0 based
            PROPSEG = CANdoStatus.Status;
            PROPSEG >>= 4;
            PROPSEG++;  // Add 1 as value is stored 0 based
            PHSEG1 = CANdoStatus.SoftwareVersion;
            PHSEG1 &= 0x0F;
            PHSEG1++;  // Add 1 as value is stored 0 based
            PHSEG2 = CANdoStatus.SoftwareVersion;
            PHSEG2 >>= 4;
            PHSEG2++;  // Add 1 as value is stored 0 based
            printf("\n\n CAN baud rate configuration -");
            printf("\n BRP = %d", BRP);
            printf("\n PROPSEG = %d", PROPSEG);
            printf("\n PHSEG1 = %d", PHSEG1);
            printf("\n PHSEG2 = %d", PHSEG2);
            printf("\n SJW = %d", ((CANdoStatus.Status >> 1) & 0x03) + 1);
            if ((CANdoStatus.Status & 0x01) == 0x00)
              printf("\n SAM = 1");
            else
              printf("\n SAM = 3");
            printf("\n Baud Rate = %.2fk", CANDO_CLK_FREQ_HIGH / (float)(2 * BRP * (1 + PROPSEG + PHSEG1 + PHSEG2)));
            printf("\n Sample point = %.1f%%\n >", ((float)(1 + PROPSEG + PHSEG1) * 100.0) / (float)(1 + PROPSEG + PHSEG1 + PHSEG2));

            CANdoStatus.NewFlag = CANDO_NO_STATUS;  // Clear flag to show status read
          }
        }
        else
          printf("\n ERROR - Analog I/P configuration not received!\n >");
      }
      else
        printf("\n ERROR - Unable to read Analog I/P configuration!\n >");
    }
    else
      printf("\n ERROR - Unable to flush buffers!\n >");
  }
}
//--------------------------------------------------------------------------
// CANdoWriteCANTransmitStore
//
// This procedure configures the CANdo AUTO device to transmit 3 fixed,
// periodic messages on the CAN bus by writing to the non-volatile CAN
// transmit store. The CAN baud rate for the device is also set to 250k bits/s.
//
// Message 1 - J1939 Total engine hours = 2200hrs
// ID = 0x18FEE500 29 bit
// Data = 0x00, 0x00, 0xAB, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF
// Period = 2000ms
//
// Message 2 - J1939 Fan speed = 600rpm
// ID = 0x18FEBD00 29 bit
// Data = 0xFF, 0xFF, 0x12, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF
// Period = 500ms
//
// Message 3 - J1939 Exhaust temperature = 310K right manifold, 325K left manifold
// ID = 0x18FE0700 29 bit
// Data = 0x48, 0xE0, 0x4A, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF
// Period = 1000ms
//
// Returns
//    Nothing
//--------------------------------------------------------------------------
void CANdoWriteCANTransmitStore(void)
{
  const unsigned int EHR = 0x18FEE500;  // J1939 Engine hours/revolutions ID, source address = 0
  const unsigned int FD = 0x18FEBD00;  // J1939 Fan drive ID, source address = 0
  const unsigned int ET = 0x18FE0700;  // J1939 Exhaust temperature ID, source address = 0
  const unsigned char TOTAL_ENGINE_HOURS[] = {0x00, 0x00, 0xAB, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF};
  const unsigned char FAN_SPEED[] = {0xFF, 0xFF, 0x12, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF};
  const unsigned char EXHAUST_TEMP[] = {0x48, 0xE0, 0x4A, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF};

  if (CANdoUSB.OpenFlag == CANDO_OPEN)
  {
    // Device connected, so clear transmit store
    if (CANdoTransmitStoreClear(&CANdoUSB) == CANDO_SUCCESS)
    {
      usleep(50000);  // Wait 50ms for store to clear

      // Write CAN transmit message 1
      if (CANdoTransmitStoreWrite(&CANdoUSB, CANDO_ID_29_BIT, EHR,
        CANDO_DATA_FRAME, 8, TOTAL_ENGINE_HOURS, REPEAT_TIME_2000MS) == CANDO_SUCCESS)
      {
        usleep(100000);  // Wait 100ms for message to be stored
        printf("\n CAN Transmit message 1 configured -");
        printf("\n ID = 0x18FEE500");
        printf("\n Repeat time = 2000ms");
      }
      else
        printf("\n ERROR - Unable to write CAN Tranmsit message 1!");

      // Write CAN transmit message 2
      if (CANdoTransmitStoreWrite(&CANdoUSB, CANDO_ID_29_BIT, FD,
        CANDO_DATA_FRAME, 8, FAN_SPEED, REPEAT_TIME_500MS) == CANDO_SUCCESS)
      {
        usleep(100000);  // Wait 100ms for message to be stored
        printf("\n\n CAN Transmit message 2 configured -");
        printf("\n ID = 0x18FEBD00");
        printf("\n Repeat time = 500ms");
      }
      else
        printf("\n\n ERROR - Unable to write CAN Tranmsit message 2!");

      // Write CAN transmit message 3
      if (CANdoTransmitStoreWrite(&CANdoUSB, CANDO_ID_29_BIT, ET,
        CANDO_DATA_FRAME, 8, EXHAUST_TEMP, REPEAT_TIME_1000MS) == CANDO_SUCCESS)
      {
        usleep(100000);  // Wait 100ms for message to be stored
        printf("\n\n CAN Transmit message 3 configured -");
        printf("\n ID = 0x18FE0700");
        printf("\n Repeat time = 1000ms");
      }
      else
        printf("\n\n ERROR - Unable to write CAN Tranmsit message 3!");

      // Configure CAN baud rate
      if (CANdoSetBaudRate(&CANdoUSB, 0, 4 + CANDO_BRP_ENHANCED_OFFSET, 7, 5, 4, 0) == CANDO_SUCCESS)
        printf("\n\n CAN bus baud rate set to 250k bits/s");
      else
        printf("\n\n ERROR - Unable to set CAN baud rate!");

      printf("\n\n Cycle power to CANdo AUTO device for settings to take effect\n >");
    }
    else
      printf("\n ERROR - Unable to clear CAN Transmit store!\n >");
  }
}
//--------------------------------------------------------------------------
// CANdoReadCANTransmitStore
//
// This function reads the CAN transmit messages stored in the CAN transmit
// store & displays them as a list. This can be useful to verify the contents
// of the store following a write.
//
// Returns
//    Nothing
//--------------------------------------------------------------------------
void CANdoReadCANTransmitStore(void)
{
  char DataByte[4], Data[32];
  unsigned char DataNo, CANMessageNo, BRP, PROPSEG, PHSEG1, PHSEG2;

  if (CANdoUSB.OpenFlag == CANDO_OPEN)
  {
    // Clear receive buffer ready to read CAN Transmit store config.
    if (CANdoFlushBuffers(&CANdoUSB) == CANDO_SUCCESS)
    {
      // Clear CAN cyclic buffer
      CANdoCANBuffer.ReadIndex = CANdoCANBuffer.WriteIndex;
      CANdoCANBuffer.FullFlag = FALSE;
      // Issue analog store read & setup status commands
      if ((CANdoTransmitStoreRead(&CANdoUSB) == CANDO_SUCCESS) &&
        (CANdoRequestSetupStatus(&CANdoUSB) == CANDO_SUCCESS))
      {
        usleep(50000);  // Wait 50ms for CANdo AUTO to reply
        CANMessageNo = 1;
        CANdoStatus.NewFlag = CANDO_NO_STATUS;
        // Collect analog store configuration sent by CANdo AUTO
        if (CANdoReceive(&CANdoUSB, &CANdoCANBuffer, &CANdoStatus) == CANDO_SUCCESS)
        {
          while ((CANdoCANBuffer.ReadIndex != CANdoCANBuffer.WriteIndex) || CANdoCANBuffer.FullFlag)
          {
            // Display message stored
            printf("\n CAN Transmit message %d configuration -", CANMessageNo);
            // ID
            if (CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].IDE == CANDO_ID_11_BIT)
              printf("\n 11 bit ID = 0x%.3X", CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].ID);  // 11 bit ID
            else
              printf("\n 29 bit ID = 0x%.8X", CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].ID);  // 29 bit ID
            // DLC
            printf("\n DLC = %d", CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].DLC);  // DLC
            // Data
            strcpy(Data, "");
            for (DataNo = 0; DataNo < 7; DataNo++)
            {
              sprintf(DataByte, " %.2X,", CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[DataNo]);
              strcat(Data, DataByte);
            }
            sprintf(DataByte, " %.2X", CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[DataNo]);
            strcat(Data, DataByte);
            printf("\n Data = %s", Data);
            // Repeat time
            if (CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].BusState < sizeof(REPEAT_TIME))
              printf("\n Repeat time = %s\n >", REPEAT_TIME[CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].BusState]);

            CANMessageNo++;  // Next message

            // Move read pointer onto next slot in cyclic buffer
            if ((CANdoCANBuffer.ReadIndex + 1) < CANDO_CAN_BUFFER_LENGTH)
              CANdoCANBuffer.ReadIndex++;  // Increment index onto next free slot
            else
              CANdoCANBuffer.ReadIndex = 0;  // Wrap back to start

            CANdoCANBuffer.FullFlag = FALSE;  // Clear flag as buffer is not full
          }

          if (CANMessageNo == 1)
            // No CAN transmit messages present
            printf("\n CAN Transmit store is empty");

          // CAN baud rate
          if (CANdoStatus.NewFlag == CANDO_SETUP_STATUS)
          {
            // Display CAN baud rate settings
            BRP = CANdoStatus.HardwareVersion;
            BRP++;  // Add 1 as value is stored 0 based
            PROPSEG = CANdoStatus.Status;
            PROPSEG >>= 4;
            PROPSEG++;  // Add 1 as value is stored 0 based
            PHSEG1 = CANdoStatus.SoftwareVersion;
            PHSEG1 &= 0x0F;
            PHSEG1++;  // Add 1 as value is stored 0 based
            PHSEG2 = CANdoStatus.SoftwareVersion;
            PHSEG2 >>= 4;
            PHSEG2++;  // Add 1 as value is stored 0 based
            printf("\n\n CAN baud rate configuration -");
            printf("\n BRP = %d", BRP);
            printf("\n PROPSEG = %d", PROPSEG);
            printf("\n PHSEG1 = %d", PHSEG1);
            printf("\n PHSEG2 = %d", PHSEG2);
            printf("\n SJW = %d", ((CANdoStatus.Status >> 1) & 0x03) + 1);
            if ((CANdoStatus.Status & 0x01) == 0x00)
              printf("\n SAM = 1");
            else
              printf("\n SAM = 3");
            printf("\n Baud Rate = %.2fk", CANDO_CLK_FREQ_HIGH / (float)(2 * BRP * (1 + PROPSEG + PHSEG1 + PHSEG2)));
            printf("\n Sample point = %.1f%%\n >", ((float)(1 + PROPSEG + PHSEG1) * 100.0) / (float)(1 + PROPSEG + PHSEG1 + PHSEG2));

            CANdoStatus.NewFlag = CANDO_NO_STATUS;  // Clear flag to show status read
          }
        }
        else
          printf("\n ERROR - CAN Transmit store contents not received!\n >");
      }
      else
        printf("\n ERROR - Unable to read CAN Transmit store!\n >");
    }
    else
      printf("\n ERROR - Unable to flush buffers!\n >");
  }
}
//------------------------------------------------------------------------------
// GetKey
//
// Simple non-blocking get key from STDIN.
//
// Returns -
//    Key pressed
//------------------------------------------------------------------------------
int GetKey(void)
{
  int Character;
  struct termios OriginalTerminalAttr;
  struct termios NewTerminalAttr;

  // Set the terminal to raw mode
  tcgetattr(fileno(stdin), &OriginalTerminalAttr);
  memcpy(&NewTerminalAttr, &OriginalTerminalAttr, sizeof(struct termios));
  NewTerminalAttr.c_lflag &= ~(ECHO | ICANON);
  NewTerminalAttr.c_cc[VTIME] = 0;
  NewTerminalAttr.c_cc[VMIN] = 0;
  tcsetattr(fileno(stdin), TCSANOW, &NewTerminalAttr);

  // Read a character from the STDIN stream without blocking
  Character = fgetc(stdin);

  // Restore the original terminal attributes
  tcsetattr(fileno(stdin), TCSANOW, &OriginalTerminalAttr);

  return Character;
}
//--------------------------------------------------------------------------
// CANdoMenu
//
// Display a menu of options.
//
// Returns -
//    Nothing
//--------------------------------------------------------------------------
void CANdoMenu(void)
{
  printf("\n Press key for option -");
  printf("\n 'v' = Get version");
  printf("\n 'd' = Get device status");
  printf("\n 's' = Get setup status");
  printf("\n 'o' = Write analogue input store");
  printf("\n 'i' = Read analogue input store");
  printf("\n 'e' = Clear analogue input store");
  printf("\n 'w' = Write CAN transmit store");
  printf("\n 'r' = Read CAN transmit store");
  printf("\n 'c' = Clear CAN transmit store");
  printf("\n");
  printf("\n '?' = Display MENU");
  printf("\n 'x' = Exit program\n >");
}
//------------------------------------------------------------------------------
// main
//
// Main starting point for program.
//
// Returns -
//    Nothing
//------------------------------------------------------------------------------
int main()
{
  unsigned char Status, Key = ' ';

  printf("\n\n----------------------------------------------------------");
  printf("\n----------------------------------------------------------");
  printf("\n Example program written in 'C' to demonstrate configuring");
  printf("\n the CANdo AUTO device via the CANdo API - v%.1f", VERSION_NO);
  printf("\n----------------------------------------------------------");
  printf("\n----------------------------------------------------------");
  printf("\n");

  // Dynamically load libCANdo.so
  Status = CANdoInitialise();
  if (Status)
  {
    // libCANdo.so loaded
    CANdoConnect();  // Open a connection to a CANdo device
    if (CANdoUSB.OpenFlag)
      // CANdo conn. open, so display menu
      CANdoMenu();  // Display the options menu
  }
  else
    Key = 'x';

  // Main loop
  while (Key != 'x')
	{
		Key = GetKey();

    // These menu options are only active when a CANdo device is connected
    if (CANdoUSB.OpenFlag)
    {
      // Process Key command
      switch (Key)
      {
        case 'd' :
          // Request device status
          CANdoGetStatus(CANDO_DEVICE_STATUS);
          break;

        case 's' :
          // Request setup status
          CANdoGetStatus(CANDO_SETUP_STATUS);
          break;

        case 'o' :
          // Write config. data for V1 & V2 to analogue I/P store
          CANdoWriteAnalogInputStore();
          break;

        case 'i' :
          // Read config. data for V1 & V2 in analogue I/P store
          CANdoReadAnalogInputStore();
          break;

        case 'e' :
          // Clear analogue I/P store & disable V1 & V2 CAN transmissions
          if (CANdoAnalogStoreClear(&CANdoUSB) == CANDO_SUCCESS)
            printf("\n Analogue I/P store cleared\n >");
          else
          	printf("\n ERROR - Unable to clear Analogue I/P store\n >");
          break;

        case 'w' :
          // Write to CAN transmit store
          CANdoWriteCANTransmitStore();
          break;

        case 'r' :
          // Read from CAN transmit store
          CANdoReadCANTransmitStore();
          break;

        case 'c' :
          // Clear CAN transmit store & disable associated CAN transmissions
          if (CANdoTransmitStoreClear(&CANdoUSB) == CANDO_SUCCESS)
            printf("\n CAN transmit store cleared\n >");
          else
            printf("\n ERROR - Unable to clear CAN transmit store\n >");
          break;

        case 'v' :
					// Display CANdo file versions
					CANdoVersion();
					break;

				case '?' :
					// Display menu
					CANdoMenu();
					break;
      }
    }
  }

  // Close connection to CANdo
  if (CANdoUSB.OpenFlag)
  {
    if (CANdoClose(&CANdoUSB) == CANDO_SUCCESS)
      printf("\n CANdo AUTO closed");
  }

  // Unload library
  CANdoFinalise();

  printf("\n----------------------------------------------------------");
  printf("\n----------------------------------------------------------");
  printf("\n");

  return 0;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
