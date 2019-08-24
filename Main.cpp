/////////////////////////////////////////////////////////////////////////////
//
// Last update	$Date: 2013/04/06 13:41:17 $ UTC
//   Revision   $Revision: 1.17 $
//
//	Renamed and Modified by Claybird <claybird.without.wing@gmail.com> 2005 May 23
// Main.cpp :	DLL �A�v���P�[�V�����p�̃G���g�� �|�C���g���`
//				LhaForge�g���V�F��DLL
//
//	Original File Information:
// TfShell.cpp :	DLL �A�v���P�[�V�����p�̃G���g�� �|�C���g���`
//					���l����(TF)�g���V�F��DLL
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShellExtClassFactory.h"
#include "registry.h"
#include "main.h"
#include "lfutil.h"


#ifndef WIN64
//�E�N���b�N���j���[�n���h��
// {713B479F-6F2B-48e9-B545-5591CCFE398F}
static const GUID CLSID_ShellExtShellMenu = 
{ 0x713b479f, 0x6f2b, 0x48e9, { 0xb5, 0x45, 0x55, 0x91, 0xcc, 0xfe, 0x39, 0x8f } };

//�E�h���b�O���j���[�n���h��
// {5E5B692B-D6ED-4103-A1FA-9A71A93DAC88}
static const GUID CLSID_ShellExtDragMenu = 
{ 0x5e5b692b, 0xd6ed, 0x4103, { 0xa1, 0xfa, 0x9a, 0x71, 0xa9, 0x3d, 0xac, 0x88 } };
#else //ifndef(WIN64)
//�E�N���b�N���j���[�n���h��
// {B7584D74-DE0C-4db5-80DD-42EEEDF42665}
static const GUID CLSID_ShellExtShellMenu = 
{ 0xb7584d74, 0xde0c, 0x4db5, { 0x80, 0xdd, 0x42, 0xee, 0xed, 0xf4, 0x26, 0x65 } };

//�E�h���b�O���j���[�n���h��
// {00521ADB-148D-45c9-8021-7446EE35609D}
static const GUID CLSID_ShellExtDragMenu = 
{ 0x521adb, 0x148d, 0x45c9, { 0x80, 0x21, 0x74, 0x46, 0xee, 0x35, 0x60, 0x9d } };

#endif

long		g_cDllRef		= 0;
HINSTANCE	g_hInstDLL		= NULL;
HWND		g_hWndMcpAppl	= NULL;

//DLL�ɃR�����g�𖄂ߍ���ł���
//#pragma comment(exestr,_T("Shell Extension Module for LhaForge by Claybird"))

BOOL WINAPI DllMain(
	HINSTANCE	hInstance,
	DWORD		dwReason,
	LPVOID		lpReserved)
{
#if defined(_DEBUG)
	// ���������[�N���o�p
	_CrtSetDbgFlag(
		_CRTDBG_ALLOC_MEM_DF
		| _CRTDBG_LEAK_CHECK_DF
		);
#endif
	UNREFERENCED_PARAMETER(lpReserved);

    if(dwReason == DLL_PROCESS_ATTACH){
		_tsetlocale(LC_ALL,_T(""));	//���P�[�������ϐ�����擾
		dprintf(_T("DllMain : DLL_PROCESS_ATTACH\n"));
		g_hInstDLL = hInstance;

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

// DLL�A�����[�h�\���`�F�b�N
STDAPI DllCanUnloadNow(void)
{
	dprintf(_T("DllCanUnloadNow[%s(g_cDllRef=%d)]\n"),
				((g_cDllRef == 0) ? _T("S_OK") : _T("S_FALSE")),g_cDllRef);
    return g_cDllRef == 0 ? S_OK : S_FALSE;
}


// �N���X�t�@�N�g���v��
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
		//---�E�N���b�N���j���[
		// �N���X�t�@�N�g���̍쐬
		pcf = new CShellExtClassFactory(false);
		dprintf(_T("DllGetClassObject:new CShellExtClassFactory(%8X):ShellMenu\n"),pcf);
		// �C���^�[�t�F�[�X�̗v��
		hr = pcf->QueryInterface(riid, ppvOut);
	}
	else if(IsEqualIID(rclsid, CLSID_ShellExtDragMenu)){
		//---�E�h���b�O���j���[
		// �N���X�t�@�N�g���̍쐬
		pcf = new CShellExtClassFactory(true);
		dprintf(_T("DllGetClassObject:new CShellExtClassFactory(%8X):DragMenu\n"),pcf);
		// �C���^�[�t�F�[�X�̗v��
		hr = pcf->QueryInterface(riid, ppvOut);
	}

	return hr;
}

// ���W�X�g���o�^
STDAPI DllRegisterServer()
{
	dprintf(_T("DllRegisterServer\n"));
	bool bFirst=(S_OK==RegisterServer(g_hInstDLL,CLSID_ShellExtShellMenu,false));
	bool bSecond=(S_OK==RegisterServer(g_hInstDLL,CLSID_ShellExtDragMenu,true));
	return (bFirst&&bSecond) ? S_OK : E_FAIL;
}


// ���W�X�g���o�^����
STDAPI DllUnregisterServer()
{
	dprintf(_T("DllUnregisterServer\n"));
	bool bFirst=(S_OK==UnregisterServer(CLSID_ShellExtShellMenu,false));
	bool bSecond=(S_OK==UnregisterServer(CLSID_ShellExtDragMenu,true));
	return (bFirst&&bSecond) ? S_OK : E_FAIL;
}

#ifdef _DEBUG
// ���O�o��
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
