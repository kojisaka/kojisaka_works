unit BilliardSimulator00;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ExtCtrls;

type
  TBallInfo = record  // 球情報型
    IsStayed: Boolean;   // 競技エリアに残っているのならTrue
    Color   : TColor ;   // 色
    XVel    : Double ;   // 速度のＸ成分
    YVel    : Double ;   // 速度のＹ成分
    XSpinVel: Double ;   // 垂直回転速度のＸ成分
    YSpinVel: Double ;   // 垂直回転速度のＹ成分
    TurnVel : Double ;   // 水平回転速度
    XPos    : Double ;   // 中心Ｘ座標
    YPos    : Double ;   // 中心Ｙ座標
  end;

type
  TContext = record                         // 情況
    BallInfoTbl: array[0..9] of TBallInfo;    // 前回の球情報テーブル
    HSpinRate  : Double                  ;    // 前回のキュー出し速度に対する水平回転速度の比率
    VSpinRate  : Double                  ;    // 前回のキュー出し速度に対する垂直回転速度の比率
    XQueDir    : Double                  ;    // 前回のキュー出し方向のＸ成分
    YQueDir    : Double                  ;    // 前回のキュー出し方向のＹ成分
    QueVel     : Double                  ;    // 前回のキュー出し速度
    ObjBallIdx : Integer                 ;    // 前回の的球の指標
  end;

