#include "stdafx.h"
#include "resource.h"
#include "frame.h"

CAppModule _Module;

//�G���g���[�|�C���g
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpCmdLine, int nCmdShow)
{
#if defined(_DEBUG)
	// ���������[�N���o�p
	_CrtSetDbgFlag(
		_CRTDBG_ALLOC_MEM_DF
		| _CRTDBG_LEAK_CHECK_DF
		);
#endif
	HRESULT hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));
	// �����Microsoft Layer for Unicode (MSLU) ���g�p���ꂽ����
	// ATL�E�C���h�E thunking ������������
	::DefWindowProc(NULL, 0, 0, 0L);

	// ���̃R���g���[�����T�|�[�g���邽�߂̃t���O��ǉ�
	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);
	_Module.Init(NULL,hInstance);

//---------------------------------------------------------------

    // BLOCK: �A�v���P�[�V�������s
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
