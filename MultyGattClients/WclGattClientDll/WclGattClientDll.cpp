// WclGattClientDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "combaseapi.h"

// Change the lines below to provide path to the Bluetooth Framework headers.
#include "..\..\..\..\..\WCL7\CPP\Source\Bluetooth\wclBluetooth.h"
#include "..\..\..\..\..\WCL7\CPP\Source\Common\wclErrors.h"
#include "..\..\..\..\..\WCL7\CPP\Source\Common\wclHelpers.h"

#pragma comment(lib, "ws2_32.lib")

// Change the line below to provide path to the Bluetooth Framework lib.
#pragma comment(lib, "..\\..\\..\\..\\..\\WCL7\\CPP\\Lib\\2017\\x64\\wclBluetoothFramework.lib")

using namespace wclCommon;
using namespace wclCommunication;
using namespace wclBluetooth;

// Helper exports.
extern "C"
{
	__declspec(dllexport) DWORD __stdcall AlertableWait(HANDLE Handle, BOOL Infinite)
	{
		DWORD Wait = 0;
		if (Infinite)
			Wait = INFINITE;
		return WaitForMultipleObjectsEx(1, &Handle, FALSE, Wait, TRUE);
	}

	__declspec(dllexport) void __stdcall ProcessApc()
	{
		SleepEx(0, TRUE);
	}

	__declspec(dllexport) void __stdcall SetApcSync()
	{
		CwclMessageBroadcaster::SetSyncMethod(skApc);
	}
}

// Event callbacks for Bluetooth Manager.
typedef void(__stdcall *NOTIFY_EVENT)(void* sender);
typedef void(__stdcall *DEVICE_FOUND_EVENT)(void* sender, CwclBluetoothRadio* Radio, __int64 Address);
typedef void(__stdcall *DISCOVERING_STARTED_EVENT)(void* sender, CwclBluetoothRadio* Radio);
typedef void(__stdcall *DISCOVERING_COMPLETED_EVENT)(void* sender, CwclBluetoothRadio* Radio, int Error);

// We need this wrapper to be able to setup event handlers from the c# code.
class CBluetoothManager : public CwclBluetoothManager
{
	DISABLE_COPY(CBluetoothManager);

private:
	NOTIFY_EVENT				FAfterOpen;
	NOTIFY_EVENT				FBeforeClose;

	DEVICE_FOUND_EVENT			FOnDeviceFound;
	DISCOVERING_STARTED_EVENT	FOnDiscoveringStarted;
	DISCOVERING_COMPLETED_EVENT	FOnDiscoveringCompleted;

protected:
	virtual void DoAfterOpen() override
	{
		if (FAfterOpen != NULL)
			FAfterOpen(this);
	}

	virtual void DoBeforeClose() override
	{
		if (FBeforeClose != NULL)
			FBeforeClose(this);
	}

	virtual void DoDeviceFound(CwclBluetoothRadio* Radio, __int64 Address) override
	{
		if (FOnDeviceFound != NULL)
			FOnDeviceFound(this, Radio, Address);
	}

	virtual void DoDiscoveringStarted(CwclBluetoothRadio* Radio) override
	{
		if (FOnDiscoveringStarted != NULL)
			FOnDiscoveringStarted(this, Radio);
	}

	virtual void DoDiscoveringCompleted(CwclBluetoothRadio* Radio, int Error) override
	{
		if (FOnDiscoveringCompleted != NULL)
			FOnDiscoveringCompleted(this, Radio, Error);
	}

public:
	CBluetoothManager()
		: CwclBluetoothManager()
	{
		FAfterOpen = NULL;
		FBeforeClose = NULL;
		FOnDeviceFound = NULL;
		FOnDiscoveringStarted = NULL;
		FOnDiscoveringCompleted = NULL;
	}

	void SetAfterOpen(NOTIFY_EVENT Event)
	{
		FAfterOpen = Event;
	}

