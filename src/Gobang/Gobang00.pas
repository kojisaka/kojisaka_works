unit Gobang00;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  ExtCtrls, StdCtrls;

type
  TLine = record           // 石の列
    Turn      : Integer;    // 手番（1:先手番, 2:後手番）
    Col       : Integer;    // 筋番号
    Row       : Integer;    // 段番号
    ColDir    : Integer;    // 石の列の方向の筋成分
    RowDir    : Integer;    // 石の列の方向の段成分
    Len       : Integer;    // 石の列の長さ
    DelLastIdx: Integer;    // 削除された時点の石の列テーブルの最後の添字
  end;

type
  TForm1 = class(TForm)
    gboxFirst: TGroupBox;
    rbtnFirstHuman: TRadioButton;
    rbtnFirstComputer: TRadioButton;
    gboxSecond: TGroupBox;
    rbtnSecondHuman: TRadioButton;
    rbtnSecondComputer: TRadioButton;
    pnlGuideMsg: TPanel;
    tmrPlay: TTimer;
    procedure FormShow(Sender: TObject);
    procedure rbtnItemClick(Sender: TObject);
    procedure FormMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure FormPaint(Sender: TObject);
    procedure tmrPlayTimer(Sender: TObject);
  private
    { Private 宣言 }
    ComPlayFlagTbl: array[1..2] of Boolean        ;  // コンピュータ競技フラグ・テーブル
    CrossStateTbl : array[0..20, 0..20] of Integer;  // 格子状態テーブル
    LineTbl       : array[1..400] of TLine        ;  // 石の列テーブル
    LastIdxOfLine : Integer                       ;  // 石の列テーブルの最後の添字
    CurrentTurn   : Integer                       ;  // 現在の手番（1:先手番, 2:後手番）
    LastCol       : Integer                       ;  // 最後に打った位置の筋
    LastRow       : Integer                       ;  // 最後に打った位置の段
    procedure PutStoneOnBoard(Col, Row: Integer);
    procedure DisplayGuideMsg;
    function Think(Depth, Turn: Integer; var SelCol, SelRow: Integer): Integer;
    function CalcValue(Turn: Integer): Integer;
    procedure RecoverLineTbl(LastIdx: Integer);
    function AdjustLineTbl(Turn, SelCol, SelRow: Integer): Boolean;
    function AdjustLineTblForDir(Turn, SelCol, SelRow, ColDir, RowDir: Integer): Boolean;
    procedure DrawStone(Col, Row: Integer);
  public
    { Public 宣言 }
  end;

var
  Form1: TForm1;

implementation

{$R *.DFM}

const
  DepthLimit =  3;       // 読みの深さの限界値
  XSCross    =  9;       // 開始格子点Ｘ座標
  YSCross    = 50;       // 開始格子点Ｙ座標
  Interval   = 20;       // 盤の線の間隔
  ValueMax   = 1000000;  // 評価値の最大値
  EValueDiff =  500000;  // 読みを打ち切っても充分な評価値差

procedure TForm1.FormShow(Sender: TObject);
var
  Col: Integer;  // 筋番号
  Row: Integer;  // 段番号
begin
  for Col := 0 to 20 do
    for Row := 0 to 20 do
      if (Col < 1) or (19 < Col) or (Row < 1) or (19 < Row) then
        CrossStateTbl[Col, Row] := -1
      else
        CrossStateTbl[Col, Row] := 0;
  LastIdxOfLine := 0;
  CurrentTurn   := 0;
  pnlGuideMsg.Caption := '人かコンピュータかを指定してください。';
end;

procedure TForm1.rbtnItemClick(Sender: TObject);
begin
  if ((not rbtnFirstHuman .Checked) and (not rbtnFirstComputer .Checked)) or
     ((not rbtnSecondHuman.Checked) and (not rbtnSecondComputer.Checked))    then
    exit;
  ComPlayFlagTbl[1] := rbtnFirstComputer .Checked;
  ComPlayFlagTbl[2] := rbtnSecondComputer.Checked;
  CurrentTurn := 1;
  gboxFirst .Enabled := False;
  gboxSecond.Enabled := False;
  tmrPlay.Enabled := ComPlayFlagTbl[CurrentTurn];
  DisplayGuideMsg;
