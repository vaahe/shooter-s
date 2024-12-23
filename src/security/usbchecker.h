#ifndef USBCHECKER_H
#define USBCHECKER_H

#include "windows.h"
#include "setupapi.h"

#define FlasfBaseContainerID L"{33000acd-5b54-5829-9321-51583167fa9b}"
#define SPDRP_BASE_CONTAINERID (0x00000024)  // Base ContainerID (R)

bool checkDevice()
{
    DWORD DataT;
    DWORD ErrorCode;
    HDEVINFO deviceInfoList;
    DWORD requiredLength = 0;
    SP_DEVINFO_DATA deviceInfoData;

    GUID GUID_DEVINTERFACE_MYFLASH = { 0x4d36e967L, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18 };
    deviceInfoList = SetupDiGetClassDevs(&GUID_DEVINTERFACE_MYFLASH, L"USBSTOR", NULL, DIGCF_PRESENT);


    if (deviceInfoList == INVALID_HANDLE_VALUE) {
        ErrorCode = GetLastError();
        std::cerr << "Error getting device list: " << ErrorCode << std::endl;
        return false;
    }

    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    DWORD i = 0;
    while (SetupDiEnumDeviceInfo(deviceInfoList, i, &deviceInfoData)) {
        requiredLength = 0;

        if (SetupDiGetDeviceRegistryProperty(deviceInfoList, &deviceInfoData, SPDRP_BASE_CONTAINERID, &DataT, NULL, 0, &requiredLength)) {
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
                SetupDiDestroyDeviceInfoList(deviceInfoList);
                std::cerr << "Error getting property size: " << GetLastError() << std::endl;
                return false;
            }
        }

        wchar_t* baseContainerID = new(std::nothrow) wchar_t[requiredLength / sizeof(wchar_t)];
        if (!baseContainerID) {
            SetupDiDestroyDeviceInfoList(deviceInfoList);
            std::cerr << "Memory allocation failure" << std::endl;
            return false;
        }

        if (!SetupDiGetDeviceRegistryProperty(deviceInfoList, &deviceInfoData, SPDRP_BASE_CONTAINERID, &DataT, (PBYTE)baseContainerID, requiredLength, NULL)) {
            delete[] baseContainerID;
            SetupDiDestroyDeviceInfoList(deviceInfoList);
            std::cerr << "Error getting property: " << GetLastError() << std::endl;
            return false;
        }

        if (!wmemcmp(baseContainerID, FlasfBaseContainerID, wcslen(FlasfBaseContainerID))) {
            delete[] baseContainerID;
            SetupDiDestroyDeviceInfoList(deviceInfoList);
            return true;
        }

        delete[] baseContainerID;
        i++;
    }

    ErrorCode = GetLastError();
    if (ErrorCode != ERROR_NO_MORE_ITEMS) {
        std::cerr << "Error enumerating devices: " << ErrorCode << std::endl;
    }

    SetupDiDestroyDeviceInfoList(deviceInfoList);
    return false;
}

#endif // USBCHECKER_H
