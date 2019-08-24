/////////////////////////////////////////////////////////////////////////////
//
//	Modified by Claybird <claybird.without.wing@gmail.com>
//
// MenuExtension.cpp :	�g���V�F�� �R���e�N�X�g���j���[�R���|�[�l���g�N���X�̒�`
//						LhaForge�g�����j���[
//
//	Original File Information:
// MenuExtension.cpp :	�g���V�F�� �R���e�N�X�g���j���[�R���|�[�l���g�N���X�̒�`
//						���l����(TF)�g�����j���[
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "MenuData.h"
#include "MenuExtension.h"
#include "Main.h"
#include "lfutil.h"

//-------------------------------------------------------------------------
// CMenuExtension	�R���X�g���N�^�E�f�X�g���N�^
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

	//���j���[�f�[�^�̔j��
	DeleteMenuData();

	InterlockedDecrement(&g_cDllRef);
}

//-------------------------------------------------------------------------
// CMenuExtension	
//
//			IUnknown �C���^�[�t�F�C�X�̃��\�b�h�Q
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
	return theRef;	// ���[�J���ϐ����g���Ă���̂� delete ��ł��l��Ԃ���悤��
}

//-------------------------------------------------------------------------
// CMenuExtension	
//
//			IShellExtInit �C���^�[�t�F�C�X�̃��\�b�h
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

	//���j���[�f�[�^�̔j��
	DeleteMenuData();

	//�Ώۃt�H���_�擾
	if(m_bRightDrag){
		TCHAR Buffer[_MAX_PATH+3]={0};
		::SHGetPathFromIDList( pIDFolder, Buffer);
		PathQuoteSpaces(Buffer);
		m_strTargetFolder=Buffer;
	}

	//�I���t�@�C���擾
    if(pDataObj){
		GetSelectedFiles(pDataObj);
    }

	// ���j���[�̏���
	//PrepareMenu();
	return NOERROR;
}

//-------------------------------------------------------------------------
// CMenuExtension	
//
//			IShellExtInit �C���^�[�t�F�C�X�̃��\�b�h�Q
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

	// uFlags �� CMF_DEFAULTONLY ���܂܂��ꍇ�͉������Ȃ�
	if ((uFlags & CMF_DEFAULTONLY)){
		return MAKE_HRESULT(SEVERITY_SUCCESS,FACILITY_NULL,0);
	}

	if((uFlags&0x000F)!=CMF_NORMAL && !(uFlags&CMF_VERBSONLY) && !(uFlags&CMF_EXPLORE)/*�c���[�r���[�ŉE�N���b�N*/){
		return MAKE_HRESULT(SEVERITY_SUCCESS,FACILITY_NULL,0);
	}

	//if(uFlags & CMF_EXTENDEDVERBS){
	//	//�g�����j���[
	//	m_bForceExtraMenu=true;
	//}

	//�t�@�C�����I������Ă��Ȃ��Ȃ牽�����Ȃ�
	if(m_SelFileArray.empty()){
		return MAKE_HRESULT(SEVERITY_SUCCESS,FACILITY_NULL,0);
	}

	PrepareMenu();

	//================
	// ���j���[�̕\��
	//================
	HMENU hTargetMenu=hMenu;
	UINT subMenuIdx=indexMenu;
	if(m_Config.bUnderSubMenu){	//�T�u���j���[�ȉ��ɕ��荞�ޏꍇ
		hTargetMenu=MakePopupMenuItem();
		subMenuIdx=0;
	}
	//���ۂɃ��j���[�ǉ�
	UINT itemsAdded=InsertContextMenu(hTargetMenu,subMenuIdx,idCmdFirst,m_Config.bUnderSubMenu);

	//��������̃��j���[��ǉ������ꍇ
	if(itemsAdded!=0){
		int nTop=itemsAdded;
		if(m_Config.bUnderSubMenu){	//�T�u���j���[
			//�uLhaForge�v��ǉ�
			CString strCaption;
			UtilLoadString(IDS_MENU_CAPTION_LHAFORGE,strCaption);
			UtilInsertMenuItem(hMenu,strCaption,indexMenu+itemsAdded,hTargetMenu,idCmdFirst+(WORD)m_CmdParamArray.size());
			AddDummyMenuCommand();

			nTop=1;
		}

		//------------------------
		// �Ō�̃Z�p���[�^�̒ǉ�
		//------------------------
		UtilInsertMenuItem(hMenu,NULL,indexMenu+nTop,NULL,idCmdFirst+(WORD)m_CmdParamArray.size());
		AddDummyMenuCommand();
	}

	//���j���[�ǉ��ς݃t���O�𗧂Ă�;�O�̂���
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
	//�|�b�v�A�b�v���j���[�̍��{
	HMENU hTempMenu=MakePopupMenuItem();
	CString strCaption;
	UtilLoadString(pMenu[0].Caption,strCaption);
