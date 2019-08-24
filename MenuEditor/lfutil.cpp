#include "stdafx.h"
#include "lfutil.h"

#if !defined(_UNICODE)&&!defined(UNICODE)
 #include <imagehlp.h> //MakeSureDirectoryPathExists()
#endif

//複数階層のディレクトリを一気に作成する
BOOL UtilMakeSureDirectoryPathExists(LPCTSTR _lpszPath)
{
#if defined(_UNICODE)||defined(UNICODE)
	CPath path(_lpszPath);
	path.RemoveFileSpec();
	path.AddBackslash();

	//TODO:UNICODE版のみでチェックを入れているのでANSIビルド時には適宜処理し直すべき
	CString tmp(path);
	if(-1!=tmp.Find(_T(" \\"))||-1!=tmp.Find(_T(".\\"))){	//パスとして処理できないファイル名がある
		return FALSE;
	}

	//UNICODE版のみで必要なチェック
	if(path.IsRoot())return TRUE;	//ドライブルートなら作成しない(できない)

	int nRet=SHCreateDirectoryEx(NULL,path,NULL);
	switch(nRet){
	case ERROR_SUCCESS:
		return TRUE;
	case ERROR_ALREADY_EXISTS:
		if(path.IsDirectory())return TRUE;	//すでにディレクトリがある場合だけセーフとする
		else return FALSE;
	default:
		return FALSE;
	}
#else//defined(_UNICODE)||defined(UNICODE)
  #pragma comment(lib, "Dbghelp.lib")
	return MakeSureDirectoryPathExists(_lpszPath);
#endif//defined(_UNICODE)||defined(UNICODE)
}


//INIに指定されたセクションがあるならtrueを返す
bool UtilCheckINISectionExists(LPCTSTR lpAppName,LPCTSTR lpFileName)
{
	TCHAR szBuffer[10];
	DWORD dwRead=GetPrivateProfileSection(lpAppName,szBuffer,9,lpFileName);
	return dwRead>0;
}
