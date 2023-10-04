~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Netronics CANdo Linux SDK

Check the website www.cananalyser.co.uk for the latest version.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This folder contains the source code for a small program that will
detect CANdo devices plugged into the PC & display their path
relative to the /dev/ folder, together with their owner & access
permissions.

The compilation requires the libusb-1.0 development library,
libusb-1.0.0-dev. On Ubuntu, this maybe installed by typing the
following command in a terminal window -

sudo apt-get install libusb-1.0.0-dev

To compile the source code, navigate to the ./Source folder & in a
terminal window type the following command -

make

To run the program, type the following command -

./CANdoDetect

In order to communicate with a CANdo device via the CANdo driver,
the user must have read & write access permission. If this program
indicates that this is not the case, then the Udev rules contained
within the /Driver/Udev directory must be copied to the correct
Udev rules folder on the PC.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Revision History
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

v1.0 - 03/06/14
v1.1 - 12/07/14
    1) Updated with Raspbian binary
    2) Source code & compilation instructions added
v2.0 - 16/09/14
    1) Binaries removed
