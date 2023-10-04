//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// TITLE : CANdoAUTOConfiguration main routines - Main.pas
//
// DESCRIPTION : Lazarus example program to demonstrate using libCANdo.so to
//  configure the non-volatile stores in the CANdo AUTO device.
//
// DATE : 27/05/14
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
  Dialogs, Menus, ComCtrls, StdCtrls, Buttons, ExtCtrls, CANdoImport, IEEE754;

const
  // CANdo bit or'ed internal status codes
  CANDO_OK = $00;
  CANDO_UART_RX_OVERRUN = $01;
  CANDO_UART_RX_CORRUPTED = $02;
  CANDO_UART_RX_CRC_ERROR = $04;
  CANDO_CAN_RX_NO_DATA = $08;
  CANDO_CAN_RX_OVERRUN = $10;
  CANDO_CAN_RX_INVALID = $20;
  CANDO_CAN_TX_OVERRUN = $40;
  CANDO_CAN_BUS_ERROR = $80;
  // CAN transmit repeat time
  REPEAT_TIME_OFF = 0;
  REPEAT_TIME_10MS = 1;
  REPEAT_TIME_20MS = 2;
  REPEAT_TIME_50MS = 3;
  REPEAT_TIME_100MS = 4;
  REPEAT_TIME_200MS = 5;
  REPEAT_TIME_500MS = 6;
  REPEAT_TIME_1000MS = 7;
  REPEAT_TIME_2000MS = 8;
  REPEAT_TIME_5000MS = 9;
  REPEAT_TIME_10000MS = 10;
  REPEAT_TIME : array [0..10] of String = ('Off (No transmission)', '10ms', '20ms', '50ms', '100ms', '200ms', '500ms', '1000ms', '2000ms', '5000ms', '10000ms');

type
  TCANData = array [0..7] of Byte;

  TCANTransmitMessage = record
    EnabledFlag : Boolean;
    IDE : Byte;
    ID : Cardinal;
    DLC : Byte;
    Data : TCANData;
    PeriodIndex : Cardinal;
    Comment : String;
  end;

  TCANTransmit = record
    Count : Integer;
    List : array [0..CANDO_AUTO_MAX_NO_OF_TX_ITEMS] of TCANTransmitMessage;  // Transmit list
  end;

  TForm1 = class(TForm)
    MainMenu1: TMainMenu;
    File1: TMenuItem;
    Exit1: TMenuItem;
    StatusBar1: TStatusBar;
    Panel1: TPanel;
    Memo1: TMemo;
    GroupBox1: TGroupBox;
    GroupBox2: TGroupBox;
    Button1: TButton;
    PopupMenu1: TPopupMenu;
    Clear1: TMenuItem;
    Button2: TButton;
    Button3: TButton;
    Button4: TButton;
    Button5: TButton;
    Button6: TButton;
    View1: TMenuItem;
    Clear2: TMenuItem;
    procedure Exit1Click(Sender: TObject);
    procedure Clear1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure Button5Click(Sender: TObject);
    procedure Button6Click(Sender: TObject);
    procedure Clear2Click(Sender: TObject);
  private
    { Private declarations }
    procedure Wait(Delay : Cardinal);
    procedure CANdoConnect;
    procedure CANdoDisconnect;
    function DecodeStatus(StatusCode : Integer) : String;
    procedure WriteAnalogInputStore;
    procedure ReadAnalogInputStore;
    procedure WriteCANTransmitStore;
    procedure ReadCANTransmitStore;

  public
    { Public declarations }
  end;

var
  Form1: TForm1;
  CANdoDevice : TCANdoDevice;  // H/W type & S/N of CANdo connected
  CANdoUSB : TCANdoUSB;  // Store for parameters relating to CANdo
  CANdoCANBuffer : TCANdoCANBuffer;  // Cyclic store for CAN messages collected from CANdo
  CANdoStatus : TCANdoStatus;  // Store for status messages collected from CANdo
  CANTransmit : TCANTransmit;  // List of CAN transmit messages

implementation

{$R *.dfm}

