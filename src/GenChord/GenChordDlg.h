// GenChordDlg.h : ヘッダー ファイル
//

#if !defined(AFX_GENCHORDDLG_H__9C2FC9E6_CAB3_11D2_9F27_9C3DA5F5D77F__INCLUDED_)
#define AFX_GENCHORDDLG_H__9C2FC9E6_CAB3_11D2_9F27_9C3DA5F5D77F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CGenChordDlg ダイアログ

#include "Generator.h"
#include "Instrument.h"
#include "Common.h"

class CGenChordDlg : public CDialog
{
// 構築
public:
	CGenChordDlg(CWnd* pParent = NULL);	// 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CGenChordDlg)
	enum { IDD = IDD_GENCHORD_DIALOG };
	CEdit	m_EditVoiceNo;
	CButton	m_CheckMinor;
	CEdit	m_EditDegTransWei;
	CEdit	m_EditDegTrans;
	CEdit	m_EditDegLinkWei;
	CEdit	m_EditDegLink;
	CListBox	m_ListDegTrans;
	CListBox	m_ListDegLink;
	CListBox	m_ListDevice;
	CListBox	m_ListChord;
	//}}AFX_DATA

	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CGenChordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
private:
	CGenerator	Generator;
	CInstrument	Instrument;
	void AddChoToListBox(TKeyChord const KeyCho);
	void InitDlgText();
	void MoveDlgTextToInstrument();
	void MoveDlgTextToGenerator();
protected:
	HICON m_hIcon;

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CGenChordDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnStart();
	afx_msg void OnBtnStop();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeListDevice();
	afx_msg void OnSelchangeListDegLink();
	afx_msg void OnBtnUpdateDegLink();
	afx_msg void OnBtnDelDegLink();
	afx_msg void OnSelchangeListDegTrans();
	afx_msg void OnBtnUpdateDegTrans();
	afx_msg void OnBtnDelDegTrans();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_GENCHORDDLG_H__9C2FC9E6_CAB3_11D2_9F27_9C3DA5F5D77F__INCLUDED_)
