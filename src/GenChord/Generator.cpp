// Generator.cpp: CGenerator クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

// stdafx.h : 標準のシステム インクルード ファイル、
//			  または参照回数が多く、かつあまり変更されない
//			  プロジェクト専用のインクルード ファイルを記述します。
//

#include "stdafx.h"
#include "GenChord.h"
#include "Generator.h"
#include "Calculator.h"
#include "KeyChordChainList.h"
#include "KeyChordChain.h"
#include "Common.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// 定数
//////////////////////////////////////////////////////////////////////

TChordElemLocList const ChordElemLocListTbl[] =
{
	{ 1, {{ 0, 0}, { 2, 0}, { 4,-1}, {-1, 0}, { 0, 0}}},
	{ 1, {{ 0, 0}, { 2,-1}, { 4,-1}, {-1, 0}, { 0, 0}}},
	{ 1, {{ 0, 0}, { 2,-1}, { 4,-2}, {-1, 0}, { 0, 0}}},
	{-1}
};

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CGenerator::CGenerator()
{
	BaseKey.Note.PosInCMajor = 'C';
	BaseKey.Note.HalfIntv    = 0;
	BaseKey.Mode             = ' ';

	MaxLenOfTrans = 16;

	ClearRules();
	Reset();
}

CGenerator::~CGenerator()
{
}

//////////////////////////////////////////////////////////////////////
// 各種処理
//////////////////////////////////////////////////////////////////////

void CGenerator::ClearRules()
{
	NumOfDegLinkRules  = 0;
	NumOfDegTransRules = 0;
}

void CGenerator::SetBaseKey(TKey Key)
{
	BaseKey = Key;
}

void CGenerator::AddDegLinkRules(TDegLink DegLink)
{
	ASSERT(NumOfDegLinkRules < NumOfElems(DegLinkRuleTbl));
	DegLinkRuleTbl[NumOfDegLinkRules] = DegLink;
	NumOfDegLinkRules++;
}

void CGenerator::AddDegTransRules(TDegTrans DegTrans)
{
	ASSERT(NumOfDegTransRules < NumOfElems(DegTransRuleTbl));
	DegTransRuleTbl[NumOfDegTransRules] = DegTrans;
	NumOfDegTransRules++;
}

void CGenerator::Reset()
{
	GenKeyChoChain.RemoveAll();

	TKeyChord KeyCho;
	KeyCho.Key = BaseKey;
	KeyCho.Cho = KeyAndDegToCho(BaseKey, 1);
	GenKeyChoChain.AddTail(KeyCho);

	CurPreCho.RootNote.PosInCMajor = ' ';

	CurKeyCho = KeyCho;
	CurKeyCho.Key.Note.PosInCMajor = ' ';

	CurLenOfTrans = 0;
}

TKeyChord CGenerator::GetNextKeyCho()
{
	ASSERT(! GenKeyChoChain.IsEmpty());
	return	GenKeyChoChain.RemoveHead();
}

void CGenerator::ExtendChoChain()
{
	if (! GenKeyChoChain.IsEmpty())
		return;

	CKeyChordChainList KeyChoChainList;

	AddChoChainsForAllTranss(FALSE, CurLenOfTrans, CurPreCho, CurKeyCho, KeyChoChainList);

	if (KeyChoChainList.GetWeiTotal() == 0)
	{
		GenKeyChoChain.AddTail(CurKeyCho);
		return;
	}

	int const RandVal = ((rand() << 15) ^ rand());
	int const PosInWeightTotal = (RandVal % (KeyChoChainList.GetWeiTotal()));

	int WeiAccum;
	WeiAccum = 0;
	int const NumOfKeyChoChains = KeyChoChainList.GetCount();
	int FoundKeyChoChainIdx;
	FoundKeyChoChainIdx = NumOfKeyChoChains;
	int KeyChoChainIdx;
	for (KeyChoChainIdx = 0; KeyChoChainIdx < NumOfKeyChoChains; KeyChoChainIdx++)
	{
		WeiAccum += KeyChoChainList.GetByIdx(KeyChoChainIdx).GetWei();
		if (PosInWeightTotal < WeiAccum)
			if (FoundKeyChoChainIdx == NumOfKeyChoChains)
				FoundKeyChoChainIdx = KeyChoChainIdx;
	}
	ASSERT(FoundKeyChoChainIdx < NumOfKeyChoChains);
	ASSERT(WeiAccum == KeyChoChainList.GetWeiTotal());
	CKeyChordChain const& KeyChoChain = KeyChoChainList.GetByIdx(FoundKeyChoChainIdx);
	GenKeyChoChain.AddListToTail(&KeyChoChain);

	CurPreCho = CurKeyCho.Cho;
	CurKeyCho = GenKeyChoChain.GetTail();
	if (CurKeyCho.Key == BaseKey)
		CurLenOfTrans = 0;
	else
		CurLenOfTrans += (KeyChoChain.GetCount());
}

bool CGenerator::AddChoChainsForAllTranss(bool IsChkOnly, int PivLenOfTrans,
										  TChord PreCho, TKeyChord PivKeyCho,
										  CKeyChordChainList& GenKeyChoChainList)
{
	TKey const SrcKey = PivKeyCho.Key;
	TChord const PivCho = PivKeyCho.Cho;
	int SrcDeg;
	if (SrcKey.Note.PosInCMajor == ' ')
		SrcDeg = 1;
	else
		SrcDeg = CCalculator::KeyAndChoToDeg(SrcKey, PivCho);
	if (SrcDeg <= 0)
	{
		TInterval const* const LocListElem = PivCho.ElemLocList.Elem;
		int const HalfIntv1 = LocListElem[1].HalfIntv;
		int const HalfIntv2 = LocListElem[2].HalfIntv;
		if (HalfIntv1 == 0 && HalfIntv2 == -1)
			SrcDeg = 5;
		else if (HalfIntv1 == -1 && HalfIntv2 == -1)
			SrcDeg = 4;
		else
			ASSERT(FALSE);
	}
	int DegTransIdx;
	for (DegTransIdx = 0; DegTransIdx < NumOfDegTransRules; DegTransIdx++)
	{
		TDegTrans const DegTrans = DegTransRuleTbl[DegTransIdx];
		char const KeyMode = DegTrans.KeyMode;
		if ((SrcKey.Note.PosInCMajor != ' ' || DegTrans.DstDeg == 1  ) &&
		    (KeyMode == '?'                 || KeyMode == SrcKey.Mode) &&
		    DegTrans.SrcDeg == SrcDeg                                    )
		{
			int const DstDeg = DegTrans.DstDeg;
			int const Wei = DegTrans.Wei;

			TNote Note;
			Note.PosInCMajor = 'G';
			Note.HalfIntv = -1;

			int Counter;
			for (Counter = 0; Counter < 16; Counter++)
			{
				TKey Key;
				Key.Note = Note;
				Key.Mode = ' ';
				if (CCalculator::CompKeyAndDegWithCho(Key, DstDeg, PivCho))
					if (AddChoChains(IsChkOnly, PivLenOfTrans, PreCho, PivCho,
					                 Key, DstDeg, Wei, GenKeyChoChainList     ))
						return	TRUE;
				Key.Note = CCalculator::AddIntvToNote(Note, CCalculator::MakeInterval(-2, 1));
				Key.Mode = 'm';
				if (CCalculator::CompKeyAndDegWithCho(Key, DstDeg, PivCho))
					if (AddChoChains(IsChkOnly, PivLenOfTrans, PreCho, PivCho,
					                 Key, DstDeg, Wei, GenKeyChoChainList    ))
						return	TRUE;
				Note = CCalculator::AddIntvToNote(Note, CCalculator::MakeInterval(4, -1));
			}
		}
	}

	return	FALSE;
}

bool CGenerator::AddChoChains(bool IsChkOnly, int PivLenOfTrans,
							  TChord const PreCho, TChord const PivCho,
							  TKey const Key, int const PivDeg, int const PivWei,
							  CKeyChordChainList& GenKeyChoChainList             )
{
	int WeiTotal;
	WeiTotal = 0;

	int DegLinkIdx;
	for (DegLinkIdx = 0; DegLinkIdx < NumOfDegLinkRules; DegLinkIdx++)
	{
		TDegLink const DegLink = DegLinkRuleTbl[DegLinkIdx];
		char const KeyMode = DegLink.KeyMode;
		if ((KeyMode == '?' || KeyMode == Key.Mode) && DegLink.Elem[0] == PivDeg)
		{
			ASSERT(DegLink.Elem[1] > 0);

			int const Wei = PivWei * (DegLink.Wei);

			CKeyChordChain PivKeyChoChain;
			PivKeyChoChain.SetWei(Wei);

			int DegIdx;
			for (DegIdx = 1; DegLink.Elem[DegIdx + 1] > 0; DegIdx++)
			{
				TKeyChord KeyCho;
				KeyCho.Key = Key;
				KeyCho.Cho = KeyAndDegToCho(Key, (DegLink.Elem[DegIdx]));
				if (! IsChkOnly)
					PivKeyChoChain.AddTail(KeyCho);
			}
			TKeyChord KeyCho;
			KeyCho.Key = Key;
			KeyCho.Cho = KeyAndDegToCho(Key, (DegLink.Elem[DegIdx]));
			TInterval* const LocListElem = KeyCho.Cho.ElemLocList.Elem;
			if (DegLink.Adjust == 'M')
			{
				LocListElem[1].HalfIntv = 0;
			}
			else if (DegLink.Adjust == 'm')
			{
				LocListElem[1].HalfIntv = -1;
				TNote const Note = CCalculator::PredNote(KeyCho.Cho.RootNote);
				if (! CCalculator::IsMajorKeyTonic(Note))
					continue;
			}
			else if (DegLink.Adjust == 'n')
			{
				KeyCho.Cho.RootNote
				    = CCalculator::AddIntvToNote((KeyCho.Cho.RootNote),
				                                 CCalculator::MakeInterval(0, -1));
				int LocIdx;
				for (LocIdx = 1; LocListElem[LocIdx].WholeIntv >= 0; LocIdx++)
					(LocListElem[LocIdx].HalfIntv)++;
				TNote const Note
				    = CCalculator::AddIntvToNote((KeyCho.Cho.RootNote),
                                                 CCalculator::MakeInterval(-4, 1));
				if (! CCalculator::IsMajorKeyTonic(Note))
					continue;
			}
			if (CCalculator::CompChoWithCho((KeyCho.Cho), PreCho))
				continue;
			if (! IsChkOnly)
				PivKeyChoChain.AddTail(KeyCho);
			DegIdx++;

			int const LenOfTrans = PivLenOfTrans + DegIdx - 1;

			if (KeyCho.Key == BaseKey)
			{
				if (IsChkOnly)
					return	TRUE;
				GenKeyChoChainList.AddTail(PivKeyChoChain);
				WeiTotal += Wei;
			}
			else if (LenOfTrans < MaxLenOfTrans)
			{
				CKeyChordChainList KeyChoChainList;
				if (AddChoChainsForAllTranss(TRUE, LenOfTrans,
				                             PivCho, KeyCho, KeyChoChainList))
				{
					if (IsChkOnly)
						return	TRUE;
					GenKeyChoChainList.AddTail(PivKeyChoChain);
					WeiTotal += Wei;
				}
			}
		}
	}

	if (IsChkOnly)
		return	FALSE;
	GenKeyChoChainList.AddWeiTotal(WeiTotal);
	return	FALSE;
}

TChord CGenerator::KeyAndDegToCho(TKey const Key, int const Deg)
{
	TNote const RootNote = CCalculator::KeyAndDegToNote(Key, Deg);

	int ChoElemLocListIdx;
	for (ChoElemLocListIdx = 0;
		 ChoElemLocListIdx < NumOfElems(ChordElemLocListTbl); ChoElemLocListIdx++)
	{
		TChord Cho;
		Cho.RootNote = RootNote;
		Cho.ElemLocList = ChordElemLocListTbl[ChoElemLocListIdx];
		if (CCalculator::CompKeyAndDegWithCho(Key, Deg, Cho))
			return	Cho;
	}

	TChord Cho;
	Cho.RootNote.PosInCMajor = ' ';
	return	Cho;
}
