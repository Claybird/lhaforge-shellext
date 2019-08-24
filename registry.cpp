/////////////////////////////////////////////////////////////////////////////
//
// Last update	$Date: 2009/12/20 12:03:28 $ UTC
//   Revision   $Revision: 1.14 $
//
//	Modified by Claybird <claybird.without.wing@gmail.com>
//
// registry.cpp :	���W�X�g���ւ̓o�^�ƍ폜�̒�`
//					LhaForge�g�����j���[
//
//	Original File Information:
// registry.cpp :	���W�X�g���ւ̓o�^�ƍ폜�̒�`
//					���l����(TF)�g�����j���[
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "registry.h"
#include "main.h"
#include "resource.h"

const int CLSID_STRING_SIZE=(39+1);

extern	HINSTANCE	g_hInstDLL;

// �V�F���G�N�X�e���V������o�^����g���q�ꗗ
static const TCHAR *g_ExtList[] =
{
	_T("*"),	//���ׂẴt�@�C��
	_T("Directory"),//�t�H���_
	_T("Drive"),	//�h���C�u
	NULL
};

//�v���g�^�C�v�錾
void WINAPI CLSIDtoString(REFCLSID, CString&);
BOOL WINAPI SetKeyAndValue(HKEY, LPCTSTR, LPCTSTR, LPCTSTR);
LONG RecursiveDeleteKey(HKEY, LPCTSTR);
BOOL DeleteNamedValue(HKEY, LPCTSTR, LPCTSTR);

//-----------

STDAPI RegisterServer(HMODULE hModule,REFCLSID clsid,bool bDrag)
{
	TCHAR szDllPath[_MAX_PATH+1];
	CString strCLSID;
	CString	strKey;

	// �o�^�p��DLL�̃t���p�X���擾����B
	GetModuleFileName(hModule, szDllPath, _MAX_PATH);

	// �n���ꂽ�N���XID����A���W�X�g���L�[�����쐬����B
	//   "CLSID\xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
	CLSIDtoString(clsid, strCLSID);
	strKey.Format(_T("CLSID\\%s"), strCLSID);

	//���W�X�g���ɓo�^����A�v���P�[�V������
#ifndef WIN64
	const LPCTSTR szAppName=_T("LhaForge");
#else
	const LPCTSTR szAppName=_T("LhaForge64");
#endif// WIN64

	// CLSID�ƃR���|�[�l���g����o�^����B
	SetKeyAndValue(HKEY_CLASSES_ROOT, strKey, NULL, szAppName);

	// ���s���W���[������o�^����B
	strKey+=_T("\\InprocServer32");
	SetKeyAndValue(HKEY_CLASSES_ROOT, strKey, NULL, szDllPath);
	SetKeyAndValue(HKEY_CLASSES_ROOT, strKey, _T("ThreadingModel"), _T("Apartment"));

	if(bDrag){
		//----------------------------------
		// �E�h���b�O���j���[�n���h���̓o�^
		//----------------------------------
		// Folder �� �h���b�O&�h���b�v�n���h�� �Ƃ��ēo�^
		CString strDnDHandlerKey;
		strDnDHandlerKey.Format(_T("Folder\\shellex\\DragDropHandlers\\%s"), szAppName);
		SetKeyAndValue(HKEY_CLASSES_ROOT,strDnDHandlerKey,NULL,strCLSID);
		// Drive �� �h���b�O&�h���b�v�n���h�� �Ƃ��ēo�^
		strDnDHandlerKey.Format(_T("Drive\\shellex\\DragDropHandlers\\%s"), szAppName);
		SetKeyAndValue(HKEY_CLASSES_ROOT,strDnDHandlerKey,NULL,strCLSID);
	}else{
		//----------------------------------
		// �E�N���b�N���j���[�n���h���̓o�^
		//----------------------------------
		// �e�g���q����ContextMenuHandlers�n���h����o�^����B
		for(int i = 0; g_ExtList[i]; i++){
			strKey.Format(_T("%s\\shellex\\ContextMenuHandlers\\%s"),g_ExtList[i],szAppName);
			SetKeyAndValue(HKEY_CLASSES_ROOT,strKey,NULL,strCLSID);
		}
	}


	// NT/2000�p�̏���o�^���邪�A����ɂ́AAdministrator�������K�v�B
	// ���s�����ꍇ�́A�����B
	SetKeyAndValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),strCLSID,szAppName);

	return S_OK;
}