//--------------------------------------------------------------------------
procedure TForm1.FormCreate(Sender: TObject);
begin
  CANdoConnect;  // Open connection to CANdo AUTO device
end;
//--------------------------------------------------------------------------
procedure TForm1.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  CANdoDisconnect;  // Disconnect from CANdo AUTO device
  Action := caFree;  // OK to exit
end;
//--------------------------------------------------------------------------
procedure TForm1.Clear1Click(Sender: TObject);
begin
  Memo1.Clear;
end;
//--------------------------------------------------------------------------
procedure TForm1.Clear2Click(Sender: TObject);
begin
  Memo1.Clear;
end;
//--------------------------------------------------------------------------
procedure TForm1.Exit1Click(Sender: TObject);
begin
  Close;
end;
//--------------------------------------------------------------------------
// Wait
//
// Wait for Delay ms before proceeding.
//
// Returns
//    Nothing
//--------------------------------------------------------------------------
procedure TForm1.Wait(Delay : Cardinal);
var
  ElapsedTime, CurrentTime : Cardinal;
begin
  ElapsedTime := GetTickCount;
  repeat
    Application.ProcessMessages;
    CurrentTime := GetTickCount;
  until Application.Terminated or ((CurrentTime - ElapsedTime) > Delay);
end;
//--------------------------------------------------------------------------
procedure TForm1.CANdoConnect;
var
  Status : Integer;
  CANdoConnected : String;
begin
  // Dynamically load libCANdo.so
  if CANdoInitialise then
  begin
    CANdoDevice.HardwareType := CANDO_TYPE_CANDO_AUTO;  // Select CANdo AUTO type device only
    CANdoDevice.SerialNo := '';  // Any S/N
    Status := CANdoOpenDevice(@CANdoUSB, @CANdoDevice);  // Open connection
    if (Status = CANDO_SUCCESS) and CANdoUSB.OpenFlag then
    begin
      // CANdo AUTO connected
      // Display connected device description & S/N
      CANdoConnected := String(CANdoUSB.Description)+'    S/N '+String(CANdoUSB.SerialNo);
      // Request status containing version no.s
      if CANdoRequestStatus(@CANdoUSB) = CANDO_SUCCESS then
      begin
        Wait(10);  // Wait 10ms for a reply from CANdo
        CANdoStatus.NewFlag := CANDO_NO_STATUS;
        if CANdoReceive(@CANdoUSB, @CANdoCANBuffer, @CANdoStatus) = CANDO_SUCCESS then
        begin
          if CANdoStatus.NewFlag = CANDO_DEVICE_STATUS then
          begin
            // Display H/W & S/W version no.s
            CANdoConnected := CANdoConnected+'    H/W v'+IntToStr(CANdoStatus.HardwareVersion div 10)+'.'+
              IntToStr(CANdoStatus.HardwareVersion mod 10)+'    S/W v'+IntToStr(CANdoStatus.SoftwareVersion div 10)+'.'+
              IntToStr(CANdoStatus.SoftwareVersion mod 10);
            // Display status
            StatusBar1.SimpleText := CANdoConnected+'    Status '+DecodeStatus(CANdoStatus.Status);
            CANdoStatus.NewFlag := CANDO_NO_STATUS;  // Clear to indicate status retrieved
            // Stop all transmissions to allow configuration
            CANdoSetState(@CANdoUSB, CANDO_STOP);
          end
          else
            StatusBar1.SimpleText := CANdoConnected+'    Status unknown';
        end
        else
          StatusBar1.SimpleText := CANdoConnected+'    Status no response';
      end
      else
        StatusBar1.SimpleText := CANdoConnected+'    Status request failed';
    end
    else
    if Status = CANDO_USB_DLL_ERROR then
      // libCANdo.so not found or version incorrect
      StatusBar1.SimpleText := 'CANdo API library not found'
    else
    if Status = CANDO_USB_DRIVER_ERROR then
      // CANdo driver not found or version incorrect
      StatusBar1.SimpleText := 'Netronics CANdo AUTO driver not found'
    else
      StatusBar1.SimpleText := 'Netronics CANdo AUTO not found';
  end
  else
    StatusBar1.SimpleText := 'CANdo API library not found';
