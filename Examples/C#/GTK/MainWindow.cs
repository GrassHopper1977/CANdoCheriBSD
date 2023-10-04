//------------------------------------------------------------------------------
//  TITLE :- CANdoCSharp GTK GUI example main window - MainWindow.cs
//  AUTHOR :- Martyn Brown
//  DATE :- 30/05/14
//
//  DESCRIPTION :- 'C#' example program written in MonoDevelop using the GTK
//  to demonstrate using the libCANdo.so to interface to the CANdo device. This
//  program automatically connects to the first CANdo device available.
//
//  UPDATES :-
//  30/05/14 Created
//
//  LICENSE :-
//  The SDK (Software Development Kit) provided for use with the CANdo device
//  is issued as FREE software, meaning that it is free for personal,
//  educational & commercial use, without restriction or time limit. The
//  software is supplied "as is", with no implied warranties or guarantees.
//
//  (c) 2014 Netronics Ltd. All rights reserved.
//------------------------------------------------------------------------------
using System;
using System.Timers;
using Gtk;
using System.Threading;
using System.Runtime.InteropServices;
using CANdoCSharp;

public partial class MainWindow: Gtk.Window
{
  enum CANBaudRate { BR62_5K, BR125K, BR250K, BR500K, BR1M };  // CAN baud rates

  byte RunState;
  string CANdoDescription;
  System.Timers.Timer timer1;  // Receive poll timer
  CANdoImport.TCANdoUSB CANdoUSB;  // Store for parameters relating to connected CANdo
  CANdoImport.TCANdoStatus CANdoStatus;  // Store for status message collected from CANdo
  Type CANDataType, CANBufferControlType;
  int CANDataSize, CANBufferControlSize, CANBufferSize;
  IntPtr CANBufferPointer, CANDataPointer, CANBufferControlPointer;
  CANdoImport.TCANdoCAN CANData;
  CANdoImport.TCANdoCAN[] CANDataBuffer;  // Cyclic buffer store for CAN receive messages
  CANdoImport.TCANdoCANBufferControl CANBufferControl;  // Cyclic buffer control

	public MainWindow (): base (Gtk.WindowType.Toplevel)
	{
		Build ();

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

    // Set the receive window font to monospaced
    Pango.FontDescription Font = Pango.FontDescription.FromString ("Courier New 12");
    textview1.ModifyFont(Font);

    // Create the receive timer
    timer1 = new System.Timers.Timer();
    timer1.Elapsed += new ElapsedEventHandler(timer1_Tick);
    timer1.Interval = 10;  // 10ms timeout

	// Connect to device
	ConnectToCANdo();
  }

	//------------------------------------------------------------------------------
	protected void OnButton8Clicked (object sender, EventArgs e)
	{
		// Exit button
		Application.Quit ();
	}

  //------------------------------------------------------------------------------
  protected void OnButton6Clicked (object sender, EventArgs e)
  {
    TextIter Iter;

    // Start button
    if (CANdoUSB.OpenFlag == CANdoImport.OPEN)
    {
      // Configure CANdo & then set to run state
      if ((SetBaudRate(CANBaudRate.BR250K)) && (SetMode()) && (SetFilters()) && SetState(true))
      {
        // CANdo running
        textview1.Buffer.Text = "";
        Iter = textview1.Buffer.EndIter;
        textview1.Buffer.Insert(ref Iter, "IDE   ID   RTR D1 D2 D3 D4 D5 D6 D7 D8 Timestamp" + System.Environment.NewLine);
        textview1.Buffer.Insert(ref Iter, "------------------------------------------------" + System.Environment.NewLine);
        // Enable transmit button
        button1.Sensitive = true;
        // Enable receiver
        timer1.Enabled = true;
      }
    }
  }

  //------------------------------------------------------------------------------
  protected void OnButton7Clicked (object sender, EventArgs e)
  {
    // Stop button
    if (RunState == CANdoImport.RUN)
    {
      if (SetState(false))
      {
        // CANdo stopped
        // Disable transmit button
        button1.Sensitive = false;
        // Stop receiver
        timer1.Enabled = false;
      }
    }
  }

