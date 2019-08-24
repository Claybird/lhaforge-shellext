/////////////////////////////////////////////////////////////////////////////
//
// Last update	$Date: 2007/11/01 13:08:19 $ UTC
//   Revision   $Revision: 1.7 $
//
//	Modified and Renamed by Claybird <claybird.without.wing@gmail.com>
//
// Main.h : DLL アプリケーション用のエントリ ポイントのヘッダ
//
//  Original File Information:
// TfShell.h : DLL アプリケーション用のエントリ ポイントのヘッダ
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

extern	long		g_cDllRef;
extern	HINSTANCE	g_hInstDLL;
extern	HWND		g_hWndMcpAppl;

STDAPI	DllRegisterServer(void);
STDAPI	DllUnregisterServer(void);
STDAPI	DllCanUnloadNow(void);
STDAPI	DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppvOut);

#ifdef _DEBUG
extern void dprintf(LPCTSTR, ...);
#else
#define dprintf
#endif