end;

procedure TForm1.FormMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
var
  Col: Integer;  // 筋番号
  Row: Integer;  // 段番号
begin
  if tmrPlay.Enabled then
    exit;
  if CurrentTurn <= 0 then
    exit;
  Col := Round((X - XSCross) / Interval) + 1;
  Row := Round((Y - YSCross) / Interval) + 1;
  if (Row < 1) or (19 < Row) then
    exit;
  if CrossStateTbl[Col, Row] <> 0 then
    exit;
  PutStoneOnBoard(Col, Row);
end;

procedure TForm1.FormPaint(Sender: TObject);
const
  Len = Interval * 18;
var
  Col: Integer;  // 筋番号
  Row: Integer;  // 段番号
begin
  Canvas.Brush.Color := RGB(192, 128, 0);
  Canvas.FillRect(Rect((XSCross       -  9), (YSCross       -  9),
                       (XSCross + Len + 10), (YSCross + Len + 10) ));
  Canvas.Brush.Color := clBlack;
  for Col := 1 to 19 do
    Canvas.FillRect(Rect((XSCross + Interval * (Col - 1)    ),  YSCross           ,
                         (XSCross + Interval * (Col - 1) + 1), (YSCross + Len + 1) ));
  for Row := 1 to 19 do
    Canvas.FillRect(Rect( XSCross           , (YSCross + Interval * (Row - 1)    ),
                         (XSCross + Len + 1), (YSCross + Interval * (Row - 1) + 1) ));
  Canvas.Pen.Color := clBlack;
  Canvas.Ellipse((XSCross + Interval * 9 - 2), (YSCross + Interval * 9 - 2),
                 (XSCross + Interval * 9 + 3), (YSCross + Interval * 9 + 3) );
  for Col := 1 to 19 do
    for Row := 1 to 19 do
      DrawStone(Col, Row);
end;

procedure TForm1.tmrPlayTimer(Sender: TObject);
var
  Value: Integer;  // 評価値
  Col  : Integer;  // 筋番号
  Row  : Integer;  // 段番号
begin
  tmrPlay.Enabled := False;
  Value := Think(1, CurrentTurn, Col, Row);
  if Col = 0 then
    begin
      CurrentTurn := 0;
      exit;
    end;
  PutStoneOnBoard(Col, Row);
  if Value = - ValueMax then
    pnlGuideMsg.Caption := pnlGuideMsg.Caption + '必勝です。';
end;

// 石を指定位置に打つ
//   Col  Integer  筋番号
//   Row  Integer  段番号
procedure TForm1.PutStoneOnBoard(Col, Row: Integer);
var
  PrevCol : Integer;  // 最後の１つ前に打った位置の筋
  PrevRow : Integer;  // 最後の１つ前に打った位置の段
  IsToWin : Boolean;  // 今打った側が勝ちならTrue
  IsToLoss: Boolean;  // 今打った側が負けならTrue
  Msg     : String ;  // メッセージ
begin
  CrossStateTbl[Col, Row] := CurrentTurn;
  PrevCol := LastCol;
  PrevRow := LastRow;
  LastCol := Col;
  LastRow := Row;
  DrawStone(PrevCol, PrevRow);
  DrawStone(Col, Row);
  IsToWin  := AdjustLineTbl(CurrentTurn, Col, Row);
  IsToLoss := (CalcValue(CurrentTurn) = - MaxInt);
  if IsToWin or IsToLoss then
    begin
      if CurrentTurn = 1 then
        Msg := '黒の'
      else
        Msg := '白の';
      if IsToWin then
        Msg := Msg + '勝ちです。'
      else
        Msg := Msg + '負けです。反則です。';
      pnlGuideMsg.Caption := Msg;
      MessageDlg(Msg, mtInformation, [mbOK], 0);
      CurrentTurn := 0;
      exit;
    end;
  CurrentTurn := (1 + 2) - CurrentTurn;
  tmrPlay.Enabled := ComPlayFlagTbl[CurrentTurn];
  DisplayGuideMsg;
end;

// 案内メッセージを表示する
procedure TForm1.DisplayGuideMsg;
var
  GuideMsg: String;
begin
  if CurrentTurn = 1 then
    GuideMsg := '黒の番です。'
  else
    GuideMsg := '白の番です。';
  if tmrPlay.Enabled then
    GuideMsg := GuideMsg + 'コンピュータが考えています。'
  else
    GuideMsg := GuideMsg + '打つ位置を指して左ボタンを押してください。';
  pnlGuideMsg.Caption := GuideMsg;
  pnlGuideMsg.SetFocus;
end;

// 手を考える
//   Depth       Integer  今読んでいる深さ
//   Turn        Integer  手番（1:先手番, 2:後手番）
//   var SelCol  Integer  選択した手の筋番号
//   var SelRow  Integer  選択した手の段番号
//   戻り値      Integer  選択した手の評価値
function TForm1.Think(Depth, Turn: Integer; var SelCol, SelRow: Integer): Integer;
type
  TPosAndVal = record  // 位置・評価値型
    Col  : Integer;        // 筋番号
    Row  : Integer;        // 段番号
    Value: Integer;        // 評価値
  end;
var
  PosAndValTbl: array[1..(19 * 19)] of TPosAndVal;  // 位置・評価値テーブル
  LastIdxOfPV : Integer;  // 位置・評価値テーブルの最後の添字
  Col         : Integer;  // 筋番号
  Row         : Integer;  // 段番号
  ResSide     : Integer;  // 応手の手番
  PrevLastIdx : Integer;  // 石の列テーブルの読み開始前の最後の添字
  ResCol      : Integer;  // 応手の筋番号
  ResRow      : Integer;  // 応手の段番号
  Value       : Integer;  // 読んでいる手の評価値
  Idx         : Integer;  // 指標
begin
  SelCol := 0;
  SelRow := 0;
  Result := - MaxInt;
  LastIdxOfPV := 0;
  for Col := 1 to 19 do
    for Row := 1 to 19 do
      if (CrossStateTbl[Col, Row] = 0) and
         ((CrossStateTbl[Col + 1, Row    ] > 0) or (CrossStateTbl[Col - 1, Row    ] > 0) or
          (CrossStateTbl[Col    , Row - 1] > 0) or (CrossStateTbl[Col    , Row + 1] > 0) or
          (CrossStateTbl[Col - 1, Row - 1] > 0) or (CrossStateTbl[Col + 1, Row - 1] > 0) or
          (CrossStateTbl[Col - 1, Row + 1] > 0) or (CrossStateTbl[Col + 1, Row + 1] > 0)   ) then
        begin
          CrossStateTbl[Col, Row] := Turn;
          PrevLastIdx := LastIdxOfLine;
          if AdjustLineTbl(Turn, Col, Row) then
            Value := ValueMax
          else
            Value := CalcValue(Turn);
          RecoverLineTbl(PrevLastIdx);
          CrossStateTbl[Col, Row] := 0;
          Inc(LastIdxOfPV);
          Idx := LastIdxOfPV;
          while Idx >= 2 do
            begin
              if PosAndValTbl[Idx - 1].Value >= Value then
                break;
              PosAndValTbl[Idx] := PosAndValTbl[Idx - 1];
              Dec(Idx);
            end;
          PosAndValTbl[Idx].Col   := Col  ;
          PosAndValTbl[Idx].Row   := Row  ;
          PosAndValTbl[Idx].Value := Value;
          if Value > ValueMax then
            break;
        end;
  if LastIdxOfPV = 0 then
    begin
      if CrossStateTbl[10, 10] > 0 then
        exit;
      LastIdxOfPV := 1;
      PosAndValTbl[1].Value :=  0;
      PosAndValTbl[1].Col   := 10;
      PosAndValTbl[1].Row   := 10;
    end;
  if (Depth >= DepthLimit) or (PosAndValTbl[1].Value >= ValueMax) then
    begin
      Result := PosAndValTbl[1].Value;
      SelCol := PosAndValTbl[1].Col  ;
      SelRow := PosAndValTbl[1].Row  ;
      exit;
    end;
  ResSide := (1 + 2) - Turn;
  for Idx := 1 to LastIdxOfPV do
    begin
      if PosAndValTbl[Idx].Value + EValueDiff < Result then
        break;
      Col := PosAndValTbl[Idx].Col;
      Row := PosAndValTbl[Idx].Row;
      CrossStateTbl[Col, Row] := Turn;
      PrevLastIdx := LastIdxOfLine;
      if AdjustLineTbl(Turn, Col, Row) then
        Value := ValueMax
      else
        Value := - Think((Depth + 1), ResSide, ResCol, ResRow);
      RecoverLineTbl(PrevLastIdx);
      CrossStateTbl[Col, Row] := 0;
      if Value >= Result then
        begin
          Result := Value;
          SelCol := Col;
          SelRow := Row;
          if Result >= EValueDiff then
            break;
        end;
    end;
