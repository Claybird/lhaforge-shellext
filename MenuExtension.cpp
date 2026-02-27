//https://learn.microsoft.com/en-us/windows/win32/shell/samples-nondefaultdropmenuverb

#include "priv.h"
#include "resource.h"
#include <SimpleIni.h>


//loads string from resource
std::wstring UtilLoadString(UINT nID)
{
	wchar_t buf[256] = {};
	LoadStringW(g_hinst, nID, buf, 256);
	return buf;
}

class CAutoFile {
protected:
	FILE* _fp;
	std::filesystem::path _path;
	CAutoFile(const CAutoFile&) = delete;
	const CAutoFile& operator=(const CAutoFile&) = delete;
public:
	CAutoFile() :_fp(NULL) {}
	virtual ~CAutoFile() {
		close();
	}
	operator FILE* () { return _fp; }
	bool is_opened() const { return _fp != NULL; }
	void close() {
		if (_fp) {
			fclose(_fp);
			_fp = NULL;
			_path.clear();
		}
	}
	void open(const std::filesystem::path& fname, const std::wstring& mode = L"rb") {
		close();
		_path = fname;
		auto err = _wfopen_s(&_fp, fname.c_str(), mode.c_str());
		if (err == 0 && _fp) {
			//set buffer size
			setvbuf(_fp, NULL, _IOFBF, 1024 * 1024);
		}
	}
	const std::filesystem::path& get_path()const { return _path; }
};

void touchFile(const std::filesystem::path& path)
{
	CAutoFile fp;
	fp.open(path, L"w");
}

std::filesystem::path UtilPathAddLastSeparator(const std::filesystem::path& path)
{
	std::wstring p = path.wstring();
	if (p.empty() || (p.back() != L'/' && p.back() != L'\\')) {
		p += std::filesystem::path::preferred_separator;
	}
	return p;
}

//returns a temp dir exclusive use of lhaforge
std::filesystem::path UtilGetTempPath()
{
	auto tempDir = std::filesystem::temp_directory_path() / L"lhaforge";
	std::filesystem::create_directories(tempDir);
	return UtilPathAddLastSeparator(tempDir);
}

std::filesystem::path UtilGetTemporaryFileName()
{
	for (size_t index = 0; ; index++) {
		auto path = std::filesystem::path(UtilGetTempPath()) / Format(L"tmp%d.tmp", index);
		if (!std::filesystem::exists(path)) {
			touchFile(path);
			return path.make_preferred();
		}
	}
}

bool UtilInsertMenuItem(HMENU hMenu, const wchar_t* lpszCaption, int indexMenu, HMENU hSubMenu, WORD wID)
{
	MENUITEMINFOW mii = { 0 };
	mii.cbSize = sizeof(mii);

	if (lpszCaption) {
		if (hSubMenu) {
			//with sub-menu
			mii.fMask = MIIM_SUBMENU | MIIM_STRING;
			mii.hSubMenu = hSubMenu;
		} else {
			mii.fMask = MIIM_STRING;
		}

		if (wID != (WORD)-1) {
			mii.fMask |= MIIM_ID;
			mii.wID = wID;
		}

		mii.dwTypeData = const_cast<wchar_t*>(lpszCaption);
		mii.cch = (UINT)wcslen(lpszCaption);

		//TRUE:indexMenu is location
		if (::InsertMenuItemW(hMenu, indexMenu, TRUE, &mii)) {
			return true;
		} else {
			//retry with MFT_STRING instead of MIIM_STRING
			mii.fMask &= ~MIIM_STRING;
			mii.fMask |= MIIM_FTYPE;	//MIIM_TYPE
			mii.fType = MFT_STRING;
			if (::InsertMenuItemW(hMenu, indexMenu, TRUE, &mii)) {
				return true;
			}
		}
		return false;
	} else {
		//adding separator
		mii.fMask = MIIM_TYPE;
		mii.fType = MFT_SEPARATOR;
		if (wID != (WORD)-1) {
			mii.fMask |= MIIM_ID;
			mii.wID = wID;
		}
		if (::InsertMenuItemW(hMenu, indexMenu, TRUE, &mii)) {
			return true;
		} else {
			mii.fMask = MIIM_FTYPE;
			mii.fType = MFT_SEPARATOR;
			if (wID != (WORD)-1) {
				mii.fMask |= MIIM_ID;
				mii.wID = wID;
			}
			if (::InsertMenuItemW(hMenu, indexMenu, TRUE, &mii)) {
				return true;
			} else {
				return false;
			}
		}
	}
}

