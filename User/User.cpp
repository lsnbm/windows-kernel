
#include <iostream>
#include <print>
#include <map>
#include <functional>
#include <windows.h>

#include "AutoDriver.h"
#include "DriverController.h"
#include "driver_data.h" 



#pragma pack(push, 1)
typedef struct _RawSMBIOSData {
	BYTE    Used20CallingMethod;
	BYTE    SMBIOSMajorVersion;
	BYTE    SMBIOSMinorVersion;
	BYTE    DmiRevision;
	DWORD   Length;
	BYTE    SMBIOSTableData[];
} RawSMBIOSData;


typedef struct _SMBIOS_HEADER {
	BYTE Type;
	BYTE Length;
	WORD Handle;
} SMBIOS_HEADER;


typedef struct _TYPE1_INFO {
	SMBIOS_HEADER   Header;
	BYTE            ManufacturerStrIndex;
	BYTE            ProductNameStrIndex;
	BYTE            VersionStrIndex;
	BYTE            SerialNumberStrIndex;
	GUID            UUID;
	BYTE            WakeUpType;
	BYTE            SKUNumberStrIndex;
	BYTE            FamilyStrIndex;
} TYPE1_INFO;

#pragma pack(pop)


void PrintSystemGUID(void) {
	const DWORD smbios_tag = 'RSMB';
	DWORD smbios_data_size = GetSystemFirmwareTable(smbios_tag, 0, NULL, 0);
	if (smbios_data_size == 0) {
		fprintf(stderr, "错误: 无法获取 SMBIOS 数据大小。错误码: %lu\n", GetLastError());
		return;
	}

	RawSMBIOSData* smbios_data = (RawSMBIOSData*)HeapAlloc(GetProcessHeap(), 0, smbios_data_size);
	if (smbios_data == NULL) {
		fprintf(stderr, "错误: 为 SMBIOS 数据分配内存失败。\n");
		return;
	}

	if (GetSystemFirmwareTable(smbios_tag, 0, smbios_data, smbios_data_size) != smbios_data_size) {
		fprintf(stderr, "错误: 无法获取 SMBIOS 表。错误码: %lu\n", GetLastError());
		HeapFree(GetProcessHeap(), 0, smbios_data);
		return;
	}

	BYTE* p = smbios_data->SMBIOSTableData;
	BOOL found = FALSE;
	for (DWORD i = 0; i < smbios_data->Length; ++i) {
		SMBIOS_HEADER* header = (SMBIOS_HEADER*)p;

		if (header->Type == 1 && header->Length >= sizeof(TYPE1_INFO)) {
			TYPE1_INFO* type1_info = (TYPE1_INFO*)p;
			GUID guid = type1_info->UUID;

			printf("系统 UUID: {%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}\n",
				guid.Data1, guid.Data2, guid.Data3,
				guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
				guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

			found = TRUE;
			break;
		}

		BYTE* next_struct = p + header->Length;
		while (*(WORD*)next_struct != 0) {
			next_struct++;
		}
		next_struct += 2;

		if (next_struct >= smbios_data->SMBIOSTableData + smbios_data->Length) {
			break;
		}
		p = next_struct;
	}

	if (!found) {
		fprintf(stderr, "错误: 未能在 SMBIOS 数据中找到系统 UUID (Type 1 表)。\n");
	}

	HeapFree(GetProcessHeap(), 0, smbios_data);
}

void PrintMachineGuidFromRegistry(void) {
	HKEY hKey;
	LONG lResult;
	char szBuffer[256];
	DWORD dwBufferSize = sizeof(szBuffer);
	const char* szSubKey = "SOFTWARE\\Microsoft\\Cryptography";
	const char* szValueName = "MachineGuid";

	lResult = RegOpenKeyExA(HKEY_LOCAL_MACHINE, szSubKey, 0, KEY_READ, &hKey);

	if (lResult != ERROR_SUCCESS) {
		fprintf(stderr, "错误: 无法打开注册表项 '%s'。错误码: %ld\n", szSubKey, lResult);
		return;
	}

	lResult = RegQueryValueExA(hKey, szValueName, NULL, NULL, (LPBYTE)szBuffer, &dwBufferSize);

	if (lResult != ERROR_SUCCESS) {
		fprintf(stderr, "错误: 无法读取注册表值 '%s'。错误码: %ld\n", szValueName, lResult);
	}
	else {
		printf("注册表中的 MachineGuid: %s\n", szBuffer);
	}

	RegCloseKey(hKey);
}

void PrintHardDriveSerialNumbers(void) {
	printf("\n正在尝试获取硬盘驱动器序列号 (需要管理员权限)...\n");

	BYTE pOutBuffer[256] = { 0 };
	DWORD dwBytesReturned = 0;
	BOOL bResult;

	for (int i = 0; i < 16; ++i) {
		char szDeviceName[64];
		sprintf_s(szDeviceName, sizeof(szDeviceName), "\\\\.\\PhysicalDrive%d", i);

		HANDLE hDevice = CreateFileA(
			szDeviceName,
			0,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
		);

		if (hDevice == INVALID_HANDLE_VALUE) {
			if (i == 0) {
				fprintf(stderr, "无法打开 PhysicalDrive0。请确保以管理员身份运行此程序。\n");
			}
			break;
		}

		STORAGE_PROPERTY_QUERY query;
		ZeroMemory(&query, sizeof(query));
		query.PropertyId = StorageDeviceProperty;
		query.QueryType = PropertyStandardQuery;

		bResult = DeviceIoControl(
			hDevice,
			IOCTL_STORAGE_QUERY_PROPERTY,
			&query,
			sizeof(query),
			pOutBuffer,
			sizeof(pOutBuffer),
			&dwBytesReturned,
			NULL
		);

		if (bResult && dwBytesReturned > 0) {
			STORAGE_DEVICE_DESCRIPTOR* pDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)pOutBuffer;

			printf("----------------------------------------\n");
			printf("驱动器 %d (%s):\n", i, szDeviceName);

			if (pDescriptor->ProductIdOffset > 0) {
				printf("  型号 (Product ID): %s\n", (char*)(pOutBuffer + pDescriptor->ProductIdOffset));
			}
			else {
				printf("  型号 (Product ID): 未提供\n");
			}

			if (pDescriptor->SerialNumberOffset > 0) {
				printf("  序列号: %s\n", (char*)(pOutBuffer + pDescriptor->SerialNumberOffset));
			}
			else {
				printf("  序列号: 未提供\n");
			}
		}

		CloseHandle(hDevice);
	}
	printf("----------------------------------------\n");
	printf("硬盘扫描完成。\n");
}