end;

// 評価値を求める
//   Turn  Integer  手番（1:先手番, 2:後手番）
function TForm1.CalcValue(Turn: Integer): Integer;
const
  StdValTbl: array[2..4] of Integer = (1, 3, 5);  // 評価標準値テーブル
var
  IsFailLoss: Boolean;  // 必敗ならTrue
  NumOfChks : Integer;  // 打つと５個並ぶ箇所の数
  NumOf3SsLs: Integer;  // ３個並んで両端が空いている石の列の数
  Idx       : Integer;  // 指標
  LineSide  : Integer;  // 手番（1:先手番, 2:後手番）
  LineColDir: Integer;  // 石の列の方向の筋成分
  LineRowDir: Integer;  // 石の列の方向の段成分
  LineLen   : Integer;  // 石の列の長さ
  LSOutCol  : Integer;  // 石の列の始点側の１つ外の位置の筋番号
  LSOutRow  : Integer;  // 石の列の始点側の１つ外の位置の筋番号
  LEOutCol  : Integer;  // 石の列の終点側の１つ外の位置の筋番号
  LEOutRow  : Integer;  // 石の列の終点側の１つ外の位置の筋番号
  IsLSOutVac: Boolean;  // 石の列の始点側の１つ外が空きならTrue
  IsLEOutVac: Boolean;  // 石の列の終点側の１つ外が空きならTrue
  Value     : Integer;  // 対象の石の列についての評価値
begin
  IsFailLoss := False;
  NumOfChks  := 0;
  NumOf3SsLs := 0;
  Result := 0;
  for Idx := 1 to LastIdxOfLine do
    if LineTbl[Idx].DelLastIdx > LastIdxOfLine then
      begin
        LineSide   := LineTbl[Idx].Turn  ;
        LineColDir := LineTbl[Idx].ColDir;
        LineRowDir := LineTbl[Idx].RowDir;
        LineLen    := LineTbl[Idx].Len   ;
        LSOutCol   := LineTbl[Idx].Col - LineColDir;
        LSOutRow   := LineTbl[Idx].Row - LineRowDir;
        LEOutCol   := LineTbl[Idx].Col + LineColDir * LineLen;
        LEOutRow   := LineTbl[Idx].Row + LineRowDir * LineLen;
        IsLSOutVac := (CrossStateTbl[LSOutCol, LSOutRow] = 0);
        IsLEOutVac := (CrossStateTbl[LEOutCol, LEOutRow] = 0);
        if LineSide = Turn then
          begin
            Value := StdValTbl[LineLen];
            if LineLen = 4 then
              begin
                if IsLSOutVac and IsLEOutVac then
                  Inc(NumOfChks, 2)
                else if IsLSOutVac or IsLEOutVac then
                  Inc(NumOfChks);
              end
            else if LineLen = 3 then
              begin
                if IsLSOutVac and IsLEOutVac then
                  Inc(NumOf3SsLs);
              end;
           end
        else
          begin
            Value := - StdValTbl[LineLen];
            if LineLen = 4 then
              begin
                if IsLSOutVac or IsLEOutVac then
                  begin
                    Result := - ValueMax;
                    exit;
                  end;
              end
            else if LineLen = 3 then
              begin
                if IsLSOutVac and IsLEOutVac then
                  IsFailLoss := True;
              end;
           end;
        Inc(Result, Value);
      end;
  if NumOf3SsLs >= 2 then
    Result := - MaxInt
  else if NumOfChks >= 2 then
    Result := ValueMax - 1
  else if IsFailLoss then
    Result := - (ValueMax - 1)
  else if NumOfChks + NumOf3SsLs >= 2 then
    Result := ValueMax - 2;
