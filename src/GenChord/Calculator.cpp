// Calculator.cpp: CCalculator クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GenChord.h"
#include "Calculator.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

char const NoteNameTbl[] = {'C', 'D', 'E', 'F', 'G', 'A', 'B'};
char const KeyModeTbl[] = {' ', 'm'};

//////////////////////////////////////////////////////////////////////
// 各種処理
//////////////////////////////////////////////////////////////////////

void CCalculator::AddNoteText(TNote const Note, CString& Text)
{
	int const HalfIntv = Note.HalfIntv;

	Text += Note.PosInCMajor;
	if (HalfIntv == 0)
		;
	else if (HalfIntv == 1)
		Text += "#";
	else if (HalfIntv == 2)
		Text += "##";
	else if (HalfIntv == -1)
		Text += "b";
	else if (HalfIntv == -2)
		Text += "bb";
	else
		Text += "?";
}

int CCalculator::KeyAndChoToDeg(TKey const Key, TChord const Cho)
{
	int Deg;
	for (Deg = 1; Deg < 8; Deg++)
		if (CompKeyAndDegWithCho(Key, Deg, Cho))
			return	Deg;

	return	0;
}

bool CCalculator::CompKeyAndDegWithCho(TKey const Key, int const Deg, TChord const Cho)
{
	int const DegDiffTbl[] = {0, 2, 4};

	TInterval const* const LocListElem = Cho.ElemLocList.Elem;

	TNote ChoElemNoteTbl[NumOfElems(Cho.ElemLocList.Elem) + 1];

	int ChoElemIdx;
	for (ChoElemIdx = 0; LocListElem[ChoElemIdx].WholeIntv >= 0; ChoElemIdx++)
		ChoElemNoteTbl[ChoElemIdx]
		    = AddIntvToNote((Cho.RootNote), LocListElem[ChoElemIdx]);
	ChoElemNoteTbl[ChoElemIdx].PosInCMajor = ' ';

	int DegDiffIdx;
	for (DegDiffIdx = 0; DegDiffIdx < NumOfElems(DegDiffTbl); DegDiffIdx++)
	{
		TNote const Note = KeyAndDegToNote(Key, (Deg + DegDiffTbl[DegDiffIdx]));
		for (ChoElemIdx = 0; ChoElemNoteTbl[ChoElemIdx].PosInCMajor != ' '; ChoElemIdx++)
		{
			TNote const ChoElemNote = ChoElemNoteTbl[ChoElemIdx];
			if (Note.PosInCMajor == ChoElemNote.PosInCMajor	&&
				Note.HalfIntv	 == ChoElemNote.HalfIntv	  )
				break;
		}
		if (ChoElemNoteTbl[ChoElemIdx].PosInCMajor == ' ')
			return	FALSE;
	}

	return	TRUE;
}

bool CCalculator::CompChoWithCho(TChord const SrcCho, TChord const DstCho)
{
	if (SrcCho.RootNote != DstCho.RootNote)
		return	FALSE;

	TInterval const* const SrcLocListElem = SrcCho.ElemLocList.Elem;
	TInterval const* const DstLocListElem = DstCho.ElemLocList.Elem;
	int ChoElemIdx;
	ChoElemIdx = 0;
	while (SrcLocListElem[ChoElemIdx].WholeIntv >= 0 ||
	       DstLocListElem[ChoElemIdx].WholeIntv >= 0   )
	{
		if (SrcLocListElem[ChoElemIdx] != DstLocListElem[ChoElemIdx])
			return	FALSE;
		ChoElemIdx++;
	}

	return	TRUE;
}

TNote CCalculator::KeyAndDegToNote(TKey const Key, int Deg)
{
	TInterval	Intv;

	while (Deg > 7)
		Deg -= 7;
	Intv.WholeIntv = Deg - 1;
	if (Key.Mode == ' ')
	{
		if (Deg < 4)
			Intv.HalfIntv = 0;
		else
			Intv.HalfIntv = -1;
	}
	else if (Key.Mode == 'm')
	{
		if (Deg < 3)
			Intv.HalfIntv = 0;
		else if (Deg != 6)
			Intv.HalfIntv = -1;
		else
			Intv.HalfIntv = -2;
	}
	else
		ASSERT(FALSE);

	return	AddIntvToNote((Key.Note), Intv);
}

