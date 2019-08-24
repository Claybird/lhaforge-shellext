/////////////////////////////////////////////////////////////////////////////
//
// Last update	$Date: 2007/11/01 15:32:33 $ UTC
//   Revision   $Revision: 1.7 $
//
//	Modified by Claybird <claybird.without.wing@gmail.com>
//
// ShellExtClassFactory.h : シェル拡張 クラスファクトリークラスのヘッダ
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

// クラスファクトリの作成 (IClassFactoryインターフェイスを継承する)
class CShellExtClassFactory : public IClassFactory
{
protected:
    
	// 参照カウント
    LONG		m_cRef;
	//右ドラッグメニューかどうか
	bool		IsRightDrag;

public:

	// コンストラクタ・デストラクタ
	CShellExtClassFactory(bool);
	virtual ~CShellExtClassFactory();

	//IUnknown インターフェイスのメソッド
	STDMETHODIMP            QueryInterface(REFIID, LPVOID *);
	STDMETHODIMP_(ULONG)    AddRef();
	STDMETHODIMP_(ULONG)    Release();

	//IClassFactory インターフェイスのメソッド
	STDMETHODIMP            CreateInstance(LPUNKNOWN, REFIID, LPVOID *);
	STDMETHODIMP            LockServer(BOOL);

};
// ポインタ型を宣言
typedef CShellExtClassFactory *LPCSHELLEXTCLASSFACTORY;
