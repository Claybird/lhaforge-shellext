#include "stdafx.h"
#include "resource.h"
#include "frame.h"

CAppModule _Module;

//エントリーポイント
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpCmdLine, int nCmdShow)
{
#if defined(_DEBUG)
	// メモリリーク検出用
	_CrtSetDbgFlag(
		_CRTDBG_ALLOC_MEM_DF
		| _CRTDBG_LEAK_CHECK_DF
		);
#endif
	HRESULT hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));
	// これはMicrosoft Layer for Unicode (MSLU) が使用された時の
	// ATLウインドウ thunking 問題を解決する
	::DefWindowProc(NULL, 0, 0, 0L);

	// 他のコントロールをサポートするためのフラグを追加
	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);
	_Module.Init(NULL,hInstance);

//---------------------------------------------------------------

    // BLOCK: アプリケーション実行
	int nRet = 0;
    {
		CMessageLoop theLoop;
		_Module.AddMessageLoop(&theLoop);

		CMainFrame wnd;
		wnd.CreateEx(NULL,NULL,WS_CAPTION|WS_BORDER|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE);
		wnd.ShowWindow(nCmdShow);
		wnd.UpdateWindow();

		nRet = theLoop.Run();

		_Module.RemoveMessageLoop();
    }
//---------------------------------------------------------------

	_Module.Term();
	::CoUninitialize();
	return nRet;//msg.wParam;
}
