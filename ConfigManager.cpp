#include "stdafx.h"
#include "resource.h"
#include "MenuData.h"
#include "ConfigManager.h"
#include "main.h"
#include "lfutil.h"

void CConfigManager::LoadConfig(BOOL bRightDrag,BOOL bForceExtra)
{
	//�ϐ�������
	ZeroMemory(&ShellMenu,sizeof(ShellMenu));
	ZeroMemory(&DragMenu,sizeof(DragMenu));
	bUnderSubMenu=false;

	CPath strIniPath;
	GetDefaultFilePath(strIniPath,_T("LhaForge.ini"));

	dprintf(_T("Common-> IniPath=%s\n"),(LPCTSTR)strIniPath);

//---------------------------------------------------
	//�g�����j���[�𗘗p���邩�ǂ���
	bExtraMenu = bForceExtra || GetPrivateProfileInt(_T("ExtraMenu"),_T("ForceExtraMenu"),0,strIniPath);

	if(bRightDrag){
		//--------------------
		// �E�h���b�O���j���[
		//--------------------
		//���k
		DragMenu.bCompress=GetPrivateProfileInt(_T("DragMenu"),_T("Compress"),1,strIniPath);
		//���k
		DragMenu.bExtract=GetPrivateProfileInt(_T("DragMenu"),_T("Extract"),1,strIniPath);
		//------------------------------
		// �T�u���j���[�ȉ��ɕ��荞�ށH
		//------------------------------
		//�L���E����
		bUnderSubMenu=GetPrivateProfileInt(_T("DragMenu"),_T("UnderSubMenu"),0,strIniPath);
	}else{
		//--------------------
		// �E�N���b�N���j���[
		//--------------------
		//���k
		ShellMenu.bCompress=GetPrivateProfileInt(_T("ShellMenu"),_T("Compress"),1,strIniPath);
		//��
		ShellMenu.bExtract=GetPrivateProfileInt(_T("ShellMenu"),_T("Extract"),1,strIniPath);
		//�{��
		ShellMenu.bList=GetPrivateProfileInt(_T("ShellMenu"),_T("List"),1,strIniPath);
		//���Ɍ���
		ShellMenu.bTestArchive=GetPrivateProfileInt(_T("ShellMenu"),_T("Test"),1,strIniPath);
		//------------------------------
		// �T�u���j���[�ȉ��ɕ��荞�ށH
		//------------------------------
		//�L���E����
		bUnderSubMenu=GetPrivateProfileInt(_T("ShellMenu"),_T("UnderSubMenu"),0,strIniPath);
	}

	if(!bExtraMenu&&(ShellMenu.bCompress||DragMenu.bCompress)){
		//�J�X�^�����j���[
		bCustomCompress=GetPrivateProfileInt(_T("CustomMenu"),_T("UseCustom"),0,strIniPath);
		if(bCustomCompress){	//�J�X�^�}�C�Y���ꂽ���j���[���g��
			GetPrivateProfileString(_T("CustomMenu"),_T("Arrange"),USER_MENU_DEFAULT,szCustomCompress,_MAX_PATH,strIniPath);
		}
	}

	//-----------------
	// XacRett���j���[
	//-----------------
	//�L���E����
	bXacRett=GetPrivateProfileInt(_T("XacRett"),_T("EnableShellMenu"),0,strIniPath);
	//-----------------
	// B2E���j���[
	//-----------------
	//�L���E����
	bB2E=GetPrivateProfileInt(_T("B2E"),_T("EnableShellMenu"),0,strIniPath);

	//----------------------
	// ���[�U�[��`���j���[
	//----------------------
	//�L���E����
	UserMenuArray.clear();
	UINT iIndex=0;
	for(;;iIndex++){
		CString strSectionName;
		strSectionName.Format(_T("UserMenu%d"),iIndex);
		if(!UtilCheckINISectionExists(strSectionName,strIniPath)){
			break;
		}
		CUserMenuItem umi={0};
		//�L���v�V����
		GetPrivateProfileString(strSectionName,_T("Caption"),strSectionName,umi.Caption,COUNTOF(umi.Caption)-1,strIniPath);
		//�p�����[�^
		GetPrivateProfileString(strSectionName,_T("Param"),_T(""),umi.Param,COUNTOF(umi.Param)-1,strIniPath);
		//�E�N���b�N�ŕ\��?
		umi.bShellMenu=GetPrivateProfileInt(strSectionName,_T("ShellMenu"),TRUE,strIniPath);
		//�E�h���b�O�ŕ\���H
		umi.bDragMenu=GetPrivateProfileInt(strSectionName,_T("DragMenu"),TRUE,strIniPath);
		//�\�����鎞�������[�h
		if( (!bRightDrag && umi.bShellMenu) || (bRightDrag && umi.bDragMenu)){
			UserMenuArray.push_back(umi);
		}
	}

	//B2E���k���j���[��ǂݍ���
	CPath strPath;
	B2ECompressMenu.clear();
	GetDefaultFilePath(strPath,_T("B2EMenu.dat"));
	if(!UtilReadMenuDefinitionFile(strPath,B2ECompressMenu)){
		B2ECompressMenu.clear();
	}
}

void CConfigManager::GetDefaultFilePath(CPath &strIniPath,LPCTSTR lpszFile)	//�W���̐ݒ�t�@�C���̃p�X���Z�b�g
{
	const LPCTSTR PROGRAMDIR_NAME=_T("LhaForge");	//ApplicationData�ɓ����Ƃ��ɕK�v�ȃf�B���N�g����
	//const LPCTSTR INI_FILE_NAME=_T("LhaForge.ini");

	//---���[�U�[�Ԃŋ��ʂ̐ݒ��p����
	//LhaForge�t�H���_�Ɠ����ꏊ��INI������Ύg�p����
	{
		TCHAR szPath[_MAX_PATH+1]={0};
		::GetModuleFileName(g_hInstDLL, szPath, _MAX_PATH);	//�����̃p�X�擾
		strIniPath=szPath;
		strIniPath.RemoveFileSpec();
		strIniPath.Append(lpszFile);
		if(strIniPath.FileExists()){
			//���ʐݒ�
			return;
		}
	}
	//CSIDL_COMMON_APPDATA��INI������Ύg�p����
	{
		TCHAR szPath[_MAX_PATH+1]={0};
		SHGetFolderPath(NULL,CSIDL_COMMON_APPDATA|CSIDL_FLAG_CREATE,NULL,SHGFP_TYPE_CURRENT,szPath);
		strIniPath=szPath;
		strIniPath.Append(PROGRAMDIR_NAME);
		strIniPath.Append(lpszFile);
		if(strIniPath.FileExists()){
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
		::GetModuleFileName(g_hInstDLL, szPath, _MAX_PATH);	//�����̃p�X�擾
		strIniPath=szPath;
		strIniPath.RemoveFileSpec();
		strIniPath.Append(UserName);
		strIniPath.Append(lpszFile);

		if(strIniPath.FileExists()){
			return;
		}
	}
	//---�f�t�H���g
	//CSIDL_APPDATA��INI������Ύg�p����:Vista�ł͂���ȊO�̓A�N�Z�X�����s���ɂȂ�\��������
	TCHAR szPath[_MAX_PATH+1]={0};
	SHGetFolderPath(NULL,CSIDL_APPDATA|CSIDL_FLAG_CREATE,NULL,SHGFP_TYPE_CURRENT,szPath);
	strIniPath=szPath;
	strIniPath.Append(PROGRAMDIR_NAME);
	strIniPath.AddBackslash();
	strIniPath.Append(lpszFile);
	return;
}