type
  TForm1 = class(TForm)
    PanelForMsg: TPanel;
    TimerForCycle: TTimer;
    procedure FormShow(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure FormKeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure FormMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure FormMouseMove(Sender: TObject; Shift: TShiftState; X,
      Y: Integer);
    procedure FormPaint(Sender: TObject);
    procedure TimerForCycleTimer(Sender: TObject);
  private
    { Private 宣言 }
    ContextTbl    : array[1..100] of TContext;  // 情況テーブル
    ContextLastIdx: Integer                  ;  // 情況最終添字
    ContextIdx    : Integer                  ;  // 情況添字
    BallInfoTbl   : array[0..9] of TBallInfo ;  // 球情報テーブル
    HSpinRate     : Double                   ;  // キュー出し速度に対する水平回転速度の比率
    VSpinRate     : Double                   ;  // キュー出し速度に対する垂直回転速度の比率
    XQueDir       : Double                   ;  // キュー出し方向のＸ成分
    YQueDir       : Double                   ;  // キュー出し方向のＹ成分
    QueVel        : Double                   ;  // キュー出し速度
    ObjBallIdx    : Integer                  ;  // 的球の指標
    IsQueBallHit  : Boolean                  ;  // 手球がどれかの球に当たったことがあるのならTrue
    IsObjBallSafe : Boolean                  ;  // 的球に最初に当てるルールに関してセーフならTrue
    IsCushionSafe : Boolean                  ;  // 球を最低１つクッションに当てるルールに関してセーフならTrue
    procedure MoveAll;
    function ChangeModeIfAllStop: Boolean;
    procedure DrawField;
    procedure DrawFrame;
    procedure DrawEstCourse;
    procedure DrawIndicatorArea;
    procedure DrawHorizontalLineInCircle(Y, XPos, YPos, R: Integer; Col: TColor);
    procedure DrawHorizontalLine(Y, XS, XE: Integer; Col: TColor);
    { Public 宣言 }
  end;

procedure RotateVector(var X, Y: Double; CosT, SinT: Double);

var
  Form1: TForm1;

implementation

{$R *.DFM}

uses
  Math, MMSystem;

const
  MaxQueVel     = 0.2      ;  // キュー出し最大速度
  StdQueVel     = 0.08     ;  // キュー出し標準速度
  RollInertRate = 0.4      ;  // 移動の慣性に対する回転の慣性の比率
  CshnVelAbRate = 0.2      ;  // クッションでの速度吸収率
  CshnSpnAbRate = 1.8      ;  // クッションでの衝突速度に対する回転速度吸収率
  SlideFrBrake  = 0.00001  ;  // 滑り摩擦による減速度
  RollFrBrake   = 0.0000004;  // ころがり摩擦による減速度
  AxisFrBrake   = 0.000001 ;  // 回転軸摩擦による減速度
  CshnBrakeRate = 0.5      ;  // クッションでの垂直方向の速度に対する水平方向の減速度の比率
  FieldSize     = 400      ;  // 競技エリアの大きさ
  Margin        =  40      ;  // 競技エリアの周りの余白の大きさ
  XIndAreaSize  = 100      ;  // 表示器エリアのＸ方向の大きさ
  RSpot         =   2      ;  // スポットの半径
  RBall         =   6      ;  // 球の半径
  RBallForImpct =  30      ;  // 撞点指定用に表示する手球の半径
  RImpctPntMark =   3      ;  // 撞点位置マークの半径
  DrawInterval  =  40      ;  // 描画周期
  CalcInterval  =   0.1    ;  // 計算間隔
  MarginColor   = $002070  ;  // 競技エリアの外枠の色
  FieldColor    = $00C000  ;  // 競技エリア内の色
  SpotColor     = $C0C0C0  ;  // スポットの色
  PocketColor   = $000000  ;  // ポケットの色
  IndAreaBColor = $FF0000  ;  // 表示器エリアの背景色
  ImpctMrkColor = $0000FF  ;  // 撞点位置マークの色
  QueCoursColor = $0000FF  ;  // 手球の予想コースの線色
  ObjCoursColor = $FFFFFF  ;  // 的球の予想コースの線色
  BallColorTbl: array[0..9] of TColor
    = (clWhite, clYellow, clBlue, clRed, clPurple, $0080FF, clGreen, clMaroon, clBlack, $60FFFF);

procedure TForm1.FormShow(Sender: TObject);
const
  XBallStep = Round(RBall * 1.8);
  BallDiffTbl: array[0..9] of TPoint =
    (
    (X:(- FieldSize); Y: 0),
    (X:(XBallStep * 0); Y:(  RBall * 0)),
    (X:(XBallStep * 2); Y:(  RBall * 2)),
    (X:(XBallStep * 4); Y:(  RBall * 0)),
    (X:(XBallStep * 2); Y:(- RBall * 2)),
    (X:(XBallStep * 3); Y:(  RBall * 1)),
    (X:(XBallStep * 3); Y:(- RBall * 1)),
    (X:(XBallStep * 1); Y:(  RBall * 1)),
    (X:(XBallStep * 1); Y:(- RBall * 1)),
    (X:(XBallStep * 2); Y:(  RBall * 0))
    );
var
  Idx: Integer;  // 球情報テーブル用指標
begin
  RandSeed := timeGetTime;
  ContextLastIdx := 0;
  ContextIdx     := 0;
  for Idx := 0 to High(BallInfoTbl) do
    begin
      BallInfoTbl[Idx].IsStayed := True;
      BallInfoTbl[Idx].Color    := BallColorTbl[Idx];
      BallInfoTbl[Idx].XVel     := 0;
      BallInfoTbl[Idx].YVel     := 0;
      BallInfoTbl[Idx].XSpinVel := 0;
      BallInfoTbl[Idx].YSpinVel := 0;
      BallInfoTbl[Idx].TurnVel  := 0;
      BallInfoTbl[Idx].XPos     := Margin + FieldSize * 3 / 2 + BallDiffTbl[Idx].X + Random - 0.5;
      BallInfoTbl[Idx].YPos     := Margin + FieldSize     / 2 + BallDiffTbl[Idx].Y + Random - 0.5;
    end;
  ObjBallIdx    := 1;
  IsObjBallSafe := True;
  IsCushionSafe := True;

  Canvas.Brush.Style := bsSolid;
  Canvas.Pen  .Style := psSolid;
  Canvas.Pen  .Mode  := pmCopy ;

  QueVel := MaxQueVel * 2;

  XQueDir := FieldSize;
  YQueDir := 0;
  TimerForCycle.Interval := DrawInterval;
  TimerForCycle.Enabled  := True;
end;

procedure TForm1.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  TimerForCycle.Enabled := False;
end;

procedure TForm1.FormKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
var
  pContext: ^TContext;  // 情況
  Idx     : Integer  ;  // 球情報テーブル用指標
  Dist    : Double   ;  // 手球の中心位置からの距離
  MoveRate: Double   ;  // キュー出し速度に対する移動速度の比率
begin
  if Key = VK_ESCAPE then
    Close
  else if Key = VK_HOME then
    FormShow(Sender)
  else if (ContextIdx > 0) and (Key = VK_BACK) then
    begin
      TimerForCycle.Enabled := False;
      pContext := @ContextTbl[ContextIdx];
      for Idx := 0 to High(BallInfoTbl) do
        BallInfoTbl[Idx] := pContext^.BallInfoTbl[Idx];
      HSpinRate  := pContext^.HSpinRate ;
      VSpinRate  := pContext^.VSpinRate ;
      XQueDir    := pContext^.XQueDir   ;
      YQueDir    := pContext^.YQueDir   ;
      QueVel     := pContext^.QueVel    ;
      ObjBallIdx := pContext^.ObjBallIdx;
      Dec(ContextIdx);
      DrawField;
      DrawFrame;
      DrawEstCourse;
      DrawIndicatorArea;
      PanelForMsg.Caption := 'Enterキーで最後まで再現。左ボタンで、撞く方向、撞点、力加減を指定。Back Spaceキーで前状態復元。Homeキーで新規開始。';
    end;

  if TimerForCycle.Enabled then
    exit;

  if Key = VK_RETURN then
    begin
      if ContextIdx < High(ContextTbl) then
        begin
          Inc(ContextIdx);
          if ContextIdx >= ContextLastIdx then
            ContextLastIdx := ContextIdx;
          pContext := @ContextTbl[ContextIdx];
          for Idx := 0 to High(BallInfoTbl) do
            pContext^.BallInfoTbl[Idx] := BallInfoTbl[Idx];
          pContext^.HSpinRate  := HSpinRate ;
          pContext^.VSpinRate  := VSpinRate ;
          pContext^.XQueDir    := XQueDir   ;
          pContext^.YQueDir    := YQueDir   ;
          pContext^.QueVel     := QueVel    ;
          pContext^.ObjBallIdx := ObjBallIdx;
        end;

      Dist := Sqrt(XQueDir * XQueDir + YQueDir * YQueDir);
      MoveRate := 1 - (HSpinRate * HSpinRate + VSpinRate * VSpinRate);
      BallInfoTbl[0].XVel     := QueVel * MoveRate  * (XQueDir / Dist);
      BallInfoTbl[0].YVel     := QueVel * MoveRate  * (YQueDir / Dist);
      BallInfoTbl[0].XSpinVel := QueVel * VSpinRate * (XQueDir / Dist);
      BallInfoTbl[0].YSpinVel := QueVel * VSpinRate * (YQueDir / Dist);
      BallInfoTbl[0].TurnVel  := QueVel * HSpinRate;

      IsQueBallHit  := False;
      IsObjBallSafe := False;
      IsCushionSafe := False;

      QueVel := StdQueVel;
      PanelForMsg.Caption := 'Back Spaceキーで中断して前状態復元。Homeキーで中断して新規開始。';

      TimerForCycle.Enabled := True;
      DrawFrame;
      DrawIndicatorArea;
    end;
end;

procedure TForm1.FormMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
  FormMouseMove(Sender, Shift, X, Y);
end;

procedure TForm1.FormMouseMove(Sender: TObject; Shift: TShiftState; X,
  Y: Integer);
var
  XDist: Double ;  // 手球の中心位置からの距離のＸ成分
  YDist: Double ;  // 手球の中心位置からの距離のＹ成分
  Dist : Double ;  // 手球の中心位置からの距離
begin
  if not (ssLeft in Shift) then
    exit;

  if TimerForCycle.Enabled then
    exit;

  if not BallInfoTbl[0].IsStayed then
    exit;

  if X < Margin + FieldSize * 2 + Margin then
    begin
      XQueDir := X - BallInfoTbl[0].XPos;
      YQueDir := Y - BallInfoTbl[0].YPos;
      DrawField;
      DrawFrame;
      DrawEstCourse;
      DrawIndicatorArea;
    end
  else if (X < Margin + FieldSize * 2 + Margin + XIndAreaSize) then
    begin
      XDist := X - (Margin + FieldSize * 2 + Margin + (XIndAreaSize div 2));
      YDist := Y - (Margin + (FieldSize div 2));
      Dist  := Sqrt(XDist * XDist + YDist * YDist);
      if Dist > RBallForImpct * 0.6 then
        begin
          XDist := (RBallForImpct * 0.6) * (XDist / Dist);
          YDist := (RBallForImpct * 0.6) * (YDist / Dist);
        end;
      HSpinRate :=   XDist / RBallForImpct;
      VSpinRate := - YDist / RBallForImpct;
      DrawIndicatorArea;
    end
  else
    begin
      if Y < Margin then
        Y := Margin
      else if Y > Margin + FieldSize then
        Y := Margin + FieldSize;
      QueVel := MaxQueVel * (1 - (Y - Margin) / FieldSize);
      DrawIndicatorArea;
    end;
  ContextLastIdx := ContextIdx;
  PanelForMsg.Caption := '左ボタンで、撞く方向、撞点、力加減を指定。Enterキーでショット。Back Spaceキーで前状態復元。Homeキーで新規開始。';
end;

procedure TForm1.FormPaint(Sender: TObject);
begin
  DrawField;
  DrawFrame;
  DrawEstCourse;
  DrawIndicatorArea;
end;

procedure TForm1.TimerForCycleTimer(Sender: TObject);
var
  PreCI: Integer;  // 直前の情況添字
  Idx  : Integer;  // 指標
  Key  : Word   ;  // 仮想キー
  STime: Word   ;  // 待ち開始時刻
begin
  DrawField;
  for Idx := 1 to Round(DrawInterval / CalcInterval) do
    MoveAll;
  if not ChangeModeIfAllStop then
    exit;
  if ContextIdx < ContextLastIdx then
    begin
      Inc(ContextIdx);
      Key := VK_BACK;
      FormKeyDown(Sender, Key, []);
    end
  else if BallInfoTbl[9].IsStayed then
    exit;
  PreCI := ContextIdx;
  STime := timeGetTime;
  while Word(timeGetTime - STime) < 2000 do
    begin
      Application.ProcessMessages;
      if ContextIdx < PreCI then
        exit;
    end;
  if ContextIdx < ContextLastIdx then
    begin
      Key := VK_RETURN;
      FormKeyDown(Sender, Key, []);
    end
  else if not BallInfoTbl[9].IsStayed then
    begin
      ContextIdx := 0;
      TimerForCycle.Enabled := True;
    end;
end;

// すべてを動かす。
procedure TForm1.MoveAll;
var
  Idx       : Integer;  // 球情報テーブル用指標
  XPos      : Double ;  // 球の中心Ｘ座標
  YPos      : Double ;  // 球の中心Ｙ座標
  PocketDiff: Double ;  // 競技エリアの角とポケットの中心位置との差（Ｘ座標とＹ座標で共通）
  IdxDst    : Integer;  // 相手用の球情報テーブル用指標
  XDist     : Double ;  // 距離のＸ成分
  YDist     : Double ;  // 距離のＹ成分
  XSpinVel  : Double ;  // 球の垂直回転速度のＸ成分
  YSpinVel  : Double ;  // 球の垂直回転速度のＹ成分
  TurnVel   : Double ;  // 球の水平回転速度
  DistSqr   : Double ;  // 距離の平方
  Dist      : Double ;  // 距離
  CosT      : Double ;  // 回転量の余弦
  SinT      : Double ;  // 回転量の正弦
  XVel      : Double ;  // 球の速度のＸ成分
  YVel      : Double ;  // 球の速度のＸ成分
  XVelDst   : Double ;  // 相手の球の速度のＸ成分
  YVelDst   : Double ;  // 相手の球の速度のＹ成分
  XVelTmp   : Double ;  // 作業用の球の速度のＸ成分
  XVelDiff  : Double ;  // 速度の差のＸ成分
  YVelDiff  : Double ;  // 速度の差のＹ成分
  VelDiff   : Double ;  // 速度の差
  Vel       : Double ;  // 速度
begin
  for Idx := 0 to High(BallInfoTbl) do
    if BallInfoTbl[Idx].IsStayed then
      begin
        BallInfoTbl[Idx].XPos := BallInfoTbl[Idx].XPos + BallInfoTbl[Idx].XVel;
        BallInfoTbl[Idx].YPos := BallInfoTbl[Idx].YPos + BallInfoTbl[Idx].YVel;
      end;

  PocketDiff := RBall * Sqrt(2);
  for Idx := 0 to High(BallInfoTbl) do
    if BallInfoTbl[Idx].IsStayed then
      begin
        XPos := BallInfoTbl[Idx].XPos;
        YPos := BallInfoTbl[Idx].YPos;
        XVel     := BallInfoTbl[Idx].XVel    ;
        YVel     := BallInfoTbl[Idx].YVel    ;
        XSpinVel := BallInfoTbl[Idx].XSpinVel;
        YSpinVel := BallInfoTbl[Idx].YSpinVel;
        TurnVel  := BallInfoTbl[Idx].TurnVel ;
        if ((XVel < 0) and (XPos - RBall < Margin                )) or
           ((XVel > 0) and (XPos + RBall > Margin + FieldSize * 2))    then
          begin
            if (YPos < Margin             + PocketDiff * 2) or
               (YPos > Margin + FieldSize - PocketDiff * 2)    then
              BallInfoTbl[Idx].IsStayed := False
            else
              begin
                if XPos - RBall < Margin then
                  TurnVel := - TurnVel;
                YVelDiff := TurnVel - YVel;
                if Abs(YVelDiff) < Abs(XVel) * CshnBrakeRate then
                  begin
                    YVel    := YVel + YVelDiff * (RollInertRate / (1 + RollInertRate));
                    TurnVel := YVel;
                  end
                else
                  begin
                    YVelDiff := Abs(XVel) * CshnBrakeRate * (YVelDiff / Abs(YVelDiff));
                    YVel     := YVel    + YVelDiff * (RollInertRate / (1 + RollInertRate));
                    TurnVel  := TurnVel - YVelDiff * (1 / (1 + RollInertRate));
                  end;
                if XPos - RBall < Margin then
                  TurnVel := - TurnVel;

                XVel     := - XVel * (1 - CshnVelAbRate);
                XSpinVel := XSpinVel + XVel * CshnSpnAbRate;
                if XSpinVel / XVel > 1 then
                  XSpinVel := XVel;
              end;
            IsCushionSafe := True;
          end;
        if ((YVel < 0) and (YPos - RBall < Margin            )) or
           ((YVel > 0) and (YPos + RBall > Margin + FieldSize))    then
          begin
            if (XPos < Margin                 + PocketDiff * 2) or
               (XPos > Margin + FieldSize * 2 - PocketDiff * 2) or
               ((XPos > Margin + FieldSize - RBall + RBall * MaxValue([(- XVel / Abs(YVel)), (-1)])) and
                (XPos < Margin + FieldSize + RBall - RBall * MaxValue([(  XVel / Abs(YVel)), (-1)]))    ) then
              BallInfoTbl[Idx].IsStayed := False
            else
              begin
                if YPos + RBall > Margin + FieldSize then
                  TurnVel := - TurnVel;
                XVelDiff := TurnVel - XVel;
                if Abs(XVelDiff) < Abs(YVel) * CshnBrakeRate then
                  begin
                    XVel    := XVel + XVelDiff / 2;
                    TurnVel := XVel;
                  end
                else
                  begin
                    XVelDiff := Abs(YVel) * CshnBrakeRate * (XVelDiff / Abs(XVelDiff));
                    XVel     := XVel    + XVelDiff;
                    TurnVel  := TurnVel - XVelDiff;
                  end;
                if YPos + RBall > Margin + FieldSize then
                  TurnVel := - TurnVel;

                YVel     := - YVel * (1 - CshnVelAbRate);
                YSpinVel := YSpinVel + YVel * CshnSpnAbRate;
                if YSpinVel / YVel > 1 then
                  YSpinVel := YVel;
              end;
            IsCushionSafe := True;
          end;
        BallInfoTbl[Idx].XVel     := XVel    ;
        BallInfoTbl[Idx].YVel     := YVel    ;
        BallInfoTbl[Idx].XSpinVel := XSpinVel;
        BallInfoTbl[Idx].YSpinVel := YSpinVel;
        BallInfoTbl[Idx].TurnVel  := TurnVel ;
      end;

  for Idx := 0 to High(BallInfoTbl) do
    if BallInfoTbl[Idx].IsStayed then
      begin
        XPos := BallInfoTbl[Idx].XPos;
        YPos := BallInfoTbl[Idx].YPos;
        for IdxDst := Idx + 1 to High(BallInfoTbl) do
          if BallInfoTbl[IdxDst].IsStayed then
            begin
              XDist := BallInfoTbl[IdxDst].XPos - XPos;
              YDist := BallInfoTbl[IdxDst].YPos - YPos;
              DistSqr := XDist * XDist + YDist * YDist;
              if DistSqr <= (RBall * 2) * (RBall * 2) then
                begin
                  Dist := Sqrt(DistSqr);
                  CosT := XDist / Dist;
                  SinT := YDist / Dist;

                  XVel    := BallInfoTbl[Idx   ].XVel;
                  YVel    := BallInfoTbl[Idx   ].YVel;
                  XVelDst := BallInfoTbl[IdxDst].XVel;
                  YVelDst := BallInfoTbl[IdxDst].YVel;

                  XDist := XDist + XVelDst - XVel;
                  YDist := YDist + YVelDst - YVel;
                  if XDist * XDist + YDist * YDist <= DistSqr then
                    begin
                      RotateVector(XVel   , YVel   , CosT, (- SinT));
                      RotateVector(XVelDst, YVelDst, CosT, (- SinT));
                      XVelTmp := XVel   ;
                      XVel    := XVelDst;
                      XVelDst := XVelTmp;
                      RotateVector(XVel   , YVel   , CosT, SinT);
                      RotateVector(XVelDst, YVelDst, CosT, SinT);

                      BallInfoTbl[Idx   ].XVel := XVel   ;
                      BallInfoTbl[Idx   ].YVel := YVel   ;
                      BallInfoTbl[IdxDst].XVel := XVelDst;
                      BallInfoTbl[IdxDst].YVel := YVelDst;

                      if Idx = 0 then
                        begin
                          if not IsQueBallHit then
                            if IdxDst = ObjBallIdx then
                              IsObjBallSafe := True;
                          IsQueBallHit := True;
                        end;
                    end;
                end;
            end;
      end;

  for Idx := 0 to High(BallInfoTbl) do
    if BallInfoTbl[Idx].IsStayed then
      begin
        XVel     := BallInfoTbl[Idx].XVel    ;
        YVel     := BallInfoTbl[Idx].YVel    ;
        XSpinVel := BallInfoTbl[Idx].XSpinVel;
        YSpinVel := BallInfoTbl[Idx].YSpinVel;
        TurnVel  := BallInfoTbl[Idx].TurnVel ;
        if (XVel <> XSpinVel) or (YVel <> YSpinVel)  then
          begin
            XVelDiff := XSpinVel - XVel;
            YVelDiff := YSpinVel - YVel;
            VelDiff := Sqrt(XVelDiff * XVelDiff + YVelDiff * YVelDiff);
            if VelDiff < SlideFrBrake then
              begin
                XVel     := XVel + XVelDiff * (RollInertRate / (1 + RollInertRate));
                YVel     := YVel + YVelDiff * (RollInertRate / (1 + RollInertRate));
                XSpinVel := XVel;
                YSpinVel := YVel;
              end
            else
              begin
                XVelDiff := SlideFrBrake * (XVelDiff / VelDiff);
                YVelDiff := SlideFrBrake * (YVelDiff / VelDiff);
                XVel     := XVel     + XVelDiff * (RollInertRate / (1 + RollInertRate));
                YVel     := YVel     + YVelDiff * (RollInertRate / (1 + RollInertRate));
                XSpinVel := XSpinVel - XVelDiff * (1 / (1 + RollInertRate));
                YSpinVel := YSpinVel - YVelDiff * (1 / (1 + RollInertRate));
              end;
          end
        else
          begin
            Vel := Sqrt(XVel * XVel + YVel * YVel);
            if Vel < RollFrBrake then
              begin
                XVel := 0;
                YVel := 0;
              end
            else
              begin
                XVel := XVel - RollFrBrake * (XVel / Vel);
                YVel := YVel - RollFrBrake * (YVel / Vel);
              end;
            XSpinVel := XVel;
            YSpinVel := YVel;
          end;
        if Abs(TurnVel) < AxisFrBrake then
          TurnVel := 0
        else if TurnVel < 0 then
          TurnVel := TurnVel + AxisFrBrake
        else if TurnVel > 0 then
          TurnVel := TurnVel - AxisFrBrake;
        BallInfoTbl[Idx].XVel     := XVel    ;
        BallInfoTbl[Idx].YVel     := YVel    ;
        BallInfoTbl[Idx].XSpinVel := XSpinVel;
        BallInfoTbl[Idx].YSpinVel := YSpinVel;
        BallInfoTbl[Idx].TurnVel  := TurnVel ;
      end;
end;

// 全要素が停止していたら入力待ちモードに切り換える。
//   戻り値  Boolean  入力待ちモードに切り換えたのならTrue
function TForm1.ChangeModeIfAllStop: Boolean;
var
  Idx: Integer;  // 球情報テーブル用指標
begin
  Result := False;

  Idx := 0;
  while Idx <= High(BallInfoTbl) do
    begin
      if BallInfoTbl[Idx].IsStayed then
        if (BallInfoTbl[Idx].XVel     <> 0) or (BallInfoTbl[Idx].YVel     <> 0) or
           (BallInfoTbl[Idx].XSpinVel <> 0) or (BallInfoTbl[Idx].YSpinVel <> 0) or
           (BallInfoTbl[Idx].TurnVel  <> 0)                                        then
          exit;
      Inc(Idx);
    end;

  DrawField;
  if not BallInfoTbl[0].IsStayed then
    begin
      PanelForMsg.Caption := '手球スクラッチです。Back Spaceキーで前状態復元。';
      DrawIndicatorArea;
      exit;
    end;
  if not IsObjBallSafe then
    begin
      PanelForMsg.Caption := '最初に的球に当たりませんでした。ファウルです。Back Spaceキーで前状態復元。';
      DrawIndicatorArea;
      exit;
    end;
  if not IsCushionSafe then
    begin
      PanelForMsg.Caption := 'ノークッション・ファウルです。Back Spaceキーで前状態復元。';
      DrawIndicatorArea;
      exit;
    end;
  ObjBallIdx := 1;
  while ObjBallIdx <= High(BallInfoTbl) do
    begin
      if BallInfoTbl[ObjBallIdx].IsStayed then
        break;
      Inc(ObjBallIdx);
    end;
  if ObjBallIdx > High(BallInfoTbl) then
    PanelForMsg.Caption := '取り切りました。２秒後、再現を始めます。Back Spaceキーで前状態復元。Homeキーで新規開始。'
  else if not BallInfoTbl[9].IsStayed then
    PanelForMsg.Caption := '９ボールが入りました。２秒後、再現を始めます。Back Spaceキーで前状態復元。Homeキーで新規開始。'
  else
    PanelForMsg.Caption := '左ボタンで、撞く方向、撞点、力加減を指定。Enterキーでショット。Back Spaceキーで前状態復元。Homeキーで新規開始。';

  TimerForCycle.Enabled := False;

  XQueDir := BallInfoTbl[ObjBallIdx].XPos - BallInfoTbl[0].XPos;
  YQueDir := BallInfoTbl[ObjBallIdx].YPos - BallInfoTbl[0].YPos;
  HSpinRate := 0;
  VSpinRate := 0;
  DrawField;
  DrawFrame;
  if BallInfoTbl[9].IsStayed then
    DrawEstCourse;
  DrawIndicatorArea;

  Result := True;
end;

// 競技エリア内を描く。
procedure TForm1.DrawField;
var
  Y   : Integer;  // 描画位置のＹ座標
  YPos: Integer;  // ポケットの中心位置のＹ座標
  Idx : Integer;  // 球情報テーブル用指標
begin
  for Y := 0 to Margin + FieldSize * 2 + Margin do
    begin
      if (Margin <= Y) and (Y <= Margin + FieldSize) then
        DrawHorizontalLine(Y, Margin, (Margin + FieldSize * 2), FieldColor);

      YPos := Margin + (FieldSize div 2);
      if Abs(Y - YPos) <= RSpot then
        begin
          DrawHorizontalLineInCircle(Y, (Margin + FieldSize     div 2), YPos, RSpot, SpotColor);
          DrawHorizontalLineInCircle(Y, (Margin + FieldSize          ), YPos, RSpot, SpotColor);
          DrawHorizontalLineInCircle(Y, (Margin + FieldSize * 3 div 2), YPos, RSpot, SpotColor);
        end;

      if (Margin <= Y) and (Y <= Margin + FieldSize) then
        for Idx := 0 to High(BallInfoTbl) do
          if BallInfoTbl[Idx].IsStayed then
            DrawHorizontalLineInCircle(Y, Round(BallInfoTbl[Idx].XPos), Round(BallInfoTbl[Idx].YPos),
                                       RBall, BallInfoTbl[Idx].Color                                 );
    end;
end;

// 競技エリアの外枠を描く。
procedure TForm1.DrawFrame;
var
  XS        : Integer;  // 競技エリアの左端Ｘ座標
  XC        : Integer;  // 競技エリアの中央Ｘ座標
  XE        : Integer;  // 競技エリアの右端Ｘ座標
  PocketDiff: Integer;  // 競技エリアの角とポケットの中心位置との差（Ｘ座標とＹ座標で共通）
  Y         : Integer;  // 描画位置のＹ座標
  YPos      : Integer;  // ポケットの中心位置のＹ座標
begin
  XS := Margin                ;
  XC := Margin + FieldSize    ;
  XE := Margin + FieldSize * 2;

  PocketDiff := Round(RBall * Sqrt(2)) + 1;

  for Y := 0 to Margin + FieldSize * 2 + Margin do
    begin
      if (Margin <= Y) and (Y <= Margin + FieldSize) then
        begin
          DrawHorizontalLine(Y, 0, (Margin - 1), MarginColor);
          DrawHorizontalLine(Y, (Margin + FieldSize * 2 + 1), (Margin + FieldSize * 2 + Margin), MarginColor);
        end
      else
        DrawHorizontalLine(Y, 0, (Margin + FieldSize * 2 + Margin), MarginColor);

      if ((Y < Margin            ) and (Y >= Margin             - PocketDiff * 2)) or
         ((Y > Margin + FieldSize) and (Y <= Margin + FieldSize + PocketDiff * 2))    then
        begin
          if Y < Margin then
            YPos := Margin             - PocketDiff * 2
          else
            YPos := Margin + FieldSize + PocketDiff * 2;
          DrawHorizontalLine(Y, XS, Round(XS + Abs(Y - YPos)), FieldColor);
          DrawHorizontalLine(Y, XE, Round(XE - Abs(Y - YPos)), FieldColor);
          if (Y >= Margin - PocketDiff) and (Y <= Margin + FieldSize + PocketDiff) then
            DrawHorizontalLine(Y, (XC - RBall * 2), (XC + RBall * 2), FieldColor);
        end;
      if ((Y >= Margin            ) and (Y < Margin             + PocketDiff * 2)) or
         ((Y <= Margin + FieldSize) and (Y > Margin + FieldSize - PocketDiff * 2))    then
        begin
           if Y < Margin + PocketDiff * 2 then
             YPos := Margin
           else
             YPos := Margin + FieldSize;
          DrawHorizontalLine(Y, Round((XS - PocketDiff * 2) + Abs(Y - YPos)), XS, FieldColor);
          DrawHorizontalLine(Y, Round((XE + PocketDiff * 2) - Abs(Y - YPos)), XE, FieldColor);
        end;

      YPos := Margin - PocketDiff;
      DrawHorizontalLineInCircle(Y, (XS - PocketDiff), YPos, (RBall * 2), PocketColor);
      DrawHorizontalLineInCircle(Y, (XE + PocketDiff), YPos, (RBall * 2), PocketColor);
      if Y < Margin then
        DrawHorizontalLineInCircle(Y, XC, YPos, (RBall * 2), PocketColor);
      YPos := Margin + FieldSize + PocketDiff;
      DrawHorizontalLineInCircle(Y, (XS - PocketDiff), YPos, (RBall * 2), PocketColor);
      DrawHorizontalLineInCircle(Y, (XE + PocketDiff), YPos, (RBall * 2), PocketColor);
      if Y > Margin + FieldSize then
        DrawHorizontalLineInCircle(Y, XC, YPos, (RBall * 2), PocketColor);
    end;
end;

// 球の予想コースを描く。
procedure TForm1.DrawEstCourse;
var
  XObj  : Double ;  // 的球の中心位置Ｘ座標
  YObj  : Double ;  // 的球の中心位置Ｘ座標
  XDiff1: Double ;  // 手球の中心位置に対する指定位置に差のＸ成分
  YDiff1: Double ;  // 手球の中心位置に対する指定位置に差のＹ成分
  XDiff2: Double ;  // 的球の中心位置に対する手球の中心位置の差のＸ成分
  YDiff2: Double ;  // 的球の中心位置に対する手球の中心位置の差のＹ成分
  A     : Double ;  // ２次方程式の２次の項の係数
  B     : Double ;  // ２次方程式の１次の項の係数の半分の値
  C     : Double ;  // ２次方程式の定数項
  D     : Double ;  // ２次方程式の判別式の計算結果
  T     : Double ;  // 直線を表すときのパラメータ
begin
  Canvas.Pen.Color := QueCoursColor;
  Canvas.MoveTo(Round(BallInfoTbl[0].XPos          ), Round(BallInfoTbl[0].YPos          ));
  Canvas.LineTo(Round(BallInfoTbl[0].XPos + XQueDir), Round(BallInfoTbl[0].YPos + YQueDir));
  if ObjBallIdx <= High(BallInfoTbl) then
    begin
      XObj := BallInfoTbl[ObjBallIdx].XPos;
      YObj := BallInfoTbl[ObjBallIdx].YPos;
      XDiff1 := XQueDir;
      YDiff1 := YQueDir;
      XDiff2 := BallInfoTbl[0].XPos - XObj;
      YDiff2 := BallInfoTbl[0].YPos - YObj;
      A := XDiff1 * XDiff1 + YDiff1 * YDiff1;
      B := XDiff1 * XDiff2 + YDiff1 * YDiff2;
      C := XDiff2 * XDiff2 + YDiff2 * YDiff2 - (RBall * 2) * (RBall * 2);
      D := B * B - A * C;
      if (A <> 0) and (D >= 0) then
        begin
          T := (- B - Sqrt(D)) / A;
          Canvas.Pen.Color := ObjCoursColor;
          Canvas.MoveTo(Round(XObj), Round(YObj));
          Canvas.LineTo(Round(XObj - (XDiff2 + XDiff1 * T) * 100),
                        Round(YObj - (YDiff2 + YDiff1 * T) * 100) );
        end;
    end;
end;

// 表示器エリアを描く。
procedure TForm1.DrawIndicatorArea;
var
  Y   : Integer;  // 描画Ｙ座標
  XPos: Integer;  // Ｘ座標
  YPos: Integer;  // Ｙ座標
begin
  for Y := 0 to ClientHeight do
    begin
      DrawHorizontalLine(Y, (Margin + FieldSize * 2 + Margin + 1), ClientWidth, IndAreaBColor);
      if not TimerForCycle.Enabled then
        begin
          XPos := (Margin + FieldSize * 2 + Margin + (XIndAreaSize div 2));
          YPos := (Margin + (FieldSize div 2));
          DrawHorizontalLineInCircle(Y, XPos, YPos, RBallForImpct, clWhite);
          XPos := Round((Margin + FieldSize * 2 + Margin + (XIndAreaSize div 2)) + RBallForImpct * HSpinRate);
          YPos := Round((Margin + (FieldSize div 2))                             - RBallForImpct * VSpinRate);
          DrawHorizontalLineInCircle(Y, XPos, YPos, RImpctPntMark, ImpctMrkColor);
          if Y <= Margin + FieldSize then
            begin
              XPos := ClientWidth - 4;
              if Y >= Margin + Round(FieldSize * (1 - QueVel / MaxQueVel)) then
                DrawHorizontalLine(Y, (XPos - 16), XPos, clWhite)
              else if Y >= Margin then
                DrawHorizontalLine(Y, (XPos - 16), XPos, clBlack);
            end;
        end;
    end;
end;

// 円内の水平線を描く。
//   Y   : Integer  Ｙ座標
//   XPos: Double   中心位置のＸ座標
//   YPos: Double   中心位置のＹ座標
//   R   : Double   半径のＹ座標
//   Col : TColor   色
procedure TForm1.DrawHorizontalLineInCircle(Y, XPos, YPos, R: Integer; Col: TColor);
var
  YDist: Integer;  // 中心位置からの距離のＹ成分
  XDist: Integer;  // 中心位置からの距離のＸ成分
begin
  YDist := Abs(Y - YPos);
  if YDist > R then
    exit;
  XDist := Round(Sqrt(R * R - (YDist - 0.5) * (YDist - 0.5)));
  DrawHorizontalLine(Y, (XPos - XDist), (XPos + XDist), Col);
end;

// 水平線を描く。
//   Y  : Integer  Ｙ座標
//   XS : Integer  開始Ｘ座標
//   XE : Integer  終了Ｘ座標
//   Col: TColor   色
procedure TForm1.DrawHorizontalLine(Y, XS, XE: Integer; Col: TColor);
var
  PX: Integer;  // Ｘ座標値入替え用変数
begin
  if XE < XS then
    begin
      PX  := XS;
      XS := XE;
      XE := PX ;
    end;
  Canvas.Brush.Color := Col;
  Canvas.FillRect(Rect(XS, Y, (XE + 1), (Y + 1) ));
end;

// ベクトルを回転させる。
//   var X: Double  ベクトルＸ座標
//   var Y: Double  ベクトルＹ座標
//   CosT : Double  回転角の余弦
//   SinT : Double  回転角の正弦
procedure RotateVector(var X, Y: Double; CosT, SinT: Double);
var
  NewX: Double;  // 回転後のＸ座標
  NewY: Double;  // 回転後のＹ座標
begin
  NewX := X * CosT - Y * SinT;
  NewY := X * SinT + Y * CosT;
  X    := NewX;
  Y    := NewY;
end;

end.
