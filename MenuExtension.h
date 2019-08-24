/////////////////////////////////////////////////////////////////////////////
//
//	Modified by Claybird <claybird.without.wing@gmail.com>
//
// MenuExtension.h :�g���V�F�� �R���e�N�X�g���j���[�R���|�[�l���g�N���X�̃w�b�_
//					LhaForge�g�����j���[
//
//	Original File Information:
// MenuExtension.h :�g���V�F�� �R���e�N�X�g���j���[�R���|�[�l���g�N���X�̃w�b�_
//					���l����(TF)�g�����j���[
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ConfigManager.h"

const int PARAM_MAX_LEN=256;


// �R���e�N�X�g���j���[�R���|�[�l���g�̍쐬�iIShellExtInit,IContextMenu�𑽏d�p������j
class CMenuExtension : public IShellExtInit,public IContextMenu
{
protected:
	//bool			m_bXP;					//WindowsXP�������͂���ȍ~�Ȃ�true
	LONG			m_cRef;					// �I�u�W�F�N�g�̎Q�ƃJ�E���g
	bool			m_bMenuAdded;			//���j���[�ǉ��ς݂Ȃ�true

	std::vector<CString> m_SelFileArray;	// �I���t�@�C�����e�[�u��
	CString			m_strTargetFolder;			//�h���b�O��̃t�H���_

	std::vector<CString> m_CmdParamArray;	//�R�}���h�p�����[�^�̔z��;���j���[ID��1��1�Ή�

	CConfigManager	m_Config;
	bool			m_bRightDrag;				//�E�h���b�O���ǂ���;CLSID�ŕ��ނ��AClassFactory�Ō��܂�
	bool			m_bForceExtraMenu;			//�g�����j���[(SHIFT+�E�N���b�N/�h���b�O)���ǂ���
	std::vector<HMENU>	m_MenuHandleList;		//�|�b�v�A�b�v���j���[�̃n���h���Ǘ�
protected:
	void PrepareMenu();			// ���j���[�̏���
	bool GetSelectedFiles(LPDATAOBJECT pDataObj);			// �I���t�@�C�������o��
	void DeleteMenuData();		// ���j���[�f�[�^�̔j��
	void DoCommand(UINT inIdx);	// �I�������g�����j���[�̃R�}���h���s

	HMENU MakePopupMenuItem();
	void AddDummyMenuCommand(){m_CmdParamArray.push_back(_T(""));}

	void BuildExtraMenu(HMENU hParentMenu,UINT idCmdFirst,int menuIdx,const SHELL_MENU_ITEM* pMenu,size_t numItems);
	void InsertCommandMenuItems(HMENU hParentMenu,UINT idCmdFirst,const SHELL_MENU_ITEM* pMenu,size_t numItems,int nOffset);

	UINT InsertContextMenu(HMENU hMenu,UINT indexMenu,UINT idCmdFirst,BOOL bInSubMenu);	//���j���[��ǉ�

	UINT InsertCompressMenu(HMENU hMenu,UINT indexMenu,UINT idCmdFirst);	//indexMenu+(�߂�l)������indexMenu
	UINT InsertExtractMenuRClick(HMENU hMenu,UINT indexMenu,UINT idCmdFirst);
	UINT InsertExtractMenuRDrag(HMENU hMenu,UINT indexMenu,UINT idCmdFirst);
	UINT InsertOperationMenu(HMENU hMenu,UINT indexMenu,UINT idCmdFirst);
	UINT InsertXacRettMenu(HMENU hMenu,UINT indexMenu,UINT idCmdFirst);
	UINT InsertB2EMenu(HMENU hMenu,UINT indexMenu,UINT idCmdFirst);
	UINT InsertUserMenu(HMENU hMenu,UINT indexMenu,UINT idCmdFirst);
public:
	// �R���X�g���N�^�E�f�X�g���N�^
	CMenuExtension(bool);
	virtual ~CMenuExtension();

	// IUnknown �C���^�[�t�F�C�X�̃��\�b�h
	STDMETHODIMP            QueryInterface(REFIID, LPVOID *);
	STDMETHODIMP_(ULONG)    AddRef();
	STDMETHODIMP_(ULONG)    Release();

	// IShellExtInit �C���^�[�t�F�C�X�̃��\�b�h
	STDMETHODIMP Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hKeyID);

	// IContextMenu �C���^�[�t�F�C�X�̃��\�b�h
	STDMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst,UINT idCmdLast, UINT uFlags);
	STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi);
	STDMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pwReserved,LPSTR pszName, UINT cchMax);
};


