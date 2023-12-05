# CANdoFreeBSD
Our attempt to convert the CANdo Linux driver to CheriBSD.

The original code is from here (https://www.cananalyser.co.uk/download.html) and is provided entirely for free. Our code is also provided entirely for free.

This appears to be working. We have nocticed that, sometimes, if you dynamically load the SO it sometimes gets confused if you have another function with the same name in your code, this even applies to functions that are internal to the SO which can be a little frustrating.

We have only converted the main driver and one of tests. The Makefiles have been converted to produce both teh Pure Caps and Hybrid versions of the SO file and to copy them into the correct places in the OS.
