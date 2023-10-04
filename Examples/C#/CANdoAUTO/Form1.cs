//------------------------------------------------------------------------------
//  TITLE :- CANdo AUTO Configuration example main form - Form1.cs
//  AUTHOR :- Martyn Brown
//  DATE :- 07/10/13
//
//  DESCRIPTION :- 'C#' example program to demonstrate configuring the CANdo
//  AUTO device via the CANdo API.
//
//  UPDATES :-
//	07/10/13 Created
//
//  LICENSE :-
//  The SDK (Software Development Kit) provided for use with the CANdo device
//  is issued as FREE software, meaning that it is free for personal,
//  educational & commercial use, without restriction or time limit. The
//  software is supplied "as is", with no implied warranties or guarantees.
//
//  (c) 2013 Netronics Ltd. All rights reserved.
//------------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace CANdoCSharp
{
  public partial class Form1 : Form
  {
    // CAN transmit repeat time
    const byte REPEAT_TIME_OFF = 0;
    const byte REPEAT_TIME_10MS = 1;
    const byte REPEAT_TIME_20MS = 2;
    const byte REPEAT_TIME_50MS = 3;
    const byte REPEAT_TIME_100MS = 4;
    const byte REPEAT_TIME_200MS = 5;
    const byte REPEAT_TIME_500MS = 6;
    const byte REPEAT_TIME_1000MS = 7;
    const byte REPEAT_TIME_2000MS = 8;
    const byte REPEAT_TIME_5000MS = 9;
    const byte REPEAT_TIME_10000MS = 10;
    readonly string[] REPEAT_TIME = { ">>>> OFF - NO TRANSMISSION <<<<", "10ms", "20ms", "50ms", "100ms", "200ms", "500ms", "1000ms", "2000ms", "5000ms", "10000ms" };
    // CANdo bit or'ed internal status codes
	  const byte CANDO_OK = 0x00;
	  const byte CANDO_UART_RX_OVERRUN = 0x01;
	  const byte CANDO_UART_RX_CORRUPTED = 0x02;
    const byte CANDO_UART_RX_CRC_ERROR = 0x04;
	  const byte CANDO_CAN_RX_NO_DATA = 0x08;
    const byte CANDO_CAN_RX_OVERRUN = 0x10;
	  const byte CANDO_CAN_RX_INVALID = 0x20;
    const byte CANDO_CAN_TX_OVERRUN = 0x40;
	  const byte CANDO_CAN_BUS_ERROR = 0x80;

    CANdoImport.TCANdoUSB CANdoUSB;  // Store for parameters relating to connected CANdo
    CANdoImport.TCANdoStatus CANdoStatus;  // Store for status message collected from CANdo
    Type CANDataType, CANBufferControlType;
    int CANDataSize, CANBufferControlSize, CANBufferSize;
    IntPtr CANBufferPointer, CANDataPointer, CANBufferControlPointer;
    CANdoImport.TCANdoCAN CANData;
    CANdoImport.TCANdoCAN[] CANDataBuffer;  // Cyclic buffer store for CAN receive messages
    CANdoImport.TCANdoCANBufferControl CANBufferControl;  // Cyclic buffer control

    //------------------------------------------------------------------------------
    public Form1()
    {
      InitializeComponent();

      // Create a store for parameters relating to connected CANdo
      CANdoUSB = new CANdoImport.TCANdoUSB();
      CANdoUSB.OpenFlag = CANdoImport.CLOSED;

      // Create a store for CAN receive messages
      CANDataBuffer = new CANdoImport.TCANdoCAN[CANdoImport.CANDO_CAN_BUFFER_LENGTH];  // Cyclic buffer data
      CANBufferControl = new CANdoImport.TCANdoCANBufferControl();  // Cyclic buffer control

      CANDataType = typeof(CANdoImport.TCANdoCAN);
      CANDataSize = Marshal.SizeOf(CANDataType);

      CANBufferControlType = typeof(CANdoImport.TCANdoCANBufferControl);
      CANBufferControlSize = Marshal.SizeOf(CANBufferControlType);

      CANBufferSize = CANDataBuffer.Length * CANDataSize + CANBufferControlSize;
      CANBufferPointer = Marshal.AllocHGlobal(CANBufferSize);  // Allocate unmanaged memory for the CAN receive message cyclic buffer
      CANBufferControlPointer = CANBufferPointer + CANDataBuffer.Length * CANDataSize;  // Get pointer to control parameters at end of allocated block of memory for CAN receive message cyclic buffer
      CANBufferControl.WriteIndex = 0;  // Reset cyclic buffer
      CANBufferControl.ReadIndex = 0;
      CANBufferControl.FullFlag = 0;
      Marshal.StructureToPtr(CANBufferControl, CANBufferControlPointer, true);  // Update unmanaged memory

      // Create a store for the status message
      CANdoStatus = new CANdoImport.TCANdoStatus();

      // Connect to CANdo AUTO device
      ConnectToCANdo();
    }

    //------------------------------------------------------------------------------
    private int ConnectToCANdo()
    {
      int Status;
      CANdoImport.TCANdoDevice CANdoDevice;  // CANdo device type      

      try
      {
        // Open connection to a CANdo AUTO device
        CANdoDevice.HardwareType = CANdoImport.CANDO_TYPE_CANDO_AUTO;  // Select CANdo AUTO type only
        CANdoDevice.SerialNo = "";  // Not bothered about the S/N
        Status = CANdoImport.CANdoOpenDevice(CANdoUSB, ref CANdoDevice);
        if (Status == CANdoImport.CANDO_SUCCESS)
        {
          // Connection open, so get status
          if (CANdoImport.CANdoRequestStatus(CANdoUSB) == CANdoImport.CANDO_SUCCESS)
          {
            Thread.Sleep(5);  // Wait 5ms to allow CANdo AUTO to respond

            // Check for a new status message
            if (CANdoImport.CANdoReceive(CANdoUSB, CANBufferPointer, CANdoStatus) == CANdoImport.CANDO_SUCCESS)
            {
              if (CANdoStatus.NewFlag == CANdoImport.CANDO_DEVICE_STATUS)
              {
                // New status message received, so display
                StatusBar1.Text = CANdoUSB.Description.ToString() + "   S/N " + CANdoUSB.SerialNo.ToString() +
                  "   H/W v" + (CANdoStatus.HardwareVersion / 10).ToString() + "." + (CANdoStatus.HardwareVersion % 10).ToString() +
                  "   S/W v" + (CANdoStatus.SoftwareVersion / 10).ToString() + "." + (CANdoStatus.SoftwareVersion % 10).ToString() +
                  "   Status " + DecodeStatus(CANdoStatus.Status);
                CANdoStatus.NewFlag = CANdoImport.CANDO_NO_STATUS;  // Clear flag to indicate message read
                // Stop all transmissions to allow configuration
                CANdoImport.CANdoSetState(CANdoUSB, CANdoImport.STOP);
                // Enable buttons
                button1.Enabled = true;
                button2.Enabled = true;
                button3.Enabled = true;
                button4.Enabled = true;
                button5.Enabled = true;
                button6.Enabled = true;
                // Clear info. window
                textBox1.Clear();
              }
            }
          }
          else
            StatusBar1.Text = CANdoUSB.Description.ToString() + "   S/N " + CANdoUSB.SerialNo.ToString();
        }
        else
        if (Status == CANdoImport.CANDO_USB_DLL_ERROR)
        {
          StatusBar1.Text = "Netronics CANdo USB DLL not found";
          CANdoImport.CANdoClose(CANdoUSB);  // Unload CANdo.dll
        }
        else
        if (Status == CANdoImport.CANDO_USB_DRIVER_ERROR)
        {
          StatusBar1.Text = "Netronics CANdo AUTO driver not found";
          CANdoImport.CANdoClose(CANdoUSB);  // Unload CANdo.dll
        }
        else
        {
          StatusBar1.Text = "Netronics CANdo AUTO not found";
          CANdoImport.CANdoClose(CANdoUSB);  // Unload CANdo.dll
        }
      }
      catch (DllNotFoundException)
      {
        StatusBar1.Text = "CANdo.dll not found";
        Status = CANdoImport.CANDO_ERROR;
      }
      catch (EntryPointNotFoundException)
      {
        StatusBar1.Text = "CANdo.dll incorrect version";
        Status = CANdoImport.CANDO_ERROR;
      }

      return Status;
    }

    //------------------------------------------------------------------------------
    private string DecodeStatus(int StatusCode)
    {
      switch (StatusCode)
      {
        case CANDO_OK : return "OK";
        case CANDO_UART_RX_OVERRUN : return "USB RX OVERRUN";
        case CANDO_UART_RX_CORRUPTED : return "USB RX CORRUPTED";
        case CANDO_UART_RX_CRC_ERROR : return "USB CRC ERROR";
        case CANDO_CAN_RX_NO_DATA : return "CAN RX NO DATA";
        case CANDO_CAN_RX_OVERRUN : return "CAN RX OVERRUN";
        case CANDO_CAN_RX_INVALID : return "CAN RX INVALID";
        case CANDO_CAN_TX_OVERRUN : return "CAN TX OVERRUN";
        case CANDO_CAN_BUS_ERROR : return "CAN BUS ERROR";
        default :
          // Multiple status codes present, so return as bit coded hex.
          return String.Format("{0, 2:X2}", StatusCode);
      }
    }

    //------------------------------------------------------------------------------
    private void exitToolStripMenuItem_Click(object sender, EventArgs e)
    {
      Close();
    }

    //------------------------------------------------------------------------------
    private void Form1_FormClosing(object sender, FormClosingEventArgs e)
    {
      // Close conection to CANdo if open & unload CANdo.dll
      if (CANdoUSB.OpenFlag == CANdoImport.OPEN)
        CANdoImport.CANdoClose(CANdoUSB);  // Close USB connection

      // Free the unmanaged memory
      Marshal.FreeHGlobal(CANBufferPointer);
      CANBufferPointer = IntPtr.Zero;
    }

    //------------------------------------------------------------------------------
    private void clearToolStripMenuItem_Click(object sender, EventArgs e)
    {
      textBox1.Clear();
    }

    //------------------------------------------------------------------------------
    private void button1_Click(object sender, EventArgs e)
    {
      WriteAnalogInputStore();
    }

    //--------------------------------------------------------------------------
    // WriteAnalogInputStore
    //
    // This procedure configures the 2 analogue input channels within the CANdo
    // AUTO device by writing to the non-volatile analogue input store. The CAN
    // baud rate for the device is also set to 250k bits/s.
    //
    // The 'V1 Input' is configured to accept a voltage in the range 0 to 10V &
    // generate a corresponding J1939 EEC1 CAN message with the engine speed
    // in rpm. The transfer function is 1 rpm per mV, eg. 3V applied to the
    // input results in a value of 3000rpm in the transmitted CAN message.
    //
    // The 'V2 Input' is configured to measure the voltage O/P of a linear
    // temperature sensor & generate a corresponding J1939 ET1 CAN message with
    // the engine coolant temperature in deg. C. The transfer function is
    // 1 deg. C per 100mV, eg. 1V applied to the input results in a value of
    // 10 deg. C in the transmitted CAN message.
    //
    // Returns
    //    Nothing
    //--------------------------------------------------------------------------
    private void WriteAnalogInputStore()
    {
      const uint EEC1 = 0x18F00400;  // J1939 EEC1 ID, source address = 0
      const uint ET1 = 0x18FEEE00;  // J1939 ET1 ID, source address = 0
      const byte PADDING = 0xFF;  // J1939 padding value

      if (CANdoUSB.OpenFlag == CANdoImport.OPEN)
      {
        if (CANdoImport.CANdoAnalogStoreClear(CANdoUSB) == CANdoImport.CANDO_SUCCESS)
        {
          Thread.Sleep(50);  // Wait 50ms to allow CANdo AUTO to clear analog store

          // Configure V1 Input
          if (CANdoImport.CANdoAnalogStoreWrite(CANdoUSB, CANdoImport.CANDO_AUTO_V1_INPUT, CANdoImport.ID_29_BIT, EEC1,
            4, 2, 8.0f, 0.0f, PADDING, REPEAT_TIME_500MS) == CANdoImport.CANDO_SUCCESS)
            textBox1.AppendText("\r\n\r\nV1 Input configured ID = 0x18F00400 with 500ms period");
          else
            textBox1.AppendText("\r\n\r\nERROR - V1 Input not configured!");
          Thread.Sleep(100);  // Wait 100ms to allow CANdo AUTO to store settings

          // Configure V2 Input
          if (CANdoImport.CANdoAnalogStoreWrite(CANdoUSB, CANdoImport.CANDO_AUTO_V2_INPUT, CANdoImport.ID_29_BIT, ET1,
            1, 1, 0.01f, 40.0f, PADDING, REPEAT_TIME_1000MS) == CANdoImport.CANDO_SUCCESS)
            textBox1.AppendText("\r\n\r\nV2 Input configured ID = 0x18FEEE00 with 1s period");
          else
            textBox1.AppendText("\r\n\r\nERROR - V2 Input not configured!");
          Thread.Sleep(100);  // Wait 100ms to allow CANdo AUTO to store setttings

          // Configure CAN baud rate
          if (CANdoImport.CANdoSetBaudRate(CANdoUSB, 0, 4 + CANdoImport.CANDO_BRP_ENHANCED_OFFSET, 7, 5, 4, 0) == CANdoImport.CANDO_SUCCESS)
            textBox1.AppendText("\r\n\r\nCAN bus baud rate set to 250k bits/s");
          else
            textBox1.AppendText("\r\n\r\nERROR - Unable to set CAN baud rate!");

          textBox1.AppendText("\r\n\r\nCycle power to CANdo AUTO device for settings to take effect");
        }
      }
    }

    //--------------------------------------------------------------------------
    private void button2_Click(object sender, EventArgs e)
    {
      ReadAnalogInputStore();
    }

    //--------------------------------------------------------------------------
    // ReadAnalogInputStore
    //
    // This function reads the configuration contained within the analogue input
    // store & displays the settings. This can be useful to verify the contents
    // of the store following a write.
    //
    // Returns
    //    Nothing
    //--------------------------------------------------------------------------
    private void ReadAnalogInputStore()
    {
      int InputNo, FloatValue;
      byte BRP, PROPSEG, PHSEG1, PHSEG2;

      if (CANdoUSB.OpenFlag == CANdoImport.OPEN)
      {
        // Clear receive buffer ready to read analog store config.
        if (CANdoImport.CANdoFlushBuffers(CANdoUSB) == CANdoImport.CANDO_SUCCESS)
        {
          // Clear CAN cyclic buffer
          CANBufferControl.ReadIndex = CANBufferControl.WriteIndex;
          CANBufferControl.FullFlag = CANdoImport.CLEAR;
          Marshal.StructureToPtr(CANBufferControl, CANBufferControlPointer, true);  // Update unmanaged memory
          // Issue analog store read & setup status commands
          if ((CANdoImport.CANdoAnalogStoreRead(CANdoUSB) == CANdoImport.CANDO_SUCCESS) &&
            (CANdoImport.CANdoRequestSetupStatus(CANdoUSB) == CANdoImport.CANDO_SUCCESS))
          {
            Thread.Sleep(50);  // Wait 50ms for CANdo AUTO to reply
            InputNo = CANdoImport.CANDO_AUTO_V1_INPUT;
            CANdoStatus.NewFlag = CANdoImport.CANDO_NO_STATUS;
            // Collect analog store configuration sent by CANdo AUTO
            if (CANdoImport.CANdoReceive(CANdoUSB, CANBufferPointer, CANdoStatus) == CANdoImport.CANDO_SUCCESS)
            {
              // Get the TCANdoCANBufferControl structure populated by the CANdo.dll CANdoReceive function
              CANBufferControl = (CANdoImport.TCANdoCANBufferControl)Marshal.PtrToStructure(CANBufferControlPointer, CANBufferControlType);
              while ((CANBufferControl.ReadIndex != CANBufferControl.WriteIndex) || (CANBufferControl.FullFlag == CANdoImport.SET))
              {
                // Get the TCANdoCAN structure for this CAN message populated by the CANdo.dll CANdoReceive function
                CANDataPointer = (IntPtr)(CANBufferPointer + CANBufferControl.ReadIndex * CANDataSize);
                CANData = (CANdoImport.TCANdoCAN)Marshal.PtrToStructure(CANDataPointer, CANDataType);
                // Display I/P config.
                textBox1.AppendText("\r\n\r\nV" + InputNo.ToString() + " Analog I/P configuration -");
                // ID
                if (CANData.IDE == CANdoImport.ID_11_BIT)
                  textBox1.AppendText("\r\n11 bit ID = 0x" + String.Format("{0, 3:X3}", CANData.ID));  // 11 bit ID
                else
                  textBox1.AppendText("\r\n29 bit ID = 0x" + String.Format("{0, 8:X8}", CANData.ID));  // 29 bit ID
                // Start, length & padding bytes
                textBox1.AppendText("\r\nStart byte no. = " + CANData.Data[0].ToString());  // Start
                textBox1.AppendText("\r\nNo. of bytes = " + CANData.Data[1].ToString());  // Length
                textBox1.AppendText("\r\nPadding byte = 0x" + String.Format("{0, 2:X2}", CANData.BusState));  // Padding
                // Scaling factor & offset
                FloatValue = CANData.Data[2] + (CANData.Data[3] << 8) + (CANData.Data[4] << 16);
                textBox1.AppendText("\r\nScaling factor = " + String.Format("{0}", IEEE754ToFloat(FloatValue)));  // Scaling factor
                FloatValue = CANData.Data[5] + (CANData.Data[6] << 8) + (CANData.Data[7] << 16);
                textBox1.AppendText("\r\nOffset = " + String.Format("{0}", IEEE754ToFloat(FloatValue)));  // Offset
                // Repeat time
                if (CANData.TimeStamp < REPEAT_TIME.Length)
                  textBox1.AppendText("\r\nRepeat time = " + REPEAT_TIME[CANData.TimeStamp]);

                InputNo++;  // Next I/P

                // Move read pointer onto next slot in cyclic buffer
                if ((CANBufferControl.ReadIndex + 1) < CANdoImport.CANDO_CAN_BUFFER_LENGTH)
                  CANBufferControl.ReadIndex++;  // Increment index onto next free slot
                else
                  CANBufferControl.ReadIndex = 0;  // Wrap back to start

                CANBufferControl.FullFlag = CANdoImport.CLEAR;  // Clear flag as buffer not full

                Marshal.StructureToPtr(CANBufferControl, CANBufferControlPointer, true);  // Update unmanaged memory
              }

              // CAN baud rate
              if (CANdoStatus.NewFlag == CANdoImport.CANDO_SETUP_STATUS)
              {
                // Display CAN baud rate settings
                BRP = CANdoStatus.HardwareVersion;
                BRP++;  // Add 1 as value is stored 0 based
                PROPSEG = CANdoStatus.Status;
                PROPSEG >>= 4;
                PROPSEG++;  // Add 1 as value is stored 0 based
                PHSEG1 = CANdoStatus.SoftwareVersion;
                PHSEG1 &= 0x0F;
                PHSEG1++;  // Add 1 as value is stored 0 based
                PHSEG2 = CANdoStatus.SoftwareVersion;
                PHSEG2 >>= 4;
                PHSEG2++;  // Add 1 as value is stored 0 based
                textBox1.AppendText("\r\n\r\nCAN baud rate configuration -");
                textBox1.AppendText("\r\nBRP = " + BRP.ToString());
                textBox1.AppendText("\r\nPROPSEG = " + PROPSEG.ToString());
                textBox1.AppendText("\r\nPHSEG1 = " + PHSEG1.ToString());
                textBox1.AppendText("\r\nPHSEG2 = " + PHSEG2.ToString());
                textBox1.AppendText("\r\nSJW = " + (((CANdoStatus.Status >> 1) & 0x03) + 1).ToString());
                if ((CANdoStatus.Status & 0x01) == 0x00)
                  textBox1.AppendText("\r\nSAM = 1");
                else
                  textBox1.AppendText("\r\nSAM = 3");
                textBox1.AppendText("\r\nBaud rate = " + String.Format("{0:F2}", CANdoImport.CANDO_CLK_FREQ_HIGH / (2 * BRP * (1 + PROPSEG + PHSEG1 + PHSEG2))) + "k");

                CANdoStatus.NewFlag = CANdoImport.CANDO_NO_STATUS;  // Clear flag to show status read
              }
            }
          }
        }
      }
    }

    //--------------------------------------------------------------------------
    private void button3_Click(object sender, EventArgs e)
    {
      if (CANdoUSB.OpenFlag == CANdoImport.OPEN)
      {
        // Clear analogue I/P store
        if (CANdoImport.CANdoAnalogStoreClear(CANdoUSB) == CANdoImport.CANDO_SUCCESS)
          textBox1.AppendText("\r\n\r\nAnalogue I/P store cleared");
        else
          textBox1.AppendText("\r\n\r\nERROR - Unable to clear Analogue I/P store");
      }
    }

    //--------------------------------------------------------------------------
    // IEEE754ToFloat
    //
    // Decode a floating point number encoded according to the truncated 24 bit
    // IEEE754 representation compatible with the Hi-Tech PICC18 compiler.
    //
    // Returns
    //    Float value decoded from IEEE754 binary representation
    //--------------------------------------------------------------------------
    private double IEEE754ToFloat(int Number)
    {
      double Exponent, Mantissa, Result = 0.0;

      if (Number != 0)
      {
        // Exponent
        Exponent = Math.Pow(2.0f, ((Number >> 15) & 0xFF) - 127);
        // Sign bit
        if ((Number & 0x800000) > 0)
          Exponent *= -1;  // -ve value
        // Mantissa
        Mantissa = 1 + ((double)(Number & 0x7FFF) / 32768.0);
        // Decoded floating point value
        Result = Exponent * Mantissa;
      }

      return Result;
    }
    
    //--------------------------------------------------------------------------
    private void button4_Click(object sender, EventArgs e)
    {
      WriteCANTransmitStore();
    }

    //--------------------------------------------------------------------------
    // WriteCANTransmitStore
    //
    // This procedure configures the CANdo AUTO device to transmit 3 fixed,
    // periodic messages on the CAN bus by writing to the non-volatile CAN
    // transmit store. The CAN baud rate for the device is also set to 250k bits/s.
    //
    // Message 1 - J1939 Total engine hours = 2200hrs
    // ID = 0x18FEE500 29 bit
    // Data = 0x00, 0x00, 0xAB, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF
    // Period = 2000ms
    //
    // Message 2 - J1939 Fan speed = 600rpm
    // ID = 0x18FEBD00 29 bit
    // Data = 0xFF, 0xFF, 0x12, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF
    // Period = 500ms
    //
    // Message 3 - J1939 Exhaust temperature = 310K right manifold, 325K left manifold
    // ID = 0x18FE0700 29 bit
    // Data = 0x48, 0xE0, 0x4A, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF
    // Period = 1000ms
    //
    // Returns
    //    Nothing
    //--------------------------------------------------------------------------
    private void WriteCANTransmitStore()
    {
      const uint EHR = 0x18FEE500;  // J1939 Engine hours/revolutions ID, source address = 0
      const uint FD = 0x18FEBD00;  // J1939 Fan drive ID, source address = 0
      const uint ET = 0x18FE0700;  // J1939 Exhaust temperature ID, source address = 0 
      byte[] TOTAL_ENGINE_HOURS = {0x00, 0x00, 0xAB, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF};
      byte[] FAN_SPEED = {0xFF, 0xFF, 0x12, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF};
      byte[] EXHAUST_TEMP = {0x48, 0xE0, 0x4A, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF};

      if (CANdoUSB.OpenFlag == CANdoImport.OPEN)
      {
        // Device connected, so clear transmit store
        if (CANdoImport.CANdoTransmitStoreClear(CANdoUSB) == CANdoImport.CANDO_SUCCESS)
        {
          Thread.Sleep(50);  // Wait 50ms for store to clear
          
          // Write CAN transmit message 1
          if (CANdoImport.CANdoTransmitStoreWrite(CANdoUSB, CANdoImport.ID_29_BIT, EHR,
            CANdoImport.DATA_FRAME, 8, TOTAL_ENGINE_HOURS, REPEAT_TIME_2000MS) == CANdoImport.CANDO_SUCCESS)
          {
            Thread.Sleep(100);  // Wait 100ms for message to be stored
            textBox1.AppendText("\r\n\r\nCAN Transmit message 1 configured -");
            textBox1.AppendText("\r\nID = 0x18FEE500");
            textBox1.AppendText("\r\nRepeat time = 2000ms");
          }
          else
            textBox1.AppendText("\r\n\r\nERROR - Unable to write CAN Tranmsit message 1!");

          // Write CAN transmit message 2
          if (CANdoImport.CANdoTransmitStoreWrite(CANdoUSB, CANdoImport.ID_29_BIT, FD,
            CANdoImport.DATA_FRAME, 8, FAN_SPEED, REPEAT_TIME_500MS) == CANdoImport.CANDO_SUCCESS)
          {
            Thread.Sleep(100);  // Wait 100ms for message to be stored
            textBox1.AppendText("\r\n\r\nCAN Transmit message 2 configured -");
            textBox1.AppendText("\r\nID = 0x18FEBD00");
            textBox1.AppendText("\r\nRepeat time = 500ms");
          }
          else
            textBox1.AppendText("\r\n\r\nERROR - Unable to write CAN Tranmsit message 2!");

          // Write CAN transmit message 3
          if (CANdoImport.CANdoTransmitStoreWrite(CANdoUSB, CANdoImport.ID_29_BIT, ET,
            CANdoImport.DATA_FRAME, 8, EXHAUST_TEMP, REPEAT_TIME_1000MS) == CANdoImport.CANDO_SUCCESS)
          {
            Thread.Sleep(100);  // Wait 100ms for message to be stored
            textBox1.AppendText("\r\n\r\nCAN Transmit message 3 configured -");
            textBox1.AppendText("\r\nID = 0x18FE0700");
            textBox1.AppendText("\r\nRepeat time = 1000ms");
          }
          else
            textBox1.AppendText("\r\n\r\nERROR - Unable to write CAN Tranmsit message 3!");

          // Configure CAN baud rate
          if (CANdoImport.CANdoSetBaudRate(CANdoUSB, 0, 4 + CANdoImport.CANDO_BRP_ENHANCED_OFFSET, 7, 5, 4, 0) == CANdoImport.CANDO_SUCCESS)
            textBox1.AppendText("\r\n\r\nCAN bus baud rate set to 250k bits/s");
          else
            textBox1.AppendText("\r\n\r\nERROR - Unable to set CAN baud rate!");

          textBox1.AppendText("\r\n\r\nCycle power to CANdo AUTO device for settings to take effect");
        }
        else
          textBox1.AppendText("\r\n\r\nERROR - Unable to clear CAN Transmit store!");
      }
    }

    //--------------------------------------------------------------------------
    private void button5_Click(object sender, EventArgs e)
    {
      ReadCANTransmitStore();
    }

    //--------------------------------------------------------------------------
    // ReadCANTransmitStore
    //
    // This function reads the CAN transmit messages stored in the CAN transmit
    // store & displays them as a list. This can be useful to verify the contents
    // of the store following a write.
    //
    // Returns
    //    Nothing
    //--------------------------------------------------------------------------
    private void ReadCANTransmitStore()
    {
      int MessageNo, DataNo;
      string Data;
      byte BRP, PROPSEG, PHSEG1, PHSEG2;

      if (CANdoUSB.OpenFlag == CANdoImport.OPEN)
      {
        // Clear receive buffer ready to read CAN Transmit store config.
        if (CANdoImport.CANdoFlushBuffers(CANdoUSB) == CANdoImport.CANDO_SUCCESS)
        {
          // Clear CAN cyclic buffer
          CANBufferControl.ReadIndex = CANBufferControl.WriteIndex;
          CANBufferControl.FullFlag = CANdoImport.CLEAR;
          Marshal.StructureToPtr(CANBufferControl, CANBufferControlPointer, true);  // Update unmanaged memory
          // Issue CAN Transmit store read & setup status commands
          if ((CANdoImport.CANdoTransmitStoreRead(CANdoUSB) == CANdoImport.CANDO_SUCCESS) &&
            (CANdoImport.CANdoRequestSetupStatus(CANdoUSB) == CANdoImport.CANDO_SUCCESS))
          {
            Thread.Sleep(50);  // Wait 50ms for CANdo AUTO to reply

            CANdoStatus.NewFlag = CANdoImport.CANDO_NO_STATUS;
            // Collect analog store configuration sent by CANdo AUTO
            if (CANdoImport.CANdoReceive(CANdoUSB, CANBufferPointer, CANdoStatus) == CANdoImport.CANDO_SUCCESS)
            {
              // Get the TCANdoCANBufferControl structure populated by the CANdo.dll CANdoReceive function
              CANBufferControl = (CANdoImport.TCANdoCANBufferControl)Marshal.PtrToStructure(CANBufferControlPointer, CANBufferControlType);
              MessageNo = 1;
              while ((CANBufferControl.ReadIndex != CANBufferControl.WriteIndex) || (CANBufferControl.FullFlag == CANdoImport.SET))
              {
                // Get the TCANdoCAN structure for this CAN message populated by the CANdo.dll CANdoReceive function
                CANDataPointer = (IntPtr)(CANBufferPointer + CANBufferControl.ReadIndex * CANDataSize);
                CANData = (CANdoImport.TCANdoCAN)Marshal.PtrToStructure(CANDataPointer, CANDataType);
                // Display message stored
                textBox1.AppendText("\r\n\r\nCAN Transmit message " + MessageNo.ToString() + " configuration -");
                // ID
                if (CANData.IDE == CANdoImport.ID_11_BIT)
                  textBox1.AppendText("\r\n11 bit ID = 0x" + String.Format("{0, 3:X3}", CANData.ID));  // 11 bit ID
                else
                  textBox1.AppendText("\r\n29 bit ID = 0x" + String.Format("{0, 8:X8}", CANData.ID));  // 29 bit ID
                // DLC
                textBox1.AppendText("\r\nDLC = " + CANData.DLC.ToString());  // DLC
                // Data
                Data = string.Empty;
                for (DataNo = 0; DataNo < 7; DataNo++)
                  Data = Data+"0x"+String.Format("{0, 2:X2}", CANData.Data[DataNo]) + ", ";
                Data = Data+"0x"+String.Format("{0, 2:X2}", CANData.Data[DataNo]);
                textBox1.AppendText("\r\nData = " + Data);
                // Repeat time
                if (CANData.BusState < REPEAT_TIME.Length)
                  textBox1.AppendText("\r\nRepeat time = " + REPEAT_TIME[CANData.BusState]);

                MessageNo++;  // Next message

                // Move read pointer onto next slot in cyclic buffer
                if ((CANBufferControl.ReadIndex + 1) < CANdoImport.CANDO_CAN_BUFFER_LENGTH)
                  CANBufferControl.ReadIndex++;  // Increment index onto next free slot
                else
                  CANBufferControl.ReadIndex = 0;  // Wrap back to start

                CANBufferControl.FullFlag = CANdoImport.CLEAR;  // Clear flag as buffer not full

                Marshal.StructureToPtr(CANBufferControl, CANBufferControlPointer, true);  // Update unmanaged memory
              }

              if (MessageNo == 1)
                // No CAN transmit messages present
                textBox1.AppendText("\r\n\r\nCAN Transmit store is empty");

              // CAN baud rate
              if (CANdoStatus.NewFlag == CANdoImport.CANDO_SETUP_STATUS)
              {
                // Display CAN baud rate settings
                BRP = CANdoStatus.HardwareVersion;
                BRP++;  // Add 1 as value is stored 0 based
                PROPSEG = CANdoStatus.Status;
                PROPSEG >>= 4;
                PROPSEG++;  // Add 1 as value is stored 0 based
                PHSEG1 = CANdoStatus.SoftwareVersion;
                PHSEG1 &= 0x0F;
                PHSEG1++;  // Add 1 as value is stored 0 based
                PHSEG2 = CANdoStatus.SoftwareVersion;
                PHSEG2 >>= 4;
                PHSEG2++;  // Add 1 as value is stored 0 based
                textBox1.AppendText("\r\n\r\nCAN baud rate configuration -");
                textBox1.AppendText("\r\nBRP = " + BRP.ToString());
                textBox1.AppendText("\r\nPROPSEG = " + PROPSEG.ToString());
                textBox1.AppendText("\r\nPHSEG1 = " + PHSEG1.ToString());
                textBox1.AppendText("\r\nPHSEG2 = " + PHSEG2.ToString());
                textBox1.AppendText("\r\nSJW = " + (((CANdoStatus.Status >> 1) & 0x03) + 1).ToString());
                if ((CANdoStatus.Status & 0x01) == 0x00)
                  textBox1.AppendText("\r\nSAM = 1");
                else
                  textBox1.AppendText("\r\nSAM = 3");
                textBox1.AppendText("\r\nBaud rate = " + String.Format("{0:F2}", CANdoImport.CANDO_CLK_FREQ_HIGH / (2 * BRP * (1 + PROPSEG + PHSEG1 + PHSEG2))) + "k");

                CANdoStatus.NewFlag = CANdoImport.CANDO_NO_STATUS;  // Clear flag to show status read
              }
            }
          }
        }
      }
    }

    //--------------------------------------------------------------------------
    private void button6_Click(object sender, EventArgs e)
    {
      if (CANdoUSB.OpenFlag == CANdoImport.OPEN)
      {
        // Clear analogue I/P store
        if (CANdoImport.CANdoTransmitStoreClear(CANdoUSB) == CANdoImport.CANDO_SUCCESS)
          textBox1.AppendText("\r\n\r\nCAN Transmit store cleared");
        else
          textBox1.AppendText("\r\n\r\nERROR - Unable to clear CAN Transmit store");
      }
    }
  }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
