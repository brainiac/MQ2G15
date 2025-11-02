
#include "pch.h"
#include "LCDSDK/lglcd.h"

bool g_lgLcdLibIsInitialized = false;

HMODULE g_hModule = nullptr;

struct LGLCDInterface
{
	using pfnInitFunc = DWORD(WINAPI*)();
	using pfnShutdownFunc = DWORD(WINAPI*)();
	using pfnLcdConnectW = DWORD(WINAPI*)(IN OUT lgLcdConnectContextW*);
	using pfnLcdConnectA = DWORD(WINAPI*)(IN OUT lgLcdConnectContextA*);
	using pfnLcdConnectExW = DWORD(WINAPI*)(IN OUT lgLcdConnectContextExW*);
	using pfnLcdConnectExA = DWORD(WINAPI*)(IN OUT lgLcdConnectContextExA*);
	using pfnLcdDisconnect = DWORD(WINAPI*)(int);
	using pfnLcdSetDeviceFamiliesToUse = DWORD(WINAPI*)(IN int, DWORD, DWORD);
	using pfnLcdEnumerate = DWORD(WINAPI*)(IN int, IN int, OUT lgLcdDeviceDesc*);
	using pfnLcdEnumerateExW = DWORD(WINAPI*)(IN int, IN int, OUT lgLcdDeviceDescExW*);
	using pfnLcdEnumerateExA = DWORD(WINAPI*)(IN int, IN int, OUT lgLcdDeviceDescExA*);
	using pfnLcdOpen = DWORD(WINAPI*)(IN OUT lgLcdOpenContext*);
	using pfnLcdOpenByType = DWORD(WINAPI*)(IN OUT lgLcdOpenByTypeContext*);
	using pfnLcdClose = DWORD(WINAPI*)(IN int device);
	using pfnLcdReadSoftButtons = DWORD(WINAPI*)(IN int device, OUT DWORD*);
	using pfnLcdUpdateBitmap = DWORD(WINAPI*)(IN int, IN const lgLcdBitmapHeader*, IN DWORD);
	using pfnLcdSetAsLCDForegroundApp = DWORD(WINAPI*)(IN int, IN int);

	/*0x00*/ pfnInitFunc Init;
	/*0x04*/ pfnShutdownFunc DeInit;
	/*0x08*/ pfnLcdConnectW ConnectW;
	/*0x0c*/ pfnLcdConnectA ConnectA;
	/*0x10*/ pfnLcdConnectExW ConnectExW;
	/*0x14*/ pfnLcdConnectExA ConnectExA;
	/*0x18*/ pfnLcdDisconnect Disconnect;
	/*0x1c*/ pfnLcdEnumerate Enumerate;
	/*0x20*/ pfnLcdEnumerateExW EnumerateExW;
	/*0x24*/ pfnLcdEnumerateExA EnumerateExA;
	/*0x28*/ pfnLcdOpen Open;
	/*0x2c*/ pfnLcdClose Close;
	/*0x30*/ pfnLcdReadSoftButtons ReadSoftButtons;
	/*0x34*/ pfnLcdUpdateBitmap UpdateBitmap;
	/*0x38*/ void* Unknown0x38;
	/*0x3c*/ void* Unknown0x3c;
	/*0x40*/ void* Unknown0x40;
	/*0x44*/ void* Unknown0x44;
	/*0x48*/ void* Unknown0x48;
	/*0x4c*/ void* Unknown0x4c;
	/*0x50*/ void* Unknown0x50;
	/*0x54*/ void* Unknown0x54;
	/*0x58*/ void* Unknown0x58;
	/*0x5c*/ void* Unknown0x5c;
	/*0x60*/ pfnLcdSetAsLCDForegroundApp SetAsLCDForegroundApp;
	/*0x64*/ pfnLcdSetDeviceFamiliesToUse SetDeviceFamiliesToUse;
	/*0x68*/ pfnLcdOpenByType OpenByType;
};

using pfnGetInterface = LGLCDInterface* (WINAPI*)(int);
pfnGetInterface linkedRemoteGetInterface = nullptr;

LGLCDInterface* g_lgLcdLibLinkedRemoteInterface = nullptr;

static BOOL UnlinkLgLcdDLL()
{
	if (g_hModule)
	{
		FreeLibrary(g_hModule);
		g_hModule = nullptr;
	}

	g_lgLcdLibLinkedRemoteInterface = nullptr;
	return TRUE;
}

static BOOL LinkLgLcdDLL()
{
	const char* clsidPath = R"(SOFTWARE\Classes\CLSID\{FE750200-B72E-11d9-829B-0050DA1A72D3}\ServerBinary)";
	HKEY hkResult;

	static char szPathToDLL[MAX_PATH];
	BOOL success = FALSE;

	if (SUCCEEDED(RegOpenKeyExA(HKEY_LOCAL_MACHINE, clsidPath, 0, KEY_READ, &hkResult)))
	{
		DWORD cbData = MAX_PATH;
		DWORD dwType;
		if (SUCCEEDED(RegQueryValueExA(hkResult, 0, 0, &dwType, (LPBYTE)szPathToDLL, &cbData)))
		{
			g_hModule = LoadLibraryA(szPathToDLL);
			if (g_hModule)
			{
				linkedRemoteGetInterface = (pfnGetInterface)GetProcAddress(g_hModule, "GetInterface");

				if (!linkedRemoteGetInterface)
				{
					UnlinkLgLcdDLL();
				}
				else
				{
					g_lgLcdLibLinkedRemoteInterface = linkedRemoteGetInterface(5);
					if (!g_lgLcdLibLinkedRemoteInterface)
					{
						UnlinkLgLcdDLL();
					}
					else
					{
						success = TRUE;
					}
				}

			}
		}

		RegCloseKey(hkResult);
	}

	return success;
}


