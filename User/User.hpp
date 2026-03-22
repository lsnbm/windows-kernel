#pragma once
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <winbase.h>
#include <fstream>
#include <thread>
#include <chrono>      // For std::chrono::milliseconds
#include <ntddmou.h>   // ��� �ṹ�� MOUSE_INPUT_DATA
#include <ntddkbd.h>   // ���� �ṹ�� KEYBOARD_INPUT_DATA

// ����IOCTL������ (�������Ķ���)
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
	// ���캯��: ����д�롢��װ����������������豸���
	AutoDriver(const std::wstring& deviceName, const unsigned char* driverBin, size_t driverLen)
	{
		AutoDriver::m_deviceName = deviceName;
		AutoDriver::m_driverBin = driverBin;
		AutoDriver::m_driverLen = driverLen;
		AutoDriver::m_hDevice = INVALID_HANDLE_VALUE;

		wchar_t tmp[MAX_PATH] = { 0 };
		if (GetTempPathW(MAX_PATH, tmp) == 0) {
			std::wcerr << L"[!] ��ȡ��ʱ·��ʧ�ܣ�����: " << GetLastError() << std::endl;
			return;
		}
		m_tempDir = tmp;
		m_driverPath = m_tempDir + L"72c9b540-4c42-4972-b4f7-08d56c243d3c.tmp"; // ʹ�ø����ص�����

		if (!writeDriverToFile()) {
			std::wcerr << L"[!] д�������ļ�ʧ��" << std::endl;
			return;
		}

		if (!installAndStartService()) {
			std::wcerr << L"[!] ���ػ������������ʧ��" << std::endl;
			return;
		}

		openDeviceHandle();
	}

	// ��������: ����ر��豸�����ж�������������ļ�
	~AutoDriver() {
		if (m_hDevice != INVALID_HANDLE_VALUE) {
			CloseHandle(m_hDevice);
		}

		if (uninstallService()) {
			std::wcout << L"[+] ��������ж�سɹ�" << std::endl;
		}
		else {
			std::wcerr << L"[!] ��������ж��ʧ��" << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		if (DeleteFileW(m_driverPath.c_str())) {
			std::wcout << L"[+] ��ʱ�����ļ�ɾ���ɹ�" << std::endl;
		}
		else {
			std::wcerr << L"[*] ɾ�������ļ�ʧ�ܣ��Ѱ�����ϵͳ����ʱɾ��" << std::endl;
			MoveFileExW(m_driverPath.c_str(), nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);
		}
	}

	// ��������Ƿ�ɹ����ز�׼������
	static inline __forceinline bool IsInitialized() {
		return m_hDevice != INVALID_HANDLE_VALUE;
	}

	// ����ȫ��Ŀ�����ID
	static inline __forceinline  void SetTargetProcessId(uint32_t pid) {
		if (!pid) return;
		s_targetPid = pid;
		m_ioPacket.TargetProcessId = pid;//��ռλ��ֹ�����������޸�
		//֪ͨһ������
		DeviceIoControl(m_hDevice, IOCTL_NULL_IO, &m_ioPacket, sizeof(DriverPacket), &m_ioPacket, sizeof(DriverPacket), &m_bytesReturned, nullptr);

	}
	// ��ȡ��ǰĿ�����ID
	static inline __forceinline  uint32_t GetTargetProcessId() {
		return s_targetPid;
	}


	// --- �ڴ���� ---
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

	// --- ����ģ�� ---

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


	// --- ���̹��� ---
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
	// ������ͨ�ŵ����ݰ��ṹ (�������Ķ���)
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

		// Ҫ�����ľ��
		HANDLE      ProcessHandle;
	};

	// =========== ��Ҫ�޸ĵ������� ===========
	// ʹ�� inline static ������ֱ�ӳ�ʼ����̬��Ա���� (C++17+)
	inline static HANDLE m_hDevice = INVALID_HANDLE_VALUE;
	inline static std::wstring m_deviceName;
	inline static std::wstring m_tempDir;
	inline static std::wstring m_driverPath;
	inline static const unsigned char* m_driverBin = nullptr;
	inline static size_t m_driverLen = 0;

	inline static DriverPacket m_ioPacket = {};
	inline static DWORD m_bytesReturned = 0;

	// ��̬Ŀ��PID��������ʵ������ (ԭ��������ȷʹ�� inline)
	static inline uint32_t s_targetPid = 0;
	// =======================================


	// --- �ײ��ں�ͨ�ŷ�װ ---
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



	// --- �������ļ����� ---
	static inline __forceinline   bool writeDriverToFile() {
		std::ofstream file(m_driverPath, std::ios::binary);
		if (!file) return false;
		file.write(reinterpret_cast<const char*>(m_driverBin), m_driverLen);
		return file.good();
	}

	static inline __forceinline  bool installAndStartService() {
		SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
		if (!scm) return false;

		// ���Դ򿪲�ɾ���Ѵ��ڵľɷ���ȷ��ȫ�°�װ
		SC_HANDLE service = OpenServiceW(scm, L"AutoDriverService", SERVICE_STOP | DELETE);
		if (service) {
			SERVICE_STATUS status;
			ControlService(service, SERVICE_CONTROL_STOP, &status);
			DeleteService(service);
			CloseServiceHandle(service);
			// �ȴ����񱻳���ɾ��
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
				// ���������Ȼ���ڣ����������
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
			m_deviceName.c_str(), // <-- ʹ�ô�����豸��
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			nullptr,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			nullptr
		);
		if (m_hDevice == INVALID_HANDLE_VALUE) {
			std::wcerr << L"[!] ���豸���ʧ�� (" << m_deviceName << L")������: " << GetLastError() << std::endl;
		}
		else {
			std::wcout << L"[+] �豸����ѳɹ��� (" << m_deviceName << L")" << std::endl;
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