	void SetBeforeClose(NOTIFY_EVENT Event)
	{
		FBeforeClose = Event;
	}

	void SetOnDeviceFound(DEVICE_FOUND_EVENT Event)
	{
		FOnDeviceFound = Event;
	}

	void SetOnDiscoveringStarted(DISCOVERING_STARTED_EVENT Event)
	{
		FOnDiscoveringStarted = Event;
	}

	void SetOnDiscoveringCompleted(DISCOVERING_COMPLETED_EVENT Event)
	{
		FOnDiscoveringCompleted = Event;
	}
};

// Bluetooth Manager exports.
extern "C"
{
	__declspec(dllexport) CBluetoothManager* __stdcall ManagerCreate()
	{
		return new CBluetoothManager();
	}

	__declspec(dllexport) void __stdcall ManagerDestroy(CBluetoothManager* Manager)
	{
		if (Manager != NULL)
			delete Manager;
	}

	__declspec(dllexport) int __stdcall ManagerOpen(CBluetoothManager* Manager)
	{
		if (Manager == NULL)
			return WCL_E_INVALID_ARGUMENT;
		return Manager->Open();
	}

	__declspec(dllexport) int __stdcall ManagerClose(CBluetoothManager* Manager)
	{
		if (Manager == NULL)
			return WCL_E_INVALID_ARGUMENT;
		return Manager->Close();
	}

	__declspec(dllexport) unsigned int ManagerGetRadioCount(CBluetoothManager* Manager)
	{
		if (Manager == NULL)
			return 0;
		return (unsigned int)Manager->GetCount();
	}

	__declspec(dllexport) CwclBluetoothRadio* ManagerGetRadio(CBluetoothManager* Manager,
		unsigned int Index)
	{
		if (Manager == NULL)
			return NULL;
		if (Index >= Manager->GetCount())
			return NULL;
		return Manager->GetRadios(Index);
	}

	__declspec(dllexport) void __stdcall ManagerSetAfterOpen(CBluetoothManager* Manager,
		NOTIFY_EVENT Event)
	{
		if (Manager != NULL)
			Manager->SetAfterOpen(Event);
	}

	__declspec(dllexport) void __stdcall ManagerSetBeforeClose(CBluetoothManager* Manager,
		NOTIFY_EVENT Event)
	{
		if (Manager != NULL)
			Manager->SetBeforeClose(Event);
	}

	__declspec(dllexport) void __stdcall ManagerSetOnDeviceFound(CBluetoothManager* Manager,
		DEVICE_FOUND_EVENT Event)
	{
		if (Manager != NULL)
			Manager->SetOnDeviceFound(Event);
	}

	__declspec(dllexport) void __stdcall ManagerSetOnDiscoveringStarted(CBluetoothManager* Manager,
		DISCOVERING_STARTED_EVENT Event)
	{
		if (Manager != NULL)
			Manager->SetOnDiscoveringStarted(Event);
	}

	__declspec(dllexport) void __stdcall ManagerSetOnDiscoveringCompleted(CBluetoothManager* Manager,
		DISCOVERING_COMPLETED_EVENT Event)
	{
		if (Manager != NULL)
			Manager->SetOnDiscoveringCompleted(Event);
	}

	__declspec(dllexport) int RadioDiscover(CwclBluetoothRadio* Radio, unsigned char Timeout)
	{
		if (Radio == NULL)
			return WCL_E_INVALID_ARGUMENT;
		return Radio->Discover(Timeout, dkBle);
	}

	__declspec(dllexport) int RadioTerminate(CwclBluetoothRadio* Radio)
	{
		if (Radio == NULL)
			return WCL_E_INVALID_ARGUMENT;
		return Radio->Terminate();
	}

	__declspec(dllexport) BOOL RadioIsAvailable(CwclBluetoothRadio* Radio)
	{
		if (Radio == NULL)
			return FALSE;
		return Radio->GetAvailable();
	}

	__declspec(dllexport) int __stdcall RadioGetDeviceName(CwclBluetoothRadio* Radio, __int64 Address, LPTSTR Name, int Len)
	{
		if (Radio == NULL || Name == NULL || Len == 0)
			return WCL_E_INVALID_ARGUMENT;

		tstring TmpName;
		int res = Radio->GetRemoteName(Address, TmpName);

		if (res == WCL_E_SUCCESS && TmpName.length() > 0)
		{
			if (TmpName.length() > Len)
				return WCL_E_OUT_OF_MEMORY;
			_tcscpy_s(Name, Len, TmpName.c_str());
		}

		return res;
	}

	__declspec(dllexport) int __stdcall RadioRemoteUnpair(CwclBluetoothRadio* Radio, __int64 Address)
	{
		if (Radio == NULL)
			return WCL_E_INVALID_ARGUMENT;

		return Radio->RemoteUnpair(Address);
	}
}

