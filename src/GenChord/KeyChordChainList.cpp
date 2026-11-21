// KeyChordChainList.cpp: CKeyChordChainList クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GenChord.h"
#include "KeyChordChainList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// 各種処理
//////////////////////////////////////////////////////////////////////

CKeyChordChainList const& CKeyChordChainList::operator =(CKeyChordChainList const& KeyChordChainList)
{
	CExList<CKeyChordChain, CKeyChordChain const&> const* const pSrc = &KeyChordChainList;
	CExList<CKeyChordChain, CKeyChordChain const&>* const pDst = this;
	*pDst = *pSrc;
	WeiTotal = KeyChordChainList.GetWeiTotal();
	return	*this;
}
