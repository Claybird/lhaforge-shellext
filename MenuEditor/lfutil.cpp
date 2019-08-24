#include "stdafx.h"
#include "lfutil.h"

#if !defined(_UNICODE)&&!defined(UNICODE)
 #include <imagehlp.h> //MakeSureDirectoryPathExists()
#endif

//�����K�w�̃f�B���N�g������C�ɍ쐬����
BOOL UtilMakeSureDirectoryPathExists(LPCTSTR _lpszPath)
{
#if defined(_UNICODE)||defined(UNICODE)
	CPath path(_lpszPath);
	path.RemoveFileSpec();
	path.AddBackslash();

	//TODO:UNICODE�ł݂̂Ń`�F�b�N�����Ă���̂�ANSI�r���h���ɂ͓K�X�����������ׂ�
	CString tmp(path);
	if(-1!=tmp.Find(_T(" \\"))||-1!=tmp.Find(_T(".\\"))){	//�p�X�Ƃ��ď����ł��Ȃ��t�@�C����������
		return FALSE;
	}

	//UNICODE�ł݂̂ŕK�v�ȃ`�F�b�N
	if(path.IsRoot())return TRUE;	//�h���C�u���[�g�Ȃ�쐬���Ȃ�(�ł��Ȃ�)

	int nRet=SHCreateDirectoryEx(NULL,path,NULL);
	switch(nRet){
	case ERROR_SUCCESS:
		return TRUE;
	case ERROR_ALREADY_EXISTS:
		if(path.IsDirectory())return TRUE;	//���łɃf�B���N�g��������ꍇ�����Z�[�t�Ƃ���
		else return FALSE;
	default:
		return FALSE;
	}
#else//defined(_UNICODE)||defined(UNICODE)
  #pragma comment(lib, "Dbghelp.lib")
	return MakeSureDirectoryPathExists(_lpszPath);
#endif//defined(_UNICODE)||defined(UNICODE)
}


//INI�Ɏw�肳�ꂽ�Z�N�V����������Ȃ�true��Ԃ�
bool UtilCheckINISectionExists(LPCTSTR lpAppName,LPCTSTR lpFileName)
{
	TCHAR szBuffer[10];
	DWORD dwRead=GetPrivateProfileSection(lpAppName,szBuffer,9,lpFileName);
	return dwRead>0;
}
