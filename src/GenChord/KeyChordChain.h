// KeyChordChain.h: CKeyChordChain クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KEYCHORDCHAIN_H__08861060_E56F_11D2_9F27_9DCD8B6EED93__INCLUDED_)
#define AFX_KEYCHORDCHAIN_H__08861060_E56F_11D2_9F27_9DCD8B6EED93__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ExList.h"
#include "Common.h"

class CKeyChordChain : public CExList<TKeyChord, TKeyChord const&>	// コード連鎖型
{
private:
	int		Wei;				// 取捨選択時の重み（価値）
public:
	CKeyChordChain(): CExList<TKeyChord, TKeyChord const&>() {Wei = 0;}
	CKeyChordChain const& operator =(CKeyChordChain const& KeyChordChain);
	void MultiplyWei(int SrcWei) {Wei *= SrcWei;}
	void SetWei(int SrcWei) {Wei = SrcWei;}
	int GetWei() const {return Wei;}
};

#endif // !defined(AFX_KEYCHORDCHAIN_H__08861060_E56F_11D2_9F27_9DCD8B6EED93__INCLUDED_)