end;
//--------------------------------------------------------------------------
procedure TForm1.CANdoDisconnect;
begin
  if CANdoUSB.OpenFlag then
  begin
    CANdoSetState(@CANdoUSB, CANDO_STOP);  // Stop CANdo AUTO running
    CANdoClose(@CANdoUSB);  // Close connection to CANdo AUTO device
  end;
  // Unload libCANdo.so
  CANdoFinalise;

  // Clear status bar
  StatusBar1.SimpleText := '';
end;
//--------------------------------------------------------------------------
function TForm1.DecodeStatus(StatusCode : Integer) : String;
begin
  case StatusCode of
    CANDO_OK : Result := 'OK';
    CANDO_UART_RX_OVERRUN : Result := 'USB RX OVERRUN';
    CANDO_UART_RX_CORRUPTED : Result := 'USB RX CORRUPTED';
    CANDO_UART_RX_CRC_ERROR : Result := 'USB CRC ERROR';
    CANDO_CAN_RX_NO_DATA : Result := 'CAN RX NO DATA';
    CANDO_CAN_RX_OVERRUN : Result := 'CAN RX OVERRUN';
    CANDO_CAN_RX_INVALID : Result := 'CAN RX INVALID';
    CANDO_CAN_TX_OVERRUN : Result := 'CAN TX OVERRUN';
    CANDO_CAN_BUS_ERROR : Result := 'CAN BUS ERROR';
  else
    Result := IntToHex(StatusCode, 2);  // Multiple status codes present
  end;
end;
//--------------------------------------------------------------------------
procedure TForm1.Button1Click(Sender: TObject);
begin
  WriteAnalogInputStore;
end;
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
procedure TForm1.WriteAnalogInputStore;
const
  EEC1 = $18F00400;  // J1939 EEC1 ID, source address = 0
  ET1 = $18FEEE00;  // J1939 ET1 ID, source address = 0
  PADDING = $FF;  // J1939 padding value
begin
  if CANdoUSB.OpenFlag then
  begin
    // Device connected
    if CANdoAnalogStoreClear(@CANdoUSB) = CANDO_SUCCESS then
    begin
      Wait(50);  // Wait 50ms for store to erase

      // Configure V1 input to transmit every 200ms
      if CANdoAnalogStoreWrite(@CANdoUSB, CANDO_AUTO_V1_INPUT, CANDO_ID_29_BIT,
        EEC1, 4, 2, 8, 0, PADDING, REPEAT_TIME_200MS) <> CANDO_SUCCESS then
        MessageDlg('Error configuring V1 I/P.', mtError, [mbOK], 0)
      else
      begin
        Wait(100);  // Wait 100ms to store
        Memo1.Lines.Add('V1 Input configured ID = 0x18F00400 with 200ms period');
      end;

      // Configure V2 input to transmit every 1s
      if CANdoAnalogStoreWrite(@CANdoUSB, CANDO_AUTO_V2_INPUT, CANDO_ID_29_BIT,
        ET1, 1, 1, 0.01, 40, PADDING, REPEAT_TIME_1000MS) <> CANDO_SUCCESS then
        MessageDlg('Error configuring V2 I/P.', mtError, [mbOK], 0)
      else
      begin
        Wait(100);  // Wait 100ms to store
        Memo1.Lines.Add('V2 Input configured ID = 0x18FEEE00 with 1s period');
      end;

      // Configure CAN bus baud rate
      if CANdoSetBaudRate(@CANdoUSB, 0, 4 + CANDO_BRP_ENHANCED_OFFSET, 7, 5, 4, 0) <> CANDO_SUCCESS then
        MessageDlg('Error setting baud rate.', mtError, [mbOK], 0)
      else
      begin
        Wait(10);  // Wait 10ms to store
        Memo1.Lines.Add('CAN baud rate set to 250k bits/s');
      end;

      Memo1.Lines.Add('');
      Memo1.Lines.Add('Cycle power to CANdo AUTO device for settings to take effect');
      Memo1.Lines.Add('');
    end
    else
      MessageDlg('Error erasing analogue store.', mtError, [mbOK], 0)
  end;
