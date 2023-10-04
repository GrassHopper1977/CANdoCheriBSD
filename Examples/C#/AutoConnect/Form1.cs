//------------------------------------------------------------------------------
//  TITLE :- CANdoCSharp example main form - Form1.cs
//  AUTHOR :- Martyn Brown
//  DATE :- 05/10/13
//
//  DESCRIPTION :- 'C#' example program to demonstrate using the CANdo.dll to
//  interface to the CANdo device. This program automatically connects to the
//  first CANdo device available.
//
//  UPDATES :-
//	19/02/06 Created
//  27/03/08 CAN control buffer in unmanaged memory reset at start
//  02/08/10 "Get Version" button added
//  21/02/11 1) Updated with CANdo API v2.2
//           2) Detect button removed
//  27/07/11 1) Updated with CANdo API v3.0
//           2) Modified to use new CANdoImport.TCANdoStatus NewFlag constants
//  03/11/11 1) "Get PID" button added
//           2) "Clear Status" button added
//           3) Timer1 interval changed from 50 to 10ms
//  05/10/13 CANDataPointer address calculation modified for 64 bit platform
//
//  LICENSE :-
//  The SDK (Software Development Kit) provided for use with the CANdo device
//  is issued as FREE software, meaning that it is free for personal,
//  educational & commercial use, without restriction or time limit. The
//  software is supplied "as is", with no implied warranties or guarantees.
//
//  (c) 2006-13 Netronics Ltd. All rights reserved.
//------------------------------------------------------------------------------
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Runtime.InteropServices;

namespace CANdoCSharp
{
  public partial class Form1 : Form
  {
    byte RunState;
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

      // Set defaults
      comboBox1.SelectedIndex = 2;
      comboBox2.SelectedIndex = 0;
      comboBox3.SelectedIndex = 0;

      // Connect to device
      ConnectToCANdo();
    }

    //------------------------------------------------------------------------------
    private int ConnectToCANdo()
    {
      int Status;

      try
      {
        Status = CANdoImport.CANdoOpen(CANdoUSB);
        if (Status == CANdoImport.CANDO_SUCCESS)
        {
          SetOpenControlsState(true);
          label19.Text = CANdoUSB.Description.ToString();
          label20.Text = " S/N " + CANdoUSB.SerialNo.ToString();
        }
        else
          if (Status == CANdoImport.CANDO_USB_DLL_ERROR)
          {
            label19.Text = "libCANdo.so";
            label20.Text = "not found";
            CANdoImport.CANdoClose(CANdoUSB);  // Disconnect
          }
          else
            if (Status == CANdoImport.CANDO_USB_DRIVER_ERROR)
            {
              label19.Text = "CANdo driver";
              label20.Text = "not found";
              CANdoImport.CANdoClose(CANdoUSB);  // Disconnect
            }
            else
            {
              label19.Text = "CANdo";
              label20.Text = "not found";
              CANdoImport.CANdoClose(CANdoUSB);  // Disconnect
            }
      }
      catch (DllNotFoundException)
      {
        label19.Text = "libCANdo.so";
        label20.Text = "not found";
        Status = CANdoImport.CANDO_ERROR;
      }
      catch (EntryPointNotFoundException)
      {
        label19.Text = "libCANdo.so";
        label20.Text = "incorrect version";
        Status = CANdoImport.CANDO_ERROR;
      }

      return Status;
    }

    //------------------------------------------------------------------------------
    private void button2_Click(object sender, EventArgs e)
    {
      // Start CANdo
      if (CANdoUSB.OpenFlag == CANdoImport.OPEN)
      {
        // Configure CANdo & then set to run state
        if ((SetBaudRate()) && (SetMode()) && (SetFilters()) && SetState(true))
        {
          // CANdo running
          label19.Text = CANdoUSB.Description.ToString();
          label20.Text = " S/N " + CANdoUSB.SerialNo.ToString();
          label19.Enabled = false;
          label20.Enabled = false;
          textBox1.Clear();
          textBox1.AppendText("IDE   ID   RTR D1 D2 D3 D4 D5 D6 D7 D8 Timestamp" + System.Environment.NewLine);
          textBox1.AppendText("------------------------------------------------" + System.Environment.NewLine);
          // Disable 'CANdo Start' button & enable other controls
          SetOpenControlsState(false);
          SetRunControlsState(true);
          // Enable receiver
          timer1.Enabled = true;
        }
      }
    }

