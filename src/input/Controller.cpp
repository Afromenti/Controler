#include "input/Controller.h"
#include "core/Game.h"
#include <setupapi.h>
#include <hidsdi.h>
#include <iostream>

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "hid.lib")

using namespace std;

Controller::Controller() : handle(INVALID_HANDLE_VALUE), isRunning(false), newDataAvailable(false)
{
    memset(readBuffer, 0, sizeof(readBuffer));
    memset(sharedBuffer, 0, sizeof(sharedBuffer));
}

Controller::~Controller()
{
    isRunning = false;
    if (handle != INVALID_HANDLE_VALUE)
    {
        CancelIoEx(handle, NULL);
        CloseHandle(handle);
    }
    if (inputThread.joinable())
    {
        inputThread.join();
    }
}

void Controller::disconnect()
{
    if (handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(handle);
        handle = INVALID_HANDLE_VALUE;
    }
}

string getRegistryPropertyString(HDEVINFO deviceInfoSet, PSP_DEVINFO_DATA deviceInfoData, DWORD property)
{
    DWORD propertyBufferSize = 0;
    SetupDiGetDeviceRegistryProperty(deviceInfoSet, deviceInfoData, property, NULL, NULL, 0, &propertyBufferSize);
    char *propertyBuffer = new char[propertyBufferSize];
    SetupDiGetDeviceRegistryProperty(deviceInfoSet, deviceInfoData, property, NULL, PBYTE(propertyBuffer), propertyBufferSize, NULL);
    string result(propertyBuffer);
    delete[] propertyBuffer;
    return result;
}

bool Controller::connect()
{
    GUID classGuid;
    HMODULE hHidLib = LoadLibrary("C:\\Windows\\System32\\hid.dll");

    void (__stdcall *HidD_GetHidGuid)(OUT LPGUID HidGuid);
    HidD_GetHidGuid = (void(__stdcall *)(OUT LPGUID))GetProcAddress(hHidLib,"HidD_GetHidGuid");

    if (!HidD_GetHidGuid)
    {
        FreeLibrary(hHidLib);
        return false;
    }

    HidD_GetHidGuid(&classGuid);
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&classGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        cerr << "SetupDiGetClassDevs failed" << std::endl;
        FreeLibrary(hHidLib);
        return false;
    }

    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); ++i)
    {
        SP_DEVICE_INTERFACE_DATA devInterfaceData;
        devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

        if (SetupDiEnumDeviceInterfaces(hDevInfo, &devInfoData, &classGuid, 0, &devInterfaceData))
        {
            DWORD requiredSize;
            SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInterfaceData, NULL, 0, &requiredSize, NULL);

            SP_DEVICE_INTERFACE_DETAIL_DATA* devInterfaceDetailData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)malloc(requiredSize);
            devInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            
            SP_DEVINFO_DATA deviceInfoData;
            deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

            if (SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInterfaceData, devInterfaceDetailData, requiredSize, &requiredSize, &deviceInfoData))
            {
                std::string s = getRegistryPropertyString(hDevInfo, &deviceInfoData, SPDRP_HARDWAREID);
                
                if (s.find("VID_045E") != std::string::npos)
                {
                    handle = CreateFileA(devInterfaceDetailData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

                    if (handle != INVALID_HANDLE_VALUE)
                    {
                        cout << "Pad polaczony! \n";
                        isRunning = true;
                        inputThread = std::thread(&Controller::readLoop, this);
                        SetupDiDestroyDeviceInfoList(hDevInfo);
                        FreeLibrary(hHidLib);
                        free(devInterfaceDetailData);
                        return true;
                    }
                    free(devInterfaceDetailData);
                }
                else
                {
                    free(devInterfaceDetailData);
                }
            }
            else
            {
                free(devInterfaceDetailData);
            }
        }
    }
    SetupDiDestroyDeviceInfoList(hDevInfo);
    FreeLibrary(hHidLib);
    return false;
}

