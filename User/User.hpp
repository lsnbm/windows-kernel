#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <winbase.h>
#include <fstream>
#include <thread>
#include <chrono>      // For std::chrono::milliseconds
#include <ntddmou.h>   // 鼠标 结构体 MOUSE_INPUT_DATA
#include <ntddkbd.h>   // 键盘 结构体 KEYBOARD_INPUT_DATA

// 定义IOCTL控制码 (根据您的定义)
#define IOCTL_READ_MEMORY           CTL_CODE(FILE_DEVICE_UNKNOWN, 0x9000, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_WRITE_MEMORY          CTL_CODE(FILE_DEVICE_UNKNOWN, 0x9001, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_GET_MODULE_BASE       CTL_CODE(FILE_DEVICE_UNKNOWN, 0x9002, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_TERMINATE_PROCESS     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x9003, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_SIMULATE_KEYBOARD		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x9004, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_SIMULATE_MOUSE		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x9005, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_HANDLE_ELEVATION		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x9006, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_DUMP					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x9007, METHOD_NEITHER, FILE_ANY_ACCESS)
#define IOCTL_NULL_IO               CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8000, METHOD_NEITHER, FILE_ANY_ACCESS)



class AutoDriver {
public:
	// 构造函数: 负责写入、安装和启动驱动，并打开设备句柄
	AutoDriver(const std::wstring& deviceName, const unsigned char* driverBin, size_t driverLen)
	{
		AutoDriver::m_deviceName = deviceName;
		AutoDriver::m_driverBin = driverBin;
		AutoDriver::m_driverLen = driverLen;
		AutoDriver::m_hDevice = INVALID_HANDLE_VALUE;

		wchar_t tmp[MAX_PATH] = { 0 };
		if (GetTempPathW(MAX_PATH, tmp) == 0) {
			std::wcerr << L"[!] 获取临时路径失败，错误: " << GetLastError() << std::endl;
			return;
		}
		m_tempDir = tmp;
		m_driverPath = m_tempDir + L"72c9b540-4c42-4972-b4f7-08d56c243d3c.tmp"; // 使用更独特的名称

		if (!writeDriverToFile()) {
			std::wcerr << L"[!] 写入驱动文件失败" << std::endl;
			return;
		}

		if (!installAndStartService()) {
			std::wcerr << L"[!] 加载或启动驱动服务失败" << std::endl;
			return;
		}

		openDeviceHandle();
	}

