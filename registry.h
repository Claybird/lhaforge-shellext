/////////////////////////////////////////////////////////////////////////////
//
// Last update	$Date: 2007/10/31 14:12:42 $ UTC
//   Revision   $Revision: 1.7 $
//
//	Modified by Claybird <claybird.without.wing@gmail.com>
//
// registry.h :	レジストリへの登録と削除のヘッダ
//				LhaForge拡張メニュー
//
//	Original File Information:
// registry.h :	レジストリへの登録と削除のヘッダ
//				他人の褌(TF)拡張メニュー
//
//////////////////////////////////////////////////////////////////////////////

#pragma once
STDAPI RegisterServer(HMODULE, REFCLSID clsid,bool bDrag);
STDAPI UnregisterServer(REFCLSID clsid,bool bDrag);
