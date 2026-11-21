// Generator.h: CGenerator クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GENERATOR_H__5553EF04_CDE3_11D2_9F27_D8D48EFBE71E__INCLUDED_)
#define AFX_GENERATOR_H__5553EF04_CDE3_11D2_9F27_D8D48EFBE71E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common.h"
#include "KeyChordChainList.h"
#include "KeyChordChain.h"

// 生成器型
class CGenerator  
{
private:
	TDegLink		DegLinkRuleTbl[1000];	// 度数連結規則表
	int				NumOfDegLinkRules;		// 度数連結規則数
	TDegTrans		DegTransRuleTbl[1000];	// 度数読替え規則表
	int				NumOfDegTransRules;		// 度数読替え規則数
	int				MaxLenOfTrans;			// 一時転調を認める最大小節数
	TKey			BaseKey;				// 基本にしている調
	CKeyChordChain	GenKeyChoChain;			// 生成したコード連鎖
	TChord			CurPreCho;				// 最終生成時の１つ前のコード
	TKeyChord		CurKeyCho;				// 最終生成時に前提にした調とコード
	int				CurLenOfTrans;			// 最終生成時の一時転調小節数
public:
	CGenerator();
	virtual ~CGenerator();
	void ClearRules();
	void SetBaseKey(TKey Key);
	void AddDegLinkRules(TDegLink DegLink);
	void AddDegTransRules(TDegTrans DegTrans);
	void Reset();
	TKeyChord GetNextKeyCho();
	void ExtendChoChain();
	bool AddChoChainsForAllTranss(bool IsChkOnly, int PivLenOfTrans,
								  TChord PreCho, TKeyChord PivKeyCho,
								  CKeyChordChainList& GenKeyChoChainList);
	bool AddChoChains(bool IsChkOnly, int PivLenOfTrans,
					  TChord const PreCho, TChord const PivCho,
					  TKey const Key, int const PivDeg, int const PivWei,
					  CKeyChordChainList& GenKeyChoChainList             );
	TChord KeyAndDegToCho(TKey const Key, int const Deg);
};

#endif // !defined(AFX_GENERATOR_H__5553EF04_CDE3_11D2_9F27_D8D48EFBE71E__INCLUDED_)
