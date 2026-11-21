// Instrument.cpp: CInstrument クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GenChord.h"
#include "Instrument.h"
#include "Calculator.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int const MaxPosInChromatic = 0x60;
int const MidPosInChromatic = 0x3C;
int const MinPosInChromatic = 0x3C - 12 * 2;

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

CInstrument::CInstrument()
{
	IsOpened = FALSE;
}

CInstrument::~CInstrument()
{
	if (IsOpened)
	{
		TurnOffAll();
		midiOutClose(MidiOut);
	}
}

//////////////////////////////////////////////////////////////////////
// 各種処理
//////////////////////////////////////////////////////////////////////

int CInstrument::GetNumOfDevs()
{
	return	midiOutGetNumDevs();
}

BOOL CInstrument::GetDevProp(int Id, CString& Name)
{
	MIDIOUTCAPS MidiOutCaps;
	if (midiOutGetDevCaps(Id, &MidiOutCaps, sizeof(MIDIOUTCAPS)) != 0)
		return	FALSE;
	Name = MidiOutCaps.szPname;
	return	TRUE;
}

BOOL CInstrument::Open(int Id)
{
	if (IsOpened)
	{
		TurnOffAll();
		midiOutClose(MidiOut);
	}
	IsOpened = TRUE;
	int const RetCode = midiOutOpen(&MidiOut, Id, 0, 0, CALLBACK_NULL);
	if (RetCode == 0)
		TurnOffAll();
	return	(RetCode == 0);
}

void CInstrument::SetVoice(int VoiceNo)
{
	TurnOffAll();
	midiOutShortMsg(MidiOut, (((VoiceNo - 1) << 8) + 0xC0));
}

void CInstrument::Reset()
{
	TurnOffAll();
}

void CInstrument::TurnOnCho(TChord const Cho)
{
	TurnOffAll();

	TNote const RootNote = Cho.RootNote;
	TInterval const* const ChoElemLocListElem = Cho.ElemLocList.Elem;

	int PosInChromatic;
	PosInChromatic = MaxPosInChromatic;
	while (PosInChromatic > MidPosInChromatic)
	{
		int ElemLocIdx;
		for (ElemLocIdx = 0; ChoElemLocListElem[ElemLocIdx].WholeIntv >= 0; ElemLocIdx++)
		{
			TNote const Note
				= CCalculator::AddIntvToNote(RootNote, ChoElemLocListElem[ElemLocIdx]);
			if ((PosInChromatic % 12) == CCalculator::NoteToPosInChromatic(Note))
				break;
		}
		if (ChoElemLocListElem[ElemLocIdx].WholeIntv >= 0)
			TurnOnNote(PosInChromatic, 0x40);
		--PosInChromatic;
	}
	while (PosInChromatic > MinPosInChromatic)
	{
		TNote const Note
			= CCalculator::AddIntvToNote(RootNote, ChoElemLocListElem[0]);
		if ((PosInChromatic % 12) == CCalculator::NoteToPosInChromatic(Note))
			TurnOnNote(PosInChromatic, 0x40);
		--PosInChromatic;
	}
}

void CInstrument::TurnOnNote(int PosInChromatic, int Vel)
{
	midiOutShortMsg(MidiOut, ((((Vel << 8) + PosInChromatic) << 8) + 0x90));
}

void CInstrument::TurnOffAll()
{
	midiOutShortMsg(MidiOut, 0x00007BB0);
}
