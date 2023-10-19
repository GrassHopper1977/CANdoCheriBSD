#----------------------------------------------------------------
#
# Netronics CANdo Linux shared library make file
#
# Date - 02/09/14
#
#----------------------------------------------------------------
CFLAGS=-g -Wall -cheri-bounds=subobject-safe -fPIC -shared
LFLAGS=-lusb -lm -lpthread
PURECAP = -mabi=purecap
HYBRID = -mabi=aapcs


libCANdo.so : CANdoAPI.c CRC.c IEEE754.c SiUSBXp.c CANdoAPI.h CRC.h IEEE754.h SiUSBXp.h
	cc $(CFLAGS) $(PURECAP) CANdoAPI.c CRC.c IEEE754.c SiUSBXp.c $(LFLAGS) -olibCANdo.so
	cp libCANdo.so /usr/lib
	cc $(CFLAGS) $(HYBRID) CANdoAPI.c CRC.c IEEE754.c SiUSBXp.c $(LFLAGS) -olibCANdohy.so
	cp libCANdohy.so /usr/lib64/libCANdo.so

#----------------------------------------------------------------
