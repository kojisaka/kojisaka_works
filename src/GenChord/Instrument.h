// Instrument.h: CInstrument クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTRUMENT_H__F6D070E1_D740_11D2_9F27_8F43757EF1F4__INCLUDED_)
#define AFX_INSTRUMENT_H__F6D070E1_D740_11D2_9F27_8F43757EF1F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mmsystem.h>
#include "Common.h"

// 楽器型
class CInstrument  
{
private:
	HMIDIOUT	MidiOut;		// MIDI出力先情報
	BOOL		IsOpened;		// MIDI出力オープン中ならTRUE
public:
	CInstrument();
	virtual ~CInstrument();
	int GetNumOfDevs();
	BOOL GetDevProp(int Id, CString& Name);
	BOOL Open(int Id);
	void SetVoice(int VoiceNo);
	void Reset();
	void TurnOnCho(TChord const Cho);
	void TurnOnNote(int PosInChromatic, int Vel);
	void TurnOffAll();
};

#endif // !defined(AFX_INSTRUMENT_H__F6D070E1_D740_11D2_9F27_8F43757EF1F4__INCLUDED_)
