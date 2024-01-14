#include <windows.h>
#include <setupapi.h>
#include <hidsdi.h>
#include <iostream>
using namespace std;

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "hid.lib")

GUID classGuid;
HMODULE hHidLib;
DWORD deviceInterfaceDetailDataSize;
SP_DEVINFO_DATA deviceInfoData;
PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData = NULL;

string getRegistryPropertyString(HDEVINFO deviceInfoSet,PSP_DEVINFO_DATA deviceInfoData, DWORD property)
{
    DWORD propertyBufferSize = 0;
    char *propertyBuffer = NULL;
    SetupDiGetDeviceRegistryProperty(deviceInfoSet, deviceInfoData, property, NULL, NULL, 0, &propertyBufferSize);

    propertyBuffer = new char[(propertyBufferSize * sizeof(TCHAR))];
    bool result=SetupDiGetDeviceRegistryProperty(deviceInfoSet, deviceInfoData, property, NULL, PBYTE(propertyBuffer), propertyBufferSize, NULL);


     
    return propertyBuffer;
}
// Funkcja do iteracji przez listę HID urządzeń
void EnumerateHIDDevices() {
    hHidLib = LoadLibrary("C:\\Windows\\System32\\hid.dll");

    void (__stdcall *HidD_GetHidGuid)(OUT LPGUID HidGuid);
    (FARPROC&) HidD_GetHidGuid = GetProcAddress(hHidLib,"HidD_GetHidGuid");

    if (!HidD_GetHidGuid)
    {
        FreeLibrary(hHidLib);
   
    }

    HidD_GetHidGuid(&classGuid);

    HDEVINFO hDevInfo = SetupDiGetClassDevs(&classGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        std::cerr << "SetupDiGetClassDevs failed" << std::endl;
        return;
    }

    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    // Iteruj przez urządzenia
    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); ++i) {
        // Pobierz interfejs urządzenia
        SP_DEVICE_INTERFACE_DATA devInterfaceData;
        devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) new DWORD[deviceInterfaceDetailDataSize];
        deviceInterfaceDetailData->cbSize=sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (SetupDiEnumDeviceInterfaces(hDevInfo, &devInfoData, &classGuid, 0, &devInterfaceData)) {
            // Pobierz wymagany rozmiar bufora
            DWORD requiredSize;
            SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInterfaceData, NULL, 0, &deviceInterfaceDetailDataSize, NULL);

            SP_DEVICE_INTERFACE_DETAIL_DATA* devInterfaceDetailData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)malloc(requiredSize);
            devInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);


            if (SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInterfaceData, devInterfaceDetailData, deviceInterfaceDetailDataSize, &requiredSize, &deviceInfoData)) 
            {
                cout << "\nClassDescr: "<<getRegistryPropertyString(hDevInfo,&deviceInfoData, SPDRP_CLASS);
                HANDLE hHIDDevice = CreateFile(devInterfaceDetailData->DevicePath, GENERIC_READ | GENERIC_WRITE,
                                               FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

                if (hHIDDevice != INVALID_HANDLE_VALUE) {
            
                    CloseHandle(hHIDDevice);
                }
            }

            free(devInterfaceDetailData);
        }
    }

    // Zwolnij uchwyt do interfejsu setupapi
    SetupDiDestroyDeviceInfoList(hDevInfo);
}

int main() {
    EnumerateHIDDevices();

    return 0;
}