end;
//--------------------------------------------------------------------------
procedure TForm1.Button2Click(Sender: TObject);
begin
  ReadAnalogInputStore;
end;
//--------------------------------------------------------------------------
// ReadAnalogInputStore
//
// This procedure reads the configuration contained within the analogue input
// store & displays the settings. This procedure is useful to verify the
// contents of the store following a write.
//
// Returns
//    Nothing
//--------------------------------------------------------------------------
procedure TForm1.ReadAnalogInputStore;
var
  InputNo, FloatValue : Integer;
  BRP, PROPSEG, PHSEG1, PHSEG2 : Byte;
begin
  if CANdoUSB.OpenFlag then
  begin
    // Clear receive buffer ready to read analog store config.
    if CANdoFlushBuffers(@CANdoUSB) = CANDO_SUCCESS then
    begin
      CANdoCANBuffer.ReadIndex := CANdoCANBuffer.WriteIndex;  // Discard any pending messages
      CANdoCANBuffer.FullFlag := False;  // Buffer empty
      // Issue analog store read & setup status commands
      if (CANdoAnalogStoreRead(@CANdoUSB) = CANDO_SUCCESS) and (CANdoRequestSetupStatus(@CANdoUSB) = CANDO_SUCCESS) then
      begin
        Wait(50);  // Wait 50ms for CANdo AUTO to reply
        InputNo := CANDO_AUTO_V1_INPUT;
        CANdoStatus.NewFlag := CANDO_NO_STATUS;
        // Collect analog store configuration sent by CANdo AUTO
        if CANdoReceive(@CANdoUSB, @CANdoCANBuffer, @CANdoStatus) = CANDO_SUCCESS then
        begin
          while (CANdoCANBuffer.ReadIndex <> CANdoCANBuffer.WriteIndex) or CANdoCANBuffer.FullFlag do
          begin
            Memo1.Lines.Add('V'+IntToStr(InputNo)+' input configuration -');
            // ID
            if CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].IDE = CANDO_ID_11_BIT then
              Memo1.Lines.Add('11 bit ID = 0x'+IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].ID, 3))  // 11 bit ID
            else
              Memo1.Lines.Add('29 bit ID = 0x'+IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].ID, 8));  // 29 bit ID
            // Start, length & padding bytes
            Memo1.Lines.Add('Start byte no. = '+IntToStr(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[0]));  // Start
            Memo1.Lines.Add('No. of bytes = '+IntToStr(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[1]));  // Length
            Memo1.Lines.Add('Padding byte = '+IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].BusState, 2));  // Padding
            // Scaling factor & offset
            FloatValue := CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[2] +
              (CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[3] shl 8) +
              (CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[4] shl 16);
            Memo1.Lines.Add('Scaling factor = '+FloatToStr(IEEE754ToFloat(FloatValue)));  // Scaling factor
            FloatValue := CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[5] +
              (CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[6] shl 8) +
              (CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[7] shl 16);
            Memo1.Lines.Add('Offset = '+FloatToStr(IEEE754ToFloat(FloatValue)));  // Offset
            // Repeat time
            if CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].TimeStamp < sizeof(REPEAT_TIME) then
              Memo1.Lines.Add('Repeat time = '+REPEAT_TIME[CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].TimeStamp]);
            Memo1.Lines.Add('');
            Inc(InputNo);  // Next input
            // Move read pointer onto next slot in cyclic buffer
            if (CANdoCANBuffer.ReadIndex + 1) < CANDO_CAN_BUFFER_LENGTH then
              Inc(CANdoCANBuffer.ReadIndex)  // Increment index onto next free slot
            else
              CANdoCANBuffer.ReadIndex := 0;  // Wrap back to start
            CANdoCANBuffer.FullFlag := False;  // Clear full flag as space now exists
          end;

          // CAN baud rate
          if CANdoStatus.NewFlag = CANDO_SETUP_STATUS then
          begin
            // Display CAN baud rate settings
            BRP := CANdoStatus.HardwareVersion;
            Inc(BRP);  // Add 1 as value is stored 0 based
            PROPSEG := CANdoStatus.Status;
            PROPSEG := PROPSEG shr 4;
            Inc(PROPSEG);  // Add 1 as value is stored 0 based
            PHSEG1 := CANdoStatus.SoftwareVersion;
            PHSEG1 := PHSEG1 And $0F;
            Inc(PHSEG1);  // Add 1 as value is stored 0 based
            PHSEG2 := CANdoStatus.SoftwareVersion;
            PHSEG2 := PHSEG2 shr 4;
            Inc(PHSEG2);  // Add 1 as value is stored 0 based
            Memo1.Lines.Add('CAN baud rate configuration -');
            Memo1.Lines.Add('BRP = '+IntToStr(BRP));
            Memo1.Lines.Add('PROPSEG = '+IntToStr(PROPSEG));
            Memo1.Lines.Add('PHSEG1 = '+IntToStr(PHSEG1));
            Memo1.Lines.Add('PHSEG2 = '+IntToStr(PHSEG2));
            Memo1.Lines.Add('SJW = '+IntToStr(((CANdoStatus.Status shr 1) and $03) + 1));
            if (CANdoStatus.Status and $01) = $00 then
              Memo1.Lines.Add('SAM = 1')
            else
              Memo1.Lines.Add('SAM = 3');
            Memo1.Lines.Add('Baud Rate = '+FloatToStr(CANDO_CLK_FREQ_HIGH / (2 * BRP * (1 + PROPSEG + PHSEG1 + PHSEG2)))+'k');

            CANdoStatus.NewFlag := CANDO_NO_STATUS;  // Indicate status read
          end;
          Memo1.Lines.Add('');
        end;
      end;
    end;
  end;
