//https://learn.microsoft.com/en-us/windows/win32/shell/samples-nondefaultdropmenuverb

#include "priv.h"

const wchar_t* APPNAME = L"LhaForge2";

const std::vector<std::wstring> g_extensions =
{
	L"*",
	L"Directory",
	L"Drive",
};


std::wstring CLSIDtoString(REFCLSID clsid)
{
	const int CLSID_STRING_SIZE = (39 + 1);
	wchar_t buf[CLSID_STRING_SIZE + 1] = {};

	StringFromGUID2(clsid, buf, CLSID_STRING_SIZE);
	return buf;
}

std::filesystem::path UtilGetModulePath(HMODULE hModule);


bool UtilSetKeyAndValue(HKEY root, const std::wstring& keyPath, const std::wstring& name, const std::wstring& value)
{
	auto result = RegSetKeyValueW(root,
		keyPath.c_str(),
		name.c_str(),
		REG_SZ,
		(const BYTE*)value.c_str(),
		(DWORD)(wcslen(value.c_str()) + 1) * sizeof(wchar_t));

	return result == ERROR_SUCCESS;
}


HRESULT UtilRegisterServer(HMODULE hModule,REFCLSID clsid)
{
	const auto strCLSID = CLSIDtoString(clsid);
	{
		auto dllPath = UtilGetModulePath(hModule);
		auto strKey = L"CLSID\\" + strCLSID;
		UtilSetKeyAndValue(HKEY_CLASSES_ROOT, strKey, L"", APPNAME);
		//module path
		strKey = L"CLSID\\" + strCLSID + L"\\InprocServer32";
		UtilSetKeyAndValue(HKEY_CLASSES_ROOT, strKey, L"", dllPath.wstring());
		UtilSetKeyAndValue(HKEY_CLASSES_ROOT, strKey, L"ThreadingModel", L"Apartment");
	}

	if (IsEqualCLSID(clsid, CLSID_LFDragMenu)) {
		auto dirHandler = Format(L"Directory\\shellex\\DragDropHandlers\\%s", APPNAME);
		UtilSetKeyAndValue(HKEY_CLASSES_ROOT, dirHandler, L"", strCLSID);

		auto driveHandler = Format(L"Drive\\shellex\\DragDropHandlers\\%s", APPNAME);
		UtilSetKeyAndValue(HKEY_CLASSES_ROOT, driveHandler, L"", strCLSID);
	}else{
		for(const auto &ext: g_extensions){
			auto handler = Format(L"%s\\shellex\\ContextMenuHandlers\\%s", ext.c_str(), APPNAME);
			UtilSetKeyAndValue(HKEY_CLASSES_ROOT,handler,L"",strCLSID);
		}
	}

	return S_OK;
}


HRESULT UtilUnregisterServer(REFCLSID clsid)
{
	const auto strCLSID = CLSIDtoString(clsid);
	{
		auto strKey = L"CLSID\\" + strCLSID;
		RegDeleteTreeW(HKEY_CLASSES_ROOT, strKey.c_str());
	}

	if (IsEqualCLSID(clsid, CLSID_LFDragMenu)) {
		auto dirHandler = Format(L"Directory\\shellex\\DragDropHandlers\\%s", APPNAME);
		RegDeleteTreeW(HKEY_CLASSES_ROOT, dirHandler.c_str());
		auto driveHandler = Format(L"Drive\\shellex\\DragDropHandlers\\%s", APPNAME);
		RegDeleteTreeW(HKEY_CLASSES_ROOT, driveHandler.c_str());
	}else{
		for (const auto& ext : g_extensions) {
			auto handler = Format(L"%s\\shellex\\ContextMenuHandlers\\%s", ext.c_str(), APPNAME);
			RegDeleteTreeW(HKEY_CLASSES_ROOT, handler.c_str());
		}
	}

	return S_OK;
}

