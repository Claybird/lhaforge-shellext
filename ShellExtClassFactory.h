/////////////////////////////////////////////////////////////////////////////
//
// Last update	$Date: 2007/11/01 15:32:33 $ UTC
//   Revision   $Revision: 1.7 $
//
//	Modified by Claybird <claybird.without.wing@gmail.com>
//
// ShellExtClassFactory.h : �V�F���g�� �N���X�t�@�N�g���[�N���X�̃w�b�_
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

// �N���X�t�@�N�g���̍쐬 (IClassFactory�C���^�[�t�F�C�X���p������)
class CShellExtClassFactory : public IClassFactory
{
protected:
    
	// �Q�ƃJ�E���g
    LONG		m_cRef;
	//�E�h���b�O���j���[���ǂ���
	bool		IsRightDrag;

public:

	// �R���X�g���N�^�E�f�X�g���N�^
	CShellExtClassFactory(bool);
	virtual ~CShellExtClassFactory();

	//IUnknown �C���^�[�t�F�C�X�̃��\�b�h
	STDMETHODIMP            QueryInterface(REFIID, LPVOID *);
	STDMETHODIMP_(ULONG)    AddRef();
	STDMETHODIMP_(ULONG)    Release();

	//IClassFactory �C���^�[�t�F�C�X�̃��\�b�h
	STDMETHODIMP            CreateInstance(LPUNKNOWN, REFIID, LPVOID *);
	STDMETHODIMP            LockServer(BOOL);

};
// �|�C���^�^��錾
typedef CShellExtClassFactory *LPCSHELLEXTCLASSFACTORY;