end;
//--------------------------------------------------------------------------
procedure TForm1.Button3Click(Sender: TObject);
begin
  if CANdoUSB.OpenFlag then
  begin
    // Device connected, so clear analogue I/P store
    if CANdoAnalogStoreClear(@CANdoUSB) = CANDO_SUCCESS then
    begin
      Wait(50);  // Wait 50ms for store to erase
      Memo1.Lines.Add('Analog input store erased');
      Memo1.Lines.Add('');
    end
    else
      MessageDlg('Error erasing analog input store.', mtError, [mbOK], 0)
  end;
end;
//--------------------------------------------------------------------------
procedure TForm1.Button4Click(Sender: TObject);
begin
  WriteCANTransmitStore;
end;
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
procedure TForm1.WriteCANTransmitStore;
const
  EHR : Cardinal = $18FEE500;  // J1939 Engine hours/revolutions ID, source address = 0
  FD : Cardinal = $18FEBD00;  // J1939 Fan drive ID, source address = 0
  ET : Cardinal = $18FE0700;  // J1939 Exhaust temperature ID, source address = 0
  TOTAL_ENGINE_HOURS : array [1..8] of Byte = ($00, $00, $AB, $E0, $FF, $FF, $FF, $FF);
  FAN_SPEED : array [1..8] of Byte = ($FF, $FF, $12, $C0, $FF, $FF, $FF, $FF);
  EXHAUST_TEMP : array [1..8] of Byte = ($48, $E0, $4A, $C0, $FF, $FF, $FF, $FF);
