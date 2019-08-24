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

	BOOL bXacRett;		//XacRett���j���[��L���ɂ���Ȃ�true
	BOOL bB2E;			//B2E���j���[��L���ɂ���Ȃ�true
	BOOL bRightDrag;	//�E�h���b�O���j���[�Ȃ�true
	BOOL bExtraMenu;	//�g�����j���[��\������Ȃ�true
	BOOL bCustomCompress;	//�J�X�^�}�C�Y�������k���j���[���g���Ȃ�true
	TCHAR szCustomCompress[_MAX_PATH+1];	//�J�X�^�}�C�Y�������k���j���[�̃f�[�^
	BOOL bUnderSubMenu;	//�uLhaForge�v�T�u���j���[�ȉ��ɕ��荞��

	std::vector<CUserMenuItem> UserMenuArray;	//���[�U�[��`���j���[�̃f�[�^
	std::vector<std::vector<CString> > B2ECompressMenu;	//B2E���j���[�̂������k���j���[�̓��e

	void LoadConfig(BOOL bRightDrag,BOOL bForceExtra);
	void GetDefaultFilePath(CPath &strIniPath,LPCTSTR lpszFile);	//�W���̐ݒ�t�@�C���̃p�X���Z�b�g

	CConfigManager(){}
	virtual ~CConfigManager(){}
};


