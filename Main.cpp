/////////////////////////////////////////////////////////////////////////////
//
// Last update	$Date: 2013/04/06 13:41:17 $ UTC
//   Revision   $Revision: 1.17 $
//
//	Renamed and Modified by Claybird <claybird.without.wing@gmail.com> 2005 May 23
// Main.cpp :	DLL アプリケーション用のエントリ ポイントを定義
//				LhaForge拡張シェルDLL
//
//	Original File Information:
// TfShell.cpp :	DLL アプリケーション用のエントリ ポイントを定義
//					他人の褌(TF)拡張シェルDLL
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShellExtClassFactory.h"
#include "registry.h"
#include "main.h"
#include "lfutil.h"


//右クリックメニューハンドラ
// {9127BC06-94AC-46F7-99DB-3D4423B8E813}
static const GUID CLSID_ShellExtShellMenu =
{ 0x9127bc06, 0x94ac, 0x46f7, { 0x99, 0xdb, 0x3d, 0x44, 0x23, 0xb8, 0xe8, 0x13 } };

//右ドラッグメニューハンドラ
// {8CE5DDDA-DDAF-476F-86FD-505DA9B94967}
static const GUID CLSID_ShellExtDragMenu =
{ 0x8ce5ddda, 0xddaf, 0x476f, { 0x86, 0xfd, 0x50, 0x5d, 0xa9, 0xb9, 0x49, 0x67 } };

long		g_cDllRef		= 0;
HINSTANCE	g_hInstDLL		= NULL;
HWND		g_hWndMcpAppl	= NULL;

//DLLにコメントを埋め込んでいる
//#pragma comment(exestr,_T("Shell Extension Module for LhaForge by Claybird"))

BOOL WINAPI DllMain(
	HINSTANCE	hInstance,
	DWORD		dwReason,
	LPVOID		lpReserved)
{
#if defined(_DEBUG)
	// メモリリーク検出用
	_CrtSetDbgFlag(
		_CRTDBG_ALLOC_MEM_DF
		| _CRTDBG_LEAK_CHECK_DF
		);
#endif
	UNREFERENCED_PARAMETER(lpReserved);

    if(dwReason == DLL_PROCESS_ATTACH){
		_tsetlocale(LC_ALL,_T(""));	//ロケールを環境変数から取得
		dprintf(_T("DllMain : DLL_PROCESS_ATTACH\n"));
		g_hInstDLL = hInstance;

		MessageBeep(MB_OK);
		HRESULT hRes = ::CoInitialize(NULL);
		ATLASSERT(SUCCEEDED(hRes));
		_Module.Init(NULL,g_hInstDLL);

	}else if(dwReason == DLL_PROCESS_DETACH){
		dprintf(_T("DllMain : DLL_PROCESS_DETACH\n"));
		_Module.Term();
		::CoUninitialize();
	}
	//UtilTraceLastError();

    return 1;
}

// DLLアンロード可能かチェック
STDAPI DllCanUnloadNow(void)
{
	dprintf(_T("DllCanUnloadNow[%s(g_cDllRef=%d)]\n"),
				((g_cDllRef == 0) ? _T("S_OK") : _T("S_FALSE")),g_cDllRef);
    return g_cDllRef == 0 ? S_OK : S_FALSE;
}


// クラスファクトリ要求
STDAPI DllGetClassObject(
	REFCLSID	rclsid,
	REFIID		riid,
	LPVOID*		ppvOut)
{
	dprintf(_T("DllGetClassObject\n"));

	HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;
	*ppvOut = NULL;

	CShellExtClassFactory *pcf=NULL;
	if(IsEqualIID(rclsid, CLSID_ShellExtShellMenu)){
		//---右クリックメニュー
		// クラスファクトリの作成
		pcf = new CShellExtClassFactory(false);
		dprintf(_T("DllGetClassObject:new CShellExtClassFactory(%8X):ShellMenu\n"),pcf);
		// インターフェースの要求
		hr = pcf->QueryInterface(riid, ppvOut);
	}
	else if(IsEqualIID(rclsid, CLSID_ShellExtDragMenu)){
		//---右ドラッグメニュー
		// クラスファクトリの作成
		pcf = new CShellExtClassFactory(true);
		dprintf(_T("DllGetClassObject:new CShellExtClassFactory(%8X):DragMenu\n"),pcf);
		// インターフェースの要求
		hr = pcf->QueryInterface(riid, ppvOut);
	}

	return hr;
}

// レジストリ登録
STDAPI DllRegisterServer()
{
	dprintf(_T("DllRegisterServer\n"));
	bool bFirst=(S_OK==RegisterServer(g_hInstDLL,CLSID_ShellExtShellMenu,false));
	bool bSecond=(S_OK==RegisterServer(g_hInstDLL,CLSID_ShellExtDragMenu,true));
	return (bFirst&&bSecond) ? S_OK : E_FAIL;
}


// レジストリ登録解除
STDAPI DllUnregisterServer()
{
	dprintf(_T("DllUnregisterServer\n"));
	bool bFirst=(S_OK==UnregisterServer(CLSID_ShellExtShellMenu,false));
	bool bSecond=(S_OK==UnregisterServer(CLSID_ShellExtDragMenu,true));
	return (bFirst&&bSecond) ? S_OK : E_FAIL;
}

#ifdef _DEBUG
// ログ出力
extern void dprintf(LPCTSTR inFormat,...)
{
	TCHAR _szPath[_MAX_PATH+1];
	FILE *fp;

	//format
	va_list arg;
	CString strTmp;
	va_start(arg, inFormat);
	strTmp.FormatV(inFormat,arg);
	va_end(arg);

	//output
	GetModuleFileName(g_hInstDLL, _szPath, _MAX_PATH);
	CPath strPath(_szPath);
	strPath.StripPath();
	strPath=(LPCTSTR)CString(_T("Z:\\temp\\")+strPath);
	strPath.RemoveExtension();
	strPath.AddExtension(_T(".log"));
	_tfopen_s(&fp,strPath, _T("a"));
	if(!fp)return;
	_ftprintf(fp,_T("%s"),(LPCTSTR)strTmp );
	fclose(fp);
}
#endif