begin
  if CANdoUSB.OpenFlag then
  begin
    // Device connected
    if CANdoTransmitStoreClear(@CANdoUSB) = CANDO_SUCCESS then
    begin
      Wait(50);  // Wait 50ms for store to erase

      // Message 1
      if CANdoTransmitStoreWrite(@CANdoUSB, CANDO_ID_29_BIT, EHR,
        CANDO_DATA_FRAME, 8, @TOTAL_ENGINE_HOURS, REPEAT_TIME_2000MS) <> CANDO_SUCCESS then
        MessageDlg('Error writing to CAN transmit store.', mtError, [mbOK], 0)
      else
      begin
        Wait(100);  // Wait 100ms
        Memo1.Lines.Add('CAN transmit message 1 configured -');
        Memo1.Lines.Add('ID = $18FEE500');
        Memo1.Lines.Add('Repeat time = 2000ms');
        Memo1.Lines.Add('');
      end;

      // Message 2
      if CANdoTransmitStoreWrite(@CANdoUSB, CANDO_ID_29_BIT, FD,
        CANDO_DATA_FRAME, 8, @FAN_SPEED, REPEAT_TIME_500MS) <> CANDO_SUCCESS then
        MessageDlg('Error writing to CAN transmit store.', mtError, [mbOK], 0)
      else
      begin
        Wait(100);  // Wait 100ms
        Memo1.Lines.Add('CAN transmit message 2 configured -');
        Memo1.Lines.Add('ID = $18FEBD00');
        Memo1.Lines.Add('Repeat time = 500ms');
        Memo1.Lines.Add('');
      end;

      // Message 3
      if CANdoTransmitStoreWrite(@CANdoUSB, CANDO_ID_29_BIT, ET,
        CANDO_DATA_FRAME, 8, @EXHAUST_TEMP, REPEAT_TIME_1000MS) <> CANDO_SUCCESS then
        MessageDlg('Error writing to CAN transmit store.', mtError, [mbOK], 0)
      else
      begin
        Wait(100);  // Wait 100ms
        Memo1.Lines.Add('CAN transmit message 3 configured -');
        Memo1.Lines.Add('ID = $18FE0700');
        Memo1.Lines.Add('Repeat time = 1000ms');
        Memo1.Lines.Add('');
      end;

      // Configure CAN bus baud rate
      if CANdoSetBaudRate(@CANdoUSB, 0, 4 + CANDO_BRP_ENHANCED_OFFSET, 7, 5, 4, 0) <> CANDO_SUCCESS then
        MessageDlg('Error setting baud rate.', mtError, [mbOK], 0)
      else
      begin
        Wait(10);  // Wait 10ms to store
        Memo1.Lines.Add('CAN baud rate set to 250k bits/s');
      end;

      Memo1.Lines.Add('');
      Memo1.Lines.Add('Cycle power to CANdo AUTO device for settings to take effect');
      Memo1.Lines.Add('');
    end
    else
      MessageDlg('Error erasing CAN transmit store.', mtError, [mbOK], 0);
  end;
end;
//--------------------------------------------------------------------------
procedure TForm1.Button5Click(Sender: TObject);
begin
  ReadCANTransmitStore;
end;
//--------------------------------------------------------------------------
// ReadCANTransmitStore
//
// This procedure reads the CAN transmit messages stored in the CAN transmit
// store & displays them as a list. This can be useful to verify the contents
// of the store following a write.
//
// Returns
//    Nothing
//--------------------------------------------------------------------------
procedure TForm1.ReadCANTransmitStore;
var
  MessageNo, DataNo : Integer;
  Data : String;
  BRP, PROPSEG, PHSEG1, PHSEG2 : Byte;
