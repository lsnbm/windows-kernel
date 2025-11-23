#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <locale>
#include <codecvt>

// Windows核心头文件
#include <windows.h>
#include <winioctl.h>

// 用于网络信息 (MAC地址)
#include <iphlpapi.h>

// 用于COM和WMI
#include <comdef.h>
#include <Wbemidl.h>


#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")


namespace SystemIdentifiers {

    // --- 辅助工具 ---

    // 宽字符串到窄字符串的转换
    static std::string WstringToString(const std::wstring& wstr) {
        if (wstr.empty()) return std::string();
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string strTo(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
        return strTo;
    }

    // 窄字符串到宽字符串的转换
    static std::wstring StringToWstring(const std::string& str) {
        if (str.empty()) return std::wstring();
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstrTo(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }

    // --- WMI辅助类 ---
    // 一个简单的RAII包装器来初始化和反初始化COM/WMI
    class WmiConnection {
    public:
        WmiConnection() : pSvc(nullptr), pLoc(nullptr) {
            HRESULT hres;
            hres = CoInitializeEx(0, COINIT_MULTITHREADED);
            if (FAILED(hres)) return;

            hres = CoInitializeSecurity(
                NULL, -1, NULL, NULL,
                RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE,
                NULL, EOAC_NONE, NULL
            );
            if (FAILED(hres)) {
                CoUninitialize();
                return;
            }

            hres = CoCreateInstance(
                CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                IID_IWbemLocator, (LPVOID*)&pLoc
            );
            if (FAILED(hres)) {
                CoUninitialize();
                return;
            }

            hres = pLoc->ConnectServer(
                _bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &pSvc
            );
            if (FAILED(hres)) {
                pLoc->Release();
                CoUninitialize();
                return;
            }

            hres = CoSetProxyBlanket(
                pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
                NULL, EOAC_NONE
            );
            if (FAILED(hres)) {
                pSvc->Release();
                pLoc->Release();
                CoUninitialize();
            }
        }

        ~WmiConnection() {
            if (pSvc) pSvc->Release();
            if (pLoc) pLoc->Release();
            CoUninitialize();
        }

        IWbemServices* GetService() const { return pSvc; }
        bool IsConnected() const { return pSvc != nullptr; }

    private:
        IWbemServices* pSvc;
        IWbemLocator* pLoc;
    };

    // --- 功能实现 ---

    /**
     * 1. 获取(创建)一个新的GUID
     * @return 返回一个格式化的GUID字符串，例如 "E8AD3E9E-D6A7-4467-A277-321E708E776F"
     */
    static std::string CreateNewGuidString() {
        GUID guid;
        if (CoCreateGuid(&guid) == S_OK) {
            wchar_t guid_string[40];
            if (StringFromGUID2(guid, guid_string, 40)) {
                return WstringToString(guid_string);
            }
        }
        return "";
    }

    /**
     * 2 & 7. 获取磁盘驱动器的硬件信息 (包括序列号)
     * @return 返回一个vector，每个元素是一个map，代表一个磁盘驱动器。map包含 "Model" 和 "SerialNumber"。
     */
    static std::vector<std::map<std::string, std::string>> GetDiskDriveInfo() {
        std::vector<std::map<std::string, std::string>> result;
        WmiConnection wmi;
        if (!wmi.IsConnected()) return result;

        IEnumWbemClassObject* pEnumerator = NULL;
        HRESULT hres = wmi.GetService()->ExecQuery(
            bstr_t("WQL"),
            bstr_t("SELECT * FROM Win32_DiskDrive"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &pEnumerator
        );

        if (FAILED(hres)) return result;

        IWbemClassObject* pclsObj = NULL;
        ULONG uReturn = 0;
        while (pEnumerator) {
            hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
            if (uReturn == 0) break;

            std::map<std::string, std::string> disk_info;
            VARIANT vtProp;

            // 获取型号
            pclsObj->Get(L"Model", 0, &vtProp, 0, 0);
            if (vtProp.vt == VT_BSTR) disk_info["Model"] = WstringToString(vtProp.bstrVal);
            VariantClear(&vtProp);

            // 获取序列号 (需要管理员权限)
            pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
            if (vtProp.vt == VT_BSTR) {
                std::wstring serial = vtProp.bstrVal;
                serial.erase(std::remove(serial.begin(), serial.end(), L' '), serial.end()); // 移除空格
                disk_info["SerialNumber"] = WstringToString(serial);
            }
            VariantClear(&vtProp);

            result.push_back(disk_info);
            pclsObj->Release();
        }

        if (pEnumerator) pEnumerator->Release();
        return result;
    }




    /**
     * 5. 获取图形处理器(GPU)的信息
     * @return 返回一个vector，每个元素是一个map，代表一个GPU。map包含"Name"和"PNPDeviceID"。
     */
    static std::vector<std::map<std::string, std::string>> GetGpuInfo() {
        std::vector<std::map<std::string, std::string>> result;
        WmiConnection wmi;
        if (!wmi.IsConnected()) return result;

        IEnumWbemClassObject* pEnumerator = NULL;
        HRESULT hres = wmi.GetService()->ExecQuery(
            bstr_t("WQL"),
            bstr_t("SELECT * FROM Win32_VideoController"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &pEnumerator
        );

        if (FAILED(hres)) return result;

        IWbemClassObject* pclsObj = NULL;
        ULONG uReturn = 0;
        while (pEnumerator) {
            hres = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
            if (uReturn == 0) break;

            std::map<std::string, std::string> gpu_info;
            VARIANT vtProp;

            pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
            if (vtProp.vt == VT_BSTR) gpu_info["Name"] = WstringToString(vtProp.bstrVal);
            VariantClear(&vtProp);

            pclsObj->Get(L"PNPDeviceID", 0, &vtProp, 0, 0);
            if (vtProp.vt == VT_BSTR) gpu_info["PNPDeviceID"] = WstringToString(vtProp.bstrVal);
            VariantClear(&vtProp);

            result.push_back(gpu_info);
            pclsObj->Release();
        }

        if (pEnumerator) pEnumerator->Release();
        return result;
    }

    /**
     * 6. 获取磁盘分区的GUID (仅适用于GPT分区)
     * @param volumePath 卷路径，例如 L"\\\\.\\C:"
     * @return 分区的GUID字符串，失败则返回空字符串。
     */
    static std::string GetPartitionGuid(const std::wstring& volumePath) {
        HANDLE hDevice = CreateFileW(
            volumePath.c_str(),
            0, // 不需要读写权限
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
        );
        if (hDevice == INVALID_HANDLE_VALUE) return "";

        PARTITION_INFORMATION_EX partInfo;
        DWORD bytesReturned = 0;
        BOOL result = DeviceIoControl(
            hDevice,
            IOCTL_DISK_GET_PARTITION_INFO_EX,
            NULL, 0,
            &partInfo, sizeof(partInfo),
            &bytesReturned, NULL
        );
        CloseHandle(hDevice);

        if (result && partInfo.PartitionStyle == PARTITION_STYLE_GPT) {
            wchar_t guid_string[40];
            if (StringFromGUID2(partInfo.Gpt.PartitionId, guid_string, 40)) {
                return WstringToString(guid_string);
            }
        }

        return "";
    }

    /**
     * 8. 获取SMBIOS信息 (如主板序列号, 系统UUID等)
     * 这是底层函数，获取原始SMBIOS数据并从中提取特定字符串。
     * @param type 要查找的SMBIOS结构类型 (e.g., 2 for Baseboard)
     * @param str_index 要提取的字符串在该结构中的索引 (1-based)
     * @return 找到的字符串，失败则返回空。
     */
    static std::string GetSmbiosStringByIndex(BYTE type, BYTE str_index) {
        if (str_index == 0) return "";

        DWORD smbiosDataSize = 0;
        // 获取大小
        smbiosDataSize = GetSystemFirmwareTable('RSMB', 0, NULL, 0);
        if (smbiosDataSize == 0) return "";

        std::unique_ptr<BYTE[]> pSmbiosData(new BYTE[smbiosDataSize]);
        GetSystemFirmwareTable('RSMB', 0, pSmbiosData.get(), smbiosDataSize);

        PBYTE pData = pSmbiosData.get() + 8; // 跳过 RawSMBIOSData header
        PBYTE pEnd = pSmbiosData.get() + smbiosDataSize;

        while (pData < pEnd) {
            BYTE struct_type = pData[0];
            BYTE struct_len = pData[1];

            if (struct_type == type) {
                BYTE target_str_id = pData[str_index]; // 获取我们要找的字符串的ID
                if (target_str_id == 0) break; // ID为0表示没有字符串

                char* pStrings = (char*)(pData + struct_len);
                int current_str_id = 1;

                // 遍历字符串区域
                while (current_str_id < target_str_id) {
                    pStrings += strlen(pStrings) + 1;
                    current_str_id++;
                }
                return std::string(pStrings);
            }

            // 移动到下一个结构
            pData += struct_len;
            while ((pData < pEnd) && (pData[0] != 0 || pData[1] != 0)) pData++;
            pData += 2; // 跳过两个0
        }

        return "";
    }

    static std::string GetBaseboardSerialNumber() {
        return GetSmbiosStringByIndex(2, 0x07); // Type 2 (Baseboard), offset 7 is Serial Number index
    }

    static std::string GetSystemUUID() {
        return GetSmbiosStringByIndex(1, 0x19); // Type 1 (System), offset 25 is UUID index
    }

    /**
     * 9. 获取磁盘卷的文件系统卷序列号
     * @param drivePath 驱动器路径，例如 L"C:\\"
     * @return 32位的卷序列号的十六进制字符串表示
     */
    static std::string GetVolumeSerialNumber(const std::wstring& drivePath) {
        DWORD serialNumber = 0;
        if (GetVolumeInformationW(drivePath.c_str(), NULL, 0, &serialNumber, NULL, NULL, NULL, 0)) {
            char buffer[12];
            snprintf(buffer, sizeof(buffer), "%08X", serialNumber);
            return std::string(buffer);
        }
        return "";
    }

  

} // namespace SystemIdentifiers