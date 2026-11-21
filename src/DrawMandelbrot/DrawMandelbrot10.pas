unit DrawMandelbrot10;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs;

const
  XPEMax = 400;
  YPEMax = 400;

type
  TForm2 = class(TForm)
    procedure FormMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure FormMouseMove(Sender: TObject; Shift: TShiftState; X,
      Y: Integer);
    procedure FormMouseUp(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure FormPaint(Sender: TObject);
    procedure FormKeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
  private
    { Private êÈåæ }
    CountLimit  : Integer ;
    CalcPrec    : Integer ;
    IsReqToBreak: Boolean ;
    XPDown      : Integer ;
    YPDown      : Integer ;
    XPMove      : Integer ;
    YPMove      : Integer ;
    XLS         : Extended;
    XLDS        : Extended;
    YLS         : Extended;
    YLDS        : Extended;
    XLL         : Extended;
    YLL         : Extended;
    Step        : Extended;
    XPE         : Integer ;
    YPE         : Integer ;
    ColorTbl    : array[0..XPEMax, 0..YPEMax] of TColor;
    SubForm2    : TForm2  ;
    procedure DrawFrame(XP, YP: Integer);
    procedure DrawPoint(XP, YP: Integer; PenColor: TColor);
    function CalcCountToOver4(A, B: Extended): Integer;
    function CalcCountToOver4Exactly(A, DA, B, DB: Extended): Integer;
    procedure PaintAll(CntLimit: Integer; CPrec: Integer);
  public
    { Public êÈåæ }
  procedure ShowDialog(CntLimit, CPrec, X, Y: Integer; XLSA, XLDSA, YLSA, YLDSA, XLLA, YLLA: Extended);
  procedure ReqToClose;
  end;

procedure RealToExtInt(R: Extended; var X0, X1, X2, X3, X4, X5: Integer);
procedure MultExtInt(var A10, A8, A6, A4, A2, A0: Integer; B10, B8, B6, B4, B2, B0: Integer);
procedure AddExtIntToTimes(var CY, C11, C10, C9, C8, C7, C6, C5, C4, C3, C2, C1, C0: Integer;
                                   A11, A10, A9, A8, A7, A6, A5, A4, A3, A2, A1, A0, B: Integer);
procedure TwiceExtInt(var X0, X1, X2, X3, X4, X5: Integer);
procedure AddExtInt(var X0, X1, X2, X3, X4, X5: Integer; Y0, Y1, Y2, Y3, Y4, Y5: Integer);
procedure SubtractExtInt(var X0, X1, X2, X3, X4, X5: Integer; Y0, Y1, Y2, Y3, Y4, Y5: Integer);
procedure NegativeExtInt(var X0, X1, X2, X3, X4, X5: Integer);
function DecodeExtPair(V, DV: Extended; Len: Integer): String;
procedure EncodeExtPair(Text: String; var V, DV: Extended);

var
  Form2: TForm2;

implementation

{$R *.DFM}

uses
  Math;

procedure TForm2.ShowDialog(CntLimit, CPrec, X, Y: Integer; XLSA, XLDSA, YLSA, YLDSA, XLLA, YLLA: Extended);
begin
  SubForm2 := nil;
  IsReqToBreak := False;

  Left := X;
  Top  := Y;

  XLS  := XLSA ;
  XLDS := XLDSA;
  YLS  := YLSA ;
  YLDS := YLDSA;
  XLL  := XLLA ;
  YLL  := YLLA ;

  PaintAll(CntLimit, CPrec);
end;

procedure TForm2.ReqToClose;
begin
  Close;
  if SubForm2 <> nil then
    begin
      SubForm2.ReqToClose;
      SubForm2 := nil;
    end;
end;

procedure TForm2.FormPaint(Sender: TObject);
var
  XP          : Integer ;
  YP          : Integer ;
  CountToOver4: Integer ;
  Degree      : Integer ;
  PenColor    : TColor  ;
begin
  DrawFrame(XPE, YPE);

  for XP := 0 to XPE do
    begin
      Application.ProcessMessages;
      if IsReqToBreak then
        begin
          Close;
          exit;
        end;
      for YP := 0 to YPE do
        begin
          PenColor := ColorTbl[XP, YP];
          if PenColor < 0 then
            begin
              if CalcPrec <= 64 then
                begin
                  CountToOver4 := CalcCountToOver4((XLS + XLDS + Step * XP),
                                                   (YLS + YLDS + Step * YP) );
                end
              else
                begin
                  Application.ProcessMessages;
                  if IsReqToBreak then
                    begin
                      Close;
                      exit;
                    end;
                  CountToOver4 := CalcCountToOver4Exactly(XLS, (XLDS + Step * XP),
                                                          YLS, (YLDS + Step * YP) );
                end;
              if CountToOver4 >= MaxInt then
                PenColor := clBlack
              else
                begin
                  Degree := (Round(Ln(CountToOver4) * 1000) mod ($FF * 3));
                  if Degree <= $FF then
                    PenColor := RGB(Degree, ($FF - Degree), $FF)
                  else if Degree <= $FF * 2 then
                    PenColor := RGB($FF, (Degree - $FF), ($FF - (Degree - $FF)))
                  else
                    PenColor := RGB(($FF - (Degree - $FF * 2)), $FF, (Degree - $FF * 2));
                end;
              ColorTbl[XP, YP] := PenColor;
            end;
          DrawPoint(XP, YP, PenColor);
        end;
    end;
end;

procedure TForm2.DrawFrame(XP, YP: Integer);
begin
  Canvas.Brush.Color := clBtnFace;
  Canvas.FillRect(Rect(0, 0, XPEMax, YPEMax));
  Canvas.Pen.Mode  := pmCopy;
  Canvas.Pen.Color := clWhite;
  Canvas.Rectangle((-1), (-1), (XP + 2), (YP + 2));
end;

procedure TForm2.DrawPoint(XP, YP: Integer; PenColor: TColor);
begin
  Canvas.Pen.Mode  := pmCopy;
  Canvas.Pen.Color := PenColor;
  Canvas.MoveTo(XP,  YP     );
  Canvas.LineTo(XP, (YP + 1));
end;

function TForm2.CalcCountToOver4(A, B: Extended): Integer;
var
  X   : Extended;
  Y   : Extended;
  SqrX: Extended;
  SqrY: Extended;
  Idx : Integer ;
begin
  X := 0.0;
  Y := 0.0;
  SqrX := 0.0;
  SqrY := 0.0;
  for Idx := 1 to CountLimit do
    begin
      Y := 2.0 * X * Y - B;
      X := SqrX - SqrY - A;
      SqrX := X * X;
      SqrY := Y * Y;
      if SqrX + SqrY > 4.0 then
        begin
          Result := Idx;
          exit;
        end;
    end;
  Result := MaxInt;
end;

function TForm2.CalcCountToOver4Exactly(A, DA, B, DB: Extended): Integer;
var
  A0   : Integer;
  A1   : Integer;
  A2   : Integer;
  A3   : Integer;
  A4   : Integer;
  A5   : Integer;
  B0   : Integer;
  B1   : Integer;
  B2   : Integer;
  B3   : Integer;
  B4   : Integer;
  B5   : Integer;
  X0   : Integer;
  X1   : Integer;
  X2   : Integer;
  X3   : Integer;
  X4   : Integer;
  X5   : Integer;
  Y0   : Integer;
  Y1   : Integer;
  Y2   : Integer;
  Y3   : Integer;
  Y4   : Integer;
  Y5   : Integer;
  SqrX0: Integer;
  SqrX1: Integer;
  SqrX2: Integer;
  SqrX3: Integer;
  SqrX4: Integer;
  SqrX5: Integer;
  SqrY0: Integer;
  SqrY1: Integer;
  SqrY2: Integer;
  SqrY3: Integer;
  SqrY4: Integer;
  SqrY5: Integer;
  T0   : Integer;
  T1   : Integer;
  T2   : Integer;
  T3   : Integer;
  T4   : Integer;
  T5   : Integer;
  Idx  : Integer;
begin
  RealToExtInt( A, A0, A1, A2, A3, A4, A5);
  RealToExtInt(DA, T0, T1, T2, T3, T4, T5);
  AddExtInt(A0, A1, A2, A3, A4, A5, T0, T1, T2, T3, T4, T5);
  RealToExtInt( B, B0, B1, B2, B3, B4, B5);
  RealToExtInt(DB, T0, T1, T2, T3, T4, T5);
  AddExtInt(B0, B1, B2, B3, B4, B5, T0, T1, T2, T3, T4, T5);

  X0 := 0;
  X1 := 0;
  X2 := 0;
  X3 := 0;
  X4 := 0;
  X5 := 0;
  Y0 := 0;
  Y1 := 0;
  Y2 := 0;
  Y3 := 0;
  Y4 := 0;
  Y5 := 0;
  SqrX0 := 0;
  SqrX1 := 0;
  SqrX2 := 0;
  SqrX3 := 0;
  SqrX4 := 0;
  SqrX5 := 0;
  SqrY0 := 0;
  SqrY1 := 0;
  SqrY2 := 0;
  SqrY3 := 0;
  SqrY4 := 0;
  SqrY5 := 0;
  for Idx := 1 to CountLimit do
    begin
      MultExtInt(Y0, Y1, Y2, Y3, Y4, Y5, X0, X1, X2, X3, X4, X5);
      TwiceExtInt(Y0, Y1, Y2, Y3, Y4, Y5);
      SubtractExtInt(Y0, Y1, Y2, Y3, Y4, Y5, B0, B1, B2, B3, B4, B5);
      X0 := SqrX0;
      X1 := SqrX1;
      X2 := SqrX2;
      X3 := SqrX3;
      X4 := SqrX4;
      X5 := SqrX5;
      SubtractExtInt(X0, X1, X2, X3, X4, X5, SqrY0, SqrY1, SqrY2, SqrY3, SqrY4, SqrY5);
      SubtractExtInt(X0, X1, X2, X3, X4, X5, A0   , A1   , A2   , A3   , A4   , A5   );
      SqrX0 := X0;
      SqrX1 := X1;
      SqrX2 := X2;
      SqrX3 := X3;
      SqrX4 := X4;
      SqrX5 := X5;
      MultExtInt(SqrX0, SqrX1, SqrX2, SqrX3, SqrX4, SqrX5, X0, X1, X2, X3, X4, X5);
      SqrY0 := Y0;
      SqrY1 := Y1;
      SqrY2 := Y2;
      SqrY3 := Y3;
      SqrY4 := Y4;
      SqrY5 := Y5;
      MultExtInt(SqrY0, SqrY1, SqrY2, SqrY3, SqrY4, SqrY5, Y0, Y1, Y2, Y3, Y4, Y5);
      T0 := 4;
      T1 := 0;
      T2 := 0;
      T3 := 0;
      T4 := 0;
      T5 := 0;
      SubtractExtInt(T0, T1, T2, T3, T4, T5, SqrX0, SqrX1, SqrX2, SqrX3, SqrX4, SqrX5);
      SubtractExtInt(T0, T1, T2, T3, T4, T5, SqrY0, SqrY1, SqrY2, SqrY3, SqrY4, SqrY5);
      if T0 < 0 then
        begin
          Result := Idx;
          exit;
        end;
    end;
  Result := MaxInt;
end;

procedure TForm2.FormMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  if Button <> mbLeft then
    exit;

  XPDown := X;
  YPDown := Y;

  Canvas.Brush.Style := bsClear;

  Canvas.Pen.Mode := pmNot;
  XPMove := X;
  YPMove := Y;
  Canvas.Rectangle(XPDown, YPDown, XPMove, YPMove);
end;

procedure TForm2.FormMouseMove(Sender: TObject; Shift: TShiftState; X,
  Y: Integer);
begin
  if not (ssLeft in Shift) then
    exit;

  Canvas.Pen.Mode := pmNot;
  Canvas.Rectangle(XPDown, YPDown, XPMove, YPMove);
  XPMove := X;
  YPMove := Y;
  Canvas.Rectangle(XPDown, YPDown, XPMove, YPMove);
end;

procedure TForm2.FormMouseUp(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
var
  XPST : Extended;
  YPST : Extended;
  XPET : Extended;
  YPET : Extended;
  XLST : Extended;
  XLDST: Extended;
  YLST : Extended;
  YLDST: Extended;
  XLLT : Extended;
  YLLT : Extended;
begin
  if Button <> mbLeft then
    exit;

  if (X = XPDown) and (Y = YPDown) then
    exit;

  Canvas.Pen.Mode := pmNot;
  Canvas.Rectangle(XPDown, YPDown, XPMove, YPMove);
  XPMove := X;
  YPMove := Y;

  if XPDown <= X then
    begin
      XPST := XPDown;
      XPET := X     ;
    end
  else
    begin
      XPST := X     ;
      XPET := XPDown;
    end;
  if YPDown <= Y then
    begin
      YPST := YPDown;
      YPET := Y     ;
    end
  else
    begin
      YPST := Y     ;
      YPET := YPDown;
    end;

  XLDST := XLDS + XLL * (XPST / XPE);
  XLST  := XLS + Ldexp(Int(Ldexp(XLDST, 48)), (-48));
  XLDST := Ldexp(Frac(Ldexp(XLDST, 48)), (-48));
  YLDST := YLDS + YLL * (YPST / YPE);
  YLST  := YLS + Ldexp(Int(Ldexp(YLDST, 48)), (-48));
  YLDST := Ldexp(Frac(Ldexp(YLDST, 48)), (-48));
  XLLT  := XLL * ((XPET - XPST) / XPE);
  YLLT  := YLL * ((YPET - YPST) / YPE);

  SubForm2 := TForm2.Create(Self);
  SubForm2.ShowDialog(CountLimit, CalcPrec,
                      (Left + 20), (Top + 20), XLST, XLDST, YLST, YLDST, XLLT, YLLT);
end;

procedure TForm2.FormKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
  if Key = VK_UP then
    PaintAll((CountLimit *   10), CalcPrec)
  else if Key = VK_DOWN then
    PaintAll((CountLimit div 10), CalcPrec)
  else if Key = VK_RIGHT then
    PaintAll(CountLimit, 150)
  else if Key = VK_LEFT then
    PaintAll(CountLimit,  64)
  else if Key = VK_RETURN then
    Repaint
  else if Key = VK_ESCAPE then
    Close;
end;

procedure TForm2.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  IsReqToBreak := True;
end;

procedure TForm2.PaintAll(CntLimit: Integer; CPrec: Integer);
var
  XP : Integer;
  YP : Integer;
  Len: Integer;
begin
  CountLimit := CntLimit;
  if CountLimit < 1 then
    CountLimit := 1
  else if CountLimit > 1000000000 then
    CountLimit := 1000000000;

  CalcPrec := CPrec;
  if CalcPrec < 64 then
    CalcPrec := 64
  else if CalcPrec > 150 then
    CalcPrec := 150;

  if XLL / XPEMax > YLL / YPEMax then
    Step := XLL / XPEMax
  else
    Step := YLL / YPEMax;

  if Step = 0.0 then
    exit;

  XPE := Trunc(XLL / Step);
  YPE := Trunc(YLL / Step);

  for XP := 0 to XPEMax do
    for YP := 0 to YPEMax do
      ColorTbl[XP, YP] := -1;

  Len := Ceil(- Log10(Step));
  Caption := Format('Lim:%d Prec:%d P(%s,%s) Len(%.4e,%.4e)',
                    [CountLimit, CalcPrec,
                     DecodeExtPair(XLS, XLDS, Len),
                     DecodeExtPair(YLS, YLDS, Len),
                     XLL, YLL                      ]         );

  Show;
  Repaint;
end;

procedure RealToExtInt(R: Extended; var X0, X1, X2, X3, X4, X5: Integer);
var
  Acc: Extended;
begin
  if R < 0.0 then
    Acc := - R
  else
    Acc := R;
  X0  := Floor(Acc);
  Acc := Ldexp((Acc - X0), 30);
  X1  := Floor(Acc);
  Acc := Ldexp((Acc - X1), 30);
  X2  := Floor(Acc);
  Acc := Ldexp((Acc - X2), 30);
  X3  := Floor(Acc);
  Acc := Ldexp((Acc - X3), 30);
  X4  := Floor(Acc);
  Acc := Ldexp((Acc - X4), 30);
  X5  := Floor(Acc);
  if R < 0.0 then
    NegativeExtInt(X0, X1, X2, X3, X4, X5);
end;

procedure MultExtInt(var A10, A8, A6, A4, A2, A0: Integer; B10, B8, B6, B4, B2, B0: Integer);
var
  IsNeg: Boolean;
  A11  : Integer;
  A9   : Integer;
  A7   : Integer;
  A5   : Integer;
  A3   : Integer;
  A1   : Integer;
  B11  : Integer;
  B9   : Integer;
  B7   : Integer;
  B5   : Integer;
  B3   : Integer;
  B1   : Integer;
  C0   : Integer;
  C1   : Integer;
  C2   : Integer;
  C3   : Integer;
  C4   : Integer;
  C5   : Integer;
  C6   : Integer;
  C7   : Integer;
  C8   : Integer;
  C9   : Integer;
  C10  : Integer;
  C11  : Integer;
  C12  : Integer;
  C13  : Integer;
  C14  : Integer;
  C15  : Integer;
  C16  : Integer;
  C17  : Integer;
  C18  : Integer;
  C19  : Integer;
  C20  : Integer;
  C21  : Integer;
  C22  : Integer;
  C23  : Integer;
begin
  IsNeg := False;
  if A10 < 0 then
    begin
      NegativeExtInt(A10, A8, A6, A4, A2, A0);
      IsNeg := (not IsNeg);
    end;
  if B10 < 0 then
    begin
      NegativeExtInt(B10, B8, B6, B4, B2, B0);
      IsNeg := (not IsNeg);
    end;

  A11 := (A10 shr 15);
  A10 := (A10 and $7FFF);
  A9  := (A8  shr 15);
  A8  := (A8  and $7FFF);
  A7  := (A6  shr 15);
  A6  := (A6  and $7FFF);
  A5  := (A4  shr 15);
  A4  := (A4  and $7FFF);
  A3  := (A2  shr 15);
  A2  := (A2  and $7FFF);
  A1  := (A0  shr 15);
  A0  := (A0  and $7FFF);
  B11 := (B10 shr 15);
  B10 := (B10 and $7FFF);
  B9  := (B8  shr 15);
  B8  := (B8  and $7FFF);
  B7  := (B6  shr 15);
  B6  := (B6  and $7FFF);
  B5  := (B4  shr 15);
  B4  := (B4  and $7FFF);
  B3  := (B2  shr 15);
  B2  := (B2  and $7FFF);
  B1  := (B0  shr 15);
  B0  := (B0  and $7FFF);

  C0  := 0;
  C1  := 0;
  C2  := 0;
  C3  := 0;
  C4  := 0;
  C5  := 0;
  C6  := 0;
  C7  := 0;
  C8  := 0;
  C9  := 0;
  C10 := 0;
  C11 := 0;

  AddExtIntToTimes(C12, C11, C10, C9 , C8 , C7 , C6 , C5 , C4 , C3 , C2 , C1 , C0 ,
                        A11, A10, A9 , A8 , A7 , A6 , A5 , A4 , A3 , A2 , A1 , A0 , B0 );
  AddExtIntToTimes(C13, C12, C11, C10, C9 , C8 , C7 , C6 , C5 , C4 , C3 , C2 , C1 ,
                        A11, A10, A9 , A8 , A7 , A6 , A5 , A4 , A3 , A2 , A1 , A0 , B1 );
  AddExtIntToTimes(C14, C13, C12, C11, C10, C9 , C8 , C7 , C6 , C5 , C4 , C3 , C2 ,
                        A11, A10, A9 , A8 , A7 , A6 , A5 , A4 , A3 , A2 , A1 , A0 , B2 );
  AddExtIntToTimes(C15, C14, C13, C12, C11, C10, C9 , C8 , C7 , C6 , C5 , C4 , C3 ,
                        A11, A10, A9 , A8 , A7 , A6 , A5 , A4 , A3 , A2 , A1 , A0 , B3 );
  AddExtIntToTimes(C16, C15, C14, C13, C12, C11, C10, C9 , C8 , C7 , C6 , C5 , C4 ,
                        A11, A10, A9 , A8 , A7 , A6 , A5 , A4 , A3 , A2 , A1 , A0 , B4 );
  AddExtIntToTimes(C17, C16, C15, C14, C13, C12, C11, C10, C9 , C8 , C7 , C6 , C5 ,
                        A11, A10, A9 , A8 , A7 , A6 , A5 , A4 , A3 , A2 , A1 , A0 , B5 );
  AddExtIntToTimes(C18, C17, C16, C15, C14, C13, C12, C11, C10, C9 , C8 , C7 , C6 ,
                        A11, A10, A9 , A8 , A7 , A6 , A5 , A4 , A3 , A2 , A1 , A0 , B6 );
  AddExtIntToTimes(C19, C18, C17, C16, C15, C14, C13, C12, C11, C10, C9 , C8 , C7 ,
                        A11, A10, A9 , A8 , A7 , A6 , A5 , A4 , A3 , A2 , A1 , A0 , B7 );
  AddExtIntToTimes(C20, C19, C18, C17, C16, C15, C14, C13, C12, C11, C10, C9 , C8 ,
                        A11, A10, A9 , A8 , A7 , A6 , A5 , A4 , A3 , A2 , A1 , A0 , B8 );
  AddExtIntToTimes(C21, C20, C19, C18, C17, C16, C15, C14, C13, C12, C11, C10, C9 ,
                        A11, A10, A9 , A8 , A7 , A6 , A5 , A4 , A3 , A2 , A1 , A0 , B9 );
  AddExtIntToTimes(C22, C21, C20, C19, C18, C17, C16, C15, C14, C13, C12, C11, C10,
                        A11, A10, A9 , A8 , A7 , A6 , A5 , A4 , A3 , A2 , A1 , A0 , B10);
  AddExtIntToTimes(C23, C22, C21, C20, C19, C18, C17, C16, C15, C14, C13, C12, C11,
                        A11, A10, A9 , A8 , A7 , A6 , A5 , A4 , A3 , A2 , A1 , A0 , B11);

  A0  := (C11 shl 15) + C10;
  A2  := (C13 shl 15) + C12;
  A4  := (C15 shl 15) + C14;
  A6  := (C17 shl 15) + C16;
  A8  := (C19 shl 15) + C18;
  A10 := (C21 shl 15) + C20;
  if IsNeg then
    NegativeExtInt(A10, A8, A6, A4, A2, A0);
end;

procedure AddExtIntToTimes(var CY, C11, C10, C9, C8, C7, C6, C5, C4, C3, C2, C1, C0: Integer;
                                   A11, A10, A9, A8, A7, A6, A5, A4, A3, A2, A1, A0, B: Integer);
var
  Acc: Integer;
begin
  Acc := C0  + A0  * B               ;
  C0  := (Acc and $7FFF);
  Acc := C1  + A1  * B + (Acc shr 15);
  C1  := (Acc and $7FFF);
  Acc := C2  + A2  * B + (Acc shr 15);
  C2  := (Acc and $7FFF);
  Acc := C3  + A3  * B + (Acc shr 15);
  C3  := (Acc and $7FFF);
  Acc := C4  + A4  * B + (Acc shr 15);
  C4  := (Acc and $7FFF);
  Acc := C5  + A5  * B + (Acc shr 15);
  C5  := (Acc and $7FFF);
  Acc := C6  + A6  * B + (Acc shr 15);
  C6  := (Acc and $7FFF);
  Acc := C7  + A7  * B + (Acc shr 15);
  C7  := (Acc and $7FFF);
  Acc := C8  + A8  * B + (Acc shr 15);
  C8  := (Acc and $7FFF);
  Acc := C9  + A9  * B + (Acc shr 15);
  C9  := (Acc and $7FFF);
  Acc := C10 + A10 * B + (Acc shr 15);
  C10 := (Acc and $7FFF);
  Acc := C11 + A11 * B + (Acc shr 15);
  C11 := (Acc and $7FFF);
  CY  :=                 (Acc shr 15);
end;

procedure TwiceExtInt(var X0, X1, X2, X3, X4, X5: Integer);
var
  Acc: Integer;
begin
  Acc := (X5 shl 1);
  X5  := (Acc and $3FFFFFFF);
  Acc := (X4 shl 1) + ((Acc shr 30) and 1);
  X4  := (Acc and $3FFFFFFF);
  Acc := (X3 shl 1) + ((Acc shr 30) and 1);
  X3  := (Acc and $3FFFFFFF);
  Acc := (X2 shl 1) + ((Acc shr 30) and 1);
  X2  := (Acc and $3FFFFFFF);
  Acc := (X1 shl 1) + ((Acc shr 30) and 1);
  X1  := (Acc and $3FFFFFFF);
  X0  := (X0 shl 1) + ((Acc shr 30) and 1);
end;

procedure AddExtInt(var X0, X1, X2, X3, X4, X5: Integer; Y0, Y1, Y2, Y3, Y4, Y5: Integer);
var
  Acc: Integer;
begin
  Acc := X5 + Y5;
  X5  := (Acc and $3FFFFFFF);
  Acc := X4 + Y4 + ((Acc shr 30) and 1);
  X4  := (Acc and $3FFFFFFF);
  Acc := X3 + Y3 + ((Acc shr 30) and 1);
  X3  := (Acc and $3FFFFFFF);
  Acc := X2 + Y2 + ((Acc shr 30) and 1);
  X2  := (Acc and $3FFFFFFF);
  Acc := X1 + Y1 + ((Acc shr 30) and 1);
  X1  := (Acc and $3FFFFFFF);
  X0  := X0 + Y0 + ((Acc shr 30) and 1);
end;

procedure SubtractExtInt(var X0, X1, X2, X3, X4, X5: Integer; Y0, Y1, Y2, Y3, Y4, Y5: Integer);
var
  Acc: Integer;
begin
  Acc := X5 - Y5;
  X5  := (Acc and $3FFFFFFF);
  Acc := X4 - Y4 - ((Acc shr 30) and 1);
  X4  := (Acc and $3FFFFFFF);
  Acc := X3 - Y3 - ((Acc shr 30) and 1);
  X3  := (Acc and $3FFFFFFF);
  Acc := X2 - Y2 - ((Acc shr 30) and 1);
  X2  := (Acc and $3FFFFFFF);
  Acc := X1 - Y1 - ((Acc shr 30) and 1);
  X1  := (Acc and $3FFFFFFF);
  X0  := X0 - Y0 - ((Acc shr 30) and 1);
end;

procedure NegativeExtInt(var X0, X1, X2, X3, X4, X5: Integer);
var
  Acc: Integer;
begin
  Acc := - X5;
  X5  := (Acc and $3FFFFFFF);
  Acc := - X4 - ((Acc shr 30) and 1);
  X4  := (Acc and $3FFFFFFF);
  Acc := - X3 - ((Acc shr 30) and 1);
  X3  := (Acc and $3FFFFFFF);
  Acc := - X2 - ((Acc shr 30) and 1);
  X2  := (Acc and $3FFFFFFF);
  Acc := - X1 - ((Acc shr 30) and 1);
  X1  := (Acc and $3FFFFFFF);
  X0  := - X0 - ((Acc shr 30) and 1);
end;

function DecodeExtPair(V, DV: Extended; Len: Integer): String;
var
  Acc   : Integer;
  ValTbl: array[1..9] of Integer;
  Idx   : Integer;
  PosDgt: Integer;
begin
  Result := '';
  if V + DV < 0.0 then
    begin
      Result := Result + '-';
      V  := -V ;
      DV := -DV;
    end;

  Acc := 0;
  for Idx := 9 downto 1 do
    begin
      Acc := Acc + Trunc(Ldexp(Frac(Ldexp(V , (15 * (Idx - 1)))), 15))
                 + Trunc(Ldexp(Frac(Ldexp(DV, (15 * (Idx - 1)))), 15));
      ValTbl[Idx] := (Acc and $7FFF);
      Acc := (Acc div $8000);
    end;
  Acc := Acc + Trunc(V) + Trunc(DV);
  Result := Result + IntToStr(Acc) + '.';

  for PosDgt := 1 to Len do
    begin
      Acc := 0;
      for Idx := 9 downto 1 do
        begin
          Acc := Acc + ValTbl[Idx] * 10;
          ValTbl[Idx] := (Acc and $7FFF);
          Acc := (Acc div $8000);
        end;
      Result := Result + IntToStr(Acc);
    end;
end;

procedure EncodeExtPair(Text: String; var V, DV: Extended);
var
  PosDP    : Integer;
  PosDgt   : Integer;
  ValTbl   : array[0..9] of Integer;
  Idx      : Integer;
  Acc      : Integer;
  Remainder: Integer;
begin
  PosDP:= Pos('.', Text);
  if PosDP = 0 then
    begin
      V  := StrToInt(Text);
      DV := 0.0;
      exit;
    end;

  for Idx := 0 to 9 do
    ValTbl[Idx] := 0;
  PosDgt := Length(Text);
  while Text[PosDgt] <> '.' do
    begin
      ValTbl[0] := ValTbl[0] + StrToInt(Text[PosDgt]);
      Remainder  := 0;
      for Idx := 0 to 9 do
        begin
          Acc := (Remainder shl 15) + ValTbl[Idx];
          Remainder := (Acc mod 10);
          ValTbl[Idx] := (Acc div 10);
        end;
      Dec(PosDgt);
    end;

  V  := 0.0;
  for Idx := 1 to 4 do
    V  := V  + Ldexp(ValTbl[Idx], (- 15 * Idx));
  DV := 0.0;
  for Idx := 5 to 9 do
    DV := DV + Ldexp(ValTbl[Idx], (- 15 * Idx));
  if Text[1] = '-' then
    begin
      V  := - V - StrToInt(Copy(Text, 2, (PosDgt - 2)));
      DV := - DV;
    end
  else
    V := V + StrToInt(Copy(Text, 1, (PosDgt - 1)));
end;

end.
