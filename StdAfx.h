// stdafx.h : 標準のシステム インクルード ファイル、
//            または参照回数が多く、かつあまり変更されない
//            プロジェクト専用のインクルード ファイルを記述します。
//

#pragma once
#define ATL_NO_LEAN_AND_MEAN
//#define _SECURE_ATL 0

//ATLのCStringを使う
#define _WTL_NO_CSTRING
#include <windows.h>
#include <shlobj.h>
#include <stdio.h>
#include <stdarg.h>
#include <wtypes.h>

#include <atlbase.h>
#include <atlstr.h>
#include <atlapp.h>
#include <atlpath.h>
extern CComModule _Module;
#include <atlmisc.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <shlwapi.h>
#include <vector>
#include <crtdbg.h>
#include <locale.h>
#include <Lmcons.h>
#include <filesystem>

#ifndef ASSERT
#define ASSERT ATLASSERT
#endif

#define COUNTOF(x)	(sizeof(x)/sizeof(x[0]))

#if defined _M_IX86
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
  #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
