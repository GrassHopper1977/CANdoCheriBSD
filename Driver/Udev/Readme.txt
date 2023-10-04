~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Netronics CANdo Linux SDK

Check the website www.cananalyser.co.uk for the latest version.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The cando.rules file is a Udev rule file that makes sure that full
read/write access is granted for any CANdo device plugged in, for
all users. This is necessary in order to communicate with the CANdo
devices via the shared library libCANdo.so provided within the SDK.

The cando.rules file must be copied to the Udev directory in order
to be read by the udev sub-system.

In a terminal window type -

sudo cp cando.rules /etc/udev/rules.d/

Note - Remove & then re-insert the CANdo device for the new
settings to take effect.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Revision History
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

v1.0 - 03/06/14