// GATT Client callbacks.
typedef void(__stdcall *GATTCLIENT_CONNECT)(void* sender, int Error);
typedef void(__stdcall *GATTCLIENT_DISCONNECT)(void* sender, int Reason);
typedef void(__stdcall *GATTCLIENT_ONCHANGED)(void* sender, unsigned short Handle,
	unsigned char* Value, unsigned int ValueLen);

// We have to re-define GATT data types.
#pragma pack(push, 1)
typedef struct
{
	BOOL			IsShortUuid;
	unsigned short	ShortUuid;
	GUID			LongUuid;
} GattUuid;

typedef struct
{
	GattUuid		Uuid;
	unsigned short	Handle;
} GattService;

typedef struct
{
	unsigned char	Count;
	GattService		Services[255];
} GattServices;

typedef struct
{
	unsigned short	ServiceHandle;
	GattUuid		Uuid;
	unsigned short	Handle;
	unsigned short	ValueHandle;
	BOOL			IsBroadcastable;
	BOOL			IsReadable;
	BOOL			IsWritable;
	BOOL			IsWritableWithoutResponse;
	BOOL			IsSignedWritable;
	BOOL			IsNotifiable;
	BOOL			IsIndicatable;
	BOOL			HasExtendedProperties;
} GattCharacteristic;

typedef struct
{
	unsigned char		Count;
	GattCharacteristic	Chars[255];
} GattCharacteristics;
#pragma pack(pop)

// GATT client wrapper. We need this one for events.
class CGattClient : public CwclGattClient
{
	DISABLE_COPY(CGattClient);

private:
	GATTCLIENT_CONNECT		FOnConnect;
	GATTCLIENT_DISCONNECT	FOnDisconnect;
	GATTCLIENT_ONCHANGED	FOnChanged;

protected:
	virtual void DoConnect(int Error) override
	{
		if (FOnConnect != NULL)
			FOnConnect(this, Error);
	}

	virtual void DoDisconnect(int Reason) override
	{
		if (FOnDisconnect != NULL)
			FOnDisconnect(this, Reason);
	}

	virtual void DoCharacteristicChanged(unsigned short Handle,
		wclGattCharacteristicValue& Value) override
	{
		if (FOnChanged != NULL)
			FOnChanged(this, Handle, Value.empty() ? NULL : &Value.front(), (unsigned int)Value.size());
	}

public:
	CGattClient()
		: CwclGattClient()
	{
		FOnConnect = NULL;
		FOnDisconnect = NULL;
		FOnChanged = NULL;
	}

	void SetOnConnect(GATTCLIENT_CONNECT Event)
	{
		FOnConnect = Event;
	}

	void SetOnDisconnect(GATTCLIENT_DISCONNECT Event)
	{
		FOnDisconnect = Event;
	}

	void SetOnChanged(GATTCLIENT_ONCHANGED Event)
	{
		FOnChanged = Event;
	}
};