end;

// 石の列テーブルを復元する
//   LastIdx  Integer  石の列テーブルの復元時の最後の添字
procedure TForm1.RecoverLineTbl(LastIdx: Integer);
var
  Idx: Integer;  // 指標
begin
  for Idx := 1 to LastIdx do
    if LineTbl[Idx].DelLastIdx > LastIdx then
      LineTbl[Idx].DelLastIdx := MaxInt;
  LastIdxOfLine := LastIdx;
end;

// 打った位置周辺について石の列テーブルを調整する
//   Turn    Integer  手番（1:先手番, 2:後手番）
//   SelCol  Integer  打った位置の筋番号
//   SelRow  Integer  打った位置の段番号
//   戻り値  Boolean  ５個並んでいるのを見つけたのならTrue
function TForm1.AdjustLineTbl(Turn, SelCol, SelRow: Integer): Boolean;
begin
  Result := (AdjustLineTblForDir(Turn, SelCol, SelRow, 1,  0) or
             AdjustLineTblForDir(Turn, SelCol, SelRow, 0,  1) or
             AdjustLineTblForDir(Turn, SelCol, SelRow, 1,  1) or
             AdjustLineTblForDir(Turn, SelCol, SelRow, 1, -1)   );
end;

// 打った位置周辺の指定方向について石の列テーブルを調整する
//   Turn    Integer  手番（1:先手番, 2:後手番）
//   SelCol  Integer  打った位置の筋番号
//   SelRow  Integer  打った位置の段番号
//   ColDir  Integer  調整対象の方向の筋成分
//   RowDir  Integer  調整対象の方向の段成分
//   戻り値  Boolean  ５個並んでいるのを見つけたのならTrue
function TForm1.AdjustLineTblForDir(Turn, SelCol, SelRow, ColDir, RowDir: Integer): Boolean;
var
  BkwdDist : Integer;  // 打った位置から逆方向への距離
  FrwdDist : Integer;  // 打った位置から順方向への距離
  Col      : Integer;  // 筋番号
  Row      : Integer;  // 段番号
  SCol     : Integer;  // すでにある石の列の始点の筋
  SRow     : Integer;  // すでにある石の列の始点の段
  ECol     : Integer;  // すでにある石の列の終点の筋
  ERow     : Integer;  // すでにある石の列の終点の段
  ColMin   : Integer;  // 今できた石の列の筋の最小値
  RowMin   : Integer;  // 今できた石の列の段の最小値
  ColMax   : Integer;  // 今できた石の列の筋の最大値
  RowMax   : Integer;  // 今できた石の列の段の最大値
  Len      : Integer;  // 今できた石の列の長さ
  Idx      : Integer;  // 指標
begin
  Result := False;
  BkwdDist := 0;
  while True do
    begin
      Col := SelCol - ColDir * (BkwdDist + 1);
      Row := SelRow - RowDir * (BkwdDist + 1);
      if CrossStateTbl[Col, Row] <> Turn then
        break;
      Inc(BkwdDist);
      if BkwdDist > 4 then
        break;
    end;
  FrwdDist := 0;
  while True do
    begin
      Col := SelCol + ColDir * (FrwdDist + 1);
      Row := SelRow + RowDir * (FrwdDist + 1);
      if CrossStateTbl[Col, Row] <> Turn then
        break;
      Inc(FrwdDist);
      if FrwdDist > 4 then
        break;
    end;
  Len := BkwdDist + FrwdDist + 1;
  if Len > 4 then
    begin
      Result := True;
      exit;
    end;
  if Len < 2 then
    exit;
  Idx := 1;
  while Idx <= LastIdxOfLine do
    begin
      if (LineTbl[Idx].Turn       = Turn         ) and
         (LineTbl[Idx].ColDir     = ColDir       ) and
         (LineTbl[Idx].RowDir     = RowDir       ) and
         (LineTbl[Idx].DelLastIdx > LastIdxOfLine) and
         (  (LineTbl[Idx].Col - SelCol) * RowDir
          = (LineTbl[Idx].Row - SelRow) * ColDir)     then
        begin
          SCol := LineTbl[Idx].Col;
          SRow := LineTbl[Idx].Row;
          ECol := LineTbl[Idx].Col + ColDir * LineTbl[Idx].Len;
          ERow := LineTbl[Idx].Row + RowDir * LineTbl[Idx].Len;
          if ColDir >= 0 then
            begin
              ColMin := SelCol - ColDir * BkwdDist;
              ColMax := SelCol + ColDir * FrwdDist;
            end
          else
            begin
              ColMin := SelCol + ColDir * FrwdDist;
              ColMax := SelCol - ColDir * BkwdDist;
            end;
          if RowDir >= 0 then
            begin
              RowMin := SelRow - RowDir * BkwdDist;
              RowMax := SelRow + RowDir * FrwdDist;
            end
          else
            begin
              RowMin := SelRow + RowDir * FrwdDist;
              RowMax := SelRow - RowDir * BkwdDist;
            end;
          if ((ColMin <= SCol) and (SCol <= ColMax) and
              (RowMin <= SRow) and (SRow <= RowMax)    ) or
             ((ColMin <= ECol) and (ECol <= ColMax) and
              (RowMin <= ERow) and (ERow <= RowMax)    )    then
            LineTbl[Idx].DelLastIdx := LastIdxOfLine + 1;
        end;
      Inc(Idx);
    end;
  Inc(LastIdxOfLine);
  LineTbl[LastIdxOfLine].Turn       := Turn  ;
  LineTbl[LastIdxOfLine].Col        := SelCol - ColDir * BkwdDist;
  LineTbl[LastIdxOfLine].Row        := SelRow - RowDir * BkwdDist;
  LineTbl[LastIdxOfLine].ColDir     := ColDir;
  LineTbl[LastIdxOfLine].RowDir     := RowDir;
  LineTbl[LastIdxOfLine].Len        := Len   ;
  LineTbl[LastIdxOfLine].DelLastIdx := MaxInt;
end;

// 石を指定位置に描く
//   Col  Integer  筋番号
//   Row  Integer  段番号
procedure TForm1.DrawStone(Col, Row: Integer);
var
  CrossState: Integer;  // 格子状態
  StoneCol  : TColor;   // 石の色
begin
  CrossState := CrossStateTbl[Col, Row];
  if CrossState = 1 then
    StoneCol := clBlack
  else if CrossState = 2 then
    StoneCol := clWhite
  else
    exit;
  if (Col = LastCol) and (Row = LastRow) then
    Canvas.Pen.Color := clRed
  else
    Canvas.Pen.Color := StoneCol;
  Canvas.Brush.Color := StoneCol;
  Canvas.Ellipse((XSCross + Interval * (Col - 1) -  9),
                 (YSCross + Interval * (Row - 1) -  9),
                 (XSCross + Interval * (Col - 1) + 10),
                 (YSCross + Interval * (Row - 1) + 10) );
end;

end.

