// ExList.h: CExList クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXLIST_H__5553EF00_CDE3_11D2_9F27_D8D48EFBE71E__INCLUDED_)
#define AFX_EXLIST_H__5553EF00_CDE3_11D2_9F27_D8D48EFBE71E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Afxtempl.h>

template<class TYPE, class ARG_TYPE>
class CExList : public CList<TYPE, ARG_TYPE>  
{
public:
	CExList();
	CExList const& operator =(CExList const& ExList);
	TYPE const& GetByIdx(int Idx) const;
	TYPE const& GetByPos(POSITION& rPosition) const;
	void AddListToTail(CExList const* pNewList);
};

template<class TYPE, class ARG_TYPE>
CExList<TYPE, ARG_TYPE>::CExList() : CList<TYPE, ARG_TYPE>()
{
}

template<class TYPE, class ARG_TYPE>
CExList<TYPE, ARG_TYPE> const& CExList<TYPE, ARG_TYPE>::operator =(CExList const& ExList)
{
	ASSERT_VALID(this);
	RemoveAll();
	POSITION pos = ExList.GetHeadPosition();
	while (pos != NULL)
		AddTail(ExList.GetByPos(pos));
	return	*this;
}

template<class TYPE, class ARG_TYPE>
TYPE const& CExList<TYPE, ARG_TYPE>::GetByIdx(int Idx) const
	{ POSITION pos = FindIndex(Idx);
		ASSERT(pos != NULL);
		return GetByPos(pos); }

template<class TYPE, class ARG_TYPE>
TYPE const& CExList<TYPE, ARG_TYPE>::GetByPos(POSITION& rPosition) const // return *Position++
	{ CNode* pNode = (CNode*) rPosition;
		ASSERT(AfxIsValidAddress(pNode, sizeof(CNode)));
		rPosition = (POSITION) pNode->pNext;
		return pNode->data; }

template<class TYPE, class ARG_TYPE>
void CExList<TYPE, ARG_TYPE>::AddListToTail(CExList const* pNewList)
{
	ASSERT_VALID(this);
	ASSERT(pNewList != NULL);
	ASSERT_VALID(pNewList);

	// add a list of same elements
	POSITION pos = pNewList->GetHeadPosition();
	while (pos != NULL)
		AddTail(pNewList->GetNext(pos));
}

#endif // !defined(AFX_EXLIST_H__5553EF00_CDE3_11D2_9F27_D8D48EFBE71E__INCLUDED_)
