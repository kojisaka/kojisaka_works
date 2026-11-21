// GenChord.h : GENCHORD アプリケーションのメイン ヘッダー ファイルです。
//

#if !defined(AFX_GENCHORD_H__9C2FC9E4_CAB3_11D2_9F27_9C3DA5F5D77F__INCLUDED_)
#define AFX_GENCHORD_H__9C2FC9E4_CAB3_11D2_9F27_9C3DA5F5D77F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// メイン シンボル

/////////////////////////////////////////////////////////////////////////////
// CGenChordApp:
// このクラスの動作の定義に関しては GenChord.cpp ファイルを参照してください。
//

class CGenChordApp : public CWinApp
{
public:
	CGenChordApp();

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CGenChordApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// インプリメンテーション

	//{{AFX_MSG(CGenChordApp)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_GENCHORD_H__9C2FC9E4_CAB3_11D2_9F27_9C3DA5F5D77F__INCLUDED_)
