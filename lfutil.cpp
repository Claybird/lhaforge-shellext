#include "stdafx.h"
#include "lfutil.h"

bool UtilInsertMenuItem(HMENU hMenu,LPCTSTR lpszCaption,int indexMenu,HMENU hSubMenu,WORD wID)
{
	ASSERT(IsMenu(hMenu));
	MENUITEMINFO mii={0};
	mii.cbSize = sizeof(mii);

	if(lpszCaption){
		if(hSubMenu){
			//サブメニュー付
			mii.fMask = MIIM_SUBMENU | MIIM_STRING;
			mii.hSubMenu=hSubMenu;
		}else{
			//単独項目
			mii.fMask = MIIM_STRING;
		}

		if(wID!=(WORD)-1){
			mii.fMask |= MIIM_ID;
			mii.wID = wID;
		}

		mii.dwTypeData=const_cast<LPTSTR>(lpszCaption);
		mii.cch=(UINT)_tcslen(lpszCaption);

		//TRUE:indexMenuは位置を表わす
		if(::InsertMenuItem(hMenu, indexMenu, TRUE,&mii)){
			return true;
		}

		//挿入に失敗した
		//MIIM_STRINGを外し、MFT_STRINGを追加する
		mii.fMask&=~MIIM_STRING;
		mii.fMask|=MIIM_FTYPE;	//MIIM_TYPE
		mii.fType=MFT_STRING;

		if(::InsertMenuItem(hMenu, indexMenu, TRUE,&mii)){
			return true;
		}
		return false;
	}else{
	//----------------------------------------
	//Separatorを追加の場合、lpszCaptionはNULL
	//----------------------------------------
		mii.fMask = MIIM_TYPE;
		mii.fType=MFT_SEPARATOR;
		if(wID!=(WORD)-1){
			mii.fMask |= MIIM_ID;
			mii.wID = wID;
		}
		if(::InsertMenuItem(hMenu, indexMenu, TRUE,&mii)){
			return true;
		}else{
			mii.fMask = MIIM_FTYPE;
			mii.fType=MFT_SEPARATOR;
			if(wID!=(WORD)-1){
				mii.fMask |= MIIM_ID;
				mii.wID = wID;
			}
			if(::InsertMenuItem(hMenu, indexMenu, TRUE,&mii)){
				return true;
			}
			else{
				return false;
			}
		}
	}
}

//ファイルに一行書込み
bool UtilWriteLine(HANDLE hFile,LPCTSTR lpszText)
{
	if(INVALID_HANDLE_VALUE==hFile)return false;
	DWORD dwWritten;
	if(!WriteFile(hFile,lpszText,(DWORD)(_tcslen(lpszText)*sizeof(lpszText[0])),&dwWritten,NULL))return false;
	TCHAR CRLF[]=_T("\r\n");
	if(!WriteFile(hFile,CRLF,DWORD(_tcslen(CRLF)*sizeof(CRLF[0])),&dwWritten,NULL))return false;

	return true;
}

//INIに指定されたセクションがあるならtrueを返す
bool UtilCheckINISectionExists(LPCTSTR lpAppName,LPCTSTR lpFileName)
{
	TCHAR szBuffer[10];
	DWORD dwRead=GetPrivateProfileSection(lpAppName,szBuffer,9,lpFileName);
	return dwRead>0;
}

//メニュー構成を読み込む(B2Eメニュー用)
bool UtilReadMenuDefinitionFile(LPCTSTR lpFileName,std::vector<std::vector<CString> >& r_MenuArr)
{
//実装が面倒なので、UNICODE版のみコードを用意している。
//ANSI/SJIS版を実装するには、文字コード変換を挟むか、もしくはメニュー定義ファイルの文字コードを変換しておくこと
#if !defined(_UNICODE)&&!defined(UNICODE)
#error "UtilReadMenuDefinitionFile() : Needs implementation for ANSI/S_JIS."
#endif

	/*
	入力ファイル形式
	文字コード:
		BOM無しUTF-16LE
	メニュー定義:[トップ](\t[形式名])+\n
		トップメニューのキャプションに続けて形式名を少なくとも一つのタブで区切って記述する。
		また、改行でメニュー定義が区切られる。
		(ex.)
		cabinet	arc1	arc2
		7z	PPMd	Deflate
	出力形式:
		CStringの二重配列に結果が格納される。
		std::vector<CString>には一つのメニュー階層が格納される。
		上の例では["cabinet","arc1","arc2"]となる。このうち、一つ目がポップアップメニューの根本のキャプションとなる。

	*/


	//入力ファイル名チェック
	ASSERT(lpFileName);
	if(!lpFileName)return false;
	if(!PathFileExists(lpFileName))return false;

	HANDLE hFile=CreateFile(lpFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(INVALID_HANDLE_VALUE==hFile)return false;

	//4GB越えファイルは扱わない
	const DWORD dwSize=GetFileSize(hFile,NULL);
	std::vector<BYTE> cReadBuffer;
	cReadBuffer.resize(dwSize+2);
	DWORD dwRead;
	//---読み込み
	if(!ReadFile(hFile,&cReadBuffer[0],dwSize,&dwRead,NULL)||dwSize!=dwRead){
		CloseHandle(hFile);
		return false;
	}
	CloseHandle(hFile);

	//終端文字追加
	*((LPWSTR)&cReadBuffer[dwSize])=L'\0';


	//解釈を行う
	LPCTSTR lpC=(LPCTSTR)&cReadBuffer[0];
	CString strTemp;
	std::vector<CString> strArr;

	r_MenuArr.clear();
	for(;*lpC!=_T('\0');lpC++){
		switch(*lpC){
		case _T('\t'):	//メニュー項目の区切り
			if(!strTemp.IsEmpty()){
				strArr.push_back(strTemp);
				strTemp.Empty();
			}
			break;
		case _T('\n'):	//FALLTHROUGH
		case _T('\r'):
			if(!strTemp.IsEmpty()){
				strArr.push_back(strTemp);
				strTemp.Empty();
			}
			if(!strArr.empty()){
				//メニューに追加
				r_MenuArr.push_back(strArr);
				strArr.clear();
			}
			break;
		default:
			strTemp+=*lpC;
			break;
		}
	}

	//最後の処理
	if(!strTemp.IsEmpty()){
		strArr.push_back(strTemp);
	}
	if(!strArr.empty()){
		r_MenuArr.push_back(strArr);
		strArr.clear();
	}

	return true;
}


BOOL UtilLoadString(UINT nID,CString& strOut)
{
	return strOut.LoadString(g_hInstDLL,nID);
}
