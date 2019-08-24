#pragma once
#include "EditForm.h"

class CMainFrame : public CFrameWindowImpl<CMainFrame>,public CMessageFilter
{
protected:
	CFormView m_view;
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	virtual BOOL PreTranslateMessage(MSG* pMsg){
		if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return m_view.PreTranslateMessage(pMsg);
	}

	BEGIN_MSG_MAP_EX(CMainFrame)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
		CHAIN_CLIENT_COMMANDS()	// ビュークラスへコマンドチェーン
	END_MSG_MAP()

	LRESULT OnCreate(LPCREATESTRUCT lpcs){
		// ビューを作成
		m_hWndClient = m_view.Create(m_hWnd);

		//ウィンドウサイズ変更
		CRect Rect;
		m_view.GetWindowRect(Rect);
		AdjustWindowRect(Rect,GetWindowLong(GWL_STYLE),true);

		CRect CurrentRect;
		GetWindowRect(CurrentRect);
		MoveWindow(CurrentRect.left,CurrentRect.top,Rect.Width(),Rect.Height());

		// メッセージループにメッセージフィルタを追加
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		pLoop->AddMessageFilter(this);

		return 0;
	}

	void OnDestroy(){
		// メッセージループからメッセージフィルタを削除
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		pLoop->RemoveMessageFilter(this);
		SetMsgHandled(false);
	}


	CMainFrame(){}
	virtual ~CMainFrame(){}
};
