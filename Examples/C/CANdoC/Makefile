#----------------------------------------------------------------
#
# Netronics CANdo Linux CANdoC make file
#
# Date - 15/09/14
#
#----------------------------------------------------------------
CFLAGS=-Wall
LFLAGS=-ldl
PURECAP = -mabi=purecap
HYBRID = -mabi=aapcs

CANdoC : CANdoC.c CANdoC.h CANdoImport.h
	cc $(PURECAP) $(CFLAGS) CANdoC.c $(LFLAGS) -oCANdoC
	cc $(HYBRID) $(CFLAGS) CANdoC.c $(LFLAGS) -oCANdoC_hy

#----------------------------------------------------------------