// GATT client helper functions
void GattClientCopyChar(GattCharacteristic* In, wclGattCharacteristic* Out)
{
	Out->Handle = In->Handle;
	Out->HasExtendedProperties = In->HasExtendedProperties;
	Out->IsBroadcastable = In->IsBroadcastable;
	Out->IsIndicatable = In->IsIndicatable;
	Out->IsNotifiable = In->IsNotifiable;
	Out->IsReadable = In->IsReadable;
	Out->IsSignedWritable = In->IsSignedWritable;
	Out->IsWritable = In->IsWritable;
	Out->IsWritableWithoutResponse = In->IsWritableWithoutResponse;
	Out->ServiceHandle = In->ServiceHandle;
	Out->Uuid.IsShortUuid = In->Uuid.IsShortUuid;
	Out->Uuid.LongUuid = In->Uuid.LongUuid;
	Out->Uuid.ShortUuid = In->Uuid.ShortUuid;
	Out->ValueHandle = In->ValueHandle;
}

// GATT client exports.
extern "C"
{
	__declspec(dllexport) CGattClient* GattClientCreate()
	{
		return new CGattClient();
	}

	__declspec(dllexport) void GattClientDestroy(CGattClient* Client)
	{
		if (Client != NULL)
			delete Client;
	}

	__declspec(dllexport) void GattClientSetOnConnect(CGattClient* Client,
		GATTCLIENT_CONNECT Event)
	{
		if (Client != NULL)
			Client->SetOnConnect(Event);
	}

	__declspec(dllexport) void GattClientSetOnDisconnect(CGattClient* Client,
		GATTCLIENT_DISCONNECT Event)
	{
		if (Client != NULL)
			Client->SetOnDisconnect(Event);
	}

	__declspec(dllexport) void GattClientSetOnChanged(CGattClient* Client,
		GATTCLIENT_ONCHANGED Event)
	{
		if (Client != NULL)
			Client->SetOnChanged(Event);
	}

	__declspec(dllexport) int GattClientConnect(CGattClient* Client,
		CwclBluetoothRadio* Radio, __int64 Address)
	{
		if (Client == NULL)
			return WCL_E_INVALID_ARGUMENT;

		if (Client->GetState() != csDisconnected)
			return WCL_E_CONNECTION_ACTIVE;

		Client->SetAddress(Address);
		return Client->Connect(Radio);
	}

	__declspec(dllexport) int GattClientDisconnect(CGattClient* Client)
	{
		if (Client == NULL)
			return WCL_E_INVALID_ARGUMENT;

		return Client->Disconnect();
	}

	__declspec(dllexport) int GattClientGetServices(CGattClient* Client, GattServices* Services)
	{
		if (Services == NULL || Client == NULL)
			return WCL_E_INVALID_ARGUMENT;

		Services->Count = 0;

		wclGattServices Svcs;
		int Res = Client->ReadServices(goNone, Svcs);
		if (Res == WCL_E_SUCCESS)
		{
			if (Svcs.size() > 0 && Svcs.size() <= 255)
			{
				Services->Count = (unsigned char)Svcs.size();
				unsigned char n = 0;
				for (wclGattServices::iterator i = Svcs.begin(); i != Svcs.end(); i++)
				{
					wclGattService Svc = (*i);
					Services->Services[n].Handle = Svc.Handle;
					Services->Services[n].Uuid.IsShortUuid = Svc.Uuid.IsShortUuid;
					Services->Services[n].Uuid.LongUuid = Svc.Uuid.LongUuid;
					Services->Services[n].Uuid.ShortUuid = Svc.Uuid.ShortUuid;
					n++;
				}
			}
		}

		return Res;
	}

	__declspec(dllexport) int GattClientGetCharas(CGattClient* Client, GattService* Service,
		GattCharacteristics* Chars)
	{
		if (Client == NULL || Service == NULL || Chars == NULL)
			return WCL_E_INVALID_ARGUMENT;

		Chars->Count = 0;

		wclGattService Svc;
		Svc.Handle = Service->Handle;
		Svc.Uuid.IsShortUuid = Service->Uuid.IsShortUuid;
		Svc.Uuid.LongUuid = Service->Uuid.LongUuid;
		Svc.Uuid.ShortUuid = Service->Uuid.ShortUuid;

		wclGattCharacteristics Characteristics;
		int Res = Client->ReadCharacteristics(Svc, goNone, Characteristics);
		if (Res == WCL_E_SUCCESS)
		{
			if (Characteristics.size() > 0 && Characteristics.size() <= 255)
			{
				Chars->Count = (unsigned char)Characteristics.size();
				unsigned char n = 0;
				for (wclGattCharacteristics::iterator i = Characteristics.begin(); i != Characteristics.end(); i++)
				{
					wclGattCharacteristic c = (*i);
					Chars->Chars[n].Handle = c.Handle;
					Chars->Chars[n].HasExtendedProperties = c.HasExtendedProperties;
					Chars->Chars[n].IsBroadcastable = c.IsBroadcastable;
					Chars->Chars[n].IsIndicatable = c.IsIndicatable;
					Chars->Chars[n].IsNotifiable = c.IsNotifiable;
					Chars->Chars[n].IsReadable = c.IsReadable;
					Chars->Chars[n].IsSignedWritable = c.IsSignedWritable;
					Chars->Chars[n].IsWritable = c.IsWritable;
					Chars->Chars[n].IsWritableWithoutResponse = c.IsWritableWithoutResponse;
					Chars->Chars[n].ServiceHandle = c.ServiceHandle;
					Chars->Chars[n].Uuid.IsShortUuid = c.Uuid.IsShortUuid;
					Chars->Chars[n].Uuid.LongUuid = c.Uuid.LongUuid;
					Chars->Chars[n].Uuid.ShortUuid = c.Uuid.ShortUuid;
					Chars->Chars[n].ValueHandle = c.ValueHandle;
					n++;
				}
			}
		}

		return Res;
	}

	__declspec(dllexport) int GattClientSubscribe(CGattClient* Client, GattCharacteristic* Char)
	{
		if (Client == NULL || Char == NULL)
			return WCL_E_INVALID_ARGUMENT;

		wclGattCharacteristic Characteristic;
		GattClientCopyChar(Char, &Characteristic);

		int Res = Client->Subscribe(Characteristic);
		if (Res == WCL_E_SUCCESS)
		{
			Res = Client->WriteClientConfiguration(Characteristic, true, goReadFromDevice);
			if (Res != WCL_E_SUCCESS)
				Client->Unsubscribe(Characteristic);
		}

		return Res;
	}

	__declspec(dllexport) int GattClientUnsubscribe(CGattClient* Client, GattCharacteristic* Char)
	{
		if (Client == NULL || Char == NULL)
			return WCL_E_INVALID_ARGUMENT;

		wclGattCharacteristic Characteristic;
		GattClientCopyChar(Char, &Characteristic);

		int Res = Client->Unsubscribe(Characteristic);
		if (Res == WCL_E_SUCCESS)
			Res = Client->WriteClientConfiguration(Characteristic, false, goReadFromDevice);

		return Res;
	}

	__declspec(dllexport) void GattClientFreeMem(void* pMem)
	{
		if (pMem != NULL)
			LocalFree((HLOCAL)pMem);
	}

	__declspec(dllexport) int GattClientReadCharacteristicValue(CGattClient* Client, GattCharacteristic* Char,
		void** ppValue, unsigned int* pSize)
	{
		if (Client == NULL || Char == NULL || ppValue == NULL || pSize == NULL)
			return WCL_E_INVALID_ARGUMENT;

		*ppValue = NULL;
		*pSize = 0;

		wclGattCharacteristic Characteristic;
		GattClientCopyChar(Char, &Characteristic);

		wclGattCharacteristicValue Value;
		int Res = Client->ReadCharacteristicValue(Characteristic, goReadFromDevice, Value);

		if (Res == WCL_E_SUCCESS && Value.size() > 0)
		{
			*pSize = (unsigned int)Value.size();
			*ppValue = (void*)LocalAlloc(LPTR, *pSize);
			if (*ppValue == NULL)
			{
				*pSize = 0;
				return WCL_E_OUT_OF_MEMORY;

			}

			memcpy_s(*ppValue, *pSize, &Value.front(), *pSize);
		}

		return Res;
	}

	__declspec(dllexport) int GattClientWriteCharacteristicValue(CGattClient* Client, GattCharacteristic* Char,
		void* pValue, unsigned int Size)
	{
		if (Client == NULL || Char == NULL || pValue == NULL || Size == 0)
			return WCL_E_INVALID_ARGUMENT;

		wclGattCharacteristic Characteristic;
		GattClientCopyChar(Char, &Characteristic);

		wclGattCharacteristicValue Value;
		Value.resize(Size);
		memcpy_s(&Value.front(), Size, pValue, Size);
		return Client->WriteCharacteristicValue(Characteristic, Value);
	}

	__declspec(dllexport) wclClientState GattClientGetState(CGattClient* Client)
	{
		if (Client == NULL)
			return csDisconnected;

		return Client->GetState();
	}

	__declspec(dllexport) int GattClientWriteClientConfiguration(CGattClient* Client, GattCharacteristic* Char,
		BOOL Subscribe, wclGattOperationFlag Flag, wclGattProtectionLevel Protection)
	{
		if (Client == NULL || Char == NULL)
			return WCL_E_INVALID_ARGUMENT;

		wclGattCharacteristic Characteristic;
		GattClientCopyChar(Char, &Characteristic);

		return Client->WriteClientConfiguration(Characteristic, Subscribe, Flag, Protection);
	}
}

