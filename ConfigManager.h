#pragma once

struct CConfigManager{
	struct{
		BOOL bCompress;
		BOOL bExtract;
		BOOL bList;
		BOOL bTestArchive;
	}ShellMenu;
	struct{
		BOOL bCompress;
		BOOL bExtract;
	}DragMenu;

	BOOL bXacRett;		//XacRettメニューを有効にするならtrue
	BOOL bB2E;			//B2Eメニューを有効にするならtrue
	BOOL bRightDrag;	//右ドラッグメニューならtrue
	BOOL bExtraMenu;	//拡張メニューを表示するならtrue
	BOOL bCustomCompress;	//カスタマイズした圧縮メニューを使うならtrue
	TCHAR szCustomCompress[_MAX_PATH+1];	//カスタマイズした圧縮メニューのデータ
	BOOL bUnderSubMenu;	//「LhaForge」サブメニュー以下に放り込む

	std::vector<CUserMenuItem> UserMenuArray;	//ユーザー定義メニューのデータ
	std::vector<std::vector<CString> > B2ECompressMenu;	//B2Eメニューのうち圧縮メニューの内容

	void LoadConfig(BOOL bRightDrag,BOOL bForceExtra);
	void GetDefaultFilePath(CPath &strIniPath,LPCTSTR lpszFile);	//標準の設定ファイルのパスをセット

	CConfigManager(){}
	virtual ~CConfigManager(){}
};


