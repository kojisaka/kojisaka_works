unit SolarEclipse00;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ExtCtrls;

const
  RangeStart   =  80;
  StdRangeStep =   0.2;
  RShine       =  80;
  RCorona      =  30;
  RSun         =  20;

type
  TForm1 = class(TForm)
    Timer1: TTimer;
    procedure FormShow(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure FormKeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure FormPaint(Sender: TObject);
  private
    { Private êÈåæ }
    XSun     : Integer;
    YSun     : Integer;
    XEdge    : Integer;
    RangeCrnt: Extended;
    procedure DrawSolarEclipse(RangeOld, RangeNew: Extended);
    procedure FillSolarEclipsePoint(Range: Extended; X, Y: Integer);
    procedure CalcShineBrights(Range: Extended; Dist: Integer; var RBright, GBright, BBright: Integer);
    procedure CalcCoronaBrights(Dist: Integer; var RBright, GBright, BBright: Integer);
    procedure DrawCircle(X, Y, R: Integer);
    procedure FillPoint(X, Y: Integer);
  public
    { Public êÈåæ }
  end;

var
  Form1: TForm1;

implementation

uses
  Math;

{$R *.dfm}

procedure TForm1.FormShow(Sender: TObject);
begin
  XSun := Ceil(ClientWidth  / 2);
  YSun := Ceil(ClientHeight / 2);

  Color := $BB6666;

  FormPaint(Sender);

  RangeCrnt := RangeStart;
  XEdge := XSun - RSun;
  Timer1.Enabled := True;
end;

procedure TForm1.Timer1Timer(Sender: TObject);
var
  RangeNew: Extended;
begin
  if XEdge <= XSun then
    RangeNew := RangeCrnt - StdRangeStep
  else
    RangeNew := RangeCrnt + StdRangeStep;
  DrawSolarEclipse(RangeCrnt, RangeNew);
  RangeCrnt := RangeNew;
  if RangeCrnt < - StdRangeStep * 10 then
    XEdge := XSun + RSun
  else if RangeCrnt > RangeStart then
    XEdge := XSun - RSun;
end;

procedure TForm1.FormKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
  if Key = VK_ESCAPE then
    Close
  else if Key = VK_SPACE then
    Timer1.Enabled := (not Timer1.Enabled);
end;

procedure TForm1.FormPaint(Sender: TObject);
begin
  DrawSolarEclipse(Round(MaxIntValue([XSun, YSun]) * 1.5), RangeCrnt);
end;

procedure TForm1.DrawSolarEclipse(RangeOld, RangeNew: Extended);
var
  Dist      : Integer;
  BrightDist: Integer;
  RBrightT  : Integer;
  GBrightT  : Integer;
  BBrightT  : Integer;
  RBright   : Integer;
  GBright   : Integer;
  BBright   : Integer;
  YDist     : Integer;
  XDistS    : Integer;
  XDistE    : Integer;
  XDist     : Integer;
begin
  if RangeNew < RShine then
    begin
      CalcShineBrights(RangeNew, (RSun + XSun), RBright, GBright, BBright);
      Canvas.Brush.Style := bsClear;
      Canvas.Pen  .Color := RGB(RBright, GBright, BBright);
      Canvas.Pen  .Width := (RSun + XSun - RShine) * 2 + 1;
      DrawCircle(XEdge, YSun, (RSun + XSun));
    end;

  Dist := Ceil(MaxValue([RangeOld, RangeNew]));
  if Dist < RShine then
    Dist := RShine;
  while Dist >= 0 do
    begin
      CalcShineBrights(RangeNew, Dist, RBright, GBright, BBright);
      BrightDist := Dist - RSun;
      if BrightDist < RSun then
        BrightDist := RSun;
      CalcCoronaBrights(BrightDist, RBrightT, GBrightT, BBrightT);
      if ( Dist > RShine          ) or
         ((Dist > RSun       ) and
          (RBrightT < RBright) and
          (GBrightT < GBright) and
          (BBrightT < BBright)    )    then
        begin
          Canvas.Brush.Style := bsClear;
          Canvas.Pen  .Color := RGB(RBright, GBright, BBright);
          Canvas.Pen  .Width := 3;
          DrawCircle(XEdge, YSun, (Dist - 1));
          Dec(Dist);
        end
      else
        begin
          Canvas.Brush.Style := bsSolid;
          Canvas.Brush.Color := clGreen;
          for YDist := Dist downto 0 do
            begin
              XDistS := Ceil(Sqrt(Dist * Dist - YDist * YDist));
              if YDist >= Dist then
                XDistE := 0
              else
                XDistE := Ceil(Sqrt((Dist - 1) * (Dist - 1) - YDist * YDist)) + 1;
              for XDist := XDistS downto XDistE do
                begin
                  FillSolarEclipsePoint(RangeNew, (XEdge + XDist), (YSun - YDist));
                  FillSolarEclipsePoint(RangeNew, (XEdge + XDist), (YSun + YDist));
                  FillSolarEclipsePoint(RangeNew, (XEdge - XDist), (YSun - YDist));
                  FillSolarEclipsePoint(RangeNew, (XEdge - XDist), (YSun + YDist));
                end;
            end;
          Dec(Dist);
        end;
    end;
end;

procedure TForm1.FillSolarEclipsePoint(Range: Extended; X, Y: Integer);
var
  Dist    : Integer;
  RBright : Integer;
  GBright : Integer;
  BBright : Integer;
  RBrightT: Integer;
  GBrightT: Integer;
  BBrightT: Integer;
begin
  Dist := Round(Sqrt((X - XEdge) * (X - XEdge) + (Y - YSun) * (Y - YSun)));
  CalcShineBrights(Range, Dist, RBright, GBright, BBright);
  Dist := Round(Sqrt((X - XSun) * (X - XSun) + (Y - YSun) * (Y - YSun)));
  CalcCoronaBrights(Dist, RBrightT, GBrightT, BBrightT);
  if RBrightT > RBright then
    RBright := RBrightT;
  if GBrightT > GBright then
    GBright := GBrightT;
  if BBrightT > BBright then
    BBright := BBrightT;
  Canvas.Brush.Color := RGB(RBright, GBright, BBright);
  FillPoint(X, Y);
end;

procedure TForm1.CalcShineBrights(Range: Extended; Dist: Integer; var RBright, GBright, BBright: Integer);
var
  Rate       : Extended;
  RBrightBase: Integer;
  GBrightBase: Integer;
  BBrightBase: Integer;
begin
  if Range > RShine then
    Rate := 1.0
  else if Range > RCorona then
    Rate := 0.8 + 0.2 * ((Range - RCorona) / (RShine - RCorona))
  else if Range > 0 then
    Rate := 0.8 * Sqrt(Range / RCorona)
  else
    Rate := 0.0;
  RBrightBase := Round($66 * Rate);
  GBrightBase := Round($66 * Rate);
  BBrightBase := Round($99 * Rate + $22);
  if RBrightBase > RBright then
    RBright := RBrightBase;
  if GBrightBase > GBright then
    GBright := GBrightBase;
  if BBrightBase > BBright then
    BBright := BBrightBase;

  if Dist < Range then
    Rate := (1.0 - Dist / Range) * (1.0 - Dist / Range)
  else
    Rate := 0.0;
  RBright := Round(($FF - RBrightBase) * Rate + RBrightBase);
  GBright := Round(($FF - GBrightBase) * Rate + GBrightBase);
  BBright := Round(($FF - BBrightBase) * Rate + BBrightBase);
end;

procedure TForm1.CalcCoronaBrights(Dist: Integer; var RBright, GBright, BBright: Integer);
var
  Rate: Extended;
begin
  if Dist < RSun then
    begin
      RBright := 0;
      GBright := 0;
      BBright := 0;
    end
  else if Dist <= RCorona then
    begin
      Rate := 1.0 - (Dist - RSun) / (RCorona - RSun);
      Rate := Rate * Rate;
      RBright := Round(Rate * $33 + $22);
      GBright := Round(Rate * $33 + $22);
      BBright := Round(Rate * $22 + $33);
    end
  else
    begin
      if Dist <= RShine - RSun then
        Rate := 1.0 - (Dist - RCorona) / (RShine - RSun - RCorona)
      else
        Rate := 0.0;
      Rate := Rate * Rate;
      RBright := Round(Rate * $22);
      GBright := Round(Rate * $22);
      BBright := Round(Rate * $11 + $22);
    end;
end;

procedure TForm1.DrawCircle(X, Y, R: Integer);
begin
  Canvas.Ellipse((X - R), (Y - R), (X + R + 1), (Y + R + 1));
end;

procedure TForm1.FillPoint(X, Y: Integer);
begin
  Canvas.FillRect(Rect(X, Y, (X + 1), (Y + 1)));
end;

end.

