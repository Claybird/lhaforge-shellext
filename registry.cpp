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


LSTATUS UtilSetKeyAndValue(HKEY root, const std::wstring& keyPath, const std::wstring& name, const std::wstring& value)
{
	auto result = RegSetKeyValueW(root,
		keyPath.c_str(),
		name.c_str(),
		REG_SZ,
		(const BYTE*)value.c_str(),
		(DWORD)(wcslen(value.c_str()) + 1) * sizeof(wchar_t));

	return result;
}

HRESULT UtilRegisterServer(HMODULE hModule,REFCLSID clsid)
{
	const auto strCLSID = CLSIDtoString(clsid);
	HRESULT hr, final_result = S_OK;
	{
		auto strKey = L"CLSID\\" + strCLSID;
		hr = UtilSetKeyAndValue(HKEY_CLASSES_ROOT, strKey, L"", APPNAME);
		if(FAILED(hr))final_result = hr;

		//module path
		strKey = L"CLSID\\" + strCLSID + L"\\InprocServer32";
		auto dllPath = UtilGetModulePath(hModule);
		hr = UtilSetKeyAndValue(HKEY_CLASSES_ROOT, strKey, L"", dllPath.wstring());
		if (FAILED(hr))final_result = hr;

		hr = UtilSetKeyAndValue(HKEY_CLASSES_ROOT, strKey, L"ThreadingModel", L"Apartment");
		if (FAILED(hr))final_result = hr;
	}

	if (IsEqualCLSID(clsid, CLSID_LFDragMenu)) {
		auto dirHandler = Format(L"Directory\\shellex\\DragDropHandlers\\%s", APPNAME);
		hr = UtilSetKeyAndValue(HKEY_CLASSES_ROOT, dirHandler, L"", strCLSID);
		if (FAILED(hr))final_result = hr;

		auto driveHandler = Format(L"Drive\\shellex\\DragDropHandlers\\%s", APPNAME);
		hr = UtilSetKeyAndValue(HKEY_CLASSES_ROOT, driveHandler, L"", strCLSID);
		if (FAILED(hr))final_result = hr;
	}else{
		for(const auto &ext: g_extensions){
			auto handler = Format(L"%s\\shellex\\ContextMenuHandlers\\%s", ext.c_str(), APPNAME);
			hr = UtilSetKeyAndValue(HKEY_CLASSES_ROOT,handler,L"",strCLSID);
			if (FAILED(hr))final_result = hr;
		}
	}

	return final_result;
}


HRESULT UtilUnregisterServer(REFCLSID clsid)
{
	HRESULT hr, final_result = S_OK;
	const auto strCLSID = CLSIDtoString(clsid);
	{
		auto strKey = L"CLSID\\" + strCLSID;
		hr = RegDeleteTreeW(HKEY_CLASSES_ROOT, strKey.c_str());
		if (FAILED(hr))final_result = hr;
	}

	if (IsEqualCLSID(clsid, CLSID_LFDragMenu)) {
		auto dirHandler = Format(L"Directory\\shellex\\DragDropHandlers\\%s", APPNAME);
		hr = RegDeleteTreeW(HKEY_CLASSES_ROOT, dirHandler.c_str());
		if (FAILED(hr))final_result = hr;

		auto driveHandler = Format(L"Drive\\shellex\\DragDropHandlers\\%s", APPNAME);
		hr = RegDeleteTreeW(HKEY_CLASSES_ROOT, driveHandler.c_str());
		if (FAILED(hr))final_result = hr;
	}else{
		for (const auto& ext : g_extensions) {
			auto handler = Format(L"%s\\shellex\\ContextMenuHandlers\\%s", ext.c_str(), APPNAME);
			hr = RegDeleteTreeW(HKEY_CLASSES_ROOT, handler.c_str());
			if (FAILED(hr))final_result = hr;
		}
	}

	return final_result;
}