void UtilExecuteCommand(const std::filesystem::path &exe, const std::wstring &args)
{
	STARTUPINFO theStartupInfo = { sizeof(STARTUPINFO) };
	theStartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	theStartupInfo.wShowWindow = SW_SHOWDEFAULT;

	std::wstring buf = L"\"" + exe.wstring() + L"\" " + args;

	PROCESS_INFORMATION	pi = {};
	BOOL result = ::CreateProcessW(nullptr, &buf[0],
		nullptr, nullptr, FALSE, CREATE_NEW_PROCESS_GROUP,
		nullptr, nullptr, &theStartupInfo, &pi);

	if (!result) {
		::MessageBeep(MB_ICONEXCLAMATION);
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}


std::filesystem::path UtilGetModulePath(HMODULE hModule)
{
	std::wstring name;
	name.resize(256);
	for (;;) {
		DWORD bufsize = (DWORD)name.size();
		auto nCopied = GetModuleFileNameW(hModule, &name[0], bufsize);
		if (nCopied < bufsize) {
			break;
		} else {
			name.resize(name.size() * 2);
		}
	}
	return name.c_str();
}


struct SHELL_MENU_ITEM {
	WORD wCaption;
	std::wstring param;
	std::vector<SHELL_MENU_ITEM> children;
};

const std::vector<SHELL_MENU_ITEM> g_other_formats = {
	{ IDS_MENU_ITEM_CAPTION_ZIP,L"",{
		{IDS_MENU_ITEM_CAPTION_ZIP,L"/c:zip"},
		{IDS_MENU_ITEM_CAPTION_ZIP_PASS,L"/c:zippass"},
		},
	},
	{IDS_MENU_ITEM_CAPTION_7Z,L"/c:7z"},
	{IDS_MENU_ITEM_CAPTION_TAR_RELATED,L"",{
		{IDS_MENU_ITEM_CAPTION_TAR,L"/c:tar"},
		{IDS_MENU_ITEM_CAPTION_GZ,L"/c:gz"},
		{IDS_MENU_ITEM_CAPTION_BZ2,L"/c:bz2"},
		{IDS_MENU_ITEM_CAPTION_XZ,L"/c:xz"},
		{IDS_MENU_ITEM_CAPTION_LZMA,L"/c:lzma"},
		{IDS_MENU_ITEM_CAPTION_LZ4,L"/c:lz4"},
		{IDS_MENU_ITEM_CAPTION_ZSTD, L"/c:zstd"},
		{IDS_MENU_ITEM_CAPTION_TGZ,L"/c:tar+gz"},
		{IDS_MENU_ITEM_CAPTION_TBZ,L"/c:tar+bz2"},
		{IDS_MENU_ITEM_CAPTION_TXZ,L"/c:tar+xz"},
		{IDS_MENU_ITEM_CAPTION_TLZ,L"/c:tar+lzma"},
		{IDS_MENU_ITEM_CAPTION_TAR_LZ4, L"/c:tar+lz4"},
		{IDS_MENU_ITEM_CAPTION_TAR_ZSTD, L"/c:tar+zstd"},
		},
	},
};

const std::vector<SHELL_MENU_ITEM> g_other_operations = {
	{ IDS_MENU_CAPTION_EXTRACT_NORMAL, L"/e"},
	{ IDS_MENU_CAPTION_EXTRACT_DESKTOP,L"/e /od" },
	{ IDS_MENU_CAPTION_EXTRACT_SAME,L"/e /os" },
	{ IDS_MENU_CAPTION_EXTRACT_TARGET,L"/e /oa" },
	{ IDS_MENU_CAPTION_TEST_ARCHIVE, L"/t"},
	{ IDS_MENU_CAPTION_LIST, L"/l"},
};

//default
const std::vector<SHELL_MENU_ITEM> g_contextMenuSub = {
	{IDS_MENU_ITEM_CAPTION_ZIP_ROOT, L"/c:zip"},
	{IDS_MENU_ITEM_CAPTION_ZIP_PASS_ROOT,L"/c:zippass"},
	{IDS_MENU_CAPTION_OTHER_FORMATS, L"", g_other_formats},

	{IDS_MENU_CAPTION_EXTRACT_NORMAL,L"/e"},
	{IDS_MENU_CAPTION_OPERATION, L"", g_other_operations},
};

//OneRootUnifyOperations
const std::vector<SHELL_MENU_ITEM> g_contextMenuSub_unified = {
	{IDS_MENU_ITEM_CAPTION_ZIP_ROOT, L"/c:zip"},
	{IDS_MENU_ITEM_CAPTION_ZIP_PASS_ROOT,L"/c:zippass"},
	{IDS_MENU_CAPTION_OTHER_FORMATS, L"", g_other_formats},

	{IDS_MENU_CAPTION_EXTRACT_NORMAL, L"/e"},
	{IDS_MENU_CAPTION_EXTRACT_DESKTOP,L"/e /od" },
	{IDS_MENU_CAPTION_EXTRACT_SAME,L"/e /os" },
	{IDS_MENU_CAPTION_EXTRACT_TARGET,L"/e /oa" },
	{IDS_MENU_CAPTION_TEST_ARCHIVE, L"/t"},
	{IDS_MENU_CAPTION_LIST, L"/l"},
};

//Conventional
const std::vector<SHELL_MENU_ITEM> g_contextMenuConventionalCompress = {
	{IDS_MENU_ITEM_CAPTION_ZIP_ROOT, L"/c:zip"},
	{IDS_MENU_ITEM_CAPTION_ZIP_PASS_ROOT,L"/c:zippass"},
	{IDS_MENU_CAPTION_OTHER_FORMATS, L"", g_other_formats},
};

const std::vector<SHELL_MENU_ITEM> g_contextMenuConventionalExtract = {
	{IDS_MENU_CAPTION_EXTRACT_NORMAL, L"/e"},
	{IDS_MENU_CAPTION_EXTRACT_DESKTOP,L"/e /od" },
	{IDS_MENU_CAPTION_EXTRACT_SAME,L"/e /os" },
	{IDS_MENU_CAPTION_EXTRACT_TARGET,L"/e /oa" },
};

const std::vector<SHELL_MENU_ITEM> g_contextMenuConventionalOperations = {
	{IDS_MENU_CAPTION_LIST, L"/l"},
	{IDS_MENU_CAPTION_TEST_ARCHIVE, L"/t"},
};

const std::vector<SHELL_MENU_ITEM> g_contextMenuConventional = {
	{IDS_MENU_CAPTION_COMPRESS_ROOT, L"", g_contextMenuConventionalCompress},
	{IDS_MENU_CAPTION_EXTRACT_ROOT, L"", g_contextMenuConventionalExtract},
	{IDS_MENU_CAPTION_OPERATION_ROOT,L"", g_contextMenuConventionalOperations },
};


const std::vector<SHELL_MENU_ITEM> g_dragMenuSub = {
	{IDS_MENU_ITEM_CAPTION_ZIP_ROOT, L"/c:zip"},
	{IDS_MENU_ITEM_CAPTION_ZIP_PASS, L"/c:zippass"},
	{IDS_MENU_CAPTION_OTHER_FORMATS, L"", g_other_formats},

	{IDS_MENU_CAPTION_EXTRACT_DRAG,L"/e"},
};

std::filesystem::path getIniPath() 
{
	const wchar_t* INI_FILE_NAME = L"LhaForge.ini";
	const wchar_t* PROGRAMDIR_NAME = L"LhaForge2";	//directory name in ApplicationData

	//user common configuration
	{
		//.ini file is in same as the executable; for portable usage
		auto candidate = UtilGetModulePath(g_hinst).parent_path() / INI_FILE_NAME;
		if (std::filesystem::is_regular_file(candidate)) {
			return candidate;
		}
	}
	{
		//.ini is in FOLDERID_ProgramData (formerly CSIDL_COMMON_APPDATA)
		wchar_t* ptr = nullptr;
		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_ProgramData, 0, nullptr, &ptr))) {
			auto candidate = std::filesystem::path(ptr) / PROGRAMDIR_NAME / INI_FILE_NAME;
			CoTaskMemFree(ptr);
			if (std::filesystem::is_regular_file(candidate)) {
				return candidate;
			}
		}
	}

	//--------------------
	//user specific configuration
	{
		//.ini is in FOLDERID_RoamingAppData (formerly CSIDL_APPDATA)
		wchar_t* ptr = nullptr;
		if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &ptr))) {
			auto candidate = std::filesystem::path(ptr) / PROGRAMDIR_NAME / INI_FILE_NAME;
			CoTaskMemFree(ptr);
			return candidate;
		}
	}

	//default fallback
	return UtilGetTemporaryFileName();
}

