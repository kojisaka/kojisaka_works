// Common.h : ヘッダー ファイル
//

#if !defined(AFX_DEF_TYPES_H__INCLUDED_)
#define AFX_DEF_TYPES_H__INCLUDED_

#define		NumOfElems(List)		(sizeof(List)/sizeof((List)[0]))	

struct TNote				// 音高情報型（ただし、オクターブの区別なし）
{
	char	PosInCMajor;		// ハ長調音階上の位置
	int		HalfIntv;			// 半音刻みの補正値
	bool operator ==(TNote const Note) const {return (PosInCMajor == Note.PosInCMajor && HalfIntv == Note.HalfIntv);}
	bool operator !=(TNote const Note) const {return (! (*this == Note));}
};

struct TKey					// 調情報型
{
	TNote	Note;				// 主音の音高
	char	Mode;				// 旋法（長音階、短音階）
	bool operator ==(TKey const Key) const {return (Note == Key.Note && Mode == Key.Mode);}
	bool operator !=(TKey const Key) const {return (! (*this == Key));}
};

struct TInterval			// 音高差情報型
{
	int		WholeIntv;			// 全音刻みの差
	int		HalfIntv;			// 半音刻みの差
	bool operator ==(TInterval const Intv) const {return (WholeIntv == Intv.WholeIntv && HalfIntv == Intv.HalfIntv);}
	bool operator !=(TInterval const Intv) const {return (! (*this == Intv));}
};

struct TChordElemLocList	// コード構成音相対位置リスト型
{
	int			Wei;			// 採用の重み
	TInterval	Elem[5];		// 根音からの音高差のリスト
};

struct TChord				// コード情報型
{
	TNote				RootNote;		// 根音の音高
	TChordElemLocList	ElemLocList;	// 構成音相対位置のリスト
};

struct TKeyChord			// 調・コード対情報型
{
	TKey	Key;				// 前提にした調
	TChord	Cho;				// コード
};

struct TDegLink				// 度数連結情報型
{
	int		Wei;				// 採用の重み
	char	KeyMode;			// 調の旋法
	int		Elem[20];			// 度数のリスト
	char	Adjust;				// 構成音の補正
};

struct TDegTrans			// 他の調での度数読替え情報型
{
	int		Wei;				// 採用の重み
	char	KeyMode;			// 読替え前の調の旋法
	int		SrcDeg;				// 読替え前の度数
	int		DstDeg;				// 読替え後の度数
};

#endif
