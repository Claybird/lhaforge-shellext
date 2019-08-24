#pragma once
#include "stdafx.h"
#include "main.h"

#ifdef _DEBUG
static void UtilTraceLastError()
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // �f�t�H���g����
		(LPTSTR)&lpMsgBuf, 0, NULL);
	dprintf(_T("API Error : %s"),lpMsgBuf);
	LocalFree(lpMsgBuf);
}
#else
#define UtilTraceLastError()
#endif

bool UtilInsertMenuItem(HMENU hMenu,LPCTSTR lpszCaption,int indexMenu,HMENU hSubMenu,WORD wID);

//�t�@�C���Ɉ�s������
bool UtilWriteLine(HANDLE hFile,LPCTSTR lpszText);

//INI�Ɏw�肳�ꂽ�Z�N�V����������Ȃ�true��Ԃ�
bool UtilCheckINISectionExists(LPCTSTR lpAppName,LPCTSTR lpFileName);

//���j���[�\����ǂݍ���(B2E���j���[�p)
bool UtilReadMenuDefinitionFile(LPCTSTR lpFileName,std::vector<std::vector<CString> >&);

//�������ǂݍ���
BOOL UtilLoadString(UINT nID,CString& strOut);