  //------------------------------------------------------------------------------
  protected void OnButton1Clicked (object sender, EventArgs e)
  {
    // Now button
    // Transmit a message
    byte IDE, RTR;
    uint ID = uint.Parse(entry1.Text, System.Globalization.NumberStyles.HexNumber);
    byte[] Data = new byte[8];
    Data[0] = byte.Parse(entry2.Text, System.Globalization.NumberStyles.HexNumber);
    Data[1] = byte.Parse(entry3.Text, System.Globalization.NumberStyles.HexNumber);
    Data[2] = byte.Parse(entry4.Text, System.Globalization.NumberStyles.HexNumber);
    Data[3] = byte.Parse(entry5.Text, System.Globalization.NumberStyles.HexNumber);
    Data[4] = byte.Parse(entry6.Text, System.Globalization.NumberStyles.HexNumber);
    Data[5] = byte.Parse(entry7.Text, System.Globalization.NumberStyles.HexNumber);
    Data[6] = byte.Parse(entry8.Text, System.Globalization.NumberStyles.HexNumber);
    Data[7] = byte.Parse(entry9.Text, System.Globalization.NumberStyles.HexNumber);

    if (checkbutton1.Active)
      IDE = CANdoImport.ID_29_BIT;
    else
      IDE = CANdoImport.ID_11_BIT;

    if (!radiobutton1.Active)
      RTR = CANdoImport.REMOTE_FRAME;
    else
      RTR = CANdoImport.DATA_FRAME;

    if (CANdoImport.CANdoTransmit(CANdoUSB, IDE, ID, RTR, (byte)spinbutton1.Value, Data, 0, 0) == CANdoImport.CANDO_SUCCESS)
      label12.LabelProp = "Sent";
    else
      label12.LabelProp = "Error";
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
        CANdoDescription = CANdoUSB.Description.ToString() + "   S/N " + CANdoUSB.SerialNo.ToString();
        statusbar2.Push(0, CANdoDescription); 
      }
			else
			if (Status == CANdoImport.CANDO_USB_DLL_ERROR)
			{
        statusbar2.Push(0, "libCANdo.so not found");
				CANdoImport.CANdoClose(CANdoUSB);
			}
			else
			if (Status == CANdoImport.CANDO_USB_DRIVER_ERROR)
			{
        statusbar2.Push(0, "CANdo driver not found");
				CANdoImport.CANdoClose(CANdoUSB);
			}
			else
			{
        statusbar2.Push(0, "CANdo not found");
				CANdoImport.CANdoClose(CANdoUSB);
			}
		}
		catch (DllNotFoundException)
		{
			Status = CANdoImport.CANDO_ERROR;
		}
		catch (EntryPointNotFoundException)
		{
			Status = CANdoImport.CANDO_ERROR;
		}

		return Status;
	}

	//------------------------------------------------------------------------------
	private bool SetBaudRate(CANBaudRate BaudRate)
	{
		byte BRP, PHSEG1, PHSEG2, PROPSEG;

		// Set CANdo CAN baud rate
		switch (BaudRate)
		{
			case CANBaudRate.BR62_5K :
			// 62.5k
			BRP = 7;
			PHSEG1 = 7;
			PHSEG2 = 7;
			PROPSEG = 2;
			break;

      case CANBaudRate.BR125K :
			// 125k
			BRP = 3;
			PHSEG1 = 7;
			PHSEG2 = 7;
			PROPSEG = 2;
			break;

      case CANBaudRate.BR250K :
			// 250k
			BRP = 1;
			PHSEG1 = 7;
			PHSEG2 = 7;
			PROPSEG = 2;
			break;

      case CANBaudRate.BR500K :
			// 500k
			BRP = 0;
			PHSEG1 = 7;
			PHSEG2 = 7;
			PROPSEG = 2;
			break;

      case CANBaudRate.BR1M :
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
		// Set CANdo operating mode to 'Normal'
		if (CANdoImport.CANdoSetMode(CANdoUSB, CANdoImport.NORMAL_MODE) == CANdoImport.CANDO_SUCCESS)
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

		// No filters
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
    TextIter Iter;

    Iter = textview1.Buffer.EndIter;
    // Check for CAN messages received
    if (CANdoImport.CANdoReceive(CANdoUSB, CANBufferPointer, CANdoStatus) != CANdoImport.CANDO_SUCCESS)
      textview1.Buffer.Insert(ref Iter, "Error receiving CAN messages." + System.Environment.NewLine);

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
      textview1.Buffer.Insert(ref Iter, CANMessageString);

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
      statusbar2.Push(0, CANdoDescription + "   Status : " + CANdoStatus.Status.ToString() +
        "   Bus State : " + CANdoStatus.BusState.ToString() + "   Timestamp : " + CANdoStatus.TimeStamp.ToString());
      CANdoStatus.NewFlag = CANdoImport.CANDO_NO_STATUS;  // Clear flag to indicate message read
    }
  }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
