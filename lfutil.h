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
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト言語
		(LPTSTR)&lpMsgBuf, 0, NULL);
	dprintf(_T("API Error : %s"),lpMsgBuf);
	LocalFree(lpMsgBuf);
}
#else
#define UtilTraceLastError()
#endif

bool UtilInsertMenuItem(HMENU hMenu,LPCTSTR lpszCaption,int indexMenu,HMENU hSubMenu,WORD wID);

//ファイルに一行書込み
bool UtilWriteLine(HANDLE hFile,LPCTSTR lpszText);

//INIに指定されたセクションがあるならtrueを返す
bool UtilCheckINISectionExists(LPCTSTR lpAppName,LPCTSTR lpFileName);

//メニュー構成を読み込む(B2Eメニュー用)
bool UtilReadMenuDefinitionFile(LPCTSTR lpFileName,std::vector<std::vector<CString> >&);

//文字列を読み込む
BOOL UtilLoadString(UINT nID,CString& strOut);
