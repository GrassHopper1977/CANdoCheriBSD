~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Netronics CANdo Linux SDK

Check the website www.cananalyser.co.uk for the latest version.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This folder contains a number of Pascal projects written & compiled
using the Lazarus v1.2.4 IDE. The projects demonstrate controlling
& communicating with the CANdo family of devices via the CANdo API
within the libCANdo.so shared library.

The projects may be compiled as either 32 or 64 bit executables.
To run a 32 bit executable, the 32 bit variant of the libCANdo.so
shared library must be present. Similarily, the 64 bit variant of
the shared library must be present to run a 64 bit executable.

1) The project in the /AutoConnect folder, automatically locates &
connects to the first CANdo device found on the PC, on start up.

2) The project in the /DeviceSelect folder, creates a list of all
CANdo devices connected to the PC & allows an individual CANdo
device to be selected from the list.

3) The project in the /CANdoAUTO folder, is specific to the CANdo
AUTO device & demonstrates configuration of the non-volatile stores
within the device for standalone operation.

CANdoImport.pas is the Pascal import unit, containing all function
definitions & constants necessary to interface to the CANdo API
shared library, libCANdo.so.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Revision History
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

v1.0 - 03/06/14
v1.1 - 15/12/14
    1) SDK examples modified to load libCANdo.so dynamically
