#include "stdafx.h"
#include "resource.h"
#include "EditForm.h"
#include "lfutil.h"

LRESULT CFormView::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	//���X�g�r���[(�ǉ��ς�)
	List_MenuAdded=GetDlgItem(IDC_LIST_MENU_ADDED);
	//�J�����ǉ�
	CRect Rect;
	List_MenuAdded.GetClientRect(Rect);
	List_MenuAdded.InsertColumn(0, _T("Name"),LVCFMT_LEFT,Rect.Width());

	//���X�g�r���[(��)
	List_MenuSource=GetDlgItem(IDC_LIST_MENU_SOURCE);
	//�J�����ǉ�
	List_MenuAdded.GetClientRect(Rect);
	List_MenuSource.InsertColumn(0, _T("Name"),LVCFMT_LEFT,Rect.Width());

	//���X�g�r���[(���[�U�[��`���j���[)
	List_UserMenu=GetDlgItem(IDC_LIST_USERMENU);
	//�J�����ǉ�
	List_UserMenu.GetClientRect(Rect);
	List_UserMenu.InsertColumn(0, _T("Name"),LVCFMT_LEFT,Rect.Width());

	Check_Shell=GetDlgItem(IDC_CHECK_USERMENU_SHELL);
	Check_Drag=GetDlgItem(IDC_CHECK_USERMENU_DRAG);

	Edit_UserMenuCaption=GetDlgItem(IDC_EDIT_USERMENU_CAPTION);
	Edit_UserMenuCaption.SetLimitText(_MAX_PATH);
	Edit_UserMenuParam=GetDlgItem(IDC_EDIT_USERMENU_PARAM);
	Edit_UserMenuParam.SetLimitText(_MAX_PATH);

	// INI�̃t���p�X�g�ݗ���
	SetDefaultIniPath();

	//���j���[�L���v�V�����̃L���b�V��
	for(int i=0;i<COUNTOF(g_UserMenuTable);i++){
		m_MenuCaptionArray.push_back(CString(MAKEINTRESOURCE(g_UserMenuTable[i].lpMenuItem->Caption)));
	}

	//�ǉ��ς݃��j���[�̍\�z
	ConstructMenuFromSetting();
	//�\�[�X���j���[�̍\�z
	List_MenuSource.SetItemCount(COUNTOF(g_UserMenuTable));
	return TRUE;
}

//���z���X�g�r���[
LRESULT CFormView::OnGetDispInfo(LPNMHDR pnmh)
{
	LV_DISPINFO* pstLVDInfo=(LV_DISPINFO*)pnmh;
	//�͈̓`�F�b�N
	const int Index=pstLVDInfo->item.iItem;
	if(Index<0)return FALSE;
	if(!(pstLVDInfo->item.mask & LVIF_TEXT))return FALSE;

	if(pstLVDInfo->item.iSubItem!=0)return FALSE;


	if(pnmh->hwndFrom==List_UserMenu){
		//���[�U�[��`���j���[
		if(Index>=(signed)m_UserMenuArray.size())return FALSE;
		_tcsncpy_s(pstLVDInfo->item.pszText,pstLVDInfo->item.cchTextMax, m_UserMenuArray[Index].Caption,pstLVDInfo->item.cchTextMax);
	}
	else{
		if(pnmh->hwndFrom==List_MenuAdded){
			//�ǉ����ꂽ���j���[
			if(Index>=(signed)m_AddedMenuArray.size())return FALSE;
			_tcsncpy_s(pstLVDInfo->item.pszText,pstLVDInfo->item.cchTextMax, m_MenuCaptionArray[m_AddedMenuArray[Index]],pstLVDInfo->item.cchTextMax);
		}
		else{
			//�\�[�X
			if(Index>=COUNTOF(g_UserMenuTable))return FALSE;
			_tcsncpy_s(pstLVDInfo->item.pszText,pstLVDInfo->item.cchTextMax, m_MenuCaptionArray[Index],pstLVDInfo->item.cchTextMax);
		}
	}
	return TRUE;
}

void CFormView::ClearMenuData()
{
	m_AddedMenuArray.clear();
	List_MenuAdded.SetItemCount(0);
}

bool CFormView::ConstructMenuFromSetting()
{
	//========================
	// �\�����郁�j���[�̑I��
	//========================
	TCHAR Buffer[_MAX_PATH+1];
	GetPrivateProfileString(_T("CustomMenu"),_T("Arrange"),USER_MENU_DEFAULT,Buffer,_MAX_PATH,m_strIniPath);

	bool bRet=ConstructMenu(Buffer);

	//------------------
	//���[�U�[��`���j���[�f�[�^
	for(UINT iIndex=0;;iIndex++){
		CString strSectionName;
		strSectionName.Format(_T("UserMenu%d"),iIndex);
		if(!UtilCheckINISectionExists(strSectionName,m_strIniPath)){
			break;
		}
		CUserMenuItem umi={0};
		//�p�����[�^
		GetPrivateProfileString(strSectionName,_T("Param"),strSectionName,umi.Param,COUNTOF(umi.Param)-1,m_strIniPath);
		//�L���v�V����
		GetPrivateProfileString(strSectionName,_T("Caption"),_T(""),umi.Caption,COUNTOF(umi.Caption)-1,m_strIniPath);
		//�E�N���b�N���j���[
		umi.bShellMenu=GetPrivateProfileInt(strSectionName,_T("ShellMenu"),TRUE,m_strIniPath);
		//�E�h���b�O���j���[
		umi.bDragMenu=GetPrivateProfileInt(strSectionName,_T("DragMenu"),TRUE,m_strIniPath);
		m_UserMenuArray.push_back(umi);
	}
	List_UserMenu.SetItemCount(m_UserMenuArray.size());

	return bRet;
}

//�����񂩂烁�j���[�̏�Ԃ��\������
bool CFormView::ConstructMenu(LPCTSTR lpString)
{
	ClearMenuData();

	LPCTSTR lpChar=lpString;

	bool bRet=true;
	for(;*lpChar!=_T('\0');lpChar++){
		int i;
		for(i=0;i<COUNTOF(g_UserMenuTable);i++){
			if(g_UserMenuTable[i].Identifier==*lpChar){	//��v
				break;
			}
		}
		if(i==COUNTOF(g_UserMenuTable)){	//��v���镨���Ȃ��A�܂�s���ȃ��j���[�w��
			MessageBeep(MB_ICONEXCLAMATION);
			bRet=false;
			continue;
		}

		//���j���[�A�C�e����ǉ�
		m_AddedMenuArray.push_back(i);
	}
	List_MenuAdded.SetItemCount(m_AddedMenuArray.size());
	return bRet;
}

void CFormView::StoreMenuData()
{
	//---���k���j���[
	CString Buffer;
	for(unsigned int i=0;i<m_AddedMenuArray.size();i++){
		int iIndex=m_AddedMenuArray[i];
		ATLASSERT(COUNTOF(g_UserMenuTable)>iIndex&&iIndex>=0);
		Buffer+=g_UserMenuTable[iIndex].Identifier;
	}

	WritePrivateProfileString(_T("CustomMenu"),_T("Arrange"),Buffer,m_strIniPath);

	//---���[�U�[��`���j���[
	UINT iIndex=0;
	UINT OldDataCount=0;
	//---�Â��f�[�^�̐����m�F
	for(;;iIndex++){
		CString strSectionName;
		strSectionName.Format(_T("UserMenu%d"),iIndex);
		if(!UtilCheckINISectionExists(strSectionName,m_strIniPath)){
			OldDataCount=iIndex;
			break;
		}
	}
	//---�f�[�^�̏㏑��
	for(iIndex=0;iIndex<m_UserMenuArray.size();iIndex++){
		CString strSectionName;
		strSectionName.Format(_T("UserMenu%d"),iIndex);
		const CUserMenuItem &umi=m_UserMenuArray[iIndex];
		//�p�����[�^
		WritePrivateProfileString(strSectionName,_T("Param"),umi.Param,m_strIniPath);
		//�L���v�V����
		WritePrivateProfileString(strSectionName,_T("Caption"),umi.Caption,m_strIniPath);
		//�E�N���b�N���j���[
		WritePrivateProfileString(strSectionName,_T("ShellMenu"),umi.bShellMenu?_T("1"):_T("0"),m_strIniPath);
		//�E�h���b�O���j���[
		WritePrivateProfileString(strSectionName,_T("DragMenu"),umi.bDragMenu?_T("1"):_T("0"),m_strIniPath);
	}
	//---�Â��Z�N�V�����̔j��
	for(;iIndex<OldDataCount;iIndex++){
		CString strSectionName;
		strSectionName.Format(_T("UserMenu%d"),iIndex);
		WritePrivateProfileString(strSectionName,NULL,NULL,m_strIniPath);
	}
}

void CFormView::OnOK(UINT,int,HWND)
{
	GetUserMenuData();
	StoreMenuData();
	::PostMessage(GetParent(),WM_CLOSE,NULL,NULL);
}

void CFormView::OnCancel(UINT,int,HWND)
{
	::PostMessage(GetParent(),WM_CLOSE,NULL,NULL);
}

void CFormView::OnReset(UINT,int,HWND)
{
	ConstructMenu(USER_MENU_DEFAULT);
}

void CFormView::OnMoveUp(UINT,int nID,HWND)
{
	if(nID==IDC_BUTTON_MOVEUP){
		//���k���j���[
		int iItem=List_MenuAdded.GetNextItem(-1,LVNI_ALL|LVNI_SELECTED);
		if(-1==iItem||iItem>=(signed)m_AddedMenuArray.size()){
			MessageBeep(MB_ICONASTERISK);
			return;
		}
		if(0==iItem){
			MessageBeep(MB_ICONASTERISK);
			return;
		}

		int iTemp=m_AddedMenuArray[iItem];
		m_AddedMenuArray[iItem]=m_AddedMenuArray[iItem-1];
		m_AddedMenuArray[iItem-1]=iTemp;

		List_MenuAdded.EnsureVisible(iItem-1,FALSE);
		List_MenuAdded.SetItemState(iItem-1,LVIS_SELECTED, LVIS_SELECTED);
	}else{
		//���[�U�[��`���j���[
		int iItem=List_UserMenu.GetNextItem(-1,LVNI_ALL|LVNI_SELECTED);
		if(-1==iItem||iItem>=(signed)m_UserMenuArray.size()){
			MessageBeep(MB_ICONASTERISK);
			return;
		}
		if(0==iItem){
			MessageBeep(MB_ICONASTERISK);
			return;
		}

		GetUserMenuData();
		CUserMenuItem umi=m_UserMenuArray[iItem];
		m_UserMenuArray[iItem]=m_UserMenuArray[iItem-1];
		m_UserMenuArray[iItem-1]=umi;

		SetUserMenuData(&m_UserMenuArray[iItem-1]);
		List_UserMenu.EnsureVisible(iItem-1,FALSE);
		List_UserMenu.SetItemState(iItem-1,LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CFormView::OnMoveDown(UINT,int nID,HWND)
{
	if(nID==IDC_BUTTON_MOVEDOWN){
		//���k���j���[
		int iItem=List_MenuAdded.GetNextItem(-1,LVNI_ALL|LVNI_SELECTED);
		if(-1==iItem||iItem>=(signed)m_AddedMenuArray.size()){
			MessageBeep(MB_ICONASTERISK);
			return;
		}
		if((signed)m_AddedMenuArray.size()-1==iItem){
			MessageBeep(MB_ICONASTERISK);
			return;
		}

		int iTemp=m_AddedMenuArray[iItem];
		m_AddedMenuArray[iItem]=m_AddedMenuArray[iItem+1];
		m_AddedMenuArray[iItem+1]=iTemp;

		List_MenuAdded.EnsureVisible(iItem+1,FALSE);
		List_MenuAdded.SetItemState(iItem+1,LVIS_SELECTED, LVIS_SELECTED);
	}else{
		//���[�U�[��`���j���[
		int iItem=List_UserMenu.GetNextItem(-1,LVNI_ALL|LVNI_SELECTED);
		if(-1==iItem||iItem>=(signed)m_UserMenuArray.size()){
			MessageBeep(MB_ICONASTERISK);
			return;
		}
		if((signed)m_UserMenuArray.size()-1==iItem){
			MessageBeep(MB_ICONASTERISK);
			return;
		}

		GetUserMenuData();
		CUserMenuItem umi=m_UserMenuArray[iItem];
		m_UserMenuArray[iItem]=m_UserMenuArray[iItem+1];
		m_UserMenuArray[iItem+1]=umi;

		SetUserMenuData(&m_UserMenuArray[iItem+1]);
		List_UserMenu.EnsureVisible(iItem+1,FALSE);
		List_UserMenu.SetItemState(iItem+1,LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CFormView::OnAdd(UINT,int nID,HWND)
{
	if(nID==IDC_BUTTON_ADD){
		//���k���j���[
		int iItem=List_MenuSource.GetNextItem(-1,LVNI_ALL|LVNI_SELECTED);
		if(-1==iItem||iItem>=COUNTOF(g_UserMenuTable)){
			MessageBeep(MB_ICONASTERISK);
			return;
		}

		m_AddedMenuArray.push_back(iItem);

		List_MenuAdded.SetItemCount(m_AddedMenuArray.size());
		List_MenuAdded.EnsureVisible(m_AddedMenuArray.size()-1,FALSE);
		List_MenuAdded.SetItemState(m_AddedMenuArray.size()-1,LVIS_SELECTED, LVIS_SELECTED);
	}else{
		GetUserMenuData();
		m_lpUserMenuItem=NULL;
		//���[�U�[��`���j���[
		CUserMenuItem umi={0};
		_tcsncpy_s(umi.Caption,_T("UserMenu"),COUNTOF(umi.Caption)-1);
		umi.bDragMenu=TRUE;
		umi.bShellMenu=TRUE;

		m_UserMenuArray.push_back(umi);

		SetUserMenuData(&m_UserMenuArray.back());
		List_UserMenu.SetItemCount(m_UserMenuArray.size());
		List_UserMenu.EnsureVisible(m_UserMenuArray.size()-1,FALSE);
		List_UserMenu.SetItemState(m_UserMenuArray.size()-1,LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CFormView::OnDelete(UINT,int nID,HWND)
{
	if(nID==IDC_BUTTON_DELETE){
		//���k���j���[
		int iItem=List_MenuAdded.GetNextItem(-1,LVNI_ALL|LVNI_SELECTED);
		if(-1==iItem||iItem>=(signed)m_AddedMenuArray.size()){
			MessageBeep(MB_ICONASTERISK);
			return;
		}

		std::vector<int>::iterator ite=m_AddedMenuArray.begin();
		ite+=iItem;
		m_AddedMenuArray.erase(ite);

		List_MenuAdded.SetItemCount(m_AddedMenuArray.size());
	}else{
		//���[�U�[��`���j���[
		int iItem=List_UserMenu.GetNextItem(-1,LVNI_ALL|LVNI_SELECTED);
		if(-1==iItem||iItem>=(signed)m_UserMenuArray.size()){
			MessageBeep(MB_ICONASTERISK);
			return;
		}

		std::vector<CUserMenuItem>::iterator ite=m_UserMenuArray.begin();
		ite+=iItem;
		m_UserMenuArray.erase(ite);
		List_UserMenu.SetItemCount(m_UserMenuArray.size());
		if(m_UserMenuArray.empty()){
			m_lpUserMenuItem=NULL;
			return;
		}
		if(iItem!=0)iItem--;
		SetUserMenuData(&m_UserMenuArray[iItem]);

		List_UserMenu.EnsureVisible(iItem,FALSE);
		List_UserMenu.SetItemState(iItem,LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CFormView::OnVersion(UINT,int,HWND)
{
	CSimpleDialog<IDD_DIALOG_VERSION> dlg;
	dlg.DoModal();
}

void CFormView::SetUserMenuData(CUserMenuItem *lpUserMenuItem)
{
	m_lpUserMenuItem=lpUserMenuItem;
	if(m_lpUserMenuItem){
		Edit_UserMenuParam.SetWindowText(m_lpUserMenuItem->Param);
		Edit_UserMenuCaption.SetWindowText(m_lpUserMenuItem->Caption);
		Check_Shell.SetCheck(m_lpUserMenuItem->bShellMenu);
		Check_Drag.SetCheck(m_lpUserMenuItem->bDragMenu);
	}
}

void CFormView::GetUserMenuData()
{
	if(m_lpUserMenuItem){
		Edit_UserMenuParam.GetWindowText(m_lpUserMenuItem->Param,COUNTOF(m_lpUserMenuItem->Param)-1);
		Edit_UserMenuCaption.GetWindowText(m_lpUserMenuItem->Caption,COUNTOF(m_lpUserMenuItem->Caption)-1);
		m_lpUserMenuItem->bShellMenu=Check_Shell.GetCheck();
		m_lpUserMenuItem->bDragMenu=Check_Drag.GetCheck();
	}
}

//�A�C�e���I��ύX
LRESULT CFormView::OnSelect(LPNMHDR pnmh)
{
	if(pnmh->hwndFrom==List_UserMenu){
		GetUserMenuData();

		int iItem=List_UserMenu.GetNextItem(-1,LVNI_ALL|LVNI_SELECTED);
		if(-1!=iItem&&iItem<(signed)m_UserMenuArray.size()){
			SetUserMenuData(&m_UserMenuArray[iItem]);
		}
	}
	return 0;
}

void CFormView::SetDefaultIniPath()	//�W���̐ݒ�t�@�C���̃p�X���Z�b�g
{
	const LPCTSTR PROGRAMDIR_NAME=_T("LhaForge");	//ApplicationData�ɓ����Ƃ��ɕK�v�ȃf�B���N�g����
	const LPCTSTR INI_FILE_NAME=_T("LhaForge.ini");

	//---���[�U�[�Ԃŋ��ʂ̐ݒ��p����
	//LhaForge�t�H���_�Ɠ����ꏊ��INI������Ύg�p����
	{
		TCHAR szPath[_MAX_PATH+1]={0};
		::GetModuleFileName(NULL, szPath, _MAX_PATH);	//�����̃p�X�擾
		m_strIniPath=szPath;
		m_strIniPath.RemoveFileSpec();
		m_strIniPath.Append(INI_FILE_NAME);
		if(m_strIniPath.FileExists()){
			//���ʐݒ�
			return;
		}
	}
	//CSIDL_COMMON_APPDATA��INI������Ύg�p����
	{
		TCHAR szPath[_MAX_PATH+1]={0};
		SHGetFolderPath(NULL,CSIDL_COMMON_APPDATA|CSIDL_FLAG_CREATE,NULL,SHGFP_TYPE_CURRENT,szPath);
		m_strIniPath=szPath;
		m_strIniPath.Append(PROGRAMDIR_NAME);
		m_strIniPath.Append(INI_FILE_NAME);
		if(m_strIniPath.FileExists()){
			//���ʐݒ�
			return;
		}
	}

	//--------------------

	//---���[�U�[�ʐݒ��p����
	//LhaForge�C���X�g�[���t�H���_�ȉ��Ƀt�@�C�������݂���ꍇ�A������g�p
	{
		//���[�U�[���擾
		TCHAR UserName[UNLEN+1]={0};
		DWORD Length=UNLEN;
		GetUserName(UserName,&Length);

		TCHAR szPath[_MAX_PATH+1]={0};
		::GetModuleFileName(NULL, szPath, _MAX_PATH);	//�����̃p�X�擾
		m_strIniPath=szPath;
		m_strIniPath.RemoveFileSpec();
		m_strIniPath.Append(UserName);
		m_strIniPath.Append(INI_FILE_NAME);

		if(m_strIniPath.FileExists()){
			return;
		}
	}
	//---�f�t�H���g
	//CSIDL_APPDATA��INI������Ύg�p����:Vista�ł͂���ȊO�̓A�N�Z�X�����s���ɂȂ�\��������
	TCHAR szPath[_MAX_PATH+1]={0};
	SHGetFolderPath(NULL,CSIDL_APPDATA|CSIDL_FLAG_CREATE,NULL,SHGFP_TYPE_CURRENT,szPath);
	m_strIniPath=szPath;
	m_strIniPath.Append(PROGRAMDIR_NAME);
	m_strIniPath.AddBackslash();
	UtilMakeSureDirectoryPathExists(m_strIniPath);
	m_strIniPath.Append(INI_FILE_NAME);
	return;
}
