// Calculator.h: CCalculator クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CALCULATOR_H__A3812362_CF6D_11D2_9F27_85B774C88BB0__INCLUDED_)
#define AFX_CALCULATOR_H__A3812362_CF6D_11D2_9F27_85B774C88BB0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common.h"

extern char const NoteNameTbl[7];
extern char const KeyModeTbl[2];

// 計算モジュール型
class CCalculator  
{
public:
	static void AddNoteText(TNote const Note, CString& Text);
	static int KeyAndChoToDeg(TKey const Key, TChord const Cho);
	static bool CompChoWithCho(TChord const SrcCho, TChord const DstCho);
	static bool CompKeyAndDegWithCho(TKey const Key, int const Deg, TChord const Cho);
	static TNote KeyAndDegToNote(TKey const Key, int Deg);
	static bool IsMajorKeyTonic(TNote const Note);
	static TNote AddIntvToNote(TNote const OrgNote, TInterval const Intv);
	static TNote SuccNote(TNote const OrgNote);
	static TNote PredNote(TNote const OrgNote);
	static int NoteToPosInChromatic(TNote const Note);
	static TInterval MakeInterval(int WholeIntv, int HalfIntv);
};

#endif // !defined(AFX_CALCULATOR_H__A3812362_CF6D_11D2_9F27_85B774C88BB0__INCLUDED_)