STDAPI UnregisterServer(REFCLSID clsid,bool bDrag)
{
	CString strCLSID;
	CString	strKey;

	// �n���ꂽ�N���XID����A���W�X�g���L�[�����쐬����B
	//   "CLSID\xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
	CLSIDtoString(clsid, strCLSID);
	strKey.Format(_T("CLSID\\%s"), strCLSID);

	// "HKEY_CLASSES_ROOT\CLSID"��������A�N���XID�̃G���g�����폜����B
	RecursiveDeleteKey(HKEY_CLASSES_ROOT, strKey);


	//���W�X�g���ɓo�^����A�v���P�[�V������
#ifndef WIN64
	const LPCTSTR szAppName=_T("LhaForge");
#else
	const LPCTSTR szAppName=_T("LhaForge64");
#endif// WIN64

	if(bDrag){
		//----------------------------------
		// �E�h���b�O���j���[�n���h���̉���
		//----------------------------------
		// Folder �� �h���b�O&�h���b�v�n���h�� ������
		CString strDnDHandlerKey;
		strDnDHandlerKey.Format(_T("Folder\\shellex\\DragDropHandlers\\%s"), szAppName);
		RecursiveDeleteKey(HKEY_CLASSES_ROOT,strDnDHandlerKey);
		// Drive �� �h���b�O&�h���b�v�n���h�� ������
		strDnDHandlerKey.Format(_T("Drive\\shellex\\DragDropHandlers\\%s"), szAppName);
		RecursiveDeleteKey(HKEY_CLASSES_ROOT,strDnDHandlerKey);
	}else{
		//----------------------------------
		// �E�N���b�N���j���[�n���h���̉���
		//----------------------------------
		// �e�g���q����ContextMenuHandlers�n���h���̓o�^����������B
		for(int i = 0; g_ExtList[i]; i++){
			strKey.Format(_T("%s\\shellex\\ContextMenuHandlers\\%s"),g_ExtList[i],szAppName);
			RecursiveDeleteKey(HKEY_CLASSES_ROOT,strKey);
		}
	}

	// NT/2000�p�̏����폜����B����ɂ́AAdministrator�������K�v�B
	DeleteNamedValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),strCLSID);

	return S_OK ;
}


// CLSID�𕶎���ɕϊ�����B
static void WINAPI CLSIDtoString(REFCLSID clsid,CString &strCLSID)
{
	WCHAR theCLSID[CLSID_STRING_SIZE+1];

	// �N���XID�𕶎���ɕϊ�����B
	StringFromGUID2(clsid, theCLSID, CLSID_STRING_SIZE);
	strCLSID=theCLSID;
	dprintf(_T("CLSID=%s\n"),(LPCTSTR)strCLSID);
}


static BOOL WINAPI SetKeyAndValue(HKEY inKeyRootH,LPCTSTR inKey,LPCTSTR inValueName,LPCTSTR inValue)
{
	HKEY theKeyH;
	long result;

	// �쐬�����L�[���ŃL�[���쐬or�擾����B
	result = RegCreateKeyEx(inKeyRootH,
							 inKey,
							 0,
							 NULL,
							 REG_OPTION_NON_VOLATILE,
							 KEY_ALL_ACCESS,
							 NULL,
							 &theKeyH,
							 NULL);
	if(result != ERROR_SUCCESS){
		return FALSE;
	}

	// �l��ݒ肷��B
	if(inValue != NULL){
		RegSetValueEx(theKeyH,
					  inValueName,
					  0,
					  REG_SZ,
					  (BYTE *)inValue,
					  (DWORD)(_tcslen(inValue)+1)*sizeof(TCHAR));
	}
	RegCloseKey(theKeyH);
	return TRUE;
}


// �ċN�I���W�X�g���폜
static LONG RecursiveDeleteKey(HKEY inKeyParentH,LPCTSTR inKeyChild)
{
	HKEY theKeyChildH;
	long theRes;
	FILETIME theTime;
	TCHAR theBuffer[256];
	DWORD theSize;

	// Open the child.
	theRes = RegOpenKeyEx(inKeyParentH, inKeyChild, 0, KEY_ALL_ACCESS, &theKeyChildH);
	if(theRes != ERROR_SUCCESS){
		return theRes;
	}

	// Enumerate all of the decendents of this child.
	theSize = 256;
	while(RegEnumKeyEx(theKeyChildH, 0, theBuffer, &theSize, NULL, NULL, NULL, &theTime) == S_OK){
		// Delete the decendents of this child.
		theRes = RecursiveDeleteKey(theKeyChildH, theBuffer);
		if(theRes != ERROR_SUCCESS){
			// Cleanup before exiting.
			RegCloseKey(theKeyChildH);
			return theRes;
		}
		theSize = 256;
	}

	// Close the child.
	RegCloseKey(theKeyChildH);

	// Delete this child.
	return RegDeleteKey(inKeyParentH, inKeyChild);
}


// ���O�t���l�̍폜
static BOOL DeleteNamedValue(HKEY inKeyRootH,LPCTSTR inSubKey,LPCTSTR inValueName)
{
	HKEY theKeyH;
	long result;
	DWORD theDisposition;

	result = RegCreateKeyEx(inKeyRootH,
							 inSubKey,
							 0,
							 NULL,
							 REG_OPTION_NON_VOLATILE,
							 KEY_ALL_ACCESS,
							 NULL,
							 &theKeyH,
							 &theDisposition);
	if(result != ERROR_SUCCESS){
		return FALSE;
	}

	if(theDisposition == REG_CREATED_NEW_KEY){
		// �V�����L�[���쐬���Ă��܂����ꍇ�́A
		// ���̃L�[���폜���āA�������I������B
		RegCloseKey(theKeyH);
		RegDeleteKey(inKeyRootH, inSubKey);
		return FALSE;
	}

	// ���O�t���l�̍폜�B
	RegDeleteValue(theKeyH, inValueName);
	RegCloseKey(theKeyH);
	return TRUE;
}