void Controller::readLoop()
{
    while (isRunning)
    {
        DWORD bytesRead = 0;
        if (ReadFile(handle, readBuffer, sizeof(readBuffer), &bytesRead, NULL))
        {
            std::lock_guard<std::mutex> lock(inputMutex);
            memcpy(sharedBuffer, readBuffer, sizeof(sharedBuffer));
            newDataAvailable = true;
        }
        else
        {
            // Jeśli ReadFile zwróci false (np. przez CloseHandle/CancelIo przy wyjściu), pętla się zakręci, 
            // isRunning będzie false i wątek bezpiecznie zgaśnie.
            Sleep(10); 
        }
    }
}

bool Controller::processInput(Game& game, float dt)
{
    if (handle != INVALID_HANDLE_VALUE)
    {
        bool hasData = false;
        BYTE localBuffer[64];
        {
            std::lock_guard<std::mutex> lock(inputMutex);
            if (newDataAvailable) 
            {
                memcpy(localBuffer, sharedBuffer, sizeof(localBuffer));
                hasData = true;
            }
        }
        
        if (!hasData) return false;

        static bool wasBPressed = false;
        static bool wasStartPressed = false;

        bool isBPressed = ((localBuffer[26] & 2) != 0) || (localBuffer[26] == 2) || ((localBuffer[11] & 2) != 0) || (localBuffer[11] == 2);
        bool isStartPressed = ((localBuffer[26] & 128) != 0) || (localBuffer[26] == 128) || ((localBuffer[11] & 128) != 0) || (localBuffer[11] == 128) || (localBuffer[11] == 16) || (localBuffer[11] == 32);

        if (isBPressed && !wasBPressed)
        {
            if (game.logicState.gameover)
            {
                game.logicState.shouldQuit = true;
            }
            else
            {
                game.logicState.gameover = true;
            }
            wasBPressed = true;
            return true;
        }
        else if (!isBPressed)
        {
            wasBPressed = false;
        }

        if (isStartPressed && !wasStartPressed)
        {
            game.setup();
            wasStartPressed = true;
            return true;
        }
        else if (!isStartPressed)
        {
            wasStartPressed = false;
        }

        if (game.logicState.gameover) return false;

        float playerSpeedX = 60.0f;
        float playerSpeedY = 60.0f;

        if (localBuffer[2] == 255 && localBuffer[4] == 0)
        {
            game.logicState.dinoX += playerSpeedX * dt; game.logicState.dinoY -= playerSpeedY * dt; return true;
        }
        if (localBuffer[2] == 0 && localBuffer[4] == 0)
        {
            game.logicState.dinoX -= playerSpeedX * dt; game.logicState.dinoY -= playerSpeedY * dt; return true;
        }
        if (localBuffer[2] == 0 && localBuffer[4] == 255)
        {
            game.logicState.dinoX -= playerSpeedX * dt; game.logicState.dinoY += playerSpeedY * dt; return true;
        }
        if (localBuffer[2] == 255 && localBuffer[4] == 255)
        {
            game.logicState.dinoX += playerSpeedX * dt; game.logicState.dinoY += playerSpeedY * dt; return true;
        }
        if (localBuffer[12] == 4)
        {
            game.logicState.dinoY -= playerSpeedY * dt; return true;
        }
        if (localBuffer[12] == 12)
        {
            game.logicState.dinoX += playerSpeedX * dt; return true;
        }
        if (localBuffer[12] == 20)
        {
            game.logicState.dinoY += playerSpeedY * dt; return true;
        }
        if (localBuffer[12] == 28)
        {
            game.logicState.dinoX -= playerSpeedX * dt; return true;
        }
        if (localBuffer[4] == 0)
        {
            game.logicState.dinoY -= playerSpeedY * dt; return true;
        }
        if (localBuffer[4] == 255)
        {
            game.logicState.dinoY += playerSpeedY * dt; return true;
        }
        if (localBuffer[2] == 0)
        {
            game.logicState.dinoX -= playerSpeedX * dt; return true;
        }
        if (localBuffer[2] == 255)
        {
            game.logicState.dinoX += playerSpeedX * dt; return true;
        }
    }
    return false;
}