bool CCalculator::IsMajorKeyTonic(TNote const Note)
{
	int const PosInCMajor = Note.PosInCMajor;
	int const HalfIntv    = Note.HalfIntv   ;
	if (PosInCMajor == 'C' || PosInCMajor == 'F')
		return	( 0 <= HalfIntv && HalfIntv <= 1);
	else if (PosInCMajor == 'D' || PosInCMajor == 'G' || PosInCMajor == 'A')
		return	(-1 <= HalfIntv && HalfIntv <= 1);
	else if (PosInCMajor == 'E' || PosInCMajor == 'B')
		return	(-1 <= HalfIntv && HalfIntv <= 0);

	return FALSE;
}

TNote CCalculator::AddIntvToNote(TNote const OrgNote, TInterval const Intv)
{
	TNote Note;

	Note = OrgNote;
	if (Intv.WholeIntv >= 0)
	{
		int Counter;
		for (Counter = 0; Counter < Intv.WholeIntv; Counter++)
			Note = SuccNote(Note);
	}
	else
	{
		int Counter;
		for (Counter = 0; Counter > Intv.WholeIntv; Counter--)
			Note = PredNote(Note);
	}

	Note.HalfIntv = Note.HalfIntv + Intv.HalfIntv;

	return	Note;
}

TNote CCalculator::SuccNote(TNote const OrgNote)
{
	TNote Note;

	Note = OrgNote;
	if (OrgNote.PosInCMajor == 'C')
		(Note.PosInCMajor) = 'D';
	else if (OrgNote.PosInCMajor == 'D')
		(Note.PosInCMajor) = 'E';
	else if (OrgNote.PosInCMajor == 'E')
	{
		(Note.PosInCMajor) = 'F';
		(Note.HalfIntv)++;
	}
	else if (OrgNote.PosInCMajor == 'F')
		(Note.PosInCMajor) = 'G';
	else if (OrgNote.PosInCMajor == 'G')
		(Note.PosInCMajor) = 'A';
	else if (OrgNote.PosInCMajor == 'A')
		(Note.PosInCMajor) = 'B';
	else if (OrgNote.PosInCMajor == 'B')
	{
		(Note.PosInCMajor) = 'C';
		(Note.HalfIntv)++;
	}
	else
		ASSERT(FALSE);

	return	Note;
}

TNote CCalculator::PredNote(TNote const OrgNote)
{
	TNote Note;

	Note = OrgNote;
	if (OrgNote.PosInCMajor == 'C')
	{
		(Note.PosInCMajor) = 'B';
		(Note.HalfIntv)--;
	}
	else if (OrgNote.PosInCMajor == 'B')
		(Note.PosInCMajor) = 'A';
	else if (OrgNote.PosInCMajor == 'A')
		(Note.PosInCMajor) = 'G';
	else if (OrgNote.PosInCMajor == 'G')
		(Note.PosInCMajor) = 'F';
	else if (OrgNote.PosInCMajor == 'F')
	{
		(Note.PosInCMajor) = 'E';
		(Note.HalfIntv)--;
	}
	else if (OrgNote.PosInCMajor == 'E')
		(Note.PosInCMajor) = 'D';
	else if (OrgNote.PosInCMajor == 'D')
		(Note.PosInCMajor) = 'C';
	else
		ASSERT(FALSE);

	return	Note;
}

int CCalculator::NoteToPosInChromatic(TNote const Note)
{
	int Offset;
	Offset = 0;
	if (Note.PosInCMajor == 'C')
		Offset = 0;
	else if (Note.PosInCMajor == 'D')
		Offset = 2;
	else if (Note.PosInCMajor == 'E')
		Offset = 4;
	else if (Note.PosInCMajor == 'F')
		Offset = 5;
	else if (Note.PosInCMajor == 'G')
		Offset = 7;
	else if (Note.PosInCMajor == 'A')
		Offset = 9;
	else if (Note.PosInCMajor == 'B')
		Offset = 11;
	else
		ASSERT(FALSE);

	return ((0x3C + Offset + (Note.HalfIntv)) % 12);
}

TInterval CCalculator::MakeInterval(int WholeIntv, int HalfIntv)
{
	TInterval Intv;
	Intv.WholeIntv = WholeIntv;
	Intv.HalfIntv  = HalfIntv ;
	return	Intv;
}