/*
�T�u���j���[�Ɛe���j���[�ŃL���v�V����������ID���}�[�W�����͗l
+(parent)[caption:ABC][id:0]+
         +--(childA)[caption:ABC][id:1]
         +--(childB)[caption:DEF][id:2]
���̌`���̎��Aid:0��id:1�������L���v�V�������g���Ă���
����ƁAchildA(id:1)��I�񂾂Ƃ��Aid:0��I�񂾕��Ƃ��Ĉ����Ă��܂�
*/
	//HACK:�T�u���j���[�Ɛe���j���[�ŃL���v�V���������Ȃ��悤�ɂ�����Ƃ���������
	UtilInsertMenuItem(hParentMenu,strCaption+_T(" "),menuIdx,hTempMenu,-1);//idCmdFirst+(WORD)m_CmdParamArray.size());
	//�_�~�[�A�C�e��
//	AddDummyMenuCommand();

	//�e���j���[
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
	//---���k���j���[
	HMENU hCompressSubMenu=MakePopupMenuItem();

	bool bMenuAdded=false;

	if(m_Config.bExtraMenu){
		//���W���j���[
		for(int menuIdx=0;menuIdx<COUNTOF(g_ExtraMenuTable);menuIdx++){
			BuildExtraMenu(hCompressSubMenu,idCmdFirst,menuIdx,g_ExtraMenuTable[menuIdx].pMenu,g_ExtraMenuTable[menuIdx].numItems);
		}
		bMenuAdded=true;
	}else if(!m_Config.bCustomCompress){
		//�ʏ�̈��k���j���[
		InsertCommandMenuItems(hCompressSubMenu,idCmdFirst,g_CompressMenu,COUNTOF(g_CompressMenu),0);
		bMenuAdded=true;
	}else{
		//�J�X�^�}�C�Y���ꂽ���j���[���g��
		std::vector<SHELL_MENU_ITEM> menu;

		LPCTSTR lpChar=m_Config.szCustomCompress;
		dprintf(_T("CustomMenu Arrange Data=%s\n"),lpChar);
		for(;*lpChar!=_T('\0');lpChar++){
			int i;
			for(i=0;i<COUNTOF(g_UserMenuTable);i++){
				if(g_UserMenuTable[i].Identifier==*lpChar){	//��v
					break;
				}
			}
			if(i==COUNTOF(g_UserMenuTable)){	//��v���镨���Ȃ��A�܂�s���ȃ��j���[�w��
				MessageBeep(MB_ICONEXCLAMATION);
				continue;
			}else{
				//���j���[�A�C�e����ǉ�
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
		//���k���j���[�̃|�b�v�A�b�v�̍��{��ǉ�
		CString strCaption;
		if(m_bRightDrag){	//�E�h���b�O
			UtilLoadString(IDS_MENU_CAPTION_COMPRESS_DRAG,strCaption);
		}else{	//�E�N���b�N
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

	//�E�N���b�N���j���[�ŉ𓀂�\������K�v������Ƃ�
	CString strCaption;
	//�uLhaForge�ŉ𓀁v��ǉ�
	UtilLoadString(IDS_MENU_CAPTION_EXTRACT,strCaption);
	dprintf(_T("MenuCaption=(%s),indexMenu=(%d)\n"),(LPCTSTR)strCaption,indexMenu);
	UtilInsertMenuItem(hMenu,strCaption,indexMenu,hExtractSubMenu,idCmdFirst+(WORD)m_CmdParamArray.size());
	AddDummyMenuCommand();

	// �R�}���h�p�����^����ǉ��ƂȂ�T�u���j���[�R�}���h��ǉ�
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
	// �{�����j���[
	UINT subIndex=0;
	if(m_Config.ShellMenu.bList){
		//�E�N���b�N���j���[�̏ꍇ�̂�
		CString strCaption;
		UtilLoadString(IDS_MENU_CAPTION_LIST,strCaption);
		UtilInsertMenuItem(hMenu,strCaption,indexMenu+subIndex,NULL,idCmdFirst+(WORD)m_CmdParamArray.size());

		m_CmdParamArray.push_back(_T("/l"));
		subIndex++;
	}

	// �������j���[
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
	if(!m_bRightDrag){	//�E�N���b�N���j���[�̏ꍇ
		HMENU hXacRettSubMenu=MakePopupMenuItem();
		//�uLhaForge+XacRett.DLL�v��ǉ�
		CString strCaption;
		UtilLoadString(IDS_MENU_CAPTION_XACRETT,strCaption);
		UtilInsertMenuItem(hMenu,strCaption,indexMenu,hXacRettSubMenu,idCmdFirst+(WORD)m_CmdParamArray.size());
		AddDummyMenuCommand();

		// �R�}���h�p�����^����ǉ��ƂȂ�T�u���j���[�R�}���h��ǉ�
		InsertCommandMenuItems(hXacRettSubMenu,idCmdFirst,g_XacRettMenu,COUNTOF(g_XacRettMenu),0);
	}else{
		//�E�h���b�O���j���[�̏ꍇ
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
	//�uLhaForge+B2E32.DLL�v��ǉ�
	UtilLoadString(IDS_MENU_CAPTION_B2E,strCaption);
	UtilInsertMenuItem(hMenu,strCaption,indexMenu,hB2ESubMenu,idCmdFirst+(WORD)m_CmdParamArray.size());
	AddDummyMenuCommand();

	int nOffset=0;
	//���k�`�������j���[�ŕ\������Ȃ�T�u���j���[�ǉ�
	//�uB2E�ł��̌`���Ɉ��k�v�I�ȓ��e
	bool bMethodSelect=!m_Config.B2ECompressMenu.empty();
	if(bMethodSelect){
		HMENU hB2ECompressPopup=MakePopupMenuItem();
		UtilLoadString(IDS_MENU_CAPTION_B2E_COMPRESS_POPUP,strCaption);
		UtilInsertMenuItem(hB2ESubMenu,strCaption,0,hB2ECompressPopup,idCmdFirst+(WORD)m_CmdParamArray.size());
		AddDummyMenuCommand();

		//�`�����Ƃ̃��j���[��p��
		for(UINT j=0;j<m_Config.B2ECompressMenu.size();j++){
			//�`�����̃��j���[
			HMENU hTempMenu=MakePopupMenuItem();
			//HACK:�`�����Ƃ̃T�u���j���[�Ɩ��O�����Ȃ��悤�ɍ׍H����
			UtilInsertMenuItem(hB2ECompressPopup,m_Config.B2ECompressMenu[j][0]+_T(" "),j,hTempMenu,-1);//idCmdFirst+(WORD)m_CmdParamArray.size());
			//AddDummyMenuCommand();

			//�`�����Ƃ̃T�u���j���[
			for(UINT i=1;i<m_Config.B2ECompressMenu[j].size();i++){
				UtilInsertMenuItem(hTempMenu,m_Config.B2ECompressMenu[j][i],i-1,NULL,idCmdFirst+(WORD)m_CmdParamArray.size());

				CString strParam;
				strParam.Format(_T("/b2e \"/c:%s\" \"/method:%s\" "),(LPCTSTR)m_Config.B2ECompressMenu[j][0],(LPCTSTR)m_Config.B2ECompressMenu[j][i]);
				m_CmdParamArray.push_back(strParam);
			}
		}

		nOffset=1;
	}

	// �R�}���h�p�����^����ǉ��ƂȂ�T�u���j���[�R�}���h��ǉ�
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
		// �R�}���h���j���[�ǉ�
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
	// ���j���[�̕\��

	UINT subIndex=0;
	//--���k���j���[
	if(m_Config.ShellMenu.bCompress || m_Config.DragMenu.bCompress){
		subIndex+=InsertCompressMenu(hMenu,indexMenu+subIndex,idCmdFirst);
	}

	//--�𓀃��j���[
	if(m_Config.ShellMenu.bExtract){		//�E�N���b�N
		subIndex+=InsertExtractMenuRClick(hMenu,indexMenu+subIndex,idCmdFirst);
	}else if(m_Config.DragMenu.bExtract){	//�E�h���b�O
		subIndex+=InsertExtractMenuRDrag(hMenu,indexMenu+subIndex,idCmdFirst);
	}

	bool bSub=/*m_bXP && */!bInSubMenu && !m_bRightDrag;
	if(bSub){
		HMENU hOperationMenu=MakePopupMenuItem();
		int subSubIndex=0;
		//--LhaForge�ő���
		subSubIndex+=InsertOperationMenu(hOperationMenu,subSubIndex,idCmdFirst);
		//--���[�U�[��`���j���[
		subSubIndex+=InsertUserMenu(hOperationMenu,subSubIndex,idCmdFirst);

		if(subSubIndex>0){
			CString strCaption;
			UtilLoadString(IDS_MENU_CAPTION_OPERATION,strCaption);
			UtilInsertMenuItem(hMenu,strCaption,indexMenu+subIndex,hOperationMenu,idCmdFirst+(WORD)m_CmdParamArray.size());
			AddDummyMenuCommand();
			subIndex++;
		}
	}else{
		//--LhaForge�ő���
		subIndex+=InsertOperationMenu(hMenu,indexMenu+subIndex,idCmdFirst);
	}

	//-- XacRett���j���[
	if(m_Config.bXacRett){
		subIndex+=InsertXacRettMenu(hMenu,indexMenu+subIndex,idCmdFirst);
	}

	//--B2E���j���[
	if(m_Config.bB2E){
		subIndex+=InsertB2EMenu(hMenu,indexMenu+subIndex,idCmdFirst);
	}
	//--���[�U�[��`���j���[
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

	// HIWORD(lpcmi->lpVerb)��0�ł͂Ȃ��Ƃ��̂ݏ���
	// LOWORD(lpcmi->lpVerb)�̓N���b�N���ꂽ���j���[ID
	// �����QueryContextMenu()�ł� (menu ID - idCmdFirst)
	if(!HIWORD(lpcmi->lpVerb)){
		UINT idCmd = LOWORD(lpcmi->lpVerb);

		// �g�����j���[�̗L���͈͂��`�F�b�N
		if(!m_CmdParamArray.empty() && idCmd >= 0 && idCmd < m_CmdParamArray.size()){
			// �R�}���h���s
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

	//ID����:�V�X�e���ɂ�郁�j���[�A�C�e���̑��݊m�F���s����
	//http://blogs.msdn.com/oldnewthing/archive/2004/10/06/238630.aspx
	if(idCmd >= m_CmdParamArray.size()){
		if (uType == GCS_VALIDATEA || uType == GCS_VALIDATEW) {
			return S_FALSE;
		}else return E_INVALIDARG;
	}

	// �X�e�[�^�X�\������
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


//--- �V�F���g���@�R���e�N�X�g���j���[�̎���


//�I�����Ă���t�@�C�����̎��o��
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
//	PrepareMenu		���j���[�̏���
/*-------------------------------------------------------------------------*/
void CMenuExtension::PrepareMenu()
{
	//�ݒ�ǂݍ���
	m_Config.LoadConfig(m_bRightDrag,m_bForceExtraMenu);
}

/*-------------------------------------------------------------------------*/
//	DeleteMenuData			���j���[�f�[�^�̔j��
/*-------------------------------------------------------------------------*/
void CMenuExtension::DeleteMenuData()
{
	m_CmdParamArray.clear();
	//�T�u���j���[�p��
	for(UINT i=0;i<m_MenuHandleList.size();i++){
		::DestroyMenu(m_MenuHandleList[i]);
	}
	m_MenuHandleList.clear();
}

/*-------------------------------------------------------------------------*/
//	DoCommand		�I�������g�����j���[�̃t�@�C���������`�i�J���j
/*-------------------------------------------------------------------------*/
void CMenuExtension::DoCommand(UINT inIdx)
{
	// �R�}���hID�ƃR�}���h�p�����^�̗L�����̓`�F�b�N��

	dprintf(_T("DoCommand-> inIdx=%d : Param=%s\n"),inIdx,(LPCTSTR)m_CmdParamArray[inIdx]);

	// ����t�H���_�ɂ��鈳�k�v���O����(LhaForge.exe)�̃t���p�X�g�ݗ���
	TCHAR szExePath[_MAX_PATH+1];
	{
		::GetModuleFileName(g_hInstDLL, szExePath, _MAX_PATH);	//DLL�̃p�X�擾
		//DLL�̃p�X������LhaForge�̃t�@�C������g�ݗ��Ă�
		PathRemoveFileSpec(szExePath);

		PathAppend(szExePath, _T("LhaForge.exe"));
		dprintf(_T("ExePath = %s\n"),szExePath);
	}
	CString CommandLine(szExePath);

	//�R�}���h���C���p�����[�^��ݒ�
	CommandLine+=_T(" ");
	CommandLine+=m_CmdParamArray[inIdx];	//�t�@�C�������I�v�V����
	//�����Ɉ��k/��
	if(m_bRightDrag){
		CommandLine+=_T(" /o:");
		CommandLine+=m_strTargetFolder;//���ɕK�v�ȏꍇ��""�Ŋ����Ă���
	}else{
		if(-1!=CommandLine.Find(OUTPUTDIR_DESKTOP)){
			//�f�X�N�g�b�v�ɉ�
			TCHAR Buffer[_MAX_PATH+1]={0};
			SHGetSpecialFolderPath(NULL,Buffer,CSIDL_DESKTOPDIRECTORY,FALSE);
			PathQuoteSpaces(Buffer);
			CommandLine.Replace(OUTPUTDIR_DESKTOP,Buffer);
		}else if(-1!=CommandLine.Find(OUTPUTDIR_SAMEDIR)){
			//�����ꏊ�ɉ�
			CPath strPath(m_SelFileArray[0]);
			strPath.RemoveFileSpec();
			strPath.QuoteSpaces();
			CommandLine.Replace(OUTPUTDIR_SAMEDIR,strPath);
		}else if(-1!=CommandLine.Find(OUTPUTDIR_SPECIFIED)){
			//�w��ꏊ�ɉ�
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

	//�t�@�C���������Ԃɏ���
	if(m_SelFileArray.size()>16){
		//�t�@�C���������̂Ń��X�|���X�t�@�C�����g��(�����ɍ�������)
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
		//���X�|���X�t�@�C���̎w��

#if defined(UNICODE)||defined(_UNICODE)
		//�R�[�h�y�[�W�w��
		CommandLine+=_T(" /cp:UNICODE");
#endif

		CommandLine+=_T(" \"/$");
		CommandLine+=szTempPath;
		CommandLine+=_T("\"");
	}else{	//���̂܂܃R�}���h���C���ɓn��
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

	// �N���v���Z�X�̕\���E�B���h�E�̐ݒ�
	STARTUPINFO theStartupInfo = {sizeof(STARTUPINFO)};
	theStartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	theStartupInfo.wShowWindow = SW_SHOWDEFAULT;

	// �v���Z�X�N���i�R�}���h���s�j
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

		//�G���[�X�g�����O�擾
		{
			TCHAR szBuffer[1024];
			FormatMessage(
				FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // �f�t�H���g����
				szBuffer, 1024, NULL);
			Message+=szBuffer;
		}
		{
			CString title;
			UtilLoadString(IDS_PROGRAM_NAME,title);
			MessageBox(NULL,Message,title,MB_OK|MB_ICONSTOP);
		}
	}

	//�s�v�ɂȂ����v���Z�X�n���h��������
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}


