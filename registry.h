/////////////////////////////////////////////////////////////////////////////
//
// Last update	$Date: 2007/10/31 14:12:42 $ UTC
//   Revision   $Revision: 1.7 $
//
//	Modified by Claybird <claybird.without.wing@gmail.com>
//
// registry.h :	���W�X�g���ւ̓o�^�ƍ폜�̃w�b�_
//				LhaForge�g�����j���[
//
//	Original File Information:
// registry.h :	���W�X�g���ւ̓o�^�ƍ폜�̃w�b�_
//				���l����(TF)�g�����j���[
//
//////////////////////////////////////////////////////////////////////////////

#pragma once
STDAPI RegisterServer(HMODULE, REFCLSID clsid,bool bDrag);
STDAPI UnregisterServer(REFCLSID clsid,bool bDrag);