    //------------------------------------------------------------------------------
    private void button3_Click(object sender, EventArgs e)
    {
      // Transmit a message
      byte IDE, RTR;
      uint ID = uint.Parse(textBox2.Text, System.Globalization.NumberStyles.HexNumber);
      byte[] Data = new byte[8];
      Data[0] = byte.Parse(textBox3.Text, System.Globalization.NumberStyles.HexNumber);
      Data[1] = byte.Parse(textBox4.Text, System.Globalization.NumberStyles.HexNumber);
      Data[2] = byte.Parse(textBox5.Text, System.Globalization.NumberStyles.HexNumber);
      Data[3] = byte.Parse(textBox6.Text, System.Globalization.NumberStyles.HexNumber);
      Data[4] = byte.Parse(textBox7.Text, System.Globalization.NumberStyles.HexNumber);
      Data[5] = byte.Parse(textBox8.Text, System.Globalization.NumberStyles.HexNumber);
      Data[6] = byte.Parse(textBox9.Text, System.Globalization.NumberStyles.HexNumber);
      Data[7] = byte.Parse(textBox10.Text, System.Globalization.NumberStyles.HexNumber);

      if (checkBox1.Checked)
        IDE = CANdoImport.ID_29_BIT;
      else
        IDE = CANdoImport.ID_11_BIT;

      if (checkBox2.Checked)
        RTR = CANdoImport.REMOTE_FRAME;
      else
        RTR = CANdoImport.DATA_FRAME;

      if (CANdoImport.CANdoTransmit(CANdoUSB, IDE, ID, RTR, (byte)numericUpDown1.Value, Data, 0, 0) == CANdoImport.CANDO_SUCCESS)
        label16.Text = "Message sent";
      else
        label16.Text = "Error sending message";
    }

    //------------------------------------------------------------------------------
    private void button4_Click(object sender, EventArgs e)
    {
      // Request CANdo status
      if (CANdoImport.CANdoRequestStatus(CANdoUSB) == CANdoImport.CANDO_SUCCESS)
      {
        if (!timer1.Enabled)
        {
          // timer1 not enabled to collect status message, so collect here
          Thread.Sleep(5);  // Wait 5ms to allow CANdo to respond

          // Check for a new status message
          if (CANdoImport.CANdoReceive(CANdoUSB, CANBufferPointer, CANdoStatus) == CANdoImport.CANDO_SUCCESS)
          {
            if (CANdoStatus.NewFlag == CANdoImport.CANDO_DEVICE_STATUS)
            {
              // New status message received
              label17.Text = "Status : " + CANdoStatus.Status.ToString() +
                "   Bus State : " + CANdoStatus.BusState.ToString() + "   Timestamp : " + CANdoStatus.TimeStamp.ToString();
              CANdoStatus.NewFlag = CANdoImport.CANDO_NO_STATUS;  // Clear flag to indicate message read
            }
          }
        }
      }
      else
        label17.Text = "Error requesting status";
    }

    //------------------------------------------------------------------------------
    private void button8_Click(object sender, EventArgs e)
    {
      // Clear CANdo status
      CANdoImport.CANdoClearStatus(CANdoUSB);
    }

    //------------------------------------------------------------------------------
    private void button7_Click(object sender, EventArgs e)
    {
      // Get CANdo file versions
      uint APIVersion, DLLVersion, DriverVersion;

      CANdoImport.CANdoGetVersion(out APIVersion, out DLLVersion, out DriverVersion);
      label18.Text = "CANdo API : v"+String.Format("{0:0.0}", ((double)APIVersion / 10))+
        "   USB API : v"+String.Format("{0:0.0}", ((double)DLLVersion / 10))+
        "   Driver : v"+String.Format("{0:0.0}", ((double)DriverVersion / 10));
    }

    //------------------------------------------------------------------------------
    private void button1_Click(object sender, EventArgs e)
    {
      // Get USB PID
      StringBuilder PID = new StringBuilder(CANdoImport.CANDO_STRING_LENGTH);

      if (CANdoUSB.OpenFlag == CANdoImport.OPEN)
      {
        // CANdo connected, so read USB PID
        if (CANdoImport.CANdoGetPID(CANdoUSB.No, PID) == CANdoImport.CANDO_SUCCESS)
          label1.Text = "USB PID : " + PID;
      }
    }

    //------------------------------------------------------------------------------
    private void button5_Click(object sender, EventArgs e)
    {
      // Stop CANdo if running
      if (RunState == CANdoImport.RUN)
      {
        if (SetState(false))
        {
          // Enable 'Start CANdo' button, but disable the other controls
          SetOpenControlsState(true);
          SetRunControlsState(false);
          label19.Enabled = true;
          label20.Enabled = true;
          // Stop receiver
          timer1.Enabled = false;
        }
      }
    }

    //------------------------------------------------------------------------------
    private void button6_Click(object sender, EventArgs e)
    {
      Close();
    }

    //------------------------------------------------------------------------------
    private void Form1_FormClosing(object sender, FormClosingEventArgs e)
    {
      // Close conection to CANdo if open & unload CANdo.dll
      if (CANdoUSB.OpenFlag == CANdoImport.OPEN)
      {
        SetState(false);  // Stop CANdo running
        CANdoImport.CANdoClose(CANdoUSB);  // Close USB connection
      }

      // Free the unmanaged memory
      Marshal.FreeHGlobal(CANBufferPointer);
      CANBufferPointer = IntPtr.Zero;
    }

    //------------------------------------------------------------------------------
    private void SetOpenControlsState(bool State)
    {
      if (State)
      {
        // Enable 'Start CANdo' button
        button1.Enabled = true;
        button2.Enabled = true;
        button4.Enabled = true;
        button7.Enabled = true;
        button8.Enabled = true;
        comboBox1.Enabled = true;
        comboBox2.Enabled = true;
        comboBox3.Enabled = true;
        label2.Enabled = true;
        label3.Enabled = true;
        label4.Enabled = true;
        label17.Enabled = true;
      }
      else
      {
        button2.Enabled = false;
        comboBox1.Enabled = false;
        comboBox2.Enabled = false;
        comboBox3.Enabled = false;
        label2.Enabled = false;
        label3.Enabled = false;
        label4.Enabled = false;
      }
    }

    //------------------------------------------------------------------------------
    private void SetRunControlsState(bool State)
    {
      if (State)
      {
        // Enable all items on the form
        label5.Enabled = true;
        label6.Enabled = true;
        label7.Enabled = true;
        label8.Enabled = true;
        label9.Enabled = true;
        label10.Enabled = true;
        label11.Enabled = true;
        label12.Enabled = true;
        label13.Enabled = true;
        label14.Enabled = true;
        label15.Enabled = true;
        label16.Enabled = true;
        checkBox1.Enabled = true;
        checkBox2.Enabled = true;
        textBox1.Enabled = true;
        textBox2.Enabled = true;
        textBox3.Enabled = true;
        textBox4.Enabled = true;
        textBox5.Enabled = true;
        textBox6.Enabled = true;
        textBox7.Enabled = true;
        textBox8.Enabled = true;
        textBox9.Enabled = true;
        textBox10.Enabled = true;
        numericUpDown1.Enabled = true;
        button3.Enabled = true;
        button5.Enabled = true;
      }
      else
      {
        // Disable all items on the form
        label5.Enabled = false;
        label6.Enabled = false;
        label7.Enabled = false;
        label8.Enabled = false;
        label9.Enabled = false;
        label10.Enabled = false;
        label11.Enabled = false;
        label12.Enabled = false;
        label13.Enabled = false;
        label14.Enabled = false;
        label15.Enabled = false;
        label16.Enabled = false;
        checkBox1.Enabled = false;
        checkBox2.Enabled = false;
        textBox1.Enabled = false;
        textBox2.Enabled = false;
        textBox3.Enabled = false;
        textBox4.Enabled = false;
        textBox5.Enabled = false;
        textBox6.Enabled = false;
        textBox7.Enabled = false;
        textBox8.Enabled = false;
        textBox9.Enabled = false;
        textBox10.Enabled = false;
        numericUpDown1.Enabled = false;
        button3.Enabled = false;
        button5.Enabled = false;
      }
    }

    //------------------------------------------------------------------------------
    private void checkBox2_CheckedChanged(object sender, EventArgs e)
    {
      bool DataEnabledState;

      if (checkBox2.Checked)
        DataEnabledState = false;
      else
        DataEnabledState = true;

      foreach (Control Item in groupBox2.Controls)
      {
        if ((Item is TextBox) && (Item.Name != "textBox2"))
          Item.Enabled = DataEnabledState;
      }
    }

