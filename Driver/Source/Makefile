#----------------------------------------------------------------
#
# Netronics CANdo Linux shared library make file
#
# Date - 02/09/14
#
#----------------------------------------------------------------
CFLAGS=-Wall -fPIC -shared
LFLAGS=-lusb-1.0 -lm -lpthread

libCANdo.so : CANdoAPI.c CRC.c IEEE754.c SiUSBXp.c CANdoAPI.h CRC.h IEEE754.h SiUSBXp.h
	gcc $(CFLAGS) CANdoAPI.c CRC.c IEEE754.c SiUSBXp.c $(LFLAGS) -olibCANdo.so

#----------------------------------------------------------------
