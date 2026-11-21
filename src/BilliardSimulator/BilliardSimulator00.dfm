object Form1: TForm1
  Left = 1
  Top = 168
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = #12499#12522#12516#12540#12489#12471#12511#12517#12524#12540#12479#12540
  ClientHeight = 500
  ClientWidth = 1016
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnMouseDown = FormMouseDown
  OnMouseMove = FormMouseMove
  OnPaint = FormPaint
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object PanelForMsg: TPanel
    Left = 0
    Top = 480
    Width = 1016
    Height = 20
    Alignment = taLeftJustify
    BevelOuter = bvNone
    BorderWidth = 4
    Font.Charset = SHIFTJIS_CHARSET
    Font.Color = clBlack
    Font.Height = -15
    Font.Name = #65325#65331' '#12468#12471#12483#12463
    Font.Style = []
    ParentFont = False
    TabOrder = 0
  end
  object TimerForCycle: TTimer
    Enabled = False
    OnTimer = TimerForCycleTimer
    Left = 4
    Top = 4
  end
end
