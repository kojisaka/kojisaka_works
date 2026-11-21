program BilliardSimulator;

uses
  Forms,
  BilliardSimulator00 in 'BilliardSimulator00.pas' {Form1};

{$R *.res}

begin
  Application.Initialize;
  Application.Title := 'ビリヤードシミュレーター';
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