// Synchronization messages.
#define MSG_ID_CONNECT		1
#define MSG_ID_DISCONNECT	2
#define MSG_ID_CHANGED		3

class CResultMessage : public CwclMessage
{
	DISABLE_COPY(CResultMessage);

private:
	int FResult;

public:
	CResultMessage(unsigned char Id, int Result)
		: CwclMessage(Id, WCL_MSG_CATEGORY_USER)
	{
		FResult = Result;
	}

	int GetResult()
	{
		return FResult;
	}
};

class CConnectMessage : public CResultMessage
{
	DISABLE_COPY(CConnectMessage);

public:
	CConnectMessage(int Result)
		: CResultMessage(MSG_ID_CONNECT, Result)
	{
	}
};

class CDisconnectMessage : public CResultMessage
{
	DISABLE_COPY(CDisconnectMessage);

public:
	CDisconnectMessage(int Result)
		: CResultMessage(MSG_ID_DISCONNECT, Result)
	{
	}
};

class CChangedMessage : public CwclMessage
{
	DISABLE_COPY(CChangedMessage);

private:
	unsigned short FHandle;
	wclGattCharacteristicValue FValue;

public:
	CChangedMessage(unsigned short Handle, wclGattCharacteristicValue Value)
		: CwclMessage(MSG_ID_CHANGED, WCL_MSG_CATEGORY_USER)
	{
		FHandle = Handle;
		FValue = Value;
	}

	unsigned short GetHandle()
	{
		return FHandle;
	}

	wclGattCharacteristicValue GetValue()
	{
		return FValue;
	}
};

// Message receiver.
class CMessageReceiver : public CwclMessageReceiver
{
	DISABLE_COPY(CMessageReceiver);

private:
	GATTCLIENT_CONNECT FOnConnect;
	GATTCLIENT_DISCONNECT FOnDisconnect;
	GATTCLIENT_ONCHANGED FOnChanged;

protected:
	virtual void DoMessage(CwclMessage* Msg) override
	{
		if (Msg->GetCategory() == WCL_MSG_CATEGORY_USER)
		{
			switch (Msg->GetId())
			{
			case MSG_ID_CONNECT:
				if (FOnConnect != NULL)
					FOnConnect(this, ((CConnectMessage*)Msg)->GetResult());
				break;
			case MSG_ID_DISCONNECT:
				if (FOnDisconnect != NULL)
					FOnDisconnect(this, ((CDisconnectMessage*)Msg)->GetResult());
				break;
			case MSG_ID_CHANGED:
				if (FOnChanged != NULL)
				{
					wclGattCharacteristicValue Value = ((CChangedMessage*)Msg)->GetValue();
					FOnChanged(this, ((CChangedMessage*)Msg)->GetHandle(), Value.empty() ? NULL : &Value.front(), (unsigned int)Value.size());
				}
				break;
			}
		}
	}

public:
	CMessageReceiver(GATTCLIENT_CONNECT OnConnect, GATTCLIENT_DISCONNECT OnDisconnect, GATTCLIENT_ONCHANGED OnChanged)
		: CwclMessageReceiver()
	{
		FOnConnect = OnConnect;
		FOnDisconnect = OnDisconnect;
		FOnChanged = OnChanged;
	}

