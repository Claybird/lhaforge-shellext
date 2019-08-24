#pragma once
#include "../MenuData.h"	//ShellExtDLLのデータを直接参照

class CFormView : public CDialogImpl<CFormView>
{
protected:
	CListViewCtrl List_MenuAdded;
	CListViewCtrl List_MenuSource;

	CListViewCtrl List_UserMenu;

	//---ユーザー定義メニュー
	CEdit Edit_UserMenuCaption;
	CEdit Edit_UserMenuParam;
	//表示条件
	CButton Check_Shell;	//右クリック
	CButton Check_Drag;		//右ドラッグ

	std::vector<int> m_AddedMenuArray;
	std::vector<CString> m_MenuCaptionArray;
	CPath m_strIniPath;

	std::vector<CUserMenuItem> m_UserMenuArray;
	CUserMenuItem *m_lpUserMenuItem;
public:
	CFormView():m_lpUserMenuItem(NULL){}
	virtual ~CFormView(){}
	enum { IDD = IDD_FORMVIEW_MENU_EDIT };

	BOOL PreTranslateMessage(MSG* pMsg){
		return CWindow::IsDialogMessage(pMsg);
	}

protected:
	// メッセージマップ
	BEGIN_MSG_MAP_EX(CFormView)
		NOTIFY_CODE_HANDLER_EX(LVN_GETDISPINFO, OnGetDispInfo)
		COMMAND_ID_HANDLER_EX(IDOK,OnOK)
		COMMAND_ID_HANDLER_EX(IDCANCEL,OnCancel)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_RESET,OnReset)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_VERSION,OnVersion)

		//---圧縮メニュー
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_MOVEUP,OnMoveUp)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_MOVEDOWN,OnMoveDown)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_ADD,OnAdd)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_DELETE,OnDelete)

		//---ユーザーメニュー
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_MOVEUP_USERMENU,OnMoveUp)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_MOVEDOWN_USERMENU,OnMoveDown)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_ADD_USERMENU,OnAdd)
		COMMAND_ID_HANDLER_EX(IDC_BUTTON_DELETE_USERMENU,OnDelete)
		NOTIFY_CODE_HANDLER_EX(LVN_ITEMCHANGED, OnSelect)

		MSG_WM_INITDIALOG(OnInitDialog)
//		MSG_WM_DESTROY(OnDestroy)
	END_MSG_MAP()

	LRESULT OnInitDialog(HWND hWnd, LPARAM lParam);

	//仮想リストビュー
	LRESULT OnGetDispInfo(LPNMHDR pnmh);

	void OnReset(UINT,int,HWND);
	void OnAdd(UINT,int,HWND);
	void OnDelete(UINT,int,HWND);
	void OnCancel(UINT,int,HWND);
	void OnOK(UINT,int,HWND);
	void OnMoveUp(UINT,int,HWND);
	void OnMoveDown(UINT,int,HWND);

	void OnVersion(UINT,int,HWND);

	bool ConstructMenu(LPCTSTR);
	bool ConstructMenuFromSetting();

	void StoreMenuData();
	void ClearMenuData();

	LRESULT OnSelect(LPNMHDR pnmh);
	//ユーザー定義メニューの設定を更新するためのメソッド
	void SetUserMenuData(CUserMenuItem *lpUserMenuItem);
	void GetUserMenuData();

	void SetDefaultIniPath();	//標準の設定ファイルのパスをセット
};
