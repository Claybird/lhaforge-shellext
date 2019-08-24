#include "stdafx.h"
#include "resource.h"
#include "MenuData.h"
#include "ConfigManager.h"
#include "main.h"
#include "lfutil.h"

void CConfigManager::LoadConfig(BOOL bRightDrag,BOOL bForceExtra)
{
	//変数初期化
	ZeroMemory(&ShellMenu,sizeof(ShellMenu));
	ZeroMemory(&DragMenu,sizeof(DragMenu));
	bUnderSubMenu=false;

	CPath strIniPath;
	GetDefaultFilePath(strIniPath,_T("LhaForge.ini"));

	dprintf(_T("Common-> IniPath=%s\n"),(LPCTSTR)strIniPath);

//---------------------------------------------------
	//拡張メニューを利用するかどうか
	bExtraMenu = bForceExtra || GetPrivateProfileInt(_T("ExtraMenu"),_T("ForceExtraMenu"),0,strIniPath);

	if(bRightDrag){
		//--------------------
		// 右ドラッグメニュー
		//--------------------
		//圧縮
		DragMenu.bCompress=GetPrivateProfileInt(_T("DragMenu"),_T("Compress"),1,strIniPath);
		//圧縮
		DragMenu.bExtract=GetPrivateProfileInt(_T("DragMenu"),_T("Extract"),1,strIniPath);
		//------------------------------
		// サブメニュー以下に放り込む？
		//------------------------------
		//有効・無効
		bUnderSubMenu=GetPrivateProfileInt(_T("DragMenu"),_T("UnderSubMenu"),0,strIniPath);
	}else{
		//--------------------
		// 右クリックメニュー
		//--------------------
		//圧縮
		ShellMenu.bCompress=GetPrivateProfileInt(_T("ShellMenu"),_T("Compress"),1,strIniPath);
		//解凍
		ShellMenu.bExtract=GetPrivateProfileInt(_T("ShellMenu"),_T("Extract"),1,strIniPath);
		//閲覧
		ShellMenu.bList=GetPrivateProfileInt(_T("ShellMenu"),_T("List"),1,strIniPath);
		//書庫検査
		ShellMenu.bTestArchive=GetPrivateProfileInt(_T("ShellMenu"),_T("Test"),1,strIniPath);
		//------------------------------
		// サブメニュー以下に放り込む？
		//------------------------------
		//有効・無効
		bUnderSubMenu=GetPrivateProfileInt(_T("ShellMenu"),_T("UnderSubMenu"),0,strIniPath);
	}

	if(!bExtraMenu&&(ShellMenu.bCompress||DragMenu.bCompress)){
		//カスタムメニュー
		bCustomCompress=GetPrivateProfileInt(_T("CustomMenu"),_T("UseCustom"),0,strIniPath);
		if(bCustomCompress){	//カスタマイズされたメニューを使う
			GetPrivateProfileString(_T("CustomMenu"),_T("Arrange"),USER_MENU_DEFAULT,szCustomCompress,_MAX_PATH,strIniPath);
		}
	}

	//-----------------
	// XacRettメニュー
	//-----------------
	//有効・無効
	bXacRett=GetPrivateProfileInt(_T("XacRett"),_T("EnableShellMenu"),0,strIniPath);
	//-----------------
	// B2Eメニュー
	//-----------------
	//有効・無効
	bB2E=GetPrivateProfileInt(_T("B2E"),_T("EnableShellMenu"),0,strIniPath);

	//----------------------
	// ユーザー定義メニュー
	//----------------------
	//有効・無効
	UserMenuArray.clear();
	UINT iIndex=0;
	for(;;iIndex++){
		CString strSectionName;
		strSectionName.Format(_T("UserMenu%d"),iIndex);
		if(!UtilCheckINISectionExists(strSectionName,strIniPath)){
			break;
		}
		CUserMenuItem umi={0};
		//キャプション
		GetPrivateProfileString(strSectionName,_T("Caption"),strSectionName,umi.Caption,COUNTOF(umi.Caption)-1,strIniPath);
		//パラメータ
		GetPrivateProfileString(strSectionName,_T("Param"),_T(""),umi.Param,COUNTOF(umi.Param)-1,strIniPath);
		//右クリックで表示?
		umi.bShellMenu=GetPrivateProfileInt(strSectionName,_T("ShellMenu"),TRUE,strIniPath);
		//右ドラッグで表示？
		umi.bDragMenu=GetPrivateProfileInt(strSectionName,_T("DragMenu"),TRUE,strIniPath);
		//表示する時だけロード
		if( (!bRightDrag && umi.bShellMenu) || (bRightDrag && umi.bDragMenu)){
			UserMenuArray.push_back(umi);
		}
	}

	//B2E圧縮メニューを読み込み
	CPath strPath;
	B2ECompressMenu.clear();
	GetDefaultFilePath(strPath,_T("B2EMenu.dat"));
	if(!UtilReadMenuDefinitionFile(strPath,B2ECompressMenu)){
		B2ECompressMenu.clear();
	}
}

void CConfigManager::GetDefaultFilePath(CPath &strIniPath,LPCTSTR lpszFile)	//標準の設定ファイルのパスをセット
{
	const LPCTSTR PROGRAMDIR_NAME=_T("LhaForge");	//ApplicationDataに入れるときに必要なディレクトリ名
	//const LPCTSTR INI_FILE_NAME=_T("LhaForge.ini");

	//---ユーザー間で共通の設定を用いる
	//LhaForgeフォルダと同じ場所にINIがあれば使用する
	{
		TCHAR szPath[_MAX_PATH+1]={0};
		::GetModuleFileName(g_hInstDLL, szPath, _MAX_PATH);	//自分のパス取得
		strIniPath=szPath;
		strIniPath.RemoveFileSpec();
		strIniPath.Append(lpszFile);
		if(strIniPath.FileExists()){
			//共通設定
			return;
		}
	}
	//CSIDL_COMMON_APPDATAにINIがあれば使用する
	{
		TCHAR szPath[_MAX_PATH+1]={0};
		SHGetFolderPath(NULL,CSIDL_COMMON_APPDATA|CSIDL_FLAG_CREATE,NULL,SHGFP_TYPE_CURRENT,szPath);
		strIniPath=szPath;
		strIniPath.Append(PROGRAMDIR_NAME);
		strIniPath.Append(lpszFile);
		if(strIniPath.FileExists()){
			//共通設定
			return;
		}
	}

	//--------------------

	//---ユーザー別設定を用いる
	//LhaForgeインストールフォルダ以下にファイルが存在する場合、それを使用
	{
		//ユーザー名取得
		TCHAR UserName[UNLEN+1]={0};
		DWORD Length=UNLEN;
		GetUserName(UserName,&Length);

		TCHAR szPath[_MAX_PATH+1]={0};
		::GetModuleFileName(g_hInstDLL, szPath, _MAX_PATH);	//自分のパス取得
		strIniPath=szPath;
		strIniPath.RemoveFileSpec();
		strIniPath.Append(UserName);
		strIniPath.Append(lpszFile);

		if(strIniPath.FileExists()){
			return;
		}
	}
	//---デフォルト
	//CSIDL_APPDATAにINIがあれば使用する:Vistaではこれ以外はアクセス権限不足になる可能性がある
	TCHAR szPath[_MAX_PATH+1]={0};
	SHGetFolderPath(NULL,CSIDL_APPDATA|CSIDL_FLAG_CREATE,NULL,SHGFP_TYPE_CURRENT,szPath);
	strIniPath=szPath;
	strIniPath.Append(PROGRAMDIR_NAME);
	strIniPath.AddBackslash();
	strIniPath.Append(lpszFile);
	return;
}
