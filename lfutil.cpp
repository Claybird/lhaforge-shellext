#include "stdafx.h"
#include "lfutil.h"

bool UtilInsertMenuItem(HMENU hMenu,LPCTSTR lpszCaption,int indexMenu,HMENU hSubMenu,WORD wID)
{
	ASSERT(IsMenu(hMenu));
	MENUITEMINFO mii={0};
	mii.cbSize = sizeof(mii);

	if(lpszCaption){
		if(hSubMenu){
			//�T�u���j���[�t
			mii.fMask = MIIM_SUBMENU | MIIM_STRING;
			mii.hSubMenu=hSubMenu;
		}else{
			//�P�ƍ���
			mii.fMask = MIIM_STRING;
		}

		if(wID!=(WORD)-1){
			mii.fMask |= MIIM_ID;
			mii.wID = wID;
		}

		mii.dwTypeData=const_cast<LPTSTR>(lpszCaption);
		mii.cch=(UINT)_tcslen(lpszCaption);

		//TRUE:indexMenu�͈ʒu��\�킷
		if(::InsertMenuItem(hMenu, indexMenu, TRUE,&mii)){
			return true;
		}

		//�}���Ɏ��s����
		//MIIM_STRING���O���AMFT_STRING��ǉ�����
		mii.fMask&=~MIIM_STRING;
		mii.fMask|=MIIM_FTYPE;	//MIIM_TYPE
		mii.fType=MFT_STRING;

		if(::InsertMenuItem(hMenu, indexMenu, TRUE,&mii)){
			return true;
		}
		return false;
	}else{
	//----------------------------------------
	//Separator��ǉ��̏ꍇ�AlpszCaption��NULL
	//----------------------------------------
		mii.fMask = MIIM_TYPE;
		mii.fType=MFT_SEPARATOR;
		if(wID!=(WORD)-1){
			mii.fMask |= MIIM_ID;
			mii.wID = wID;
		}
		if(::InsertMenuItem(hMenu, indexMenu, TRUE,&mii)){
			return true;
		}else{
			mii.fMask = MIIM_FTYPE;
			mii.fType=MFT_SEPARATOR;
			if(wID!=(WORD)-1){
				mii.fMask |= MIIM_ID;
				mii.wID = wID;
			}
			if(::InsertMenuItem(hMenu, indexMenu, TRUE,&mii)){
				return true;
			}
			else{
				return false;
			}
		}
	}
}

//�t�@�C���Ɉ�s������
bool UtilWriteLine(HANDLE hFile,LPCTSTR lpszText)
{
	if(INVALID_HANDLE_VALUE==hFile)return false;
	DWORD dwWritten;
	if(!WriteFile(hFile,lpszText,(DWORD)(_tcslen(lpszText)*sizeof(lpszText[0])),&dwWritten,NULL))return false;
	TCHAR CRLF[]=_T("\r\n");
	if(!WriteFile(hFile,CRLF,DWORD(_tcslen(CRLF)*sizeof(CRLF[0])),&dwWritten,NULL))return false;

	return true;
}

//INI�Ɏw�肳�ꂽ�Z�N�V����������Ȃ�true��Ԃ�
bool UtilCheckINISectionExists(LPCTSTR lpAppName,LPCTSTR lpFileName)
{
	TCHAR szBuffer[10];
	DWORD dwRead=GetPrivateProfileSection(lpAppName,szBuffer,9,lpFileName);
	return dwRead>0;
}

//���j���[�\����ǂݍ���(B2E���j���[�p)
bool UtilReadMenuDefinitionFile(LPCTSTR lpFileName,std::vector<std::vector<CString> >& r_MenuArr)
{
//�������ʓ|�Ȃ̂ŁAUNICODE�ł̂݃R�[�h��p�ӂ��Ă���B
//ANSI/SJIS�ł���������ɂ́A�����R�[�h�ϊ������ނ��A�������̓��j���[��`�t�@�C���̕����R�[�h��ϊ����Ă�������
#if !defined(_UNICODE)&&!defined(UNICODE)
#error "UtilReadMenuDefinitionFile() : Needs implementation for ANSI/S_JIS."
#endif

	/*
	���̓t�@�C���`��
	�����R�[�h:
		BOM����UTF-16LE
	���j���[��`:[�g�b�v](\t[�`����])+\n
		�g�b�v���j���[�̃L���v�V�����ɑ����Č`���������Ȃ��Ƃ���̃^�u�ŋ�؂��ċL�q����B
		�܂��A���s�Ń��j���[��`����؂���B
		(ex.)
		cabinet	arc1	arc2
		7z	PPMd	Deflate
	�o�͌`��:
		CString�̓�d�z��Ɍ��ʂ��i�[�����B
		std::vector<CString>�ɂ͈�̃��j���[�K�w���i�[�����B
		��̗�ł�["cabinet","arc1","arc2"]�ƂȂ�B���̂����A��ڂ��|�b�v�A�b�v���j���[�̍��{�̃L���v�V�����ƂȂ�B

	*/


	//���̓t�@�C�����`�F�b�N
	ASSERT(lpFileName);
	if(!lpFileName)return false;
	if(!PathFileExists(lpFileName))return false;

	HANDLE hFile=CreateFile(lpFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(INVALID_HANDLE_VALUE==hFile)return false;

	//4GB�z���t�@�C���͈���Ȃ�
	const DWORD dwSize=GetFileSize(hFile,NULL);
	std::vector<BYTE> cReadBuffer;
	cReadBuffer.resize(dwSize+2);
	DWORD dwRead;
	//---�ǂݍ���
	if(!ReadFile(hFile,&cReadBuffer[0],dwSize,&dwRead,NULL)||dwSize!=dwRead){
		CloseHandle(hFile);
		return false;
	}
	CloseHandle(hFile);

	//�I�[�����ǉ�
	*((LPWSTR)&cReadBuffer[dwSize])=L'\0';


	//���߂��s��
	LPCTSTR lpC=(LPCTSTR)&cReadBuffer[0];
	CString strTemp;
	std::vector<CString> strArr;

	r_MenuArr.clear();
	for(;*lpC!=_T('\0');lpC++){
		switch(*lpC){
		case _T('\t'):	//���j���[���ڂ̋�؂�
			if(!strTemp.IsEmpty()){
				strArr.push_back(strTemp);
				strTemp.Empty();
			}
			break;
		case _T('\n'):	//FALLTHROUGH
		case _T('\r'):
			if(!strTemp.IsEmpty()){
				strArr.push_back(strTemp);
				strTemp.Empty();
			}
			if(!strArr.empty()){
				//���j���[�ɒǉ�
				r_MenuArr.push_back(strArr);
				strArr.clear();
			}
			break;
		default:
			strTemp+=*lpC;
			break;
		}
	}

	//�Ō�̏���
	if(!strTemp.IsEmpty()){
		strArr.push_back(strTemp);
	}
	if(!strArr.empty()){
		r_MenuArr.push_back(strArr);
		strArr.clear();
	}

	return true;
}


BOOL UtilLoadString(UINT nID,CString& strOut)
{
	return strOut.LoadString(g_hInstDLL,nID);
}