// 主菜单显示函数
void show_main_menu() {
	system("cls");
	std::cout << "\n==================== 驱动控制器主菜单 ====================\n"
		<< " [1] 运行功能测试 (子菜单)\n"
		<< " [2] 运行性能基准测试\n"
		<< " [3] 启动实时输入模拟\n"
		<< " [4] 启动内存查看/反汇编工具\n"
		<< " [0] 退出程序\n"
		<< "========================================================\n"
		<< "请选择一个选项: ";
}

// 控制台事件处理程序的回调函数
BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
	switch (CEvent)
	{
	case CTRL_C_EVENT:
		std::cout << "捕获到 CTRL+C 事件，程序将忽略。" << std::endl;
		return TRUE; // 返回TRUE表示已处理该事件
	case CTRL_BREAK_EVENT:
		std::cout << "捕获到 CTRL+BREAK 事件，程序将忽略。" << std::endl;
		return TRUE;
	case CTRL_CLOSE_EVENT:
		std::cout << "用户尝试关闭控制台窗口。正在执行清理操作..." << std::endl;
		AutoDriver::ExitCommunication();


		std::cout << "清理完成。程序现在将退出。" << std::endl;



		return FALSE; // 返回FALSE，让系统继续关闭进程
		// 如果您想完全阻止关闭，可以返回TRUE，但这通常不推荐，因为它会使窗口无法关闭
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		std::cout << "用户注销或系统关闭，执行清理..." << std::endl;
		Sleep(2000);
		return FALSE;
	}
	return FALSE;
}

int main() {

	AutoDriver driver(Driver_sys, sizeof(Driver_sys));


	// 注册控制台事件处理程序
	if (SetConsoleCtrlHandler(ConsoleHandler, TRUE))
	{
		std::cout << "已安装控制台事件处理程序。" << std::endl;
	}
	else
	{
		std::cerr << "错误：无法安装控制台事件处理程序。" << std::endl;
		return 1;
	}



	try {
		DriverController controller;
		bool is_running = true;

		while (is_running) {
			show_main_menu();
			int choice = -1;
			std::string line;
			std::getline(std::cin, line);

			// 更健壮的输入处理
			if (!line.empty()) {
				try {
					choice = std::stoi(line);
				}
				catch (const std::exception&) {
					choice = -1; // 无效输入，例如 "abc"
				}
			}
			else {
				choice = -1; // 用户直接按回车
			}

			switch (choice) {
			case 1: controller.RunFunctionalTests(); break;
			case 2: controller.RunAllPerformanceTests(); break;
			case 3: controller.RunLiveInputTest(); break;
			case 4: controller.RunMemoryInspectionSession(); break;
			case 0:
				is_running = false;
				std::cout << "程序已退出。" << std::endl;
				break;
			default:
				std::cerr << "无效的选项，请重试。" << std::endl;
				system("pause");
				break;
			}
		}
	}
	catch (const std::exception& e) {
		std::cerr << "\n捕获到严重异常: " << e.what() << std::endl;
		system("pause");
		return 1;
	}

	std::cout << "正在退出程序。驱动程序将会被自动卸载。" << std::endl;
	return 0;
}


