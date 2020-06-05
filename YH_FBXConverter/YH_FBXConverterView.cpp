
// YH_FBXConverterView.cpp: CYHFBXConverterView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "YH_FBXConverter.h"
#endif

#include "YH_FBXConverterView.h"
#include"DXUTWrapper.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CYHFBXConverterView

IMPLEMENT_DYNCREATE(CYHFBXConverterView, CView)

BEGIN_MESSAGE_MAP(CYHFBXConverterView, CView)
END_MESSAGE_MAP()


// CYHFBXConverterView 생성/소멸

CYHFBXConverterView::CYHFBXConverterView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CYHFBXConverterView::~CYHFBXConverterView()
{
}

BOOL CYHFBXConverterView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CYHFBXConverterView 그리기

void CYHFBXConverterView::OnDraw(CDC* /*pDC*/)
{
	CYHFBXConverterDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CYHFBXConverterView 진단

#ifdef _DEBUG
void CYHFBXConverterView::AssertValid() const
{
	CView::AssertValid();
}

void CYHFBXConverterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CYHFBXConverterDoc* CYHFBXConverterView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CYHFBXConverterDoc)));
	return (CYHFBXConverterDoc*)m_pDocument;
}
#endif //_DEBUG


// CYHFBXConverterView 메시지 처리기


void CYHFBXConverterView::OnInitialUpdate()
{
    CView::OnInitialUpdate();

    DXUTSetCallbackDeviceChanging(DXUTWrapper::ModifyDeviceSettings);
    DXUTSetCallbackMsgProc(DXUTWrapper::MsgProc);
    DXUTSetCallbackFrameMove(DXUTWrapper::OnFrameMove);

    // Set the D3D11 DXUT callbacks. Remove these sets if the app doesn't need to support D3D11
    DXUTSetCallbackD3D11DeviceAcceptable(DXUTWrapper::IsD3D11DeviceAcceptable);
    DXUTSetCallbackD3D11DeviceCreated(DXUTWrapper::OnD3D11CreateDevice);
    DXUTSetCallbackD3D11SwapChainResized(DXUTWrapper::OnD3D11ResizedSwapChain);
    DXUTSetCallbackD3D11FrameRender(DXUTWrapper::OnD3D11FrameRender);
    DXUTSetCallbackD3D11SwapChainReleasing(DXUTWrapper::OnD3D11ReleasingSwapChain);
    DXUTSetCallbackD3D11DeviceDestroyed(DXUTWrapper::OnD3D11DestroyDevice);
    DXUTInit(true, true, NULL); // Parse the command line, show msgboxes on error, no extra command line params
    //DXUTSetCursorSettings(true, true); // Show the cursor and clip it when in full screen

    DXUTSetWindow(m_hWnd, m_hWnd, m_hWnd, false);
    CRect rect;
    GetClientRect(&rect);
    DXUTCreateDevice(D3D_FEATURE_LEVEL_10_0, true, rect.right-rect.left, rect.bottom-rect.top);
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}


LRESULT CYHFBXConverterView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    LRESULT result = CView::WindowProc(message, wParam, lParam);
    DXUTStaticWndProc(m_hWnd, message, wParam, lParam);
    return result;
}
