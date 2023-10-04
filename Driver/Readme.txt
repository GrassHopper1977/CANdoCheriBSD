~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Netronics CANdo Linux SDK

Check the website www.cananalyser.co.uk for the latest version.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This folder contains the source code for the CANdo driver & API
shared library, libCANdo.so. The API functions within this library
allow full control over the CANdo device. The API functions are
described within the document 'CANdoProgrammersGuide.pdf'.

The compilation requires the libusb-1.0 development library,
libusb-1.0.0-dev. On Ubuntu, this maybe installed by typing the
following command in a terminal window -

sudo apt-get install libusb-1.0.0-dev

To compile the source code, navigate to the /Source folder & in a
terminal window type the following command -

make

The compiled shared library libCANdo.so must be copied to a system
library folder such as /usr/lib/. To copy the library, open a
terminal window & type the following command -

sudo cp libCANdo.so /usr/lib/

It may also be necessary to set the permissions of the library to
executable for all users by typing the following command -

sudo chmod +x /usr/lib/libCANdo.so

Note - The libCANdo.so library communicates with the CANdo device
using the libusb-1.0 USB driver library. The libusb-1.0 library
must be installed in order for the CANdo API to work. Most recent
Linux distributions include this library by default.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Revision History
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

v1.0 - 03/06/14
v1.1 - 12/07/14
    1) Updated with Raspbian binary
    2) Source code & compilation instructions added
v2.0 - 16/09/14
    1) Bulk reads changed to asynchronous for improved performance
    2) CANdoVersion(...) function modified to work with older
       versions of libusb-1.0
    3) Binaries removed
