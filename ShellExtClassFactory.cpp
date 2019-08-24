/////////////////////////////////////////////////////////////////////////////
//
// Last update	$Date: 2010/12/05 06:24:46 $ UTC
//   Revision   $Revision: 1.10 $
//
//	Modified by Claybird <claybird.without.wing@gmail.com>
//
// ShellExtClassFactory.cpp : �V�F���g�� �N���X�t�@�N�g���[�N���X�̒�`
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ShellExtClassFactory.h"
#include "resource.h"
#include "MenuData.h"
#include "MenuExtension.h"
#include "Main.h"

/*-------------------------------------------------------------------------*/
// CShellExtClassFactory	�R���X�g���N�^�E�f�X�g���N�^
/*-------------------------------------------------------------------------*/

// CShellExtClassFactory
CShellExtClassFactory::CShellExtClassFactory(bool bRightDrag)
	: m_cRef(0L),IsRightDrag(bRightDrag)
{
	dprintf(_T("CShellExtClassFactory::CShellExtClassFactory\n"));

	InterlockedIncrement(&g_cDllRef);	
}

// ~CShellExtClassFactory
CShellExtClassFactory::~CShellExtClassFactory()
{
	dprintf(_T("CShellExtClassFactory::~CShellExtClassFactory\n"));

	InterlockedDecrement(&g_cDllRef);
}

/*-------------------------------------------------------------------------*/
// CShellExtClassFactory	
//
//			IUnknown �C���^�[�t�F�C�X�̃��\�b�h�Q
//
//				QueryInterface
//				AddRef
//				Release
//
/*-------------------------------------------------------------------------*/

// QueryInterface
STDMETHODIMP CShellExtClassFactory::QueryInterface(
	REFIID		riid,
	LPVOID *	ppv)
{
	dprintf(_T("CShellExtClassFactory::QueryInterface\n"));

	HRESULT hr = E_NOINTERFACE;
	*ppv = NULL;
	if(IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))	{
		*ppv = (LPCSHELLEXTCLASSFACTORY)this;
		static_cast<IUnknown*>(*ppv)->AddRef();
		hr = NOERROR;
	}
	return hr;
}

// AddRef
STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef()
{
	dprintf(_T("CShellExtClassFactory::AddRef (m_cRef=%d)\n"),m_cRef);

	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

// Release
STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
	dprintf(_T("CShellExtClassFactory::Release (m_cRef=%d)\n"),m_cRef);

	ULONG theRef = InterlockedDecrement(&m_cRef);
	if(theRef == 0L){
		delete this;
	}
	return theRef;	// ���[�J���ϐ����g���Ă���̂� delete ��ł��l��Ԃ���悤��
}

/*-------------------------------------------------------------------------*/
// CShellExtClassFactory	
//
//			IClassFactory �C���^�[�t�F�C�X�̃��\�b�h�Q
//
//				CreateInstance
//				LockServer
//
/*-------------------------------------------------------------------------*/

// CreateInstance
STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,REFIID riid,LPVOID* ppv)
{
 	dprintf(_T("CShellExtClassFactory::CreateInstance\n"));

	HRESULT hr = S_OK;
	CMenuExtension* pShellExt = (CMenuExtension*)NULL;

	*ppv = NULL;

	if(pUnkOuter != (LPUNKNOWN)NULL){
		// �A�O���Q�[�V�������T�|�[�g���Ȃ��B
		hr = CLASS_E_NOAGGREGATION;
	}
	else{
		// �R���|�[�l���g�̍쐬�Ə��������s���B
		pShellExt = new CMenuExtension(IsRightDrag);
		if(NULL == pShellExt){
    		hr = E_OUTOFMEMORY;
			dprintf(_T("\tCreateInstance: new CMenuExtension = NULL\n"));
		}
		else{
			dprintf(_T("\tCreateInstance: new CMenuExtension\n"));
		}
	}

	if(hr == S_OK){
		// �C���^�[�t�F�[�X��v������B
		hr = pShellExt->QueryInterface(riid, ppv);
	}
	return hr;
}

// LockServer
STDMETHODIMP CShellExtClassFactory::LockServer(BOOL fLock)
{
	dprintf(_T("CShellExtClassFactory::LockServer\n"));

	UNREFERENCED_PARAMETER(fLock);
	return NOERROR;
}
