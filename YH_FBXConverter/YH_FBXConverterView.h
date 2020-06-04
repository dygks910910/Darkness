
// YH_FBXConverterView.h: CYHFBXConverterView 클래스의 인터페이스
//

#pragma once

#include"YH_FBXConverterDoc.h"
class CYHFBXConverterView : public CView
{
protected: // serialization에서만 만들어집니다.
	CYHFBXConverterView() noexcept;
	DECLARE_DYNCREATE(CYHFBXConverterView)

// 특성입니다.
public:
	CYHFBXConverterDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 구현입니다.
public:
	virtual ~CYHFBXConverterView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // YH_FBXConverterView.cpp의 디버그 버전
inline CYHFBXConverterDoc* CYHFBXConverterView::GetDocument() const
   { return reinterpret_cast<CYHFBXConverterDoc*>(m_pDocument); }
#endif

