
// SpaceTorpedoGame.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CSpaceTorpedoGameApp:
// このクラスの実装については、SpaceTorpedoGame.cpp を参照してください
//

class CSpaceTorpedoGameApp : public CWinApp
{
public:
	CSpaceTorpedoGameApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CSpaceTorpedoGameApp theApp;