    //------------------------------------------------------------------------------
    private bool SetBaudRate()
    {
      byte BRP, PHSEG1, PHSEG2, PROPSEG;

      // Set CANdo baud rate
			switch (comboBox1.SelectedIndex)
			{
				case 0 :
					// 62.5k
					BRP = 7;
					PHSEG1 = 7;
					PHSEG2 = 7;
					PROPSEG = 2;
					break;

				case 1 :
					// 125k
					BRP = 3;
					PHSEG1 = 7;
					PHSEG2 = 7;
					PROPSEG = 2;
					break;

				case 2 :
					// 250k
					BRP = 1;
					PHSEG1 = 7;
					PHSEG2 = 7;
					PROPSEG = 2;
					break;

				case 3 :
					// 500k
					BRP = 0;
					PHSEG1 = 7;
					PHSEG2 = 7;
					PROPSEG = 2;
					break;

				case 4 :
					// 1M
					BRP = 0;
					PHSEG1 = 2;
					PHSEG2 = 2;
					PROPSEG = 2;
					break;

				default :
					// 250k
					BRP = 1;
					PHSEG1 = 7;
					PHSEG2 = 7;
					PROPSEG = 2;
					break;
			}

      if (CANdoImport.CANdoSetBaudRate(CANdoUSB, 0, BRP, PHSEG1, PHSEG2, PROPSEG, 0) == CANdoImport.CANDO_SUCCESS)
      {
        Thread.Sleep(100);  // Wait 100ms to allow CANdo to store settings in EEPROM if changed
        return true;  // Baud rate set
      }
      else
        return false;  // Error
    }

    //------------------------------------------------------------------------------
    private bool SetMode()
    {
      // Set CANdo operating mode
      if (CANdoImport.CANdoSetMode(CANdoUSB, (byte)comboBox2.SelectedIndex) == CANdoImport.CANDO_SUCCESS)
      {
        Thread.Sleep(10);  // Wait 10ms to allow CANdo to store setting in EEPROM if changed
        return true;  // Mode set
      }
      else
        return false;  // Error
    }

    //------------------------------------------------------------------------------
    private bool SetFilters()
    {
      // Set CANdo receive buffer filters
      uint Rx1Mask, Rx2Mask;
      byte Rx1IDE1, Rx1IDE2, Rx2IDE1, Rx2IDE2, Rx2IDE3, Rx2IDE4;
      uint Rx1Filter1, Rx1Filter2, Rx2Filter1, Rx2Filter2, Rx2Filter3, Rx2Filter4;

      switch (comboBox3.SelectedIndex)
      {
        case 0 :
          // None
          // Rx buffer 1
          Rx1Mask = 0;
          Rx1IDE1 = CANdoImport.ID_29_BIT;
          Rx1Filter1 = 0;
          Rx1IDE2 = CANdoImport.ID_11_BIT;
          Rx1Filter2 = 0;
          // Rx buffer 2
          Rx2Mask = 0;
          Rx2IDE1 = CANdoImport.ID_29_BIT;
          Rx2Filter1 = 0;
          Rx2IDE2 = CANdoImport.ID_11_BIT;
          Rx2Filter2 = 0;
          Rx2IDE3 = CANdoImport.ID_29_BIT;
          Rx2Filter3 = 0;
          Rx2IDE4 = CANdoImport.ID_11_BIT;
          Rx2Filter4 = 0;
          break;

        case 1:
          // 11 Bit ID 0x000 - 0x2FF Only
          // Rx buffer 1
          Rx1Mask = 0x700;
          Rx1IDE1 = CANdoImport.ID_11_BIT;
          Rx1Filter1 = 0x000;
          Rx1IDE2 = CANdoImport.ID_11_BIT;
          Rx1Filter2 = 0x100;
          // Rx buffer 2
          Rx2Mask = 0x700;
          Rx2IDE1 = CANdoImport.ID_11_BIT;
          Rx2Filter1 = 0x200;
          Rx2IDE2 = CANdoImport.ID_11_BIT;
          Rx2Filter2 = 0x200;
          Rx2IDE3 = CANdoImport.ID_11_BIT;
          Rx2Filter3 = 0x200;
          Rx2IDE4 = CANdoImport.ID_11_BIT;
          Rx2Filter4 = 0x200;
          break;

        case 2:
          // 29 Bit ID Only
          // Rx buffer 1
          Rx1Mask = 0;
          Rx1IDE1 = CANdoImport.ID_29_BIT;
          Rx1Filter1 = 0;
          Rx1IDE2 = CANdoImport.ID_29_BIT;
          Rx1Filter2 = 0;
          // Rx buffer 2
          Rx2Mask = 0;
          Rx2IDE1 = CANdoImport.ID_29_BIT;
          Rx2Filter1 = 0;
          Rx2IDE2 = CANdoImport.ID_29_BIT;
          Rx2Filter2 = 0;
          Rx2IDE3 = CANdoImport.ID_29_BIT;
          Rx2Filter3 = 0;
          Rx2IDE4 = CANdoImport.ID_29_BIT;
          Rx2Filter4 = 0;
          break;

        default :
          // None
          // Rx buffer 1
          Rx1Mask = 0;
          Rx1IDE1 = CANdoImport.ID_29_BIT;
          Rx1Filter1 = 0;
          Rx1IDE2 = CANdoImport.ID_11_BIT;
          Rx1Filter2 = 0;
          // Rx buffer 2
          Rx2Mask = 0;
          Rx2IDE1 = CANdoImport.ID_29_BIT;
          Rx2Filter1 = 0;
          Rx2IDE2 = CANdoImport.ID_11_BIT;
          Rx2Filter2 = 0;
          Rx2IDE3 = CANdoImport.ID_29_BIT;
          Rx2Filter3 = 0;
          Rx2IDE4 = CANdoImport.ID_11_BIT;
          Rx2Filter4 = 0;
          break;
      }

      if (CANdoImport.CANdoSetFilters(CANdoUSB,
        Rx1Mask,
        Rx1IDE1, Rx1Filter1,
        Rx1IDE2, Rx1Filter2,
        Rx2Mask,
        Rx2IDE1, Rx2Filter1,
        Rx2IDE2, Rx2Filter2,
        Rx2IDE3, Rx2Filter3,
        Rx2IDE4, Rx2Filter4) == CANdoImport.CANDO_SUCCESS)
      {
        Thread.Sleep(10);  // Wait 10ms to allow filters to be configured in CAN module
        return true;  // Flters set
      }
      else
        return false;  // Error
    }

