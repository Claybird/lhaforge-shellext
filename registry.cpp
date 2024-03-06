/////////////////////////////////////////////////////////////////////////////
//
// Last update	$Date: 2009/12/20 12:03:28 $ UTC
//   Revision   $Revision: 1.14 $
//
//	Modified by Claybird <claybird.without.wing@gmail.com>
//
// registry.cpp :	レジストリへの登録と削除の定義
//					LhaForge拡張メニュー
//
//	Original File Information:
// registry.cpp :	レジストリへの登録と削除の定義
//					他人の褌(TF)拡張メニュー
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "registry.h"
#include "main.h"
#include "resource.h"

const int CLSID_STRING_SIZE=(39+1);

extern	HINSTANCE	g_hInstDLL;

// シェルエクステンションを登録する拡張子一覧
static const TCHAR *g_ExtList[] =
{
	_T("*"),	//すべてのファイル
	_T("Directory"),//フォルダ
	_T("Drive"),	//ドライブ
	NULL
};

//プロトタイプ宣言
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

	// 登録用にDLLのフルパスを取得する。
	GetModuleFileName(hModule, szDllPath, _MAX_PATH);

	// 渡されたクラスIDから、レジストリキー名を作成する。
	//   "CLSID\xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
	CLSIDtoString(clsid, strCLSID);
	strKey.Format(_T("CLSID\\%s"), strCLSID);

	//レジストリに登録するアプリケーション名
#ifndef WIN64
	const LPCTSTR szAppName=_T("LhaForge");
#else
	const LPCTSTR szAppName=_T("LhaForge64");
#endif// WIN64

	// CLSIDとコンポーネント名を登録する。
	SetKeyAndValue(HKEY_CLASSES_ROOT, strKey, NULL, szAppName);

	// 実行モジュール情報を登録する。
	strKey+=_T("\\InprocServer32");
	SetKeyAndValue(HKEY_CLASSES_ROOT, strKey, NULL, szDllPath);
	SetKeyAndValue(HKEY_CLASSES_ROOT, strKey, _T("ThreadingModel"), _T("Apartment"));

	if(bDrag){
		//----------------------------------
		// 右ドラッグメニューハンドラの登録
		//----------------------------------
		// Folder の ドラッグ&ドロップハンドラ として登録
		CString strDnDHandlerKey;
		strDnDHandlerKey.Format(_T("Folder\\shellex\\DragDropHandlers\\%s"), szAppName);
		SetKeyAndValue(HKEY_CLASSES_ROOT,strDnDHandlerKey,NULL,strCLSID);
		// Drive の ドラッグ&ドロップハンドラ として登録
		strDnDHandlerKey.Format(_T("Drive\\shellex\\DragDropHandlers\\%s"), szAppName);
		SetKeyAndValue(HKEY_CLASSES_ROOT,strDnDHandlerKey,NULL,strCLSID);
	}else{
		//----------------------------------
		// 右クリックメニューハンドラの登録
		//----------------------------------
		// 各拡張子毎のContextMenuHandlersハンドラを登録する。
		for(int i = 0; g_ExtList[i]; i++){
			strKey.Format(_T("%s\\shellex\\ContextMenuHandlers\\%s"),g_ExtList[i],szAppName);
			SetKeyAndValue(HKEY_CLASSES_ROOT,strKey,NULL,strCLSID);
		}
	}


	// NT/2000用の情報を登録するが、これには、Administrator権限が必要。
	// 失敗した場合は、無視。
	SetKeyAndValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),strCLSID,szAppName);

	return S_OK;
}


STDAPI UnregisterServer(REFCLSID clsid,bool bDrag)
{
	CString strCLSID;
	CString	strKey;

	// 渡されたクラスIDから、レジストリキー名を作成する。
	//   "CLSID\xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
	CLSIDtoString(clsid, strCLSID);
	strKey.Format(_T("CLSID\\%s"), strCLSID);

	// "HKEY_CLASSES_ROOT\CLSID"直下から、クラスIDのエントリを削除する。
	RecursiveDeleteKey(HKEY_CLASSES_ROOT, strKey);


	//レジストリに登録するアプリケーション名
#ifndef WIN64
	const LPCTSTR szAppName=_T("LhaForge");
#else
	const LPCTSTR szAppName=_T("LhaForge64");
#endif// WIN64

	if(bDrag){
		//----------------------------------
		// 右ドラッグメニューハンドラの解除
		//----------------------------------
		// Folder の ドラッグ&ドロップハンドラ を解除
		CString strDnDHandlerKey;
		strDnDHandlerKey.Format(_T("Folder\\shellex\\DragDropHandlers\\%s"), szAppName);
		RecursiveDeleteKey(HKEY_CLASSES_ROOT,strDnDHandlerKey);
		// Drive の ドラッグ&ドロップハンドラ を解除
		strDnDHandlerKey.Format(_T("Drive\\shellex\\DragDropHandlers\\%s"), szAppName);
		RecursiveDeleteKey(HKEY_CLASSES_ROOT,strDnDHandlerKey);
	}else{
		//----------------------------------
		// 右クリックメニューハンドラの解除
		//----------------------------------
		// 各拡張子毎のContextMenuHandlersハンドラの登録を解除する。
		for(int i = 0; g_ExtList[i]; i++){
			strKey.Format(_T("%s\\shellex\\ContextMenuHandlers\\%s"),g_ExtList[i],szAppName);
			RecursiveDeleteKey(HKEY_CLASSES_ROOT,strKey);
		}
	}

	// NT/2000用の情報を削除する。これには、Administrator権限が必要。
	DeleteNamedValue(HKEY_LOCAL_MACHINE,_T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"),strCLSID);

	return S_OK ;
}


// CLSIDを文字列に変換する。
static void WINAPI CLSIDtoString(REFCLSID clsid,CString &strCLSID)
{
	WCHAR theCLSID[CLSID_STRING_SIZE+1];

	// クラスIDを文字列に変換する。
	StringFromGUID2(clsid, theCLSID, CLSID_STRING_SIZE);
	strCLSID=theCLSID;
	dprintf(_T("CLSID=%s\n"),(LPCTSTR)strCLSID);
}


static BOOL WINAPI SetKeyAndValue(HKEY inKeyRootH,LPCTSTR inKey,LPCTSTR inValueName,LPCTSTR inValue)
{
	HKEY theKeyH;
	long result;

	// 作成したキー名でキーを作成or取得する。
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

	// 値を設定する。
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


// 再起的レジストリ削除
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


// 名前付き値の削除
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
		// 新しいキーを作成してしまった場合は、
		// そのキーを削除して、処理を終了する。
		RegCloseKey(theKeyH);
		RegDeleteKey(inKeyRootH, inSubKey);
		return FALSE;
	}

	// 名前付き値の削除。
	RegDeleteValue(theKeyH, inValueName);
	RegCloseKey(theKeyH);
	return TRUE;
}
