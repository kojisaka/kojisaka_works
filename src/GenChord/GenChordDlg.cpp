// GenChordDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "GenChord.h"
#include "GenChordDlg.h"
#include "Calculator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int const DefaultVoiceNo = 49;

int const TimeInterval = 2000;
int const DegLinkSPos  = 10;
int const DegTransSPos = 10;
int const GenChordSPos = 6;
char const* const DegLinkTextTbl[] =
{
	"  20     m11M",
	"  80     ?16",
	"  20      16M",
	"  80     ?14",
	"  10      14m",
	"  40      62",
	"  10     m62n",
	"  40     ?251",
	"  40     ?25",
	"  20      44m",
	"  80     ?45",
	"  20     m47",
	"  10     ?15",
	"  80     ?51",
	"  80     ?71",
	"  80     ?56"
};
char const* const DegTransTextTbl[] =
{
	"  40     ?11",
	"  40     ?22",
	"  40     ?33",
	"  40     ?44",
	"  40     ?55",
	"  40     ?66",
	"  40     ?77",
	"  80      24",
	"  80     m64",
	"  20     ?15",
	"  10      14"
};
int const TimerID = 0;

/////////////////////////////////////////////////////////////////////////////
// アプリケーションのバージョン情報で使われている CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard は仮想関数のオーバーライドを生成します
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// メッセージ ハンドラがありません。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenChordDlg ダイアログ

CGenChordDlg::CGenChordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGenChordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGenChordDlg)
	//}}AFX_DATA_INIT
	// メモ: LoadIcon は Win32 の DestroyIcon のサブシーケンスを要求しません。
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGenChordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenChordDlg)
	DDX_Control(pDX, IDC_EDIT_VOICE_NO, m_EditVoiceNo);
	DDX_Control(pDX, IDC_CHECK_MINOR, m_CheckMinor);
	DDX_Control(pDX, IDC_EDIT_DEG_TRANS_WEI, m_EditDegTransWei);
	DDX_Control(pDX, IDC_EDIT_DEG_TRANS, m_EditDegTrans);
	DDX_Control(pDX, IDC_EDIT_DEG_LINK_WEI, m_EditDegLinkWei);
	DDX_Control(pDX, IDC_EDIT_DEG_LINK, m_EditDegLink);
	DDX_Control(pDX, IDC_LIST_DEG_TRANS, m_ListDegTrans);
	DDX_Control(pDX, IDC_LIST_DEG_LINK, m_ListDegLink);
	DDX_Control(pDX, IDC_LIST_DEVICE, m_ListDevice);
	DDX_Control(pDX, IDC_LIST_CHORD, m_ListChord);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CGenChordDlg, CDialog)
	//{{AFX_MSG_MAP(CGenChordDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, OnBtnStart)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStop)
	ON_WM_TIMER()
	ON_LBN_SELCHANGE(IDC_LIST_DEVICE, OnSelchangeListDevice)
	ON_LBN_SELCHANGE(IDC_LIST_DEG_LINK, OnSelchangeListDegLink)
	ON_BN_CLICKED(IDC_BTN_UPDATE_DEG_LINK, OnBtnUpdateDegLink)
	ON_BN_CLICKED(IDC_BTN_DEL_DEG_LINK, OnBtnDelDegLink)
	ON_LBN_SELCHANGE(IDC_LIST_DEG_TRANS, OnSelchangeListDegTrans)
	ON_BN_CLICKED(IDC_BTN_UPDATE_DEG_TRANS, OnBtnUpdateDegTrans)
	ON_BN_CLICKED(IDC_BTN_DEL_DEG_TRANS, OnBtnDelDegTrans)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenChordDlg メッセージ ハンドラ

BOOL CGenChordDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニュー項目をシステム メニューへ追加します。

	// IDM_ABOUTBOX はコマンド メニューの範囲でなければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログ用のアイコンを設定します。フレームワークはアプリケーションのメイン
	// ウィンドウがダイアログでない時は自動的に設定しません。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンを設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンを設定
	
	// TODO: 特別な初期化を行う時はこの場所に追加してください。
	InitDlgText();

	if (m_ListDevice.GetCount() > 0)
	{
		m_ListDevice.SetCurSel(0);
		OnSelchangeListDevice();
	}
	m_ListDegLink.SetCurSel(m_ListDegLink.GetCount() - 1);
	m_ListDegTrans.SetCurSel(m_ListDegTrans.GetCount() - 1);

	return TRUE;  // TRUE を返すとコントロールに設定したフォーカスは失われません。
}

void CGenChordDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// もしダイアログボックスに最小化ボタンを追加するならば、アイコンを描画する
// コードを以下に記述する必要があります。MFC アプリケーションは document/view
// モデルを使っているので、この処理はフレームワークにより自動的に処理されます。

void CGenChordDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画用のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// クライアントの矩形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンを描画します。
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// システムは、ユーザーが最小化ウィンドウをドラッグしている間、
// カーソルを表示するためにここを呼び出します。
HCURSOR CGenChordDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CGenChordDlg::OnSelchangeListDevice() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください

	int const DevId = m_ListDevice.GetCurSel();
	if (! Instrument.Open(DevId))
		MessageBox("<Open Error>");
}

