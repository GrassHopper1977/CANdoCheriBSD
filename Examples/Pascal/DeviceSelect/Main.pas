//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// TITLE : DeviceSelect main routines - Main.pas
//
// DESCRIPTION : Lazarus example program to demonstrate using libCANdo.so to
//  interface to the CANdo device.
//
// DATE : 15/12/14
//
// MODIFICATIONS :
//  27/05/14 Created
//  15/12/14 Modified to load libCANdo.so library dynamically
//
// AUTHOR : Martyn Brown
//
// LICENSE :
//  The SDK (Software Development Kit) provided for use with the CANdo
//  device is issued as FREE software, meaning that it is free for personal,
//  educational & commercial use, without restriction or time limit. The
//  software is supplied "as is", with no implied warranties or guarantees.
//
// © 2014 Netronics Ltd. All rights reserved.
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
unit Main;

interface

uses
  LCLIntf, LCLType, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, Menus, ComCtrls, StdCtrls, Buttons, Spin, ExtCtrls, CANdoImport;

const
  ONE_SECOND = 100;  // 10ms * 100 = 1s

type
  TForm1 = class(TForm)
    MainMenu1: TMainMenu;
    File1: TMenuItem;
    Exit1: TMenuItem;
    StatusBar1: TStatusBar;
    BitBtn2: TBitBtn;
    BitBtn4: TBitBtn;
    Label17: TLabel;
    Timer1: TTimer;
    GroupBox1: TGroupBox;
    BitBtn3: TBitBtn;
    Label6: TLabel;
    Label7: TLabel;
    CheckBox1: TCheckBox;
    CheckBox2: TCheckBox;
    Edit14: TEdit;
    Label8: TLabel;
    Label9: TLabel;
    Label10: TLabel;
    Label11: TLabel;
    Label12: TLabel;
    Label13: TLabel;
    Label14: TLabel;
    Label15: TLabel;
    Label16: TLabel;
    SpinEdit1: TSpinEdit;
    Edit6: TEdit;
    Edit7: TEdit;
    Edit8: TEdit;
    Edit9: TEdit;
    Edit10: TEdit;
    Edit11: TEdit;
    Edit12: TEdit;
    Edit13: TEdit;
    GroupBox2: TGroupBox;
    BitBtn1: TBitBtn;
    Edit1: TEdit;
    Edit2: TEdit;
    Edit3: TEdit;
    Edit4: TEdit;
    Edit5: TEdit;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    GroupBox3: TGroupBox;
    Memo1: TMemo;
    BitBtn5: TBitBtn;
    ComboBox1: TComboBox;
    BitBtn6: TBitBtn;
    Label18: TLabel;
    Label19: TLabel;
    procedure Exit1Click(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure FormCreate(Sender: TObject);
    procedure BitBtn1Click(Sender: TObject);
    procedure BitBtn2Click(Sender: TObject);
    procedure BitBtn4Click(Sender: TObject);
    procedure BitBtn3Click(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure BitBtn5Click(Sender: TObject);
    procedure CheckBox2Click(Sender: TObject);
    procedure BitBtn6Click(Sender: TObject);
  private
    { Private declarations }
    RunState : Boolean;
    CANdoAPIDLLFoundFlag : Boolean;
    CANdoDevices : array [0..9] of TCANdoDevice;
    BusLoadRequestTimer : Integer;
    procedure Wait(Delay : Longint);
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

var
  CANdoUSB : TCANdoUSB;  // Store for parameters relating to connected CANdo
  CANdoCANBuffer : TCANdoCANBuffer;  // Cyclic store for CAN messages collected from CANdo
  CANdoStatus : TCANdoStatus;  // Store for status message collected from CANdo

{$R *.lfm}

//--------------------------------------------------------------------------
procedure TForm1.FormCreate(Sender: TObject);
var
  Status : Integer;
  NoOfDevices, DeviceNo : Integer;
begin
  // Dynamically load libCANdo.so
  if CANdoInitialise then
  begin
    CANdoAPIDLLFoundFlag := True;
    // See how many devices available
    NoOfDevices := Length(CANdoDevices);  // Max. no. of devices to enumerate
    Status := CANdoGetDevices(@CANdoDevices, @NoOfDevices);
    if Status = CANDO_SUCCESS then
    begin
      // List the devices available
      ComboBox1.Clear;
      for DeviceNo := 0 to NoOfDevices - 1 do
      begin
        if CANdoDevices[DeviceNo].HardwareType = CANDO_TYPE_CANDO then
          ComboBox1.Items.Add('CANdo S/N ' + CANdoDevices[DeviceNo].SerialNo)
        else
        if CANdoDevices[DeviceNo].HardwareType = CANDO_TYPE_CANDOISO then
          ComboBox1.Items.Add('CANdoISO S/N ' + CANdoDevices[DeviceNo].SerialNo)
        else
        if CANdoDevices[DeviceNo].HardwareType = CANDO_TYPE_CANDO_AUTO then
          ComboBox1.Items.Add('CANdo AUTO S/N ' + CANdoDevices[DeviceNo].SerialNo)
        else
          ComboBox1.Items.Add('CANdo type unknown.');
      end;
      if ComboBox1.Items.Count > 0 then
        ComboBox1.ItemIndex := 0;
    end
    else
    if Status = CANDO_USB_DLL_ERROR then
      StatusBar1.SimpleText := 'CANdo API DLL not found'
    else
    if Status = CANDO_USB_DRIVER_ERROR then
      StatusBar1.SimpleText := 'CANdo driver not found';
  end
  else
  begin
    CANdoAPIDLLFoundFlag := False;
    StatusBar1.SimpleText := 'CANdo API DLL not found';
  end;

  RunState := False;  // Stopped
end;
//--------------------------------------------------------------------------
procedure TForm1.Exit1Click(Sender: TObject);
begin
  Close;
end;
//--------------------------------------------------------------------------
procedure TForm1.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  // Clean up ready to exit application
  if CANdoUSB.OpenFlag then
  begin
    // Close connection to the CANdo device
    CANdoSetState(@CANdoUSB, 0);  // Stop CANdo running
    CANdoClose(@CANdoUSB);  // Deallocate CANdo USB resources
  end;
  // Unload libCANdo.so
  CANdoFinalise;

  Action := caFree;
end;
//--------------------------------------------------------------------------
// Wait
//
// Wait for Delay ms before proceeding.
//
// Returns
//    Nothing
//--------------------------------------------------------------------------
procedure TForm1.Wait(Delay : Longint);
var
	ElapsedTime, CurrentTime : Longint;
begin
  ElapsedTime := GetTickCount;
  repeat
    Application.ProcessMessages;
    CurrentTime := GetTickCount;
  until Application.Terminated or ((CurrentTime - ElapsedTime) > Delay);
end;
//--------------------------------------------------------------------------
procedure TForm1.BitBtn1Click(Sender: TObject);
// Request status
begin
  if CANdoUSB.OpenFlag then
  begin
    if CANdoRequestStatus(@CANdoUSB) <> CANDO_SUCCESS then
      MessageDlg('Error communicating with CANdo.', mtWarning, [mbOK], 0);
  end;
end;
//--------------------------------------------------------------------------
procedure TForm1.BitBtn5Click(Sender: TObject);
// Get date
begin
  if CANdoUSB.OpenFlag then
  begin
    if CANdoRequestDateStatus(@CANdoUSB) <> CANDO_SUCCESS then
      MessageDlg('Error communicating with CANdo.', mtWarning, [mbOK], 0);
  end;
end;
//--------------------------------------------------------------------------
procedure TForm1.BitBtn6Click(Sender: TObject);
// Get version
var
  APIVersion, DLLVersion, DriverVersion : Cardinal;
begin
  if CANdoAPIDLLFoundFlag then
  begin
    CANdoGetVersion(@APIVersion, @DLLVersion, @DriverVersion);
    MessageDlg('CANdo API v'+Format('%.1f', [APIVersion / 10])+#13#10+
      'CANdo USB v'+Format('%.1f', [DLLVersion / 10])+#13#10+
      'CANdo Driver v'+Format('%.1f', [DriverVersion / 10]), mtInformation, [mbOK], 0);
  end;
end;
//--------------------------------------------------------------------------
procedure TForm1.CheckBox2Click(Sender: TObject);
var
  DataNo : Integer;
  DataBox : TEdit;
begin
  if CheckBox2.Checked then
  begin
    for DataNo := 0 to 7 do
    begin
      DataBox := TEdit(FindComponent('Edit'+IntToStr(DataNo + 6)));
      DataBox.Text := '';
      DataBox.Color := clBtnFace;
      DataBox.Enabled := False;
    end;
  end
  else
  begin
    for DataNo := 0 to 7 do
    begin
      DataBox := TEdit(FindComponent('Edit'+IntToStr(DataNo + 6)));
      DataBox.Text := '00';
      DataBox.Enabled := True;
      DataBox.Color := clWindow;
    end;
  end;
end;
//--------------------------------------------------------------------------
procedure TForm1.BitBtn2Click(Sender: TObject);
// Start CANdo
begin
  if ComboBox1.Items.Count > 0 then
  begin
    // CANdo device available, so connect
    if CANdoOpenDevice(@CANdoUSB, @CANdoDevices[ComboBox1.ItemIndex]) = CANDO_SUCCESS then
    begin
      // Connected, so configure
      // Set baud rate to 250k
      if CANdoSetBaudRate(@CANdoUSB, 0, 1, 7, 7, 2, 0) = CANDO_SUCCESS then
      begin
        Wait(100);  // Wait 100ms to allow CANdo to store baud rate in EEPROM, in case modified
        // Set mode to 'Normal'
        if CANdoSetMode(@CANdoUSB, CANDO_NORMAL_MODE) = CANDO_SUCCESS then
        begin
          Wait(10);  // Wait 10ms to allow CANdo to store mode in EEPROM, in case modified
          // Set filters to accept all messages
          if CANdoSetFilters(@CANdoUSB,
            0,
            CANDO_ID_29_BIT, 0,
            CANDO_ID_11_BIT, 0,
            0,
            CANDO_ID_29_BIT, 0,
            CANDO_ID_11_BIT, 0,
            CANDO_ID_29_BIT, 0,
            CANDO_ID_11_BIT, 0) = CANDO_SUCCESS then
            begin
              Wait(10);  // Wait 10ms to allow filters to be configured in CAN module
              // Flush USB buffers
              if CANdoFlushBuffers(@CANdoUSB) = CANDO_SUCCESS then
                // Set CANdo state to run
                if CANdoSetState(@CANdoUSB, CANDO_RUN) = CANDO_SUCCESS then
                begin
                  RunState := True;  // Running
                  if (CANdoDevices[ComboBox1.ItemIndex].HardwareType = CANDO_TYPE_CANDOISO) or
                    (CANdoDevices[ComboBox1.ItemIndex].HardwareType = CANDO_TYPE_CANDO_AUTO) then
                  begin
                    // CANdoISO or CANdo AUTO selected, so display bus load in status bar
                    StatusBar1.SimpleText := 'CAN Bus Load 0.0%';
                    BusLoadRequestTimer := 0;
                  end
                  else
                    StatusBar1.SimpleText := '';
                end;
            end;
        end;
      end;
    end
    else
      Label17.Caption := 'CANdo unavailable';

    if RunState then
    begin
      Memo1.Lines.Clear;
      Memo1.Lines.Add('29   ID    F L D1 D2 D3 D4 D5 D6 D7 D8 TIMESTAMP');
      Memo1.Lines.Add('------------------------------------------------');
      Timer1.Enabled := True;  // Enable receiver
      Label17.Caption := 'CANdo started @250k baud';
    end;
  end;
end;
//--------------------------------------------------------------------------
procedure TForm1.BitBtn4Click(Sender: TObject);
// Stop CANdo
begin
  if CANdoUSB.OpenFlag then
  begin
    if CANdoSetState(@CANdoUSB, CANDO_STOP) = CANDO_SUCCESS then
    begin
      if (CANdoDevices[ComboBox1.ItemIndex].HardwareType = CANDO_TYPE_CANDOISO) or
        (CANdoDevices[ComboBox1.ItemIndex].HardwareType = CANDO_TYPE_CANDO_AUTO) then
        StatusBar1.SimpleText := 'CAN Bus Load 0.0%';
      RunState := False;  // Stopped
      CANdoClose(@CANdoUSB);  // Close the connection
    end;
  end;

  if not RunState then
  begin
    Timer1.Enabled := False;  // Disable receiver
    Label17.Caption := 'CANdo stopped';
  end;
end;
//--------------------------------------------------------------------------
procedure TForm1.BitBtn3Click(Sender: TObject);
// Message transmit
var
  IDExtended, Frame : Byte;
  Data : array [0..7] of Byte;
begin
  if RunState then
  begin
    if CheckBox2.Checked then
    begin
      // Remote frame
      Data[0] := 0;
      Data[1] := 0;
      Data[2] := 0;
      Data[3] := 0;
      Data[4] := 0;
      Data[5] := 0;
      Data[6] := 0;
      Data[7] := 0;
      Frame := CANDO_REMOTE_FRAME
    end
    else
    begin
      // Data frame
      Data[0] := StrToInt('$'+Edit6.Text);
      Data[1] := StrToInt('$'+Edit7.Text);
      Data[2] := StrToInt('$'+Edit8.Text);
      Data[3] := StrToInt('$'+Edit9.Text);
      Data[4] := StrToInt('$'+Edit10.Text);
      Data[5] := StrToInt('$'+Edit11.Text);
      Data[6] := StrToInt('$'+Edit12.Text);
      Data[7] := StrToInt('$'+Edit13.Text);
      Frame := CANDO_DATA_FRAME;
    end;

    if Checkbox1.Checked then
      IDExtended := CANDO_ID_29_BIT
    else
      IDExtended := CANDO_ID_11_BIT;

    CANdoTransmit(@CANdoUSB, IDExtended, StrToInt('$'+Edit14.Text), Frame,
      SpinEdit1.Value, @Data, 0, 0);
  end
  else
    MessageDlg('CANdo stopped.', mtWarning, [mbOK], 0);
end;
//--------------------------------------------------------------------------
procedure TForm1.Timer1Timer(Sender: TObject);
// Message receive
begin
  // Collect any messages sent by CANdo
  if CANdoReceive(@CANdoUSB, @CANdoCANBuffer, @CANdoStatus) <> CANDO_SUCCESS then
    Memo1.Lines.Add('Error receiving CAN messages');

  // Check for CAN messages received
  while (CANdoCANBuffer.ReadIndex <> CANdoCANBuffer.WriteIndex) or CANdoCANBuffer.FullFlag do
  begin
    // Add message to list
    Memo1.Lines.Add(
      IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].IDE, 1)+' '+
      IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].ID, 8)+' '+
      IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].RTR, 1)+' '+
      IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].DLC, 1)+' '+
      IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[0], 2)+' '+
      IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[1], 2)+' '+
      IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[2], 2)+' '+
      IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[3], 2)+' '+
      IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[4], 2)+' '+
      IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[5], 2)+' '+
      IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[6], 2)+' '+
      IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[7], 2)+' '+
      IntToStr(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].TimeStamp));
    // Move read pointer onto next slot in cyclic buffer
    if (CANdoCANBuffer.ReadIndex + 1) < CANDO_CAN_BUFFER_LENGTH then
      Inc(CANdoCANBuffer.ReadIndex)  // Increment index onto next free slot
    else
      CANdoCANBuffer.ReadIndex := 0;  // Wrap back to start

    CANdoCANBuffer.FullFlag := False;  // Clear flag as buffer is no full
  end;

  // Check to see if a new status message sent
  if CANdoStatus.NewFlag <> CANDO_NO_STATUS then
  begin
    case CANdoStatus.NewFlag of
      CANDO_DEVICE_STATUS :
      begin
        // New device status message received
        Edit1.Text := IntToStr(CANdoStatus.HardwareVersion);
        Edit2.Text := IntToStr(CANdoStatus.SoftwareVersion);
        Edit3.Text := IntToHex(CANdoStatus.Status, 2);
        Edit4.Text := IntToHex(CANdoStatus.BusState, 2);
        Edit5.Text := IntToStr(CANdoStatus.TimeStamp);
      end;

      CANDO_DATE_STATUS :
      begin
        // New date status message received
        Label19.Caption := IntToStr(CANdoStatus.SoftwareVersion)+'/'+
          IntToStr(CANdoStatus.Status)+'/'+IntToStr(CANdoStatus.BusState);
      end;

      CANDO_BUS_LOAD_STATUS :
      begin
        // New bus load status message received
        StatusBar1.SimpleText := 'CAN Bus Load '+
          Format('%.1f', [(CANdoStatus.HardwareVersion * 10 + CANdoStatus.SoftwareVersion) / 10.0])+'%';
      end;
    end;
    CANdoStatus.NewFlag := CANDO_NO_STATUS;  // Clear flag to indicate status read
  end;

  if (CANdoDevices[ComboBox1.ItemIndex].HardwareType = CANDO_TYPE_CANDOISO) or
    (CANdoDevices[ComboBox1.ItemIndex].HardwareType = CANDO_TYPE_CANDO_AUTO) then
  begin
    // CANdoISO or CANdo AUTO connected, so request bus load status every second
    // Note : Only CANdoISO & CANdo AUTO calculate the CAN bus load
    Inc(BusLoadRequestTimer);
    if BusLoadRequestTimer = ONE_SECOND then
    begin
      // 1s elapsed since last bus load status request
      CANdoRequestBusLoadStatus(@CANdoUSB);  // Request bus load status
      BusLoadRequestTimer := 0;  // Reset timer
    end;
  end;
end;
//--------------------------------------------------------------------------
end.
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------