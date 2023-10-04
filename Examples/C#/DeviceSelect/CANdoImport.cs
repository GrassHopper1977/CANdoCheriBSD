//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// THIS HEADER FILE CONTAINS THE FUNCTION IMPORTS AND CONSTANTS FOR THE
// FUNCTIONS EXPORTED BY THE CANdo API DLL - CANdo.dll v4.1
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  TITLE :- CANdo.dll import wrapper - CANdoImport.cs
//  AUTHOR :- Martyn Brown
//  DATE :- 29/09/14
//
//  DESCRIPTION :- 'C#' class to wrap up the properties & methods exported by
//  CANdo.dll.
//
//  UPDATES :-
//	19/02/06 Created
//  02/08/10 1) Function return values updated
//           2) CANdoGetVersion function added
//  09/01/11 1) InAttribute added to CANdoOpen & CANdoClose prototypes
//           2) CANdoClose prototype return type modified
//           3) CANDO_INVALID_HANDLE return value added
//  21/02/11 1) CANdo H/W type constants added
//           2) CANdoDevice structure added
//           3) CANdoGetDevices function added
//           4) CANdoOpenDevice function added
//           5) CAN_BUFFER_LENGTH renamed to CANDO_CAN_BUFFER_LENGTH
//           6) CANDO_CLOSED return value renamed CANDO_CONNECTION_CLOSED
//  27/07/11 1) CANdoRequestDateStatus function added
//           2) CANdoRequestBusLoadStatus function added
//           3) CANdo status type constants added
//  01/11/11 1) CANdoGetPID function added
//           2) CANdoClearStatus function added
//  27/08/13 1) CANdo AUTO support added
//           2) CANdoRequestSetupStatus function added
//           3) CANdoRequestAnalogInputStatus function added
//           4) CANdoAnalogStoreRead function added
//           5) CANdoAnalogStoreWrite function added
//           6) CANdoAnalogStoreClear function added
//           7) CANdoTransmitStoreRead function added
//           8) CANdoTransmitStoreWrite function added
//           9) CANdoTransmitStoreClear function added
//  05/10/13 StructLayoutAttribute.Pack set to 0 for 32/64 bit compatibility
//
//  LICENSE :-
//  The SDK (Software Development Kit) provided for use with the CANdo device
//  is issued as FREE software, meaning that it is free for personal,
//  educational & commercial use, without restriction or time limit. The
//  software is supplied "as is", with no implied warranties or guarantees.
//
//  (c) 2006-14 Netronics Ltd. All rights reserved.
//------------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace CANdoCSharp
{
  class CANdoImport
  {
    // CANdo constants
    public const byte CLOSED = 0;  // USB channel closed
    public const byte OPEN = 1;  // USB channel open
    public const byte STOP = 0;  // Stop Rx/Tx of CAN messages
    public const byte RUN = 1;  // Start Rx/Tx of CAN messages
    public const byte CLEAR = 0;  // Status message flag clear
    public const byte SET = 1;  // Status message flag set
    public const byte NORMAL_MODE = 0;  // Rx/Tx CAN mode
    public const byte LISTEN_ONLY_MODE = 1;  // Rx only mode, no ACKs
    public const byte LOOPBACK_MODE = 2;  // Tx internally looped back to Rx
    public const uint CANDO_CLK_FREQ = 20000;  // CANdo clk. freq. in kHz for baud rate calc.
    public const uint CANDO_CLK_FREQ_HIGH = 40000;  // CANdoISO & CANdo AUTO clk. freq. in kHz for baud rate calc.
    public const byte CANDO_BRP_ENHANCED_OFFSET = 63;  // BRP enhanced baud rate setting offset, CANdoISO & CANdo AUTO only

    // CANdo AUTO constants
    public const byte CANDO_AUTO_V1_INPUT = 1;  // V1 analogue I/P
    public const byte CANDO_AUTO_V2_INPUT = 2;  // V2 analogue I/P
    public const byte CANDO_AUTO_MAX_NO_OF_TX_ITEMS = 10;  // Max. no. of items in CAN transmit store

    // CAN message constants
    public const byte ID_11_BIT = 0;  // Standard 11 bit ID
    public const byte ID_29_BIT = 1;  // Extended 29 bit ID
    public const byte DATA_FRAME = 0;  // CAN data frame
    public const byte REMOTE_FRAME = 1;  // CAN remote frame

    // CAN receive cyclic buffer size
    public const int CANDO_CAN_BUFFER_LENGTH = 2048;

    // CANdo string type length
    public const int CANDO_STRING_LENGTH = 256;

    // CANdo H/W types
    public const int CANDO_TYPE_ANY = 0x0000;  // Any H/W type
    public const int CANDO_TYPE_CANDO = 0x0001;  // CANdo H/W type
    public const int CANDO_TYPE_CANDOISO = 0x0002;  // CANdoISO H/W type
    public const int CANDO_TYPE_CANDO_AUTO = 0x0003;  // CANdo AUTO H/W type
    public const int CANDO_TYPE_UNKNOWN = 0x8000;  // Unknown H/W type

    // CANdo status types
    public const byte CANDO_NO_STATUS = 0;  // No device status type
    public const byte CANDO_DEVICE_STATUS = 1;  // Device status type
    public const byte CANDO_DATE_STATUS = 2;  // Date status type
    public const byte CANDO_BUS_LOAD_STATUS = 3;  // Bus load status type
    public const byte CANDO_SETUP_STATUS = 4;  // CAN setup status type
    public const byte CANDO_ANALOG_INPUT_STATUS = 5;  // Analogue I/P status type

    // CANdo USB PIDs
    public const string CANDO_PID = "8095";  // CANdo PID
    public const string CANDOISO_PID = "8660";  // CANdoISO PID
    public const string CANDO_AUTO_PID = "889B";  // CANdo AUTO PID

    // Function return values
    public const int CANDO_SUCCESS = 0x0000;  // All OK
    public const int CANDO_USB_DLL_ERROR = 0x0001;  // CANdo USB DLL error
    public const int CANDO_USB_DRIVER_ERROR = 0x0002;  // CANdo USB driver error
    public const int CANDO_NOT_FOUND = 0x0004;  // CANdo not found
    public const int CANDO_IO_FAILED = 0x0008;  // Failed to initialise USB UART parameters
    public const int CANDO_CONNECTION_CLOSED = 0x0010;  // No CANdo channel open
    public const int CANDO_READ_ERROR = 0x0020;  // USB UART read error
    public const int CANDO_WRITE_ERROR = 0x0040;  // USB UART write error
    public const int CANDO_WRITE_INCOMPLETE = 0x0080;  // Not all requested bytes written to CANdo
    public const int CANDO_BUFFER_OVERFLOW = 0x0100;  // Overflow in cyclic buffer
    public const int CANDO_RX_OVERRUN = 0x0200;  // Message received greater than max. message size
    public const int CANDO_RX_TYPE_UNKNOWN = 0x0400;  // Unknown message type received
    public const int CANDO_RX_CRC_ERROR = 0x0800;  // CRC mismatch
    public const int CANDO_RX_DECODE_ERROR = 0x1000;  // Error decoding message
    public const int CANDO_INVALID_HANDLE = 0x2000;  // Invalid device handle
    public const int CANDO_ERROR = 0x8000;  // Non specific error

    // Structure used to store device identification for CANdo
    [StructLayout(LayoutKind.Sequential, Pack = 0, CharSet = CharSet.Ansi)]
    public struct TCANdoDevice
    {
      public int HardwareType;  // H/W type of this CANdo
      [MarshalAs(UnmanagedType.ByValTStr, SizeConst = CANDO_STRING_LENGTH)]
      public string SerialNo;  // USB S/N for this CANdo
    }

    // Structure used to store USB info. for CANdo
    [StructLayout(LayoutKind.Sequential, Pack = 0, CharSet = CharSet.Ansi)]
    public class TCANdoUSB
    {
      public int TotalNo;  // Total no. of CANdo on USB bus
      public int No;  // No. of this CANdo
      public byte OpenFlag;  // USB communications channel state
      [MarshalAs(UnmanagedType.ByValTStr, SizeConst = CANDO_STRING_LENGTH)]
      public string Description;  // USB decriptor string for CANdo
      [MarshalAs(UnmanagedType.ByValTStr, SizeConst = CANDO_STRING_LENGTH)]
      public string SerialNo;  // USB S/N for this CANdo
      public IntPtr Handle;  // Handle to connected CANdo
    }

    // Structure used to store a CAN message
    [StructLayout(LayoutKind.Sequential, Pack = 0)]
    public struct TCANdoCAN
    {
      public const byte CAN_DATA_LENGTH = 8;
      public byte IDE;
      public byte RTR;
      public uint ID;
      public byte DLC;
      [MarshalAs(UnmanagedType.ByValArray, SizeConst = CAN_DATA_LENGTH)]
      public byte[] Data;
      public byte BusState;
      public uint TimeStamp;
    }

    // Structure used to store cyclic buffer control parameters for CAN messages received from CANdo
    [StructLayout(LayoutKind.Sequential, Pack = 0)]
    public struct TCANdoCANBufferControl
    {
      public int WriteIndex;
      public int ReadIndex;
      public byte FullFlag;
    }

    // Structure used to store status information received from CANdo
    [StructLayout(LayoutKind.Sequential, Pack = 0)]
    public class TCANdoStatus
    {
      public byte HardwareVersion;
      public byte SoftwareVersion;
      public byte Status;
      public byte BusState;
      public int TimeStamp;
      public byte NewFlag;
    }

    // Functions imported from 'CANdo.dll'
    [DllImportAttribute("CANdo.dll", CharSet = CharSet.Ansi)]
    public static extern int CANdoGetPID(
      int CANdoNo,
			StringBuilder PID);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoGetDevices(
      [InAttribute, OutAttribute, MarshalAs(UnmanagedType.LPArray)]
			TCANdoDevice[] CANdoDevicesPointer,
      ref uint NoOfDevicesPointer);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoOpen(
      [InAttribute, OutAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoOpenDevice(
      [InAttribute, OutAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer,
      ref TCANdoDevice CANdoDevicePointer);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoClose(
      [InAttribute, OutAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoFlushBuffers(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoSetBaudRate(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer,
      byte SJW, byte BRP, byte PHSEG1, byte PHSEG2, byte PROPSEG, byte SAM);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoSetMode(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer,
      byte Mode);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoSetFilters(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer,
      uint Rx1Mask,
      byte Rx1IDE1, uint Rx1Filter1,
      byte Rx1IDE2, uint Rx1Filter2,
      uint Rx2Mask,
      byte Rx2IDE1, uint Rx2Filter1,
      byte Rx2IDE2, uint Rx2Filter2,
      byte Rx2IDE3, uint Rx2Filter3,
      byte Rx2IDE4, uint Rx2Filter4);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoSetState(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer,
      byte State);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoReceive(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer,
      IntPtr CANdoCANBufferPointer,
      [OutAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoStatus CANdoStatusPointer);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoTransmit(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer,
      byte IDExtended, uint ID, byte RTR, byte DLC,
      [InAttribute, MarshalAs(UnmanagedType.LPArray, SizeConst = 8)]
			byte[] Data,
      byte BufferNo, byte RepeatTime);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoRequestStatus(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoRequestDateStatus(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoRequestBusLoadStatus(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoRequestSetupStatus(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoRequestAnalogInputStatus(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoClearStatus(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer);

    [DllImportAttribute("CANdo.dll")]
    public static extern void CANdoGetVersion(
      out uint APIVersion,
      out uint DLLVersion,
      out uint DriverVersion);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoAnalogStoreRead(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoAnalogStoreWrite(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer,
      byte InputNo, byte IDExtended, uint ID,
      byte Start, byte Length, double ScalingFactor, double Offset,
      byte Padding, byte RepeatTime);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoAnalogStoreClear(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoTransmitStoreRead(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoTransmitStoreWrite(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer,
      byte IDExtended, uint ID, byte RTR, byte DLC,
      [InAttribute, MarshalAs(UnmanagedType.LPArray, SizeConst = 8)]
			byte[] Data,
      byte RepeatTime);

    [DllImportAttribute("CANdo.dll")]
    public static extern int CANdoTransmitStoreClear(
      [InAttribute, MarshalAs(UnmanagedType.LPStruct)]
			TCANdoUSB CANdoUSBPointer);
  }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
