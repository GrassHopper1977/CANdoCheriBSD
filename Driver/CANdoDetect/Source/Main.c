//------------------------------------------------------------------------------
//  TITLE :- CANdo device detect - Main.c
//  AUTHOR :- Martyn Brown
//  DATE :- 07/07/14
//
//  DESCRIPTION :- Utility program to detect CANdo devices & print their file
//  permissions.
//
//  UPDATES :-
//  26/05/14 Created
//  07/07/14 NULL replaced with Context in relevant libusb function calls
//
//  (c) 2014 Netronics Ltd. All rights reserved.
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libusb-1.0/libusb.h>

// Flag constants
#define FALSE 0
#define TRUE 1
// CANdo USB IDs
#define SI_VID 0x10C4
#define SI_CANDO_PID 0x8095
#define SI_CANDOISO_PID 0x8660
#define SI_CANDO_AUTO_PID 0x889B

// Globals
libusb_context * Context = NULL; // Context for libusb in application
char DevicePath[256];  // Device path for CANdo device
//------------------------------------------------------------------------------
// IsDeviceCANdoInterface
//
// Checks to see if USB device is a CANdo I/F based on VID & PID.
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
//------------------------------------------------------------------------------
// GetDevicePath
//
// Get the path to the specified CANdo device.
//
// Returns -
//    FALSE = Error
//		TRUE = Ok
//------------------------------------------------------------------------------
int GetDevicePath(unsigned int DeviceNum)
{
  int Status = FALSE;
	ssize_t DevNo, NoOfDevs, CANdoNo = -1;
  libusb_device ** List, * Device = NULL;
  unsigned int BusNo, PortNo;

  strcpy(DevicePath, "");

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

  if ((CANdoNo == DeviceNum) && (Device != NULL))
  {
  	BusNo = libusb_get_bus_number(Device);
  	PortNo = libusb_get_device_address(Device);

  	sprintf(DevicePath, "/dev/bus/usb/%.3d/%.3d", BusNo, PortNo);
  	Status = TRUE;
  }

  // Free list of USB devices
  libusb_free_device_list(List, 1);

  return Status;
}
//------------------------------------------------------------------------------
// DecodePermissions
//
// Decode hex. encoded file permissions.
//
// Returns -
//    Permissions as a string
//------------------------------------------------------------------------------
char * DecodePermissions(unsigned int Permissions)
{
	static char Decode[256];

	strcpy(Decode, "");

	// Read
	if (Permissions & 0x4)
	{
		strcat(Decode, "Read");

		if (Permissions & 0x3)
			strcat(Decode, "/");
	}

	// Write
	if (Permissions & 0x2)
	{
		strcat(Decode, "Write");

		if (Permissions & 0x1)
			strcat(Decode, "/");
	}

	// Execute
	if (Permissions & 0x1)
		strcat(Decode, "Execute");

	return Decode;
}
//------------------------------------------------------------------------------
// SI_Initialise
//
// Initialise libusb.
//
// Returns -
//    FALSE = Error
//		TRUE = Ok
//------------------------------------------------------------------------------
int SI_Initialise(void)
{
  // Init. libusb
  if (libusb_init(&Context) == LIBUSB_SUCCESS)
    return TRUE;
  else
    return FALSE;
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
//    FALSE = Error
//		TRUE = Ok
//------------------------------------------------------------------------------
int SI_GetNumDevices(unsigned int * NumDevices)
{
  int Status = FALSE;
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
    Status = TRUE;
  }

  libusb_free_device_list(List, 1);

  return Status;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int main (void)
{
  unsigned int NoOfDevices, DevNo, Permissions;
  struct stat Buffer;

  printf("-------------------------------------------\n");
  printf("      CANdo Device Detection Utility\n");
  printf("-------------------------------------------\n");

  if (SI_Initialise())
  {
    if (SI_GetNumDevices(&NoOfDevices))
    {
      printf("%d CANdo device(s) found\n\n", NoOfDevices);

      // Iterate through all the CANdo devices & print their access details
      for (DevNo = 0; DevNo < NoOfDevices; DevNo++)
      {
        if (GetDevicePath(DevNo))
        {
          // Path for device found, so get file permissions
          if (!stat(DevicePath, &Buffer))
          {
            // Device ownership
            printf("CANdo device %d -\n     Path = %s\n     Ownership -\n", DevNo + 1, DevicePath);
            if ((long)Buffer.st_uid == 0)
              printf("          Owner = root\n");
            else
              printf("          Owner = %ld\n", (long)Buffer.st_uid);
            if ((long)Buffer.st_gid == 0)
              printf("          Group = root\n");
            else
              printf("          Group = %ld\n", (long)Buffer.st_gid);

            // Device permissions
            Permissions = (unsigned int)Buffer.st_mode;
            printf("     Permissions -\n");
            printf("          Owner = %s\n", DecodePermissions(Permissions % 512));
            printf("          Group = %s\n", DecodePermissions(Permissions % 64));
            printf("          World = %s\n\n", DecodePermissions(Permissions % 8));
          }
          else
            printf("CANdo device %d - No information available\n", DevNo);
        }
        else
          printf("CANdo device %d - No found\n", DevNo);
      }
      if (DevNo > 0)
        printf("Note : Read/Write permissions are required\nto communicate with the CANdo device via the\nlibCANdo.so shared library.\n\n");
    }
    else
      printf("No CANdo devices found\n");
  }
  else
    printf("Failed to initialise libusb-1.0\n");

  SI_Finalise();
  return 0;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