	// 析构函数: 负责关闭设备句柄、卸载驱动并清理文件
	~AutoDriver() {
		if (m_hDevice != INVALID_HANDLE_VALUE) {
			CloseHandle(m_hDevice);
		}

		if (uninstallService()) {
			std::wcout << L"[+] 驱动服务卸载成功" << std::endl;
		}
		else {
			std::wcerr << L"[!] 驱动服务卸载失败" << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		if (DeleteFileW(m_driverPath.c_str())) {
			std::wcout << L"[+] 临时驱动文件删除成功" << std::endl;
		}
		else {
			std::wcerr << L"[*] 删除驱动文件失败，已安排在系统重启时删除" << std::endl;
			MoveFileExW(m_driverPath.c_str(), nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);
		}
	}

	// 检查驱动是否成功加载并准备就绪
	static inline __forceinline bool IsInitialized() {
		return m_hDevice != INVALID_HANDLE_VALUE;
	}

	// 设置全局目标进程ID
	static inline __forceinline  void SetTargetProcessId(uint32_t pid) {
		if (!pid) return;
		s_targetPid = pid;
		m_ioPacket.TargetProcessId = pid;//先占位防止蓝屏，后续修改
		//通知一次驱动
		DeviceIoControl(m_hDevice, IOCTL_NULL_IO, &m_ioPacket, sizeof(DriverPacket), &m_ioPacket, sizeof(DriverPacket), &m_bytesReturned, nullptr);

	}
	// 获取当前目标进程ID
	static inline __forceinline  uint32_t GetTargetProcessId() {
		return s_targetPid;
	}


	// --- 内存操作 ---
	template<typename T>
	static inline __forceinline   T Read(uintptr_t address) { T value = {}; KernelReadMemory(reinterpret_cast<PVOID>(address), &value, sizeof(T)); return value; }

	static inline __forceinline  bool Read(uintptr_t address, PVOID buffer, size_t size) { return KernelReadMemory(reinterpret_cast<PVOID>(address), buffer, size); }

	template<typename T>
	static inline __forceinline   bool Write(uintptr_t address, const T& value) { return KernelWriteMemory(reinterpret_cast<PVOID>(address), const_cast<T*>(&value), sizeof(T)); }

	static inline __forceinline    bool Write(uintptr_t address, PVOID buffer, size_t size) { return KernelWriteMemory(reinterpret_cast<PVOID>(address), buffer, size); }

	static inline __forceinline   uintptr_t GetModuleBase(const std::string& moduleName) { return KernelGetModuleBase(moduleName.c_str()); }

	static inline __forceinline   std::string ReadString(uintptr_t address, size_t len) {
		std::vector<char> buf(len);
		if (Read(address, buf.data(), len)) {
			return std::string(buf.data(), len);
		}
		return "";
	}
	static inline __forceinline     std::wstring ReadFStringW(uintptr_t fstrAddr) {
		uintptr_t dataPtr = Read<uintptr_t>(fstrAddr);
		int32_t count = Read<int32_t>(fstrAddr + 0x08);
		if (!dataPtr || count <= 0 || count > 1024) return L"";
		std::vector<wchar_t> buf(count + 1);
		KernelReadMemory(reinterpret_cast<PVOID>(dataPtr), buf.data(), count * sizeof(wchar_t));
		buf[count] = L'\0';
		return std::wstring(buf.data());
	}

	// --- 输入模拟 ---

	static inline __forceinline    bool SendKeyboardEvent(USHORT UnitId, USHORT MakeCode, USHORT Flags, USHORT Reserved, ULONG ExtraInformation) {
		m_ioPacket.KeyboardData.UnitId = UnitId;
		m_ioPacket.KeyboardData.MakeCode = MakeCode;
		m_ioPacket.KeyboardData.Flags = Flags;
		m_ioPacket.KeyboardData.Reserved = Reserved;
		m_ioPacket.KeyboardData.ExtraInformation = ExtraInformation;

		return DeviceIoControl(m_hDevice, IOCTL_SIMULATE_KEYBOARD, &m_ioPacket, sizeof(m_ioPacket), nullptr, 0, &m_bytesReturned, nullptr);
	}

	static inline __forceinline   bool SendMouseEvent(USHORT UnitId, USHORT Flags, USHORT ButtonFlags, USHORT ButtonData, ULONG RawButtons, LONG dx, LONG dy, ULONG ExtraInformation) {
		m_ioPacket.MouseData.UnitId = UnitId;
		m_ioPacket.MouseData.Flags = Flags;
		m_ioPacket.MouseData.ButtonFlags = ButtonFlags;
		m_ioPacket.MouseData.ButtonData = ButtonData;
		m_ioPacket.MouseData.RawButtons = RawButtons;
		m_ioPacket.MouseData.LastX = dx;
		m_ioPacket.MouseData.LastY = dy;
		m_ioPacket.MouseData.ExtraInformation = ExtraInformation;

		return DeviceIoControl(m_hDevice, IOCTL_SIMULATE_MOUSE, &m_ioPacket, sizeof(m_ioPacket), nullptr, 0, &m_bytesReturned, nullptr);
	}


	// --- 进程管理 ---
	static inline __forceinline   bool TerminateTargetProcess(int pid) {
		m_ioPacket.TargetProcessId = pid;

		BOOL result = DeviceIoControl(m_hDevice, IOCTL_TERMINATE_PROCESS,
			&m_ioPacket, sizeof(m_ioPacket), nullptr, 0, &m_bytesReturned, nullptr);

		if (!result) {
			std::cerr << "Terminate process failed. Error: " << GetLastError() << std::endl;
		}
		return result;
	}

	static inline __forceinline   bool HandleElevation(HANDLE ProcessHandle) {
		m_ioPacket.ProcessHandle = ProcessHandle;
		return DeviceIoControl(m_hDevice, IOCTL_HANDLE_ELEVATION, &m_ioPacket, sizeof(m_ioPacket), &m_ioPacket, sizeof(m_ioPacket), &m_bytesReturned, nullptr);
	}

private:
	// 与驱动通信的数据包结构 (根据您的定义)
	struct DriverPacket {
		// Memory operation fields
		UINT32      TargetProcessId;
		PVOID       TargetAddress;
		PVOID       UserBufferAddress;
		ULONG       TransferSize;

		// Module information fields
		const char* ModuleName;
		ULONG64     ModuleBaseAddress;
		ULONG64     ModuleSize;

		// Input simulation fields
		MOUSE_INPUT_DATA    MouseData;
		KEYBOARD_INPUT_DATA KeyboardData;

		// 要提升的句柄
		HANDLE      ProcessHandle;
	};

	// =========== 主要修改点在这里 ===========
	// 使用 inline static 在类内直接初始化静态成员变量 (C++17+)
	inline static HANDLE m_hDevice = INVALID_HANDLE_VALUE;
	inline static std::wstring m_deviceName;
	inline static std::wstring m_tempDir;
	inline static std::wstring m_driverPath;
	inline static const unsigned char* m_driverBin = nullptr;
	inline static size_t m_driverLen = 0;

	inline static DriverPacket m_ioPacket = {};
	inline static DWORD m_bytesReturned = 0;

	// 静态目标PID，由所有实例共享 (原代码已正确使用 inline)
	static inline uint32_t s_targetPid = 0;
	// =======================================


	// --- 底层内核通信封装 ---
	static inline __forceinline   bool KernelReadMemory(PVOID address, PVOID buffer, SIZE_T size) {

		m_ioPacket.TargetProcessId = s_targetPid;
		m_ioPacket.TargetAddress = address;
		m_ioPacket.UserBufferAddress = buffer;
		m_ioPacket.TransferSize = static_cast<ULONG>(size);

		return DeviceIoControl(m_hDevice, IOCTL_READ_MEMORY, &m_ioPacket, sizeof(m_ioPacket), nullptr, 0, &m_bytesReturned, nullptr);
	}

	static inline __forceinline    bool KernelWriteMemory(PVOID address, PVOID buffer, SIZE_T size) {
		m_ioPacket.TargetProcessId = s_targetPid;
		m_ioPacket.TargetAddress = address;
		m_ioPacket.UserBufferAddress = buffer;
		m_ioPacket.TransferSize = static_cast<ULONG>(size);

		return DeviceIoControl(m_hDevice, IOCTL_WRITE_MEMORY, &m_ioPacket, sizeof(m_ioPacket), nullptr, 0, &m_bytesReturned, nullptr);
	}

	static inline __forceinline  uintptr_t KernelGetModuleBase(const char* moduleName) {
		m_ioPacket.TargetProcessId = s_targetPid;
		m_ioPacket.ModuleName = moduleName;

		DeviceIoControl(m_hDevice, IOCTL_GET_MODULE_BASE, &m_ioPacket, sizeof(m_ioPacket), &m_ioPacket, sizeof(m_ioPacket), &m_bytesReturned, nullptr);

		return m_ioPacket.ModuleBaseAddress;

	}



	// --- 服务与文件管理 ---
	static inline __forceinline   bool writeDriverToFile() {
		std::ofstream file(m_driverPath, std::ios::binary);
		if (!file) return false;
		file.write(reinterpret_cast<const char*>(m_driverBin), m_driverLen);
		return file.good();
	}

	static inline __forceinline  bool installAndStartService() {
		SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
		if (!scm) return false;

		// 尝试打开并删除已存在的旧服务，确保全新安装
		SC_HANDLE service = OpenServiceW(scm, L"AutoDriverService", SERVICE_STOP | DELETE);
		if (service) {
			SERVICE_STATUS status;
			ControlService(service, SERVICE_CONTROL_STOP, &status);
			DeleteService(service);
			CloseServiceHandle(service);
			// 等待服务被彻底删除
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		service = CreateServiceW(
			scm,
			L"AutoDriverService",
			L"Auto Driver Service",
			SERVICE_ALL_ACCESS,
			SERVICE_KERNEL_DRIVER,
			SERVICE_DEMAND_START,
			SERVICE_ERROR_NORMAL,
			m_driverPath.c_str(),
			nullptr, nullptr, nullptr, nullptr, nullptr);

		if (!service) {
			if (GetLastError() == ERROR_SERVICE_EXISTS) {
				// 如果服务仍然存在，尝试启动它
				service = OpenServiceW(scm, L"AutoDriverService", SERVICE_START);
			}
			else {
				CloseServiceHandle(scm);
				return false;
			}
		}

		bool success = StartServiceW(service, 0, nullptr) || GetLastError() == ERROR_SERVICE_ALREADY_RUNNING;

		CloseServiceHandle(service);
		CloseServiceHandle(scm);
		return success;
	}

	static inline __forceinline   void openDeviceHandle() {
		m_hDevice = CreateFileW(
			m_deviceName.c_str(), // <-- 使用传入的设备名
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
		if (m_hDevice == INVALID_HANDLE_VALUE) {
			std::wcerr << L"[!] 打开设备句柄失败 (" << m_deviceName << L")，错误: " << GetLastError() << std::endl;
		}
		else {
			std::wcout << L"[+] 设备句柄已成功打开 (" << m_deviceName << L")" << std::endl;
		}
	}

	static inline __forceinline   bool uninstallService() {
		SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
		if (!scm) return false;

		SC_HANDLE service = OpenServiceW(scm, L"AutoDriverService", SERVICE_STOP | DELETE);
		if (!service) {
			CloseServiceHandle(scm);
			return false;
		}

		SERVICE_STATUS status;
		ControlService(service, SERVICE_CONTROL_STOP, &status);
		bool success = DeleteService(service);

		CloseServiceHandle(service);
		CloseServiceHandle(scm);
		return success;
	}
};
