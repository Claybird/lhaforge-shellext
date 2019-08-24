//共通してincludeすべきファイルがすべて書いてある

#pragma once
//#define WINVER 0x0500
#define _WIN32_WINNT 0x0600
//#define _ATL_NO_COM
//#define _WTL_NO_WTYPES
//#define _WTL_NO_UNION_CLASSES
//#define _ATL_NO_MSIMG
//#define _ATL_NO_OPENGL
#define ATL_NO_LEAN_AND_MEAN

//ATLのCStringを使う
#define _WTL_NO_CSTRING
#define _STLP_USE_NEWALLOC	//STLで標準のアロケータを使う

#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <atlbase.h>
#include <atlstr.h>
#include <atlapp.h>
#include <atlpath.h>
extern CAppModule _Module;
#include <atlwin.h>
#include <atlcrack.h>
#include <atlmisc.h>
#include <atldlgs.h>
#include <atlctrls.h>
#include <atlframe.h>
#include <vector>
#include <Lmcons.h>
#include <imagehlp.h>

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
