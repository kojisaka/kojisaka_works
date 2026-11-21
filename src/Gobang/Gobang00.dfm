object Form1: TForm1
  Left = 198
  Top = 180
  BorderStyle = bsDialog
  Caption = #20116#30446#20006#12409
  ClientHeight = 450
  ClientWidth = 378
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#12468#12471#12483#12463
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnMouseDown = FormMouseDown
  OnPaint = FormPaint
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object gboxFirst: TGroupBox
    Left = 4
    Top = 2
    Width = 181
    Height = 35
    Caption = #40658
    TabOrder = 0
    object rbtnFirstHuman: TRadioButton
      Left = 24
      Top = 14
      Width = 37
      Height = 13
      Caption = #20154
      TabOrder = 0
      OnClick = rbtnItemClick
    end
    object rbtnFirstComputer: TRadioButton
      Left = 80
      Top = 14
      Width = 93
      Height = 13
      Caption = #12467#12531#12500#12517#12540#12479
      TabOrder = 1
      OnClick = rbtnItemClick
    end
  end
  object gboxSecond: TGroupBox
    Left = 192
    Top = 2
    Width = 181
    Height = 35
    Caption = #30333
    TabOrder = 1
    object rbtnSecondHuman: TRadioButton
      Left = 24
      Top = 14
      Width = 37
      Height = 13
      Caption = #20154
      TabOrder = 0
      OnClick = rbtnItemClick
    end
    object rbtnSecondComputer: TRadioButton
      Left = 80
      Top = 14
      Width = 93
      Height = 13
      Caption = #12467#12531#12500#12517#12540#12479
      TabOrder = 1
      OnClick = rbtnItemClick
    end
  end
  object pnlGuideMsg: TPanel
    Left = 0
    Top = 424
    Width = 378
    Height = 25
    Alignment = taLeftJustify
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Font.Charset = SHIFTJIS_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = #65325#65331' '#12468#12471#12483#12463
    Font.Style = []
    ParentFont = False
    TabOrder = 2
  end
  object tmrPlay: TTimer
    Enabled = False
    Interval = 100
    OnTimer = tmrPlayTimer
    Left = 348
    Top = 392
  end
end
