unit DrawMandelbrot00;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls;

type
  TForm1 = class(TForm)
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    Label7: TLabel;
    Label8: TLabel;
    Label9: TLabel;
    Label10: TLabel;
    Label11: TLabel;
    Label12: TLabel;
    Label13: TLabel;
    Label14: TLabel;
    Button1: TButton;
    Button2: TButton;
    procedure FormCreate(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
  private
    { Private êÈåæ }
  public
    { Public êÈåæ }
  end;

var
  Form1: TForm1;

implementation

uses
  DrawMandelbrot10;

{$R *.DFM}

procedure TForm1.FormCreate(Sender: TObject);
begin
  Left := 0;
  Top  := 0;
  Form2 := nil;
end;

procedure TForm1.Button1Click(Sender: TObject);
var
  XLS, XLDS, YLS, YLDS: Extended;
  XLL, YLL            : Extended;
begin
  XLS  := -0.5;
  XLDS :=  0.0;
  YLS  := -1.2;
  YLDS :=  0.0;
  XLL  :=  2.4;
  YLL  :=  2.4;
  if ParamStr(1) <> '' then
    EncodeExtPair(ParamStr(1), XLS, XLDS);
  if ParamStr(2) <> '' then
    EncodeExtPair(ParamStr(2), YLS, YLDS);
  if ParamStr(3) <> '' then
    XLL  := StrToFloat(ParamStr(3));
  if ParamStr(4) <> '' then
    YLL  := StrToFloat(ParamStr(4));
  Form2 := TForm2.Create(Self);
  Form2.ShowDialog(1000, 64,
                   (Left + 20), (Top + 20), XLS, XLDS, YLS, YLDS, XLL, YLL);
end;

procedure TForm1.Button2Click(Sender: TObject);
begin
  Close;
end;

procedure TForm1.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  if Form2 <> nil then
    begin
      Form2.ReqToClose;
      Form2 := nil;
    end;
end;

end.
