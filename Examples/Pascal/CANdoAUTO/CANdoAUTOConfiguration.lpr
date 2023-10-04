program CANdoAUTOConfiguration;

uses
  Forms, Interfaces,
  Main in 'Main.pas' {Form1},
  CANdoImport in 'CANdoImport.pas',
  IEEE754 in 'IEEE754.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.Title := 'Lazarus libCANdo.so Example';
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.