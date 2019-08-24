/////////////////////////////////////////////////////////////////////////////
//
//	Modified by Claybird <claybird.without.wing@gmail.com>
//
// MenuExtension.cpp :	拡張シェル コンテクストメニューコンポーネントクラスの定義
//						LhaForge拡張メニュー
//
//	Original File Information:
// MenuExtension.cpp :	拡張シェル コンテクストメニューコンポーネントクラスの定義
//						他人の褌(TF)拡張メニュー
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MenuData.h"
#include "MenuExtension.h"
#include "Main.h"
#include "lfutil.h"

//-------------------------------------------------------------------------
// CMenuExtension	コンストラクタ・デストラクタ
//-------------------------------------------------------------------------

// CMenuExtension
CMenuExtension::CMenuExtension(bool bRightDrag)
	:	m_cRef(0L),
		m_bRightDrag(bRightDrag),
		m_bMenuAdded(false),
		m_bForceExtraMenu(GetKeyState(VK_SHIFT)<0)
{
	InterlockedIncrement(&g_cDllRef);
	dprintf(_T("\n\nCMenuExtension::CMenuExtension\n"));
}

// ~CMenuExtension
CMenuExtension::~CMenuExtension()
{
	dprintf(_T("CMenuExtension::~CMenuExtension\n\n\n"));

	//メニューデータの破棄
	DeleteMenuData();

	InterlockedDecrement(&g_cDllRef);
}

//-------------------------------------------------------------------------
// CMenuExtension	
//
//			IUnknown インターフェイスのメソッド群
//
//				QueryInterface
//				AddRef
//				Release
//
//-------------------------------------------------------------------------

// QueryInterface
STDMETHODIMP CMenuExtension::QueryInterface(
	REFIID		riid,
	LPVOID *	ppv)
{
	dprintf(_T("CMenuExtension::QueryInterface(ppvObj=%x , riid=%d)\n"),ppv,riid);

	*ppv = NULL;
	if(IsEqualIID(riid, IID_IShellExtInit) || IsEqualIID(riid, IID_IUnknown)){
		*ppv = (LPSHELLEXTINIT)this;
	}
	else if(IsEqualIID(riid, IID_IContextMenu)){
		*ppv = (LPCONTEXTMENU)this;
	}

    if(*ppv){
		static_cast<IUnknown*>(*ppv)->AddRef();
		return NOERROR;
	}

	return E_NOINTERFACE;
}

// AddRef
STDMETHODIMP_(ULONG) CMenuExtension::AddRef()
{
	dprintf(_T("CMenuExtension::AddRef (m_cRef=%d)\n"),m_cRef);

	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

// Release
STDMETHODIMP_(ULONG) CMenuExtension::Release()
{
	dprintf(_T("CMenuExtension::Release (m_cRef=%d)\n"),m_cRef);

	ULONG theRef = InterlockedDecrement(&m_cRef);
	if(theRef == 0L)delete this;
	return theRef;	// ローカル変数を使っているのは delete 後でも値を返せるように
}

//-------------------------------------------------------------------------
// CMenuExtension	
//
//			IShellExtInit インターフェイスのメソッド
//
//				Initialize
//
//-------------------------------------------------------------------------

// Initialize
STDMETHODIMP CMenuExtension::Initialize(
	LPCITEMIDLIST	pIDFolder,
	LPDATAOBJECT	pDataObj,
	HKEY			hRegKey)
{
	dprintf(_T("CMenuExtension::Initialize\n"));

	UNREFERENCED_PARAMETER(hRegKey);

	m_SelFileArray.clear();

	//メニューデータの破棄
	DeleteMenuData();

	//対象フォルダ取得
	if(m_bRightDrag){
		TCHAR Buffer[_MAX_PATH+3]={0};
		::SHGetPathFromIDList( pIDFolder, Buffer);
		PathQuoteSpaces(Buffer);
		m_strTargetFolder=Buffer;
	}

	//選択ファイル取得
    if(pDataObj){
		GetSelectedFiles(pDataObj);
    }

	// メニューの準備
	//PrepareMenu();
	return NOERROR;
}

//-------------------------------------------------------------------------
// CMenuExtension	
//
//			IShellExtInit インターフェイスのメソッド群
//
//				QueryContextMenu
//				InvokeCommand
//				GetCommandString
//
//-------------------------------------------------------------------------

// QueryContextMenu
STDMETHODIMP CMenuExtension::QueryContextMenu(
	HMENU	hMenu,
	UINT	indexMenu,
	UINT	idCmdFirst,
	UINT	idCmdLast,
	UINT	uFlags)
{
	dprintf(_T("CMenuExtension::QueryContextMenu\n"));
	if(m_bMenuAdded)return MAKE_HRESULT(SEVERITY_SUCCESS,FACILITY_NULL,0);

	// uFlags に CMF_DEFAULTONLY が含まれる場合は何もしない
	if ((uFlags & CMF_DEFAULTONLY)){
		return MAKE_HRESULT(SEVERITY_SUCCESS,FACILITY_NULL,0);
	}

	if((uFlags&0x000F)!=CMF_NORMAL && !(uFlags&CMF_VERBSONLY) && !(uFlags&CMF_EXPLORE)/*ツリービューで右クリック*/){
		return MAKE_HRESULT(SEVERITY_SUCCESS,FACILITY_NULL,0);
	}

	//if(uFlags & CMF_EXTENDEDVERBS){
	//	//拡張メニュー
	//	m_bForceExtraMenu=true;
	//}

	//ファイルが選択されていないなら何もしない
	if(m_SelFileArray.empty()){
		return MAKE_HRESULT(SEVERITY_SUCCESS,FACILITY_NULL,0);
	}

	PrepareMenu();

	//================
	// メニューの表示
	//================
	HMENU hTargetMenu=hMenu;
	UINT subMenuIdx=indexMenu;
	if(m_Config.bUnderSubMenu){	//サブメニュー以下に放り込む場合
		hTargetMenu=MakePopupMenuItem();
		subMenuIdx=0;
	}
	//実際にメニュー追加
	UINT itemsAdded=InsertContextMenu(hTargetMenu,subMenuIdx,idCmdFirst,m_Config.bUnderSubMenu);

	//何かしらのメニューを追加した場合
	if(itemsAdded!=0){
		int nTop=itemsAdded;
		if(m_Config.bUnderSubMenu){	//サブメニュー
			//「LhaForge」を追加
			CString strCaption;
			UtilLoadString(IDS_MENU_CAPTION_LHAFORGE,strCaption);
			UtilInsertMenuItem(hMenu,strCaption,indexMenu+itemsAdded,hTargetMenu,idCmdFirst+(WORD)m_CmdParamArray.size());
			AddDummyMenuCommand();

			nTop=1;
		}

		//------------------------
		// 最後のセパレータの追加
		//------------------------
		UtilInsertMenuItem(hMenu,NULL,indexMenu+nTop,NULL,idCmdFirst+(WORD)m_CmdParamArray.size());
		AddDummyMenuCommand();
	}

	//メニュー追加済みフラグを立てる;念のため
	m_bMenuAdded=true;

	/*
	 * From MSDN:
	 * If successful, returns an HRESULT value that has its severity value set to SEVERITY_SUCCESS and its code value
	 * set to the offset of the largest command identifier that was assigned, plus one.
	 * 
	 * For example, assume that idCmdFirst is set to 5 and you add three items to the menu with
	 * command identifiers of 5, 7, and 8.
	 * 
	 * The return value should be MAKE_HRESULT(SEVERITY_SUCCESS, 0, 8 - 5 + 1). Otherwise, it returns an OLE error value.
	 */
	return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL,(WORD)m_CmdParamArray.size());
}

HMENU CMenuExtension::MakePopupMenuItem()
{
	HMENU hTempMenu=::CreatePopupMenu();
	m_MenuHandleList.push_back(hTempMenu);
	return hTempMenu;
}

void CMenuExtension::BuildExtraMenu(HMENU hParentMenu,UINT idCmdFirst,int menuIdx,const SHELL_MENU_ITEM* pMenu,size_t numItems)
{
	//ポップアップメニューの根本
	HMENU hTempMenu=MakePopupMenuItem();
	CString strCaption;
	UtilLoadString(pMenu[0].Caption,strCaption);
/*
サブメニューと親メニューでキャプションが被るとIDをマージされる模様
+(parent)[caption:ABC][id:0]+
         +--(childA)[caption:ABC][id:1]
         +--(childB)[caption:DEF][id:2]
この形式の時、id:0とid:1が同じキャプションを使っている
すると、childA(id:1)を選んだとき、id:0を選んだ物として扱われてしまう
*/
	//HACK:サブメニューと親メニューでキャプションが被らないようにちょっとだけいじる
	UtilInsertMenuItem(hParentMenu,strCaption+_T(" "),menuIdx,hTempMenu,-1);//idCmdFirst+(WORD)m_CmdParamArray.size());
	//ダミーアイテム
//	AddDummyMenuCommand();

	//各メニュー
	InsertCommandMenuItems(hTempMenu,idCmdFirst,pMenu+1,numItems-1,0);
}

void CMenuExtension::InsertCommandMenuItems(HMENU hParentMenu,UINT idCmdFirst,const SHELL_MENU_ITEM* pMenu,size_t numItems,int nOffset)
{
	for(UINT idx=0;idx<numItems;idx++){
		CString strCaption;
		UtilLoadString(pMenu[idx].Caption,strCaption);

		WORD menuID = idCmdFirst + (WORD)m_CmdParamArray.size();
		UtilInsertMenuItem(hParentMenu,strCaption,idx+nOffset,NULL,menuID);

		dprintf(_T("[InserteCommandMenuItems]: cmdId=%d : %s : %s\n"),m_CmdParamArray.size(),(LPCTSTR)strCaption,pMenu[idx].Param);
		m_CmdParamArray.push_back(pMenu[idx].Param);
	}
}

UINT CMenuExtension::InsertCompressMenu(
	HMENU	hMenu,
	UINT	indexMenu,
	UINT	idCmdFirst)
{
	//---圧縮メニュー
	HMENU hCompressSubMenu=MakePopupMenuItem();

	bool bMenuAdded=false;

	if(m_Config.bExtraMenu){
		//発展メニュー
		for(int menuIdx=0;menuIdx<COUNTOF(g_ExtraMenuTable);menuIdx++){
			BuildExtraMenu(hCompressSubMenu,idCmdFirst,menuIdx,g_ExtraMenuTable[menuIdx].pMenu,g_ExtraMenuTable[menuIdx].numItems);
		}
		bMenuAdded=true;
	}else if(!m_Config.bCustomCompress){
		//通常の圧縮メニュー
		InsertCommandMenuItems(hCompressSubMenu,idCmdFirst,g_CompressMenu,COUNTOF(g_CompressMenu),0);
		bMenuAdded=true;
	}else{
		//カスタマイズされたメニューを使う
		std::vector<SHELL_MENU_ITEM> menu;

		LPCTSTR lpChar=m_Config.szCustomCompress;
		dprintf(_T("CustomMenu Arrange Data=%s\n"),lpChar);
		for(;*lpChar!=_T('\0');lpChar++){
			int i;
			for(i=0;i<COUNTOF(g_UserMenuTable);i++){
				if(g_UserMenuTable[i].Identifier==*lpChar){	//一致
					break;
				}
			}
			if(i==COUNTOF(g_UserMenuTable)){	//一致する物がない、つまり不正なメニュー指定
				MessageBeep(MB_ICONEXCLAMATION);
				continue;
			}else{
				//メニューアイテムを追加
				SHELL_MENU_ITEM smi;
				smi.Caption=g_UserMenuTable[i].lpMenuItem->Caption;
				smi.Param=g_UserMenuTable[i].lpMenuItem->Param;
				menu.push_back(smi);
			}
		}

		if(!menu.empty()){
			InsertCommandMenuItems(hCompressSubMenu,idCmdFirst,&menu[0],menu.size(),0);
			bMenuAdded=true;
		}
	}


	if(bMenuAdded){
		//圧縮メニューのポップアップの根本を追加
		CString strCaption;
		if(m_bRightDrag){	//右ドラッグ
			UtilLoadString(IDS_MENU_CAPTION_COMPRESS_DRAG,strCaption);
		}else{	//右クリック
			UtilLoadString(IDS_MENU_CAPTION_COMPRESS,strCaption);
		}
		dprintf(_T("MenuCaption=(%s),indexMenu=(%d)\n"),(LPCTSTR)strCaption,m_CmdParamArray.size());
		UtilInsertMenuItem(hMenu,strCaption,indexMenu,hCompressSubMenu,idCmdFirst+(WORD)m_CmdParamArray.size());
		AddDummyMenuCommand();
		return 1;
	}else{
		return 0;
	}
}


UINT CMenuExtension::InsertExtractMenuRClick(
	HMENU	hMenu,
	UINT	indexMenu,
	UINT	idCmdFirst)
{
	HMENU hExtractSubMenu=MakePopupMenuItem();

	//右クリックメニューで解凍を表示する必要があるとき
	CString strCaption;
	//「LhaForgeで解凍」を追加
	UtilLoadString(IDS_MENU_CAPTION_EXTRACT,strCaption);
	dprintf(_T("MenuCaption=(%s),indexMenu=(%d)\n"),(LPCTSTR)strCaption,indexMenu);
	UtilInsertMenuItem(hMenu,strCaption,indexMenu,hExtractSubMenu,idCmdFirst+(WORD)m_CmdParamArray.size());
	AddDummyMenuCommand();

	// コマンドパラメタから追加となるサブメニューコマンドを追加
	InsertCommandMenuItems(hExtractSubMenu,idCmdFirst,g_ExtractMenu,COUNTOF(g_ExtractMenu),0);

	return 1;
}

UINT CMenuExtension::InsertExtractMenuRDrag(
	HMENU	hMenu,
	UINT	indexMenu,
	UINT	idCmdFirst)
{
	CString strCaption;
	UtilLoadString(IDS_MENU_CAPTION_EXTRACT_DRAG,strCaption);

	UtilInsertMenuItem(hMenu,strCaption,indexMenu,NULL,idCmdFirst+(WORD)m_CmdParamArray.size());

	m_CmdParamArray.push_back(_T("/e"));
	return 1;
}

UINT CMenuExtension::InsertOperationMenu(
	HMENU	hMenu,
	UINT	indexMenu,
	UINT	idCmdFirst)
{
	// 閲覧メニュー
	UINT subIndex=0;
	if(m_Config.ShellMenu.bList){
		//右クリックメニューの場合のみ
		CString strCaption;
		UtilLoadString(IDS_MENU_CAPTION_LIST,strCaption);
		UtilInsertMenuItem(hMenu,strCaption,indexMenu+subIndex,NULL,idCmdFirst+(WORD)m_CmdParamArray.size());

		m_CmdParamArray.push_back(_T("/l"));
		subIndex++;
	}

	// 検査メニュー
	if(m_Config.ShellMenu.bTestArchive){
		CString strCaption;
		UtilLoadString(IDS_MENU_CAPTION_TEST_ARCHIVE,strCaption);
		UtilInsertMenuItem(hMenu,strCaption,indexMenu+subIndex,NULL,idCmdFirst+(WORD)m_CmdParamArray.size());

		m_CmdParamArray.push_back(_T("/t"));
		subIndex++;
	}

	return subIndex;
}

UINT CMenuExtension::InsertXacRettMenu(
	HMENU	hMenu,
	UINT	indexMenu,
	UINT	idCmdFirst)
{
	if(!m_bRightDrag){	//右クリックメニューの場合
		HMENU hXacRettSubMenu=MakePopupMenuItem();
		//「LhaForge+XacRett.DLL」を追加
		CString strCaption;
		UtilLoadString(IDS_MENU_CAPTION_XACRETT,strCaption);
		UtilInsertMenuItem(hMenu,strCaption,indexMenu,hXacRettSubMenu,idCmdFirst+(WORD)m_CmdParamArray.size());
		AddDummyMenuCommand();

		// コマンドパラメタから追加となるサブメニューコマンドを追加
		InsertCommandMenuItems(hXacRettSubMenu,idCmdFirst,g_XacRettMenu,COUNTOF(g_XacRettMenu),0);
	}else{
		//右ドラッグメニューの場合
		CString strCaption;
		UtilLoadString(IDS_MENU_CAPTION_XACRETT_DRAG,strCaption);

		UtilInsertMenuItem(hMenu,strCaption,indexMenu,NULL,idCmdFirst+(WORD)m_CmdParamArray.size());

		m_CmdParamArray.push_back(_T("/xacrett /e"));
	}
	return 1;
}

UINT CMenuExtension::InsertB2EMenu(
	HMENU	hMenu,
	UINT	indexMenu,
	UINT	idCmdFirst)
{
	HMENU hB2ESubMenu=MakePopupMenuItem();

	CString strCaption;
	//「LhaForge+B2E32.DLL」を追加
	UtilLoadString(IDS_MENU_CAPTION_B2E,strCaption);
	UtilInsertMenuItem(hMenu,strCaption,indexMenu,hB2ESubMenu,idCmdFirst+(WORD)m_CmdParamArray.size());
	AddDummyMenuCommand();

	int nOffset=0;
	//圧縮形式をメニューで表示するならサブメニュー追加
	//「B2Eでこの形式に圧縮」的な内容
	bool bMethodSelect=!m_Config.B2ECompressMenu.empty();
	if(bMethodSelect){
		HMENU hB2ECompressPopup=MakePopupMenuItem();
		UtilLoadString(IDS_MENU_CAPTION_B2E_COMPRESS_POPUP,strCaption);
		UtilInsertMenuItem(hB2ESubMenu,strCaption,0,hB2ECompressPopup,idCmdFirst+(WORD)m_CmdParamArray.size());
		AddDummyMenuCommand();

		//形式ごとのメニューを用意
		for(UINT j=0;j<m_Config.B2ECompressMenu.size();j++){
			//形式名のメニュー
			HMENU hTempMenu=MakePopupMenuItem();
			//HACK:形式ごとのサブメニューと名前が被らないように細工する
			UtilInsertMenuItem(hB2ECompressPopup,m_Config.B2ECompressMenu[j][0]+_T(" "),j,hTempMenu,-1);//idCmdFirst+(WORD)m_CmdParamArray.size());
			//AddDummyMenuCommand();

			//形式ごとのサブメニュー
			for(UINT i=1;i<m_Config.B2ECompressMenu[j].size();i++){
				UtilInsertMenuItem(hTempMenu,m_Config.B2ECompressMenu[j][i],i-1,NULL,idCmdFirst+(WORD)m_CmdParamArray.size());

				CString strParam;
				strParam.Format(_T("/b2e \"/c:%s\" \"/method:%s\" "),(LPCTSTR)m_Config.B2ECompressMenu[j][0],(LPCTSTR)m_Config.B2ECompressMenu[j][i]);
				m_CmdParamArray.push_back(strParam);
			}
		}

		nOffset=1;
	}

	// コマンドパラメタから追加となるサブメニューコマンドを追加
	if(m_bRightDrag){
		InsertCommandMenuItems(hB2ESubMenu,idCmdFirst,g_B2EDragMenu,COUNTOF(g_B2EDragMenu),nOffset);
	}else{
		InsertCommandMenuItems(hB2ESubMenu,idCmdFirst,g_B2EMenu,COUNTOF(g_B2EMenu),nOffset);
	}
	return 1;
}

UINT CMenuExtension::InsertUserMenu(
	HMENU	hMenu,
	UINT	indexMenu,
	UINT	idCmdFirst)
{
	UINT subIndex=0;
	for(UINT i=0;i<m_Config.UserMenuArray.size();i++){
		// コマンドメニュー追加
		UtilInsertMenuItem(hMenu,m_Config.UserMenuArray[i].Caption,indexMenu+subIndex,NULL,idCmdFirst+(WORD)m_CmdParamArray.size());
		m_CmdParamArray.push_back(m_Config.UserMenuArray[i].Param);
		subIndex++;
	}
	return subIndex;
}

UINT CMenuExtension::InsertContextMenu(
	HMENU	hMenu,
	UINT	indexMenu,
	UINT	idCmdFirst,
	BOOL	bInSubMenu)
{
	// メニューの表示

	UINT subIndex=0;
	//--圧縮メニュー
	if(m_Config.ShellMenu.bCompress || m_Config.DragMenu.bCompress){
		subIndex+=InsertCompressMenu(hMenu,indexMenu+subIndex,idCmdFirst);
	}

	//--解凍メニュー
	if(m_Config.ShellMenu.bExtract){		//右クリック
		subIndex+=InsertExtractMenuRClick(hMenu,indexMenu+subIndex,idCmdFirst);
	}else if(m_Config.DragMenu.bExtract){	//右ドラッグ
		subIndex+=InsertExtractMenuRDrag(hMenu,indexMenu+subIndex,idCmdFirst);
	}

	bool bSub=/*m_bXP && */!bInSubMenu && !m_bRightDrag;
	if(bSub){
		HMENU hOperationMenu=MakePopupMenuItem();
		int subSubIndex=0;
		//--LhaForgeで操作
		subSubIndex+=InsertOperationMenu(hOperationMenu,subSubIndex,idCmdFirst);
		//--ユーザー定義メニュー
		subSubIndex+=InsertUserMenu(hOperationMenu,subSubIndex,idCmdFirst);

		if(subSubIndex>0){
			CString strCaption;
			UtilLoadString(IDS_MENU_CAPTION_OPERATION,strCaption);
			UtilInsertMenuItem(hMenu,strCaption,indexMenu+subIndex,hOperationMenu,idCmdFirst+(WORD)m_CmdParamArray.size());
			AddDummyMenuCommand();
			subIndex++;
		}
	}else{
		//--LhaForgeで操作
		subIndex+=InsertOperationMenu(hMenu,indexMenu+subIndex,idCmdFirst);
	}

	//-- XacRettメニュー
	if(m_Config.bXacRett){
		subIndex+=InsertXacRettMenu(hMenu,indexMenu+subIndex,idCmdFirst);
	}

	//--B2Eメニュー
	if(m_Config.bB2E){
		subIndex+=InsertB2EMenu(hMenu,indexMenu+subIndex,idCmdFirst);
	}
	//--ユーザー定義メニュー
	if(!bSub){
		subIndex+=InsertUserMenu(hMenu,indexMenu+subIndex,idCmdFirst);
	}

	return subIndex;
}

// InvokeCommand
STDMETHODIMP CMenuExtension::InvokeCommand(
	LPCMINVOKECOMMANDINFO	lpcmi)
{
	dprintf(_T("CMenuExtension::InvokeCommand\n"));

	HRESULT hr = E_INVALIDARG;

	// HIWORD(lpcmi->lpVerb)が0ではないときのみ処理
	// LOWORD(lpcmi->lpVerb)はクリックされたメニューID
	// これはQueryContextMenu()での (menu ID - idCmdFirst)
	if(!HIWORD(lpcmi->lpVerb)){
		UINT idCmd = LOWORD(lpcmi->lpVerb);

		// 拡張メニューの有効範囲をチェック
		if(!m_CmdParamArray.empty() && idCmd >= 0 && idCmd < m_CmdParamArray.size()){
			// コマンド実行
			DoCommand(idCmd);
			hr = NOERROR;
		}
	}
	return hr;
}

// GetCommandString
STDMETHODIMP CMenuExtension::GetCommandString(
	UINT_PTR	idCmd,
	UINT	uType,
	UINT*	pwReserved,
	LPSTR	pszName,
	UINT cchMax)
{
	UNREFERENCED_PARAMETER(pwReserved);

	dprintf(_T("CMenuExtension::GetCommandString(ParamCount=%d : idCmd=%d)\n"),m_CmdParamArray.size(),idCmd);

	//ID検証:システムによるメニューアイテムの存在確認も行われる
	//http://blogs.msdn.com/oldnewthing/archive/2004/10/06/238630.aspx
	if(idCmd >= m_CmdParamArray.size()){
		if (uType == GCS_VALIDATEA || uType == GCS_VALIDATEW) {
			return S_FALSE;
		}else return E_INVALIDARG;
	}

	// ステータス表示文字
	CString strCaption;
	UtilLoadString(IDS_DEFAULT_STATUS_CAPTION,strCaption);
	switch (uType) {
	case GCS_VERBA:
	case GCS_VERBW:
		return E_NOTIMPL;

	case GCS_HELPTEXTA:
		lstrcpynA(pszName, CT2A(strCaption), cchMax);
		return S_OK;

	case GCS_HELPTEXTW:
		lstrcpynW((LPWSTR)pszName, strCaption, cchMax);
		return S_OK;

	case GCS_VALIDATEA:
	case GCS_VALIDATEW:
		return S_OK;    // all they wanted was validation
	}

	return E_NOTIMPL;
}


//--- シェル拡張　コンテクストメニューの実装


//選択しているファイル名の取り出し
bool CMenuExtension::GetSelectedFiles(LPDATAOBJECT pDataObj)
{
	if(!pDataObj)return false;

	IEnumFORMATETC *theEnumFormatEtc = 0;
	HRESULT thrResultH = pDataObj->EnumFormatEtc(DATADIR_GET, &theEnumFormatEtc);

	if(SUCCEEDED(thrResultH)){
		thrResultH = theEnumFormatEtc->Reset();

		if(SUCCEEDED(thrResultH)){
			FORMATETC theFormatEtc;
			ULONG theFetched = 0L;
			while(TRUE){
				thrResultH = theEnumFormatEtc->Next(1, &theFormatEtc, &theFetched);
				if(FAILED(thrResultH) || (theFetched <= 0)){
					dprintf(__FUNCTIONW__ _T(":End of Data\n"));
					break;
				}
				theFormatEtc.cfFormat	= CF_HDROP;
				theFormatEtc.dwAspect	= DVASPECT_CONTENT;
				theFormatEtc.lindex		= -1;
				theFormatEtc.ptd		= NULL;
				theFormatEtc.tymed		= TYMED_HGLOBAL;
				STGMEDIUM theStgm;
				thrResultH = pDataObj->GetData(&theFormatEtc, &theStgm);
				if(SUCCEEDED(thrResultH)){
					if(theStgm.tymed == TYMED_HGLOBAL){
						UINT nFileCount = DragQueryFile((HDROP)theStgm.hGlobal, 0xFFFFFFFF, NULL, 0);
						if(nFileCount >= 1){
							m_SelFileArray.clear();
							for(UINT i=0; i<nFileCount;i++){
								TCHAR Buffer[_MAX_PATH+2]={0};
								DragQueryFile((HDROP)theStgm.hGlobal, i, Buffer, _MAX_PATH + 1);
								dprintf(__FUNCTIONW__ _T(":%d/%d:%s\n"),i,nFileCount,Buffer);
								m_SelFileArray.push_back(Buffer);
							}
						}
					}
				}
			}
		}
	}
	if(theEnumFormatEtc)theEnumFormatEtc->Release();
	
	return true;
}

/*-------------------------------------------------------------------------*/
//	PrepareMenu		メニューの準備
/*-------------------------------------------------------------------------*/
void CMenuExtension::PrepareMenu()
{
	//設定読み込み
	m_Config.LoadConfig(m_bRightDrag,m_bForceExtraMenu);
}

/*-------------------------------------------------------------------------*/
//	DeleteMenuData			メニューデータの破棄
/*-------------------------------------------------------------------------*/
void CMenuExtension::DeleteMenuData()
{
	m_CmdParamArray.clear();
	//サブメニュー廃棄
	for(UINT i=0;i<m_MenuHandleList.size();i++){
		::DestroyMenu(m_MenuHandleList[i]);
	}
	m_MenuHandleList.clear();
}

/*-------------------------------------------------------------------------*/
//	DoCommand		選択した拡張メニューのファイルをランチ（開く）
/*-------------------------------------------------------------------------*/
void CMenuExtension::DoCommand(UINT inIdx)
{
	// コマンドIDとコマンドパラメタの有効性はチェック済

	dprintf(_T("DoCommand-> inIdx=%d : Param=%s\n"),inIdx,(LPCTSTR)m_CmdParamArray[inIdx]);

	// 同一フォルダにある圧縮プログラム(LhaForge.exe)のフルパス組み立て
	TCHAR szExePath[_MAX_PATH+1];
	{
		::GetModuleFileName(g_hInstDLL, szExePath, _MAX_PATH);	//DLLのパス取得
		//DLLのパスを元にLhaForgeのファイル名を組み立てる
		PathRemoveFileSpec(szExePath);

		PathAppend(szExePath, _T("LhaForge.exe"));
		dprintf(_T("ExePath = %s\n"),szExePath);
	}
	CString CommandLine(szExePath);

	//コマンドラインパラメータを設定
	CommandLine+=_T(" ");
	CommandLine+=m_CmdParamArray[inIdx];	//ファイル処理オプション
	//ここに圧縮/解凍
	if(m_bRightDrag){
		CommandLine+=_T(" /o:");
		CommandLine+=m_strTargetFolder;//既に必要な場合は""で括られている
	}else{
		if(-1!=CommandLine.Find(OUTPUTDIR_DESKTOP)){
			//デスクトップに解凍
			TCHAR Buffer[_MAX_PATH+1]={0};
			SHGetSpecialFolderPath(NULL,Buffer,CSIDL_DESKTOPDIRECTORY,FALSE);
			PathQuoteSpaces(Buffer);
			CommandLine.Replace(OUTPUTDIR_DESKTOP,Buffer);
		}else if(-1!=CommandLine.Find(OUTPUTDIR_SAMEDIR)){
			//同じ場所に解凍
			CPath strPath(m_SelFileArray[0]);
			strPath.RemoveFileSpec();
			strPath.QuoteSpaces();
			CommandLine.Replace(OUTPUTDIR_SAMEDIR,strPath);
		}else if(-1!=CommandLine.Find(OUTPUTDIR_SPECIFIED)){
			//指定場所に解凍
			CString title;
			UtilLoadString(IDS_INPUT_TARGET_FOLDER,title);
			CFolderDialog dlg(NULL,title,BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE);
			if(IDOK!=dlg.DoModal()){
				return;
			}
			CPath strPath(dlg.GetFolderPath());
			strPath.QuoteSpaces();
			CommandLine.Replace(OUTPUTDIR_SPECIFIED,strPath);
		}
	}

	//ファイル名を順番に処理
	if(m_SelFileArray.size()>16){
		//ファイルが多いのでレスポンスファイルを使う(数字に根拠無し)
		TCHAR szTempPath[_MAX_PATH+1];
		{
			TCHAR szTempDir[_MAX_PATH+1];
			::GetTempPath(_MAX_PATH,szTempDir);
			if(!::GetTempFileName(szTempDir,_T("lf"),0,szTempPath)){
				MessageBeep(MB_ICONHAND);
				return;
			}
		}
		HANDLE hFile=::CreateFile(szTempPath,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		if(INVALID_HANDLE_VALUE==hFile){
			MessageBeep(MB_ICONHAND);
			return;
		}
		for(UINT file_count=0;file_count<m_SelFileArray.size();file_count++){
			UtilWriteLine(hFile,m_SelFileArray[file_count]);
		}
		::CloseHandle(hFile);
		//レスポンスファイルの指定

#if defined(UNICODE)||defined(_UNICODE)
		//コードページ指定
		CommandLine+=_T(" /cp:UNICODE");
#endif

		CommandLine+=_T(" \"/$");
		CommandLine+=szTempPath;
		CommandLine+=_T("\"");
	}else{	//そのままコマンドラインに渡す
		for(UINT file_count=0;file_count<m_SelFileArray.size();file_count++){
			if(-1!=m_SelFileArray[file_count].Find(_T(' '))){
				//PathQuoteSpaces...
				CommandLine+=_T(" \"");
				CommandLine+=m_SelFileArray[file_count];
				CommandLine+=_T("\"");
			}else{
				CommandLine+=_T(" ");
				CommandLine+=m_SelFileArray[file_count];
			}
		}
	}

	// 起動プロセスの表示ウィンドウの設定
	STARTUPINFO theStartupInfo = {sizeof(STARTUPINFO)};
	theStartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	theStartupInfo.wShowWindow = SW_SHOWDEFAULT;

	// プロセス起動（コマンド実行）
	PROCESS_INFORMATION	pi;
	BOOL	result = ::CreateProcess(NULL,CommandLine.GetBuffer(CommandLine.GetLength()+1),
						NULL,NULL,FALSE,CREATE_NEW_PROCESS_GROUP,
						NULL,NULL,&theStartupInfo,&pi);
	CommandLine.ReleaseBuffer();
	dprintf(_T("\tCreateProcess(%s)"),CommandLine);
	dprintf(_T(" result=%s\n"),result ? _T("OK") : _T("NG"));

	if(!result){
		CString Message=szExePath;
		Message+=_T("\n");
		{
			CString msg;
			UtilLoadString(IDS_ERROR_MSG_FAILURE_EXECUTE,msg);
			Message+=msg;
		}

		//エラーストリング取得
		{
			TCHAR szBuffer[1024];
			FormatMessage(
				FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト言語
				szBuffer, 1024, NULL);
			Message+=szBuffer;
		}
		{
			CString title;
			UtilLoadString(IDS_PROGRAM_NAME,title);
			MessageBox(NULL,Message,title,MB_OK|MB_ICONSTOP);
		}
	}

	//不要になったプロセスハンドルを処理
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}


