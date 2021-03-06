// WfdDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "combaseapi.h"

#include "..\..\..\..\..\..\WCL7\CPP\Source\WiFi\wclWiFi.h"

#ifdef _WIN64
#pragma comment(lib, "..\\..\\..\\..\\..\\..\\WCL7\\CPP\\Lib\\2017\\x64\\wclWiFiFramework.lib")
#else
#pragma comment(lib, "..\\..\\..\\..\\..\\..\\WCL7\\CPP\\Lib\\2017\\x32\\wclWiFiFramework.lib")
#endif

using namespace wclCommon;
using namespace wclWiFi;

typedef void(__stdcall *DEVICE_EVENT)(void* Ap, void* Device);
typedef void(__stdcall *AP_EVENT)(void* Ap);

class CSoftAp : public CwclWiFiSoftAP
{
	DISABLE_COPY(CSoftAp);

private:
	AP_EVENT FOnStarted;
	AP_EVENT FOnStopped;
	DEVICE_EVENT FOnConnected;
	DEVICE_EVENT FOnDisconnected;

protected:
	virtual void DoStarted() override
	{
		if (FOnStarted != NULL)
			FOnStarted(this);
	}

	virtual void DoStopped() override
	{
		if (FOnStopped != NULL)
			FOnStopped(this);
	}

	virtual void DoDeviceConnected(CwclWiFiDirectDevice* Device) override
	{
		if (FOnConnected != NULL)
			FOnConnected(this, Device);
	}

	virtual void DoDeviceDisconnected(CwclWiFiDirectDevice* Device) override
	{
		if (FOnDisconnected != NULL)
			FOnDisconnected(this, Device);
	}

public:
	CSoftAp(AP_EVENT OnStarted, AP_EVENT OnStopped, DEVICE_EVENT OnConnected, DEVICE_EVENT OnDisconnected)
		: CwclWiFiSoftAP()
	{
		FOnStarted = OnStarted;
		FOnStopped = OnStopped;
		FOnConnected = OnConnected;
		FOnDisconnected = OnDisconnected;
	}
};

extern "C" __declspec(dllexport) void* __stdcall CreateSoftAp(AP_EVENT OnStarted, AP_EVENT OnStopped,
	DEVICE_EVENT OnConnected, DEVICE_EVENT OnDisconnected)
{
	return new CSoftAp(OnStarted, OnStopped, OnConnected, OnDisconnected);
}

extern "C" __declspec(dllexport) void __stdcall DestroySoftAp(void* o)
{
	if (o != NULL)
		delete ((CSoftAp*)o);
}

extern "C" __declspec(dllexport) int __stdcall StartSoftAp(void* o, LPWSTR Ssid, LPWSTR Passphrase)
{
	if (o != NULL)
		return ((CSoftAp*)o)->Start(Ssid, Passphrase);
	return WCL_E_INVALID_ARGUMENT;
}

extern "C" __declspec(dllexport) int __stdcall StopSoftAp(void* o)
{
	if (o != NULL)
		return ((CSoftAp*)o)->Stop();
	return WCL_E_INVALID_ARGUMENT;
}

extern "C" __declspec(dllexport) int __stdcall GetSoftApPassphrase(void* o, void* pPassphrase)
{
	ZeroMemory(pPassphrase, 512);
	if (o != NULL)
	{
		tstring s;
		int Res = ((CSoftAp*)o)->GetPassphrase(s);
		if (Res == WCL_E_SUCCESS)
			CopyMemory(pPassphrase, s.c_str(), s.length() * sizeof(WCHAR));
		return Res;
	}
	return WCL_E_INVALID_ARGUMENT;
}

extern "C" __declspec(dllexport) int __stdcall GetSoftApSsid(void* o, void* pSsid)
{
	ZeroMemory(pSsid, 512);
	if (o != NULL)
	{
		tstring s;
		int Res = ((CSoftAp*)o)->GetSsid(s);
		if (Res == WCL_E_SUCCESS)
			CopyMemory(pSsid, s.c_str(), s.length() * sizeof(WCHAR));
		return Res;
	}
	return WCL_E_INVALID_ARGUMENT;
}

extern "C" __declspec(dllexport) BOOL __stdcall GetSoftApActive(void* o)
{
	if (o != NULL)
		return ((CSoftAp*)o)->GetActive();
	return FALSE;
}

extern "C" __declspec(dllexport) void __stdcall GetDeviceLocalMac(void* o, void* pMac)
{
	ZeroMemory(pMac, 512);

	if (o != NULL)
	{
		tstring s = ((CwclWiFiDirectDevice*)o)->GetLocalAddress();
		CopyMemory(pMac, s.c_str(), s.length() * sizeof(WCHAR));
	}
}

extern "C" __declspec(dllexport) void __stdcall GetDeviceRemoteMac(void* o, void* pMac)
{
	ZeroMemory(pMac, 512);

	if (o != NULL)
	{
		tstring s = ((CwclWiFiDirectDevice*)o)->GetRemoteAddress();
		CopyMemory(pMac, s.c_str(), s.length() * sizeof(WCHAR));
	}
}

extern "C" __declspec(dllexport) int __stdcall DisconnectDevice(void* o)
{
	if (o != NULL)
		return ((CwclWiFiDirectDevice*)o)->Disconnect();
	return WCL_E_INVALID_ARGUMENT;
}