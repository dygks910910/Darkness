
// MainFrm.h: CMainFrame 클래스의 인터페이스
//

#pragma once
class CSplitterX : public CSplitterWnd
{
public:
	CSplitterX();
	~CSplitterX();
protected:
	
public:
//	virtual HRESULT accHitTest(long xLeft, long yTop, VARIANT* pvarChild);
	DECLARE_MESSAGE_MAP();
	afx_msg LRESULT OnNcHitTest(CPoint point);
};
class CMainFrame : public CFrameWnd
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
protected:
	//CSplitterWnd m_wndSplitter;
	CSplitterX m_wndSplitter;

public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// 생성된 메시지 맵 함수
protected:
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

public:
//	virtual HRESULT accHitTest(long xLeft, long yTop, VARIANT* pvarChild);
};


