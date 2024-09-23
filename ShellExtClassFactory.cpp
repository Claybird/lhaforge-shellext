//https://learn.microsoft.com/en-us/windows/win32/shell/samples-nondefaultdropmenuverb

#include "priv.h"

//-----
long g_cRefModule = 0;

// Handle the the DLL's module
HINSTANCE g_hinst = NULL;

// Standard DLL functions
STDAPI_(BOOL) DllMain(HINSTANCE hInstance, DWORD dwReason, void*)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		_wsetlocale(LC_ALL, L"");
		g_hinst = hInstance;
		DisableThreadLibraryCalls(hInstance);
	}
	return TRUE;
}

STDAPI DllCanUnloadNow()
{
	// Only allow the DLL to be unloaded after all outstanding references have been released
	return (g_cRefModule == 0) ? S_OK : S_FALSE;
}

void DllAddRef()
{
	InterlockedIncrement(&g_cRefModule);
}

void DllRelease()
{
	InterlockedDecrement(&g_cRefModule);
}

STDAPI DllRegisterServer()
{
	int count = 0;
	if (S_OK == UtilRegisterServer(g_hinst, CLSID_LFContextMenu))count++;
	if (S_OK == UtilRegisterServer(g_hinst, CLSID_LFDragMenu))count++;
	return (count == 2) ? S_OK : E_FAIL;
}

STDAPI DllUnregisterServer()
{
	int count = 0;
	if (S_OK == UtilUnregisterServer(CLSID_LFContextMenu))count++;
	if (S_OK == UtilUnregisterServer(CLSID_LFDragMenu))count++;
	return (count == 2) ? S_OK : E_FAIL;
}


//--------


typedef HRESULT(*PFNCREATEINSTANCE)(REFIID riid, void** ppvObject);
struct CLASS_OBJECT_INIT
{
	const CLSID* pClsid;
	PFNCREATEINSTANCE pfnCreate;
};

// add classes supported by this module here
const CLASS_OBJECT_INIT c_rgClassObjectInit[] =
{
	{ &CLSID_LFContextMenu, CreateInstance_ContextMenu},
	{ &CLSID_LFDragMenu, CreateInstance_DragMenu },
};

class CLFClassFactory : public IClassFactory
{
public:
	static HRESULT CreateInstance(REFCLSID clsid, const CLASS_OBJECT_INIT* pClassObjectInits, size_t cClassObjectInits, REFIID riid, void** ppv)
	{
		*ppv = NULL;
		HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;
		for (size_t i = 0; i < cClassObjectInits; i++) {
			if (clsid == *pClassObjectInits[i].pClsid) {
				IClassFactory* pClassFactory = new (std::nothrow) CLFClassFactory(pClassObjectInits[i].pfnCreate);
				hr = pClassFactory ? S_OK : E_OUTOFMEMORY;
				if (SUCCEEDED(hr)) {
					hr = pClassFactory->QueryInterface(riid, ppv);
					pClassFactory->Release();
				}
				break; // match found
			}
		}
		return hr;
	}

	CLFClassFactory(PFNCREATEINSTANCE pfnCreate) : _cRef(1), _pfnCreate(pfnCreate) {
		DllAddRef();
	}

	// IUnknown
	IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) {
		static const QITAB qit[] =
		{
			QITABENT(CLFClassFactory, IClassFactory),
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

	// IClassFactory
	IFACEMETHODIMP CreateInstance(IUnknown* punkOuter, REFIID riid, void** ppv) {
		return punkOuter ? CLASS_E_NOAGGREGATION : _pfnCreate(riid, ppv);
	}

	IFACEMETHODIMP LockServer(BOOL fLock) {
		if (fLock) {
			DllAddRef();
		} else {
			DllRelease();
		}
		return S_OK;
	}

private:
	~CLFClassFactory() {
		DllRelease();
	}

	long _cRef;
	PFNCREATEINSTANCE _pfnCreate;
};

STDAPI DllGetClassObject(REFCLSID clsid, REFIID riid, void** ppv)
{
	return CLFClassFactory::CreateInstance(
		clsid,c_rgClassObjectInit, ARRAYSIZE(c_rgClassObjectInit), riid, ppv);
}

