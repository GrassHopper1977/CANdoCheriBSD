~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Netronics CANdo Linux SDK

Check the website www.cananalyser.co.uk for the latest version.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This folder contains a couple of small example programs written in
C. These programs demonstrate communicating with the CANdo devices
via the CANdo API contained within the shared library libCANdo.so.
The libCANdo.so shared library must be installed in a system
library folder in order to execute these programs.

1) The program in the /CANdoC folder, connects to the 1st CANdo
device it finds & allows reception & transmission of CAN messages
from a terminal window.

To compile the program, navigate to the source code & in a terminal
window type the following command -

make

To run the program, type the following command -

./CANdoC

2) The project in the /CANdoAUTOC folder, is specific to the CANdo
AUTO Module & demonstrates configuration of the non-volatile stores
within the device for standalone operation.

To compile the program, navigate to the source code & in a terminal
window type the following command -

make

To run the program, type the following command -

./CANdoAUTOC

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Revision History
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

v1.0 - 03/06/14
v1.1 - 12/07/14
    1) Updated with Raspbian binary
v2.0 - 16/09/14
    1) GetKey(...) function added 
    2) Binaries removed
v2.1 - 15/12/14
    1) SDK examples modified to load libCANdo.so dynamically