class CLFMenuExtension : public IContextMenu, public IShellExtInit
{
	enum class MODE {
		Default,	//LhaForge->{zip/other_formats/extract/other_operations}
		OneRootUnifyOperations,	//LhaForge->{zip/other_formats/extract/test/etc.}
		Conventional,	//{Compress}/{Extract}/{Operations}; similar to LhaForge Ver.1.6.7
	};
	//----------------
	const bool _isContextMenu;
	MODE _menuMode = MODE::Default;
	long        _cRef;
	std::filesystem::path _targetFolder;	//drop target
	IDataObject* _pdtobj;       // data object

	std::vector<std::wstring> _cmdArgs;
public:
	CLFMenuExtension(bool isContextMenu) : _cRef(1), _pdtobj(NULL), _isContextMenu(isContextMenu) {
		DllAddRef();
		CSimpleIniW ini;
		ini.SetUnicode(true);

		ini.LoadFile(getIniPath().c_str());
		switch (ini.GetLongValue(L"ShellExt", L"MenuMode", 0)) {
		case 2:
			_menuMode = MODE::Conventional;
			break;
		case 1:
			_menuMode = MODE::OneRootUnifyOperations;
			break;
		case 0:
		default:
			_menuMode = MODE::Default;
		}
	}

	// IUnknown methods
	IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) {
		static const QITAB qit[] =
		{
			QITABENT(CLFMenuExtension, IContextMenu),
			QITABENT(CLFMenuExtension, IShellExtInit),
			{ 0 }
		};
		return QISearch(this, qit, riid, ppv);
	}

	IFACEMETHODIMP_(ULONG) AddRef() {
		return InterlockedIncrement(&_cRef);
	}

	IFACEMETHODIMP_(ULONG) Release() {
		long cRef = InterlockedDecrement(&_cRef);
		if (cRef == 0) {
			delete this;
		}
		return cRef;
	}

	// IContextMenu
	IFACEMETHODIMP CLFMenuExtension::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) {
		if ((uFlags & CMF_DEFAULTONLY)) {
			return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
		}

		if ((uFlags & 0x000F) != CMF_NORMAL &&
			!(uFlags & CMF_VERBSONLY) &&
			!(uFlags & CMF_EXPLORE)/* context menu on tree view */) {
			return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
		}

		if (!_pdtobj) {
			//no file selected?
			return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
		}

		std::vector<SHELL_MENU_ITEM> menuItems;
		if (_isContextMenu) {
			switch(_menuMode) {
			case MODE::Conventional:
				menuItems = g_contextMenuConventional;
				break;
			case MODE::OneRootUnifyOperations:
				menuItems.push_back({ IDS_PROGRAM_NAME, L"", g_contextMenuSub_unified });
				break;
			case MODE::Default:
			default:
				menuItems.push_back({ IDS_PROGRAM_NAME, L"", g_contextMenuSub });
				break;
			}
		} else {
			menuItems.push_back({ IDS_PROGRAM_NAME, L"", g_dragMenuSub });
		}
		int numItems = _insertMenuItems(hMenu, idCmdFirst, menuItems);

		/*
		 * From MSDN:
		 * If successful, returns an HRESULT value that has its severity value set to SEVERITY_SUCCESS and its code value
		 * set to the offset of the largest command identifier that was assigned, plus one.
		 *
		 * For example, assume that idCmdFirst is set to 5 and you add three items to the menu with
		 * command identifiers of 5, 7, and 8.
		 *
		 * The return value should be MAKE_HRESULT(SEVERITY_SUCCESS, 0, 8 - 5 + 1). Otherwise, it returns an OLE error value.
		 */
		return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, numItems);
	}
	IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO lpici) {
		// No need to support string based command.
		if (!HIWORD(lpici->lpVerb) && _pdtobj) {
			auto [hr, files] = _getDroppedFiles(_pdtobj);
			if (SUCCEEDED(hr)) {
				UINT idCmd = LOWORD(lpici->lpVerb);

				if (!_cmdArgs.empty() && idCmd >= 0 && idCmd < _cmdArgs.size()) {
					_executeCommand(_cmdArgs[idCmd], files);
					return NOERROR;
				}
			}
		}
		return E_INVALIDARG;
	}
	IFACEMETHODIMP GetCommandString(UINT_PTR /*idCmd*/, UINT /*uType*/, UINT* /*pRes*/, LPSTR /*pszName*/, UINT /*cchMax*/) { return E_NOTIMPL; }

	// IShellExtInit
	IFACEMETHODIMP Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID) {
		// Initialize can be called more than once.
		SafeRelease(&_pdtobj);
		_targetFolder.clear();

		HRESULT hr = pdtobj->QueryInterface(&_pdtobj);
		// Get the path to the drop target folder
		if (SUCCEEDED(hr) && pidlFolder) {
			wchar_t buf[MAX_PATH * 2] = {};
			hr = SHGetPathFromIDListW(pidlFolder, buf);
			_targetFolder = buf;
		}
		return hr;
	}