    //------------------------------------------------------------------------------
    private bool SetState(bool Run)
    {
      // Set CANdo run state
      if (Run)
        RunState = CANdoImport.RUN;
      else
        RunState = CANdoImport.STOP;

      if ((CANdoImport.CANdoFlushBuffers(CANdoUSB) == CANdoImport.CANDO_SUCCESS) &&
        (CANdoImport.CANdoSetState(CANdoUSB, RunState) == CANdoImport.CANDO_SUCCESS))
        return true;  // State set
      else
        return false;  // Error
    }

    //------------------------------------------------------------------------------
    private void timer1_Tick(object sender, EventArgs e)
    {
      string CANMessageString;
      byte DataNo;

      // Check for CAN messages received
      if (CANdoImport.CANdoReceive(CANdoUSB, CANBufferPointer, CANdoStatus) != CANdoImport.CANDO_SUCCESS)
        textBox1.AppendText("Error receiving CAN messages." + System.Environment.NewLine);

      // Get the TCANdoCANBufferControl structure populated by the CANdo.dll CANdoReceive function
      CANBufferControl = (CANdoImport.TCANdoCANBufferControl)Marshal.PtrToStructure(CANBufferControlPointer, CANBufferControlType);
      while ((CANBufferControl.ReadIndex != CANBufferControl.WriteIndex) || (CANBufferControl.FullFlag == CANdoImport.SET))
      {
        // Get the TCANdoCAN structure for this CAN message populated by the CANdo.dll CANdoReceive function
        CANDataPointer = (IntPtr)(CANBufferPointer + CANBufferControl.ReadIndex * CANDataSize);
        CANData = (CANdoImport.TCANdoCAN)Marshal.PtrToStructure(CANDataPointer, CANDataType);

        // Display the data
        CANMessageString = " " + CANData.IDE.ToString() + String.Format(" {0, 8:X} ", CANData.ID) + CANData.RTR.ToString() + "  ";
        for (DataNo = 0; DataNo < CANData.DLC; DataNo++)
          CANMessageString += String.Format("{0, 2:X} ", CANData.Data[DataNo]);
        for (DataNo = CANData.DLC; DataNo < 8; DataNo++)
          CANMessageString += "   ";
        CANMessageString += CANData.TimeStamp.ToString() + System.Environment.NewLine;
        textBox1.AppendText(CANMessageString);

        // Move read pointer onto next slot in cyclic buffer
        if ((CANBufferControl.ReadIndex + 1) < CANdoImport.CANDO_CAN_BUFFER_LENGTH)
          CANBufferControl.ReadIndex++;  // Increment index onto next free slot
        else
          CANBufferControl.ReadIndex = 0;  // Wrap back to start

        CANBufferControl.FullFlag = CANdoImport.CLEAR;  // Clear flag as buffer not full

        // Update unmanaged memory
        Marshal.StructureToPtr(CANBufferControl, CANBufferControlPointer, true);
      }

      if (CANdoStatus.NewFlag == CANdoImport.CANDO_DEVICE_STATUS)
      {
        // New status message received
        label17.Text = "Status : " + CANdoStatus.Status.ToString() +
          "   Bus State : " + CANdoStatus.BusState.ToString() + "   Timestamp : " + CANdoStatus.TimeStamp.ToString();
        CANdoStatus.NewFlag = CANdoImport.CANDO_NO_STATUS;  // Clear flag to indicate message read
      }
    }
  }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
