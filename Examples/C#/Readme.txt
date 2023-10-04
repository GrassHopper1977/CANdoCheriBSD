~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Netronics CANdo Linux SDK

Check the website www.cananalyser.co.uk for the latest version.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This folder contains a number of Visual C# projects written &
compiled using the Visual Studio Express 2012 for Windows Desktop
IDE. These projects may also be opened & compiled with MonoDevelop
on Linux without any modifications. The function calls to the CANdo
API CANdo.dll within the CANdoImport.cs import file are
automatically redirected by the MonoDevelop compiler/linker to the
shared library libCANdo.so on Linux. The projects demonstrate
controlling & communicating with the CANdo family of devices via
the CANdo API within the libCANdo.so shared library.

Note - The visual GUI design capability is only available for
these projects within the Visual Studio IDE on Windows, except for
the example in the /GTK folder which uses the Stetic GUI designer
within MonoDevelop. However, all the projects maybe opened &
compiled within MonoDevelop.

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

4) The project in the /GTK folder, was developed within
MonoDevelop using the Stetic GUI visual designer & uses the GTK.

CANdoImport.cs is the C# class wrapper, that contains all the
properties & methods necessary to interface to the CANdo API
shared library, libCANdo.so.

The programs were all compiled & tested using MonoDevelop v4.0.12.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Revision History
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

v1.0 - 03/06/14
v1.1 - 15/12/14
    1) SDK examples modified to load libCANdo.so dynamically
