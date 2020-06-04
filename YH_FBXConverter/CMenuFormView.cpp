// CMenuFormView.cpp: 구현 파일
//

#include "pch.h"
#include "YH_FBXConverter.h"
#include "CMenuFormView.h"

CYHFBXConverterApp* g_pView;
// CMenuFormView

IMPLEMENT_DYNCREATE(CMenuFormView, CFormView)

CMenuFormView::CMenuFormView()
	: CFormView(IDD_CMenuFormView)
{

}

CMenuFormView::~CMenuFormView()
{
}

void CMenuFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMenuFormView, CFormView)
END_MESSAGE_MAP()


// CMenuFormView 진단

#ifdef _DEBUG
void CMenuFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMenuFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMenuFormView 메시지 처리기
