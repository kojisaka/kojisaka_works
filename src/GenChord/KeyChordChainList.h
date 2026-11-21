// KeyChordChainList.h: CKeyChordChainList クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYCHORDCHAINLIST_H__08861061_E56F_11D2_9F27_9DCD8B6EED93__INCLUDED_)
#define AFX_KEYCHORDCHAINLIST_H__08861061_E56F_11D2_9F27_9DCD8B6EED93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AfxTempl.h>
#include "KeyChordChain.h"

class CKeyChordChainList : public CExList<CKeyChordChain, CKeyChordChain const&>	// コード連鎖のリスト型
{
private:
	int		WeiTotal;			// 取捨選択時の重み（価値）の合計
public:
	CKeyChordChainList(): CExList<CKeyChordChain, CKeyChordChain const&>() {WeiTotal = 0;}
	CKeyChordChainList const& operator =(CKeyChordChainList const& KeyChordChainList);
	void AddWeiTotal(int SrcWeiTotal) {WeiTotal += SrcWeiTotal;}
	int GetWeiTotal() const {return WeiTotal;}
};

#endif // !defined(AFX_KEYCHORDCHAINLIST_H__08861061_E56F_11D2_9F27_9DCD8B6EED93__INCLUDED_)
