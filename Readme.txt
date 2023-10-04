~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Netronics CANdo Linux SDK

Check the website www.cananalyser.co.uk for the latest version.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Netronics CANdo Linux SDK is designed to allow the CANdo family
of devices to be easily integrated into any Linux based application.

The SDK consists of a shared library for interfacing to the CANdo
devices & example programs written in C, C# & Pascal that use the
library. The shared library is located in the /Driver/Source folder
& the example programs are located in the /Examples folder.

The 'CANdoProgrammersGuide.pdf' describes the CANdo API within the
shared library & is located in the /Documents folder.

The SDK has been tested under -
Ubuntu 12.04 32 bit (Intel x86) 
Ubuntu 14.04 64 bit (Intel x86)
openSUSE 13.1 64 bit (Intel x86)
openSUSE 13.1 32 bit (Intel x86)
Linux Mint 16 32 bit (Intel x86)
Raspbian 09/14 32 bit (ARM11)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Revision History
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

v1.0 - 03/06/14
v1.1 - 12/07/14
    1) Segmentation fault corrected for use with older versions
    of libusb-1.0
    2) Source code included for library - libCANdo.so
    3) Library & 'C' examples compiled & tested on Raspberry Pi
v2.0 - 16/09/14
    1) Read performance of shared library improved
    2) CANdoGetVersion(...) function fixed to work with older
       versions of the libusb-1.0 library
v2.1 - 15/12/14
    1) SDK examples modified to load libCANdo.so dynamically