void CGenChordDlg::InitDlgText()
{
	int const NumOfDevs = Instrument.GetNumOfDevs();
	int DevId;
	for (DevId = 0; DevId < NumOfDevs; DevId++)
	{
		CString DevName;
		if (! Instrument.GetDevProp(DevId, DevName))
			DevName = "<Error>";
		m_ListDevice.AddString(DevName);
	}

	CString VoiceNoText;
	VoiceNoText.Format("%4d", DefaultVoiceNo);
	m_EditVoiceNo.SetWindowText(VoiceNoText);

	int DegLinkIdx;
	for (DegLinkIdx = 0;
		 DegLinkIdx < NumOfElems(DegLinkTextTbl); DegLinkIdx++)
		m_ListDegLink.AddString(DegLinkTextTbl[DegLinkIdx]);
	m_ListDegLink.AddString("");

	int DegTransIdx;
	for (DegTransIdx = 0;
		 DegTransIdx < NumOfElems(DegTransTextTbl); DegTransIdx++)
		m_ListDegTrans.AddString(DegTransTextTbl[DegTransIdx]);
	m_ListDegTrans.AddString("");
}

void CGenChordDlg::MoveDlgTextToInstrument()
{
	CString Text;
	m_EditVoiceNo.GetWindowText(Text);
	int const VoiceNo = atoi(Text);
	Instrument.SetVoice(VoiceNo);
}

void CGenChordDlg::MoveDlgTextToGenerator()
{
	Generator.ClearRules();

	TKey BaseKey;
	if (! m_CheckMinor.GetCheck())
	{
		BaseKey.Note.PosInCMajor = 'C';
		BaseKey.Note.HalfIntv    = 0;
		BaseKey.Mode             = ' ';
	}
	else
	{
		BaseKey.Note.PosInCMajor = 'A';
		BaseKey.Note.HalfIntv    = 0;
		BaseKey.Mode             = 'm';
	}
	Generator.SetBaseKey(BaseKey);

	int const NumOfDegLinks = m_ListDegLink.GetCount() - 1;
	int DegLinkIdx;
	for (DegLinkIdx = 0; DegLinkIdx < NumOfDegLinks; DegLinkIdx++)
	{
		TDegLink DegLink;

		CString Text;
		m_ListDegLink.GetText(DegLinkIdx, Text);
		Text.TrimRight();
		int const Len = Text.GetLength() - DegLinkSPos;
		ASSERT(Len >= 2);
		ASSERT(Len < int(NumOfElems(DegLink.Elem)));

		DegLink.Wei = atoi(Text);
		DegLink.KeyMode = Text.GetAt(DegTransSPos - 1);
		int Pos;
		for (Pos = 0; Pos < Len - 1; Pos++)
			DegLink.Elem[Pos] = (Text.GetAt(DegLinkSPos + Pos)) - '0';
		char const Char = Text.GetAt(DegLinkSPos + Pos);
		if ('1' <= Char && Char <= '7')
		{
			DegLink.Elem[Pos] = Char - '0';
			Pos++;
			DegLink.Adjust = ' ';
		}
		else
		{
			DegLink.Adjust = Char;
		}
		ASSERT(Pos >= 2);
		DegLink.Elem[Pos] = 0;
		Generator.AddDegLinkRules(DegLink);
	}

	int const NumOfDegTranss = m_ListDegTrans.GetCount() - 1;
	int DegTransIdx;
	for (DegTransIdx = 0; DegTransIdx < NumOfDegTranss; DegTransIdx++)
	{
		TDegTrans DegTrans;

		CString Text;
		m_ListDegTrans.GetText(DegTransIdx, Text);
		Text.TrimRight();
		int const Len = Text.GetLength() - DegTransSPos;
		DegTrans.Wei = atoi(Text);
		ASSERT(Len == 2);

		DegTrans.KeyMode = (Text.GetAt(DegTransSPos - 1))      ;
		DegTrans.SrcDeg  = (Text.GetAt(DegTransSPos    )) - '0';
		DegTrans.DstDeg  = (Text.GetAt(DegTransSPos + 1)) - '0';
		Generator.AddDegTransRules(DegTrans);
	}
}

void CGenChordDlg::OnBtnStart() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください

	MoveDlgTextToInstrument();
	MoveDlgTextToGenerator();

	m_ListChord.ResetContent();
	Generator.Reset();
	Instrument.Reset();

	TKeyChord const KeyCho = Generator.GetNextKeyCho();
	Instrument.TurnOnCho(KeyCho.Cho);
	AddChoToListBox(KeyCho);

	SetTimer(TimerID, TimeInterval, NULL);

	Generator.ExtendChoChain();
}

void CGenChordDlg::OnBtnStop() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください

	Instrument.Reset();

	KillTimer(TimerID);
}

void CGenChordDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください

	if (nIDEvent == TimerID)
	{
		TKeyChord const KeyCho = Generator.GetNextKeyCho();
		Instrument.TurnOnCho(KeyCho.Cho);
		AddChoToListBox(KeyCho);
		Generator.ExtendChoChain();
	}
}

void CGenChordDlg::AddChoToListBox(TKeyChord const KeyCho)
{
	CString Text;

	CCalculator::AddNoteText((KeyCho.Key.Note), Text);
	if (KeyCho.Key.Mode == 'm')
		Text += "m";
	int Pos;
	for (Pos = Text.GetLength(); Pos < GenChordSPos - 2; Pos++)
		Text += " ";
	Text += ": ";

	CCalculator::AddNoteText((KeyCho.Cho.RootNote), Text);
	TInterval const* const LocListElem = KeyCho.Cho.ElemLocList.Elem;
	int const HalfIntv1 = LocListElem[1].HalfIntv;
	int const HalfIntv2 = LocListElem[2].HalfIntv;
	if (HalfIntv1 == 0 && HalfIntv2 == -1)
		;
	else if (HalfIntv1 == -1 && HalfIntv2 == -1)
		Text += "m";
	else if (HalfIntv1 == -1 && HalfIntv2 == -2)
		Text += "dim";
	else
		Text += "?";

	if (m_ListChord.GetCount() >= 100)
		m_ListChord.DeleteString(0);
	m_ListChord.AddString(Text);
	m_ListChord.SetCurSel(m_ListChord.GetCount() - 1);
}

void CGenChordDlg::OnSelchangeListDegLink() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください

	int const Idx = m_ListDegLink.GetCurSel();
	CString ListItemText;
	m_ListDegLink.GetText(Idx, ListItemText);

	if (ListItemText == "")
	{
		m_EditDegLinkWei.SetWindowText("");
		m_EditDegLink.SetWindowText("");
	}
	else
	{
		m_EditDegLinkWei.SetWindowText(ListItemText.Left(4));
		m_EditDegLink.SetWindowText(ListItemText.Mid(DegLinkSPos - 1));
	}
}

void CGenChordDlg::OnBtnUpdateDegLink() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください

	CString WeiText;
	m_EditDegLinkWei.GetWindowText(WeiText);
	WeiText.TrimRight();
	CString Text;
	m_EditDegLink.GetWindowText(Text);
	Text.TrimRight();
	CString ListItemText;
	ListItemText.Format("%4.4s", WeiText);
	int Pos;
	for (Pos = 4; Pos < DegLinkSPos - 1; Pos++)
	    ListItemText += " ";
    ListItemText += Text;

	int const Idx = m_ListDegLink.GetCurSel();
	if (Idx < m_ListDegLink.GetCount() - 1)
		m_ListDegLink.DeleteString(Idx);
	m_ListDegLink.InsertString(Idx, ListItemText);
	m_ListDegLink.SetCurSel(Idx);
}

void CGenChordDlg::OnBtnDelDegLink() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください

	int const Idx = m_ListDegLink.GetCurSel();
	if (Idx < m_ListDegLink.GetCount() - 1)
	{
		m_ListDegLink.DeleteString(Idx);
		m_EditDegLinkWei.SetWindowText("");
		m_EditDegLink.SetWindowText("");
		m_ListDegLink.SetCurSel(Idx);
	}
}

void CGenChordDlg::OnSelchangeListDegTrans() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください

	int const Idx = m_ListDegTrans.GetCurSel();
	CString ListItemText;
	m_ListDegTrans.GetText(Idx, ListItemText);

	if (ListItemText == "")
	{
		m_EditDegTransWei.SetWindowText("");
		m_EditDegTrans.SetWindowText("");
	}
	else
	{
		m_EditDegTransWei.SetWindowText(ListItemText.Left(4));
		m_EditDegTrans.SetWindowText(ListItemText.Mid(DegTransSPos - 1));
	}
}

void CGenChordDlg::OnBtnUpdateDegTrans() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください

	CString WeiText;
	m_EditDegTransWei.GetWindowText(WeiText);
	WeiText.TrimRight();
	CString Text;
	m_EditDegTrans.GetWindowText(Text);
	Text.TrimRight();
	CString ListItemText;
	ListItemText.Format("%4.4s", WeiText);
	int Pos;
	for (Pos = 4; Pos < DegTransSPos - 1; Pos++)
	    ListItemText += " ";
    ListItemText += Text;

	int const Idx = m_ListDegTrans.GetCurSel();
	if (Idx < m_ListDegTrans.GetCount() - 1)
		m_ListDegTrans.DeleteString(Idx);
	m_ListDegTrans.InsertString(Idx, ListItemText);
	m_ListDegTrans.SetCurSel(Idx);
}

void CGenChordDlg::OnBtnDelDegTrans() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください

	int const Idx = m_ListDegTrans.GetCurSel();
	if (Idx < m_ListDegTrans.GetCount() - 1)
	{
		m_ListDegTrans.DeleteString(Idx);
		m_EditDegTransWei.SetWindowText("");
		m_EditDegTrans.SetWindowText("");
		m_ListDegTrans.SetCurSel(Idx);
	}
}
