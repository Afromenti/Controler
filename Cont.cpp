#include <windows.h>
#pragma option push -a1
#pragma comment (lib, "Setupapi.lib")
#include <setupapi.h>
#pragma option pop
#include <iostream>
#include <assert.h>
#include <cstring>

using namespace std;


void displayError(const char *msg)
{
    cout << msg << "\n";
    system("PAUSE");
    exit(0);
}

template <class T>
inline void releaseMemory(T &x)
{
    assert(x != NULL);
    delete [] x;
    x = NULL;
}

void showHIDS()
{
    GUID classGuid;
    HMODULE hHidLib;
    DWORD memberIndex = 0;
    DWORD deviceInterfaceDetailDataSize;
    DWORD requiredSize;
    HDEVINFO deviceInfoSet;
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData = NULL;

    hHidLib = LoadLibrary("C:\\Windows\\System32\\hid.dll");
    if (!hHidLib)
    {
        displayError("Blad wczytania hid.dll");
    }
    void (__stdcall *HidD_GetHidGuid)(OUT LPGUID HidGuid);
    (FARPROC&) HidD_GetHidGuid = GetProcAddress(hHidLib,"HidD_GetHidGuid");

    if (!HidD_GetHidGuid)
    {
        FreeLibrary(hHidLib);
        displayError("Nie znaleziono identyfikatora GUID");
    }

    HidD_GetHidGuid(&classGuid);
    deviceInfoSet = SetupDiGetClassDevs(&classGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);
    if (deviceInfoSet == INVALID_HANDLE_VALUE)
        displayError("Nie zidentyfikowano podłączonych urządzeń.\n");

    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    while(SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &classGuid, memberIndex, &deviceInterfaceData))
    {
        memberIndex++; //inkrementacja numeru interfejsu
        SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &deviceInterfaceDetailDataSize, NULL);
        deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) new DWORD[deviceInterfaceDetailDataSize];
        deviceInterfaceDetailData->cbSize=sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (!SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, deviceInterfaceDetailData, deviceInterfaceDetailDataSize, &requiredSize, NULL))
        {
            releaseMemory(deviceInterfaceDetailData);
            SetupDiDestroyDeviceInfoList(deviceInfoSet);
            displayError("Nie można pobrać informacji o interfejsie.\n");
            
        }
        cout << deviceInterfaceDetailData->DevicePath << endl;
        releaseMemory(deviceInterfaceDetailData);
    }
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    FreeLibrary(hHidLib);
}

void showProperties()
{
    GUID classGuid;
    HMODULE hHidLib;
    DWORD memberIndex = 0;
    DWORD deviceInterfaceDetailDataSize;
    DWORD requiredSize;
    HDEVINFO deviceInfoSet;
    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData = NULL;
    SP_DEVINFO_DATA deviceInfoData;

    DWORD propertyBufferSize = 0;
    

}

int main()
{
    showHIDS();
}
