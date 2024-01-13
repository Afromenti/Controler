#include <windows.h>
#pragma option push -a1
#pragma comment (lib, "Setupapi.lib")
#include <setupapi.h>
#pragma option pop
#include <iostream>
#include <assert.h>
#include <cstring>

using namespace std;

GUID classGuid;
HMODULE hHidLib;
DWORD memberIndex = 0;
DWORD deviceInterfaceDetailDataSize;
DWORD requiredSize;
    
HDEVINFO deviceInfoSet;
SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData = NULL;
SP_DEVINFO_DATA deviceInfoData;



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
string getRegistryPropertyString(HDEVINFO deviceInfoSet,PSP_DEVINFO_DATA deviceInfoData, DWORD property)
{
    DWORD propertyBufferSize = 0;
    char *propertyBuffer = NULL;
    SetupDiGetDeviceRegistryProperty(deviceInfoSet, deviceInfoData, property, NULL, NULL, 0, &propertyBufferSize);

    propertyBuffer = new char[(propertyBufferSize * sizeof(TCHAR))];
    bool result=SetupDiGetDeviceRegistryProperty(deviceInfoSet, deviceInfoData, property, NULL, PBYTE(propertyBuffer), propertyBufferSize, NULL);

    if(!result)
        releaseMemory(propertyBuffer);
    return propertyBuffer;
}
void showProperties()
{
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
    while(SetupDiEnumDeviceInterfaces(deviceInfoSet, NULL, &classGuid,memberIndex, &deviceInterfaceData))
    {
        memberIndex++;
        SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, NULL, 0, &deviceInterfaceDetailDataSize, NULL);
        deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) new DWORD[deviceInterfaceDetailDataSize];
        deviceInterfaceDetailData->cbSize=sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

        if (!SetupDiGetDeviceInterfaceDetail(deviceInfoSet, &deviceInterfaceData, deviceInterfaceDetailData, deviceInterfaceDetailDataSize, &requiredSize, &deviceInfoData))
        {
            releaseMemory(deviceInterfaceDetailData);
            SetupDiDestroyDeviceInfoList(deviceInfoSet);
            displayError ("Nie można pobrać informacji o interfejsie.\n");
        }

        cout << "\nClassDescr: "<<getRegistryPropertyString(deviceInfoSet,&deviceInfoData, SPDRP_CLASS);
        cout << "\nClassGUID: "<<getRegistryPropertyString(deviceInfoSet,&deviceInfoData, SPDRP_CLASSGUID);
        cout << "\nCompatibileIDs: "<<getRegistryPropertyString(deviceInfoSet, &deviceInfoData, SPDRP_COMPATIBLEIDS);
        cout << "\nConfigFlags: "<<getRegistryPropertyString(deviceInfoSet,&deviceInfoData, SPDRP_CONFIGFLAGS);
        cout << "\nDeviceDescr: "<<getRegistryPropertyString(deviceInfoSet, &deviceInfoData, SPDRP_DEVICEDESC);
        cout << "\nDriver: "<<getRegistryPropertyString(deviceInfoSet, &deviceInfoData, SPDRP_DRIVER);
        cout << "\nHardwareID: "<<getRegistryPropertyString(deviceInfoSet,&deviceInfoData, SPDRP_HARDWAREID);
        cout << "\nMfg: "<<getRegistryPropertyString(deviceInfoSet, &deviceInfoData, SPDRP_MFG);
        cout << "\nEnumeratorName: "<<getRegistryPropertyString(deviceInfoSet, &deviceInfoData, SPDRP_ENUMERATOR_NAME);
        cout << "\nPhysDevObjName: "<<getRegistryPropertyString(deviceInfoSet, &deviceInfoData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME);
        cout << endl;
        releaseMemory(deviceInterfaceDetailData);
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    FreeLibrary(hHidLib);
}

int main()
{
    //showHIDS();
    showProperties();
}