begin
  if CANdoUSB.OpenFlag then
  begin
    // Clear receive buffer ready to read CAN transmit store config.
    if CANdoFlushBuffers(@CANdoUSB) = CANDO_SUCCESS then
    begin
      CANdoCANBuffer.ReadIndex := CANdoCANBuffer.WriteIndex;  // Discard any pending messages
      CANdoCANBuffer.FullFlag := False;  // Buffer empty
      // Issue transmit store read & setup status commands
      if (CANdoTransmitStoreRead(@CANdoUSB) = CANDO_SUCCESS) and (CANdoRequestSetupStatus(@CANdoUSB) = CANDO_SUCCESS) then
      begin
        Wait(50);  // Wait 50ms for CANdo AUTO to reply
        CANdoStatus.NewFlag := CANDO_NO_STATUS;
        // Collect analog store configuration sent by CANdo AUTO
        if CANdoReceive(@CANdoUSB, @CANdoCANBuffer, @CANdoStatus) = CANDO_SUCCESS then
        begin
          MessageNo := 1;
          while (CANdoCANBuffer.ReadIndex <> CANdoCANBuffer.WriteIndex) or CANdoCANBuffer.FullFlag do
          begin
            Memo1.Lines.Add('CAN transmit message '+IntToStr(MessageNo)+' configuration -');
            // ID
            if CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].IDE = CANDO_ID_11_BIT then
              Memo1.Lines.Add('11 bit ID = 0x'+IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].ID, 3))  // 11 bit ID
            else
              Memo1.Lines.Add('29 bit ID = 0x'+IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].ID, 8));  // 29 bit ID
            // DLC
            Memo1.Lines.Add('DLC = '+IntToStr(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].DLC));  // DLC
            // Data
            Data := '';
            for DataNo := 0 to 6 do
              Data := Data+'0x'+IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[DataNo], 2)+', ';
            Data := Data+'0x'+IntToHex(CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].Data[7], 2);
            Memo1.Lines.Add('Data = '+Data);
            // Repeat time
            if CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].BusState < sizeof(REPEAT_TIME) then
              Memo1.Lines.Add('Repeat time = '+REPEAT_TIME[CANdoCANBuffer.CANMessage[CANdoCANBuffer.ReadIndex].BusState]);
            Memo1.Lines.Add('');
            Inc(MessageNo);
            // Move read pointer onto next slot in cyclic buffer
            if (CANdoCANBuffer.ReadIndex + 1) < CANDO_CAN_BUFFER_LENGTH then
              Inc(CANdoCANBuffer.ReadIndex)  // Increment index onto next free slot
            else
              CANdoCANBuffer.ReadIndex := 0;  // Wrap back to start
            CANdoCANBuffer.FullFlag := False;  // Clear full flag as space now exists
          end;

          if MessageNo = 1 then
            Memo1.Lines.Add('CAN transmit store is empty'+#13#10);  // No messages in transmit store

          // CAN baud rate
          if CANdoStatus.NewFlag = CANDO_SETUP_STATUS then
          begin
            // Display CAN baud rate settings
            // Display CAN baud rate settings
            BRP := CANdoStatus.HardwareVersion;
            Inc(BRP);  // Add 1 as value is stored 0 based
            PROPSEG := CANdoStatus.Status;
            PROPSEG := PROPSEG shr 4;
            Inc(PROPSEG);  // Add 1 as value is stored 0 based
            PHSEG1 := CANdoStatus.SoftwareVersion;
            PHSEG1 := PHSEG1 And $0F;
            Inc(PHSEG1);  // Add 1 as value is stored 0 based
            PHSEG2 := CANdoStatus.SoftwareVersion;
            PHSEG2 := PHSEG2 shr 4;
            Inc(PHSEG2);  // Add 1 as value is stored 0 based
            Memo1.Lines.Add('CAN baud rate configuration -');
            Memo1.Lines.Add('BRP = '+IntToStr(BRP));
            Memo1.Lines.Add('PROPSEG = '+IntToStr(PROPSEG));
            Memo1.Lines.Add('PHSEG1 = '+IntToStr(PHSEG1));
            Memo1.Lines.Add('PHSEG2 = '+IntToStr(PHSEG2));
            Memo1.Lines.Add('SJW = '+IntToStr(((CANdoStatus.Status shr 1) and $03) + 1));
            if (CANdoStatus.Status and $01) = $00 then
              Memo1.Lines.Add('SAM = 1')
            else
              Memo1.Lines.Add('SAM = 3');
            Memo1.Lines.Add('Baud Rate = '+FloatToStr(CANDO_CLK_FREQ_HIGH / (2 * BRP * (1 + PROPSEG + PHSEG1 + PHSEG2)))+'k');

            CANdoStatus.NewFlag := CANDO_NO_STATUS;  // Indicate status read
          end;
          Memo1.Lines.Add('');
        end;
      end;
    end;
  end;
end;
//--------------------------------------------------------------------------
procedure TForm1.Button6Click(Sender: TObject);
begin
  if CANdoUSB.OpenFlag then
  begin
    // Device connected
    if CANdoTransmitStoreClear(@CANdoUSB) = CANDO_SUCCESS then
    begin
      Wait(50);  // Wait 50ms for store to erase
      Memo1.Lines.Add('CAN transmit store erased');
      Memo1.Lines.Add('');
    end
    else
      MessageDlg('Error erasing CAN transmit store.', mtError, [mbOK], 0)
  end;
end;
//--------------------------------------------------------------------------
end.
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------