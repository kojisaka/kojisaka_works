// KeyChordChain.cpp: CKeyChordChain クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GenChord.h"
#include "KeyChordChain.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// 各種処理
//////////////////////////////////////////////////////////////////////

CKeyChordChain const& CKeyChordChain::operator =(CKeyChordChain const& KeyChordChain)
{
	CExList<TKeyChord, TKeyChord const&> const* const pSrc = &KeyChordChain;
	CExList<TKeyChord, TKeyChord const&>* const pDst = this;
	*pDst = *pSrc;
	Wei = KeyChordChain.GetWei();
	return	*this;
}
