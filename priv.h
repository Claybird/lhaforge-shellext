#pragma once

#include <windows.h>
#include <vector>
#include <locale.h>
#include <filesystem>
#include <shlwapi.h>	//QISearch
#include <shlobj.h>	//IContextMenu, IShellExtInit

#ifndef ASSERT
#define ASSERT ATLASSERT
#endif

#define COUNTOF(x)	(sizeof(x)/sizeof(x[0]))

template <class T> void SafeRelease(T** ppT)
{
    if (*ppT) {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

template <typename ...Args>
std::wstring Format(const std::wstring& fmt, Args && ...args)
{
	//snprintf_s will not return the required buffer size
	std::wstring work;
#pragma warning(push)
#pragma warning(disable:4996)
	auto size = _snwprintf(nullptr, 0, fmt.c_str(), std::forward<Args>(args)...);
	work.resize(size + 1);
	_snwprintf(&work[0], work.size(), fmt.c_str(), std::forward<Args>(args)...);
#pragma warning(pop)
	return work.c_str();
}

extern HINSTANCE    g_hinst;

void DllAddRef();
void DllRelease();

HRESULT UtilRegisterServer(HMODULE, REFCLSID clsid);
HRESULT UtilUnregisterServer(REFCLSID clsid);



//right click context menu
// {9127BC06-94AC-46F7-99DB-3D4423B8E813}
const GUID CLSID_LFContextMenu =
{ 0x9127bc06, 0x94ac, 0x46f7, { 0x99, 0xdb, 0x3d, 0x44, 0x23, 0xb8, 0xe8, 0x13 } };

//right drag menu
// {8CE5DDDA-DDAF-476F-86FD-505DA9B94967}
const GUID CLSID_LFDragMenu =
{ 0x8ce5ddda, 0xddaf, 0x476f, { 0x86, 0xfd, 0x50, 0x5d, 0xa9, 0xb9, 0x49, 0x67 } };

HRESULT CreateInstance_ContextMenu(REFIID riid, void** ppv);
HRESULT CreateInstance_DragMenu(REFIID riid, void** ppv);


#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