private:
	virtual ~CLFMenuExtension() {
		SafeRelease(&_pdtobj);
		DllRelease();
	}

	int _insertMenuItems(HMENU hMenu, UINT idCmdFirst, const std::vector<SHELL_MENU_ITEM> menuItem) {
		int idx = 0;
		for (const auto& item: menuItem) {
			auto caption = UtilLoadString(item.wCaption);
			if (item.children.empty()) {
				WORD menuID = idCmdFirst + (WORD)_cmdArgs.size();
				UtilInsertMenuItem(hMenu, caption.c_str(), idx, NULL, menuID);
				_cmdArgs.push_back(item.param);
			} else {
				HMENU hSubMenu = ::CreatePopupMenu();
				UtilInsertMenuItem(hMenu, caption.c_str(), idx, hSubMenu, 0);
				_cmdArgs.push_back(item.param);	//could be dummy

				_insertMenuItems(hSubMenu, idCmdFirst, item.children);
			}
			idx++;
		}
		return (int)_cmdArgs.size();
	}

	static std::pair<HRESULT, std::vector<std::filesystem::path> > _getDroppedFiles(IDataObject* lpDataObject) {
		FORMATETC fmt;
		STGMEDIUM medium;

		fmt.cfFormat = CF_HDROP;
		fmt.ptd = NULL;
		fmt.dwAspect = DVASPECT_CONTENT;
		fmt.lindex = -1;
		fmt.tymed = TYMED_HGLOBAL;

		std::vector<std::filesystem::path> files;

		HRESULT hr = lpDataObject->GetData(&fmt, &medium);
		if (S_OK == hr) {
			//files dropped
			UINT nFileCount = ::DragQueryFileW((HDROP)medium.hGlobal, 0xFFFFFFFF, NULL, 0);

			//get files
			for (UINT i = 0; i < nFileCount; i++) {
				auto size = ::DragQueryFileW((HDROP)medium.hGlobal, i, nullptr, 0) + 1;
				std::vector<wchar_t> szBuffer(size);
				::DragQueryFileW((HDROP)medium.hGlobal, i, &szBuffer[0], size);
				files.push_back(&szBuffer[0]);
			}
			::ReleaseStgMedium(&medium);
		}
		return { hr, files };
	}
	void _executeCommand(const std::wstring& arg, const std::vector<std::filesystem::path>& subjects) {
		std::filesystem::path exePath;
		{
			exePath = UtilGetModulePath(g_hinst);
			//executable in the same directory
			exePath = exePath.parent_path();
			exePath /= L"LhaForge.exe";
		}

		std::wstring strCommandLine;
		strCommandLine += arg;
		if (!_isContextMenu) {
			//drag & drop
			strCommandLine += L" /o:\"";
			strCommandLine += _targetFolder.make_preferred().wstring();
			strCommandLine += L"\" ";
		}

		//make filelist
		{
			auto tempPath = UtilGetTemporaryFileName();
			CAutoFile fp;
			fp.open(tempPath, L"wb");
			for(const auto& fname: subjects){
				fwprintf_s(fp, L"%s\n", fname.c_str());
			}

			strCommandLine += L" /cp:UNICODE";

			strCommandLine += L" /$:\"";
			strCommandLine += tempPath.wstring();
			strCommandLine += L"\"";
		}

		//run
		UtilExecuteCommand(exePath, strCommandLine);
	}

};


//---------------

HRESULT CreateInstance_ContextMenu(REFIID riid, void** ppv)
{
	*ppv = NULL;
	auto pnddmv = new (std::nothrow) CLFMenuExtension(true);
	HRESULT hr = pnddmv ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr)) {
		hr = pnddmv->QueryInterface(riid, ppv);
		pnddmv->Release();
	}
	return hr;
}

HRESULT CreateInstance_DragMenu(REFIID riid, void** ppv)
{
	*ppv = NULL;
	auto pnddmv = new (std::nothrow) CLFMenuExtension(false);
	HRESULT hr = pnddmv ? S_OK : E_OUTOFMEMORY;
	if (SUCCEEDED(hr)) {
		hr = pnddmv->QueryInterface(riid, ppv);
		pnddmv->Release();
	}
	return hr;
}

