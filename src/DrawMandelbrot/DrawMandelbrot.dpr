program DrawMandelbrot;

uses
  Forms,
  DrawMandelbrot00 in 'DrawMandelbrot00.pas' {Form1},
  DrawMandelbrot10 in 'DrawMandelbrot10.pas' {Form2};

{$R *.RES}

begin
  Application.Initialize;
  Application.Title := 'マンデルブロー集合模様描画';
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