// Shims for SDK compatibility
DWORD WINAPI lgLcdInit()
{
	if (g_lgLcdLibIsInitialized)
	{
		return ERROR_ALREADY_INITIALIZED;
	}

	if (!LinkLgLcdDLL())
	{
		return RPC_S_SERVER_UNAVAILABLE;
	}

	g_lgLcdLibIsInitialized = true;
	return g_lgLcdLibLinkedRemoteInterface->Init();
}

DWORD WINAPI lgLcdDeInit()
{
	if (!g_lgLcdLibIsInitialized)
		return 0;

	DWORD result = g_lgLcdLibLinkedRemoteInterface->DeInit();
	UnlinkLgLcdDLL();
	return result;
}

DWORD WINAPI lgLcdConnectW(IN OUT lgLcdConnectContextW* ctx)
{
	if (!g_lgLcdLibIsInitialized)
		return ERROR_SERVICE_NOT_ACTIVE;

	return g_lgLcdLibLinkedRemoteInterface->ConnectW(ctx);
}

DWORD WINAPI lgLcdConnectA(IN OUT lgLcdConnectContextA* ctx)
{
	if (!g_lgLcdLibIsInitialized)
		return ERROR_SERVICE_NOT_ACTIVE;

	return g_lgLcdLibLinkedRemoteInterface->ConnectA(ctx);
}

DWORD WINAPI lgLcdConnectExW(IN OUT lgLcdConnectContextExW* ctx)
{
	if (!g_lgLcdLibIsInitialized)
		return ERROR_SERVICE_NOT_ACTIVE;

	return g_lgLcdLibLinkedRemoteInterface->ConnectExW(ctx);
}

DWORD WINAPI lgLcdConnectExA(IN OUT lgLcdConnectContextExA* ctx)
{
	if (!g_lgLcdLibIsInitialized)
		return ERROR_SERVICE_NOT_ACTIVE;

	return g_lgLcdLibLinkedRemoteInterface->ConnectExA(ctx);
}

DWORD WINAPI lgLcdDisconnect(int connection)
{
	if (!g_lgLcdLibIsInitialized)
		return ERROR_SERVICE_NOT_ACTIVE;

	return g_lgLcdLibLinkedRemoteInterface->Disconnect(connection);
}

DWORD WINAPI lgLcdSetDeviceFamiliesToUse(int connection, DWORD dwDeviceFamiliesSupported, DWORD dwReserved1)
{
	if (!g_lgLcdLibIsInitialized)
		return ERROR_SERVICE_NOT_ACTIVE;

	return g_lgLcdLibLinkedRemoteInterface->SetDeviceFamiliesToUse(connection, dwDeviceFamiliesSupported, dwReserved1);
}

DWORD WINAPI lgLcdEnumerate(int connection, int index, lgLcdDeviceDesc* description)
{
	if (!g_lgLcdLibIsInitialized)
		return ERROR_SERVICE_NOT_ACTIVE;

	return g_lgLcdLibLinkedRemoteInterface->Enumerate(connection, index, description);
}

DWORD WINAPI lgLcdEnumerateExW(int connection, int index, lgLcdDeviceDescExW* description)
{
	if (!g_lgLcdLibIsInitialized)
		return ERROR_SERVICE_NOT_ACTIVE;

	return g_lgLcdLibLinkedRemoteInterface->EnumerateExW(connection, index, description);
}

DWORD WINAPI lgLcdEnumerateExA(int connection, int index, lgLcdDeviceDescExA* description)
{
	if (!g_lgLcdLibIsInitialized)
		return ERROR_SERVICE_NOT_ACTIVE;

	return g_lgLcdLibLinkedRemoteInterface->EnumerateExA(connection, index, description);
}

DWORD WINAPI lgLcdOpen(lgLcdOpenContext* ctx)
{
	if (!g_lgLcdLibIsInitialized)
		return ERROR_SERVICE_NOT_ACTIVE;

	return g_lgLcdLibLinkedRemoteInterface->Open(ctx);
}

DWORD WINAPI lgLcdOpenByType(lgLcdOpenByTypeContext* ctx)
{
	if (!g_lgLcdLibIsInitialized)
		return ERROR_SERVICE_NOT_ACTIVE;

	return g_lgLcdLibLinkedRemoteInterface->OpenByType(ctx);
}

DWORD WINAPI lgLcdClose(int device)
{
	if (!g_lgLcdLibIsInitialized)
		return ERROR_SERVICE_NOT_ACTIVE;

	return g_lgLcdLibLinkedRemoteInterface->Close(device);
}

// Reads the state of the soft buttons for the device.
DWORD WINAPI lgLcdReadSoftButtons(int device, DWORD* buttons)
{
	if (!g_lgLcdLibIsInitialized)
		return ERROR_SERVICE_NOT_ACTIVE;

	return g_lgLcdLibLinkedRemoteInterface->ReadSoftButtons(device, buttons);
}

DWORD WINAPI lgLcdUpdateBitmap(int device, const lgLcdBitmapHeader* bitmap, DWORD priority)
{
	if (!g_lgLcdLibIsInitialized)
		return ERROR_SERVICE_NOT_ACTIVE;

	return g_lgLcdLibLinkedRemoteInterface->UpdateBitmap(device, bitmap, priority);
}

DWORD WINAPI lgLcdSetAsLCDForegroundApp(int device, int foregroundYesNoFlag)
{
	if (!g_lgLcdLibIsInitialized)
		return ERROR_SERVICE_NOT_ACTIVE;

	return g_lgLcdLibLinkedRemoteInterface->SetAsLCDForegroundApp(device, foregroundYesNoFlag);
}