	void NotifyConnect(int Error)
	{
		CwclMessage* Msg = new CConnectMessage(Error);
		Post(Msg);
		Msg->Release();
	}

	void NotifyDisconnect(int Reason)
	{
		CwclMessage* Msg = new CDisconnectMessage(Reason);
		Post(Msg);
		Msg->Release();
	}

	void NotifyChanged(unsigned short Handle, unsigned char* Value, unsigned short Len)
	{
		wclGattCharacteristicValue Val;
		if (Value != NULL && Len > 0)
		{
			for (unsigned short i = 0; i < Len; i++)
				Val.push_back(Value[i]);
		}
		
		CwclMessage* Msg = new CChangedMessage(Handle, Val);
		Post(Msg);
		Msg->Release();
	}
};

// Message receiver expoirts.
extern "C"
{
	__declspec(dllexport) CMessageReceiver* MessageReciverCreate(GATTCLIENT_CONNECT OnConnect,
		GATTCLIENT_DISCONNECT OnDisconnect, GATTCLIENT_ONCHANGED OnChanged)
	{
		return new CMessageReceiver(OnConnect, OnDisconnect, OnChanged);
	}

	__declspec(dllexport) void MessageReciverDestroy(CMessageReceiver* Receiver)
	{
		if (Receiver != NULL)
			delete Receiver;
	}

	__declspec(dllexport) int MessageReciverOpen(CMessageReceiver* Receiver)
	{
		if (Receiver == NULL)
			return WCL_E_INVALID_ARGUMENT;
		return Receiver->Open();
	}

	__declspec(dllexport) int MessageReciverClose(CMessageReceiver* Receiver)
	{
		if (Receiver == NULL)
			return WCL_E_INVALID_ARGUMENT;
		return Receiver->Close();
	}

	__declspec(dllexport) void MessageReceiverNotifyConnect(CMessageReceiver* Receiver, int Error)
	{
		if (Receiver != NULL)
			Receiver->NotifyConnect(Error);
	}

	__declspec(dllexport) void MessageReceiverNotifyDisconnect(CMessageReceiver* Receiver, int Result)
	{
		if (Receiver != NULL)
			Receiver->NotifyDisconnect(Result);
	}

	__declspec(dllexport) void MessageReceiverNotifyChanged(CMessageReceiver* Receiver, unsigned short Handle,
		unsigned char* Value, unsigned short Len)
	{
		if (Receiver != NULL)
			Receiver->NotifyChanged(Handle, Value, Len);
	}
}