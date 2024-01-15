#include <windows.h>
#include <setupapi.h>
#include <hidsdi.h>
#include <iostream>
#include <regex>
#include <conio.h>
using namespace std;

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "hid.lib")


//buffer[11] == A_BUTTON {1}
//buffer[12] == BOTTTOM_ARROW {20} // RIGHT_ARROW {12} // TOP_ARROW {4} // LEFT_ARROW {28}
//buffer[4] == left_joystick // TOP {0} // BOT {255}

class Game
{
    public:
        bool gameover;
        int score;
        int dinoX, dinoY;
        int obstacleX, obstacleY;
        int lifes = 3;

    void setup() 
    {
        gameover = false;
        score = 0;
        dinoX = 10;
        dinoY = 10;
        obstacleX = 30;
        obstacleY = 10;
    }

    
    void draw() 
    {
        system("cls");
        for(unsigned int i = 0; i < 31; i++)
            cout << "*";
        for (int i = 0; i < 20; i++) 
        {
            cout << "*";
            for (int j = 0; j < 30; j++) 
            {
                if (i == dinoY && j == dinoX)
                    cout << "D";
                else if (i == obstacleY && j == obstacleX)
                    cout << "O";
                else
                    cout << " ";
            }
            cout << "*\n";

        }
        for(unsigned int i = 0; i < 32; i++)
            cout << "*";
        cout << "\nScore: " << score << " Lives: " << lifes << "\n";
    }
    
    bool input(HANDLE &controller) 
    {
        BYTE buffer[64]; 
        DWORD bytesRead;
        
        if (ReadFile(controller, buffer, sizeof(buffer), &bytesRead, NULL))
        {

            if (buffer[2] == 255 && buffer[4] == 0)
            {
                dinoX++;
                dinoY -= 2;                                                                                     
                return true;
            }

            if (buffer[2] == 0 && buffer[4] == 0)
            {
                dinoX--;
                dinoY -= 2;                                                                                     
                return true;
            }

            if (buffer[2] == 0 && buffer[4] == 255)
            {
                dinoX--;
                dinoY += 2;                                                                                     
                return true;
            }

            if (buffer[2] == 255 && buffer[4] == 255)
            {
                dinoX++;
                dinoY += 2;                                                                                     
                return true;
            }

            if (buffer[12] == 4)
            {
                dinoY--;
                return true;
            }

            if (buffer[12] == 12)
            {
                dinoX++;
                return true;
            }

            if (buffer[12] == 20)
            {
                dinoY++;
                return true;
            }

            if (buffer[12] == 28)
            {
                dinoX--;
                return true;
            }

            if (buffer[4] == 0)
            {
                dinoY--;
                return true;
            }

            if (buffer[4] == 255)
            {
                dinoY++;
                return true;
            }

            if (buffer[2] == 0)
            {
                dinoX--;
                return true;
            }
            if (buffer[2] == 255)
            {
                dinoX++;
                return true;
            }

        }
    return false;
     } 


    
    void logic() 
    {
        if (obstacleX == dinoX && obstacleY == dinoY)
            lifes--;
        obstacleX--;

        if (obstacleX < 0) 
        {
            obstacleX = 29;
            obstacleY = rand() % 19;
            score++;
        }

        if(!lifes)
        {
            gameover = true;
        }
        if(dinoX > 29 || dinoY > 18 || dinoX < 0 || dinoY < 0)
            lifes = 0;
    }
    

};


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
HANDLE findController() {
    hHidLib = LoadLibrary("C:\\Windows\\System32\\hid.dll");

    void (__stdcall *HidD_GetHidGuid)(OUT LPGUID HidGuid);
    (FARPROC&) HidD_GetHidGuid = GetProcAddress(hHidLib,"HidD_GetHidGuid");

    if (!HidD_GetHidGuid)
    {
        FreeLibrary(hHidLib);
   
    }

    HidD_GetHidGuid(&classGuid);

    HDEVINFO hDevInfo = SetupDiGetClassDevs(&classGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE) 
    {
        cerr << "SetupDiGetClassDevs failed" << std::endl;
        return INVALID_HANDLE_VALUE;

    }

    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData); ++i) 
    {
        SP_DEVICE_INTERFACE_DATA devInterfaceData;
        devInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) new DWORD[deviceInterfaceDetailDataSize];
        deviceInterfaceDetailData->cbSize=sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (SetupDiEnumDeviceInterfaces(hDevInfo, &devInfoData, &classGuid, 0, &devInterfaceData)) 
        {
            DWORD requiredSize;
            SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInterfaceData, NULL, 0, &deviceInterfaceDetailDataSize, NULL);

            SP_DEVICE_INTERFACE_DETAIL_DATA* devInterfaceDetailData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)malloc(requiredSize);
            devInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);


            if (SetupDiGetDeviceInterfaceDetail(hDevInfo, &devInterfaceData, devInterfaceDetailData, deviceInterfaceDetailDataSize, &requiredSize, &deviceInfoData)) 
            {
            std::string s = getRegistryPropertyString(hDevInfo,&deviceInfoData, SPDRP_HARDWAREID);
            
            if (s.find("VID_045E") != std::string::npos) 
            {
                cout << devInterfaceDetailData->DevicePath;

                HANDLE hHIDDevice = CreateFile(devInterfaceDetailData->DevicePath, GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

                if (hHIDDevice != INVALID_HANDLE_VALUE) 
                {
                    return hHIDDevice;
                }
    
            }

        }
            free(devInterfaceDetailData);
        }
    }
    SetupDiDestroyDeviceInfoList(hDevInfo);
    return INVALID_HANDLE_VALUE;
}

void checkInput(HANDLE &controller)
{
    BYTE buffer[64];
    DWORD bytesRead;

    while (true)
    {
        if (ReadFile(controller, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0)
        {
            cout << "Odczytano dane od kontrolera:" << endl;
            for (DWORD i = 0; i < bytesRead; ++i)
            {
                cout << "Byte " << i << ": " << (int)buffer[i] << endl;
            }
        }
        Sleep(10);
    }
}

int main() 
{
    HANDLE controller = findController();
    if (controller != INVALID_HANDLE_VALUE)
    {
        cout << "Polaczono z padem \n";
        Game game = Game();
        game.setup();
        while (!game.gameover)
        {
            game.draw();
            game.input(controller);
            game.logic();
        }
    }
    else
    {
        cout << "Nie znaleziono pada \n";
    }

    return 0;
}
