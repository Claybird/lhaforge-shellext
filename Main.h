/////////////////////////////////////////////////////////////////////////////
//
// Last update	$Date: 2007/11/01 13:08:19 $ UTC
//   Revision   $Revision: 1.7 $
//
//	Modified and Renamed by Claybird <claybird.without.wing@gmail.com>
//
// Main.h : DLL �A�v���P�[�V�����p�̃G���g�� �|�C���g�̃w�b�_
//
//  Original File Information:
// TfShell.h : DLL �A�v���P�[�V�����p�̃G���g�� �|�C���g�̃w�b�_
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


