/////////////////////////////////////////////////////////////////////////////
//
//	Modified by Claybird <claybird.without.wing@gmail.com>
//
// MenuExtension.h :拡張シェル コンテクストメニューコンポーネントクラスのヘッダ
//					LhaForge拡張メニュー
//
//	Original File Information:
// MenuExtension.h :拡張シェル コンテクストメニューコンポーネントクラスのヘッダ
//					他人の褌(TF)拡張メニュー
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ConfigManager.h"

const int PARAM_MAX_LEN=256;


// コンテクストメニューコンポーネントの作成（IShellExtInit,IContextMenuを多重継承する）
class CMenuExtension : public IShellExtInit,public IContextMenu
{
protected:
	//bool			m_bXP;					//WindowsXPもしくはそれ以降ならtrue
	LONG			m_cRef;					// オブジェクトの参照カウント
	bool			m_bMenuAdded;			//メニュー追加済みならtrue

	std::vector<CString> m_SelFileArray;	// 選択ファイル名テーブル
	CString			m_strTargetFolder;			//ドラッグ先のフォルダ

	std::vector<CString> m_CmdParamArray;	//コマンドパラメータの配列;メニューIDと1対1対応

	CConfigManager	m_Config;
	bool			m_bRightDrag;				//右ドラッグかどうか;CLSIDで分類し、ClassFactoryで決まる
	bool			m_bForceExtraMenu;			//拡張メニュー(SHIFT+右クリック/ドラッグ)かどうか
	std::vector<HMENU>	m_MenuHandleList;		//ポップアップメニューのハンドル管理
protected:
	void PrepareMenu();			// メニューの準備
	bool GetSelectedFiles(LPDATAOBJECT pDataObj);			// 選択ファイル名取り出し
	void DeleteMenuData();		// メニューデータの破棄
	void DoCommand(UINT inIdx);	// 選択した拡張メニューのコマンド実行

	HMENU MakePopupMenuItem();
	void AddDummyMenuCommand(){m_CmdParamArray.push_back(_T(""));}

	void BuildExtraMenu(HMENU hParentMenu,UINT idCmdFirst,int menuIdx,const SHELL_MENU_ITEM* pMenu,size_t numItems);
	void InsertCommandMenuItems(HMENU hParentMenu,UINT idCmdFirst,const SHELL_MENU_ITEM* pMenu,size_t numItems,int nOffset);

	UINT InsertContextMenu(HMENU hMenu,UINT indexMenu,UINT idCmdFirst,BOOL bInSubMenu);	//メニューを追加

	UINT InsertCompressMenu(HMENU hMenu,UINT indexMenu,UINT idCmdFirst);	//indexMenu+(戻り値)が次のindexMenu
	UINT InsertExtractMenuRClick(HMENU hMenu,UINT indexMenu,UINT idCmdFirst);
	UINT InsertExtractMenuRDrag(HMENU hMenu,UINT indexMenu,UINT idCmdFirst);
	UINT InsertOperationMenu(HMENU hMenu,UINT indexMenu,UINT idCmdFirst);
	UINT InsertXacRettMenu(HMENU hMenu,UINT indexMenu,UINT idCmdFirst);
	UINT InsertB2EMenu(HMENU hMenu,UINT indexMenu,UINT idCmdFirst);
	UINT InsertUserMenu(HMENU hMenu,UINT indexMenu,UINT idCmdFirst);
public:
	// コンストラクタ・デストラクタ
	CMenuExtension(bool);
	virtual ~CMenuExtension();

	// IUnknown インターフェイスのメソッド
	STDMETHODIMP            QueryInterface(REFIID, LPVOID *);
	STDMETHODIMP_(ULONG)    AddRef();
	STDMETHODIMP_(ULONG)    Release();

	// IShellExtInit インターフェイスのメソッド
	STDMETHODIMP Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hKeyID);

	// IContextMenu インターフェイスのメソッド
	STDMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst,UINT idCmdLast, UINT uFlags);
	STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi);
	STDMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pwReserved,LPSTR pszName, UINT cchMax);
};


