#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <chrono>
#include <windows.h>
#include <ntddmou.h>   // 鼠标 结构体 MOUSE_INPUT_DATA
#include <ntddkbd.h>   // 键盘 结构体 KEYBOARD_INPUT_DATA



class AutoDriver {
public:
	// 构造函数: 负责写入、安装和启动驱动，并打开设备句柄
	AutoDriver(const unsigned char* driverBin, size_t driverLen)
	{

		AutoDriver::m_driverBin = driverBin;
		AutoDriver::m_driverLen = driverLen;

		wchar_t tmp[MAX_PATH] = { 0 };
		if (GetTempPathW(MAX_PATH, tmp) == 0) {
			std::cout << "[!] 获取临时路径失败，错误: " << GetLastError() << std::endl;
			return;
		}
		m_tempDir = tmp;
		m_driverPath = m_tempDir + L"ataport.sys";

		if (!writeDriverToFile()) {
			std::cout << "[!] 写入驱动文件失败：可能已近存在" << std::endl;
			initCommunication();
			return;
		}

		if (!installAndStartService()) {
			std::cout << "[!] 加载或启动驱动服务失败：可能已近存在" << std::endl;
			initCommunication();
			return;
		}

		initCommunication();

	}

	// 析构函数: 负责关闭设备句柄、卸载驱动并清理文件
	~AutoDriver() {

		ExitCommunication();

		//这里不能停止驱动服务应为不是标准的设备io使用的是无限循环的线程
			//if (uninstallService()) {
			//	std::wcout << L"[+] 驱动服务卸载成功" << std::endl;
			//}
			//else {
			//	std::wcerr << L"[!] 驱动服务卸载失败" << std::endl;
			//}

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		if (DeleteFileW(m_driverPath.c_str())) {
			std::cout << "[+] 临时驱动文件删除成功" << std::endl;
		}
		else {
			std::cout << "[*] 删除驱动文件失败，已安排在系统重启时删除" << std::endl;
			MoveFileExW(m_driverPath.c_str(), nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);
		}
	}
	static inline __forceinline void initCommunication() {

		// 在用户模式下分配共享内存
		m_ioPacket = (Requests*)VirtualAlloc((void*)0x20258270000, sizeof(Requests), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (m_ioPacket == nullptr) {
			std::cerr << "[-] 分配共享内存失败，错误码: " << GetLastError() << std::endl;
			return;
		}

		std::cout << "[+] 分配虚拟地址成功，地址: 0x" << std::hex << reinterpret_cast<uintptr_t>(m_ioPacket) << std::endl;

		memset(m_ioPacket, 0, sizeof(Requests));
		std::cout << "等待驱动握手\n";
		//等内核通知
		while (InterlockedCompareExchange(&m_ioPacket->user, 0, 1) != 1) {
			_mm_pause(); // 自旋等待
		}
		InterlockedExchange(&m_ioPacket->user, 0); // 设置为用户已接通知

		if (m_ioPacket->status == 0x1344D60) {
			std::cout << "内核握手成功\n";
		}


	}
	static inline __forceinline void ExitCommunication() {
		memset(m_ioPacket, 0, sizeof(Requests));
		m_ioPacket->Op = exit;
		InterlockedExchange(&m_ioPacket->kernel, 1); // 通知内核有新请求
		while (InterlockedCompareExchange(&m_ioPacket->user, 0, 1) != 1) {
			_mm_pause(); // 自旋等待
		}
		InterlockedExchange(&m_ioPacket->user, 0);//设置为用户已接通知

	}


	static inline __forceinline void SetTargetProcessId(int PID) {
		TargetPid = PID;

		printf("m_ioPacket=%llx\n", m_ioPacket);
		printf("m_ioPacket->Op=%llx\n", &m_ioPacket->Op);
		printf("m_ioPacket->kernel=%llx\n", &m_ioPacket->kernel);
		printf("m_ioPacket->mouse=%llx\n", &m_ioPacket->MouseData);

	}
	static inline __forceinline int  GetTargetProcessId() {
		return TargetPid;
	}
	// --- 内存操作 ---
	template<typename T>
	static inline __forceinline T Read(uintptr_t address) { T value = {}; KernelReadMemory(reinterpret_cast<PVOID>(address), &value, sizeof(T)); return value; }
	static inline __forceinline bool Read(uintptr_t address, PVOID buffer, size_t size) { return KernelReadMemory(reinterpret_cast<PVOID>(address), buffer, size); }
	template<typename T>
	static inline __forceinline bool Write(uintptr_t address, const T& value) { return KernelWriteMemory(reinterpret_cast<PVOID>(address), const_cast<T*>(&value), sizeof(T)); }
	static inline __forceinline bool Write(uintptr_t address, PVOID buffer, size_t size) { return KernelWriteMemory(reinterpret_cast<PVOID>(address), buffer, size); }
	static inline __forceinline uintptr_t GetModuleBase(const std::string& moduleName) { return KernelGetModuleBase(moduleName.c_str()); }
	
	// 从 remoteAddr 读取一个以 '\0' 结束的 ASCII/UTF-8 字符串
	static inline __forceinline std::string ReadRemoteAnsiString(uintptr_t remoteAddr, size_t maxLen = 256) {
		std::vector<char> buffer(maxLen + 1, 0);
		SIZE_T bytesRead = 0;
		if (!KernelReadMemory(
			(PVOID)remoteAddr,
			buffer.data(),
			maxLen
		)) {
			// 读失败，返回空串
			return "";
		}
		// 确保 '\0' 结束
		buffer[bytesRead < maxLen ? bytesRead : maxLen] = '\0';
		return std::string(buffer.data());
	}

	
	// --- 输入模拟 ---
	static inline __forceinline bool SendKeyboardEvent(USHORT UnitId, USHORT MakeCode, USHORT Flags, USHORT Reserved, ULONG ExtraInformation) {
		memset(m_ioPacket, 0, sizeof(Requests));
		m_ioPacket->Op = op_j;

		m_ioPacket->KeyboardData.UnitId = UnitId;
		m_ioPacket->KeyboardData.MakeCode = MakeCode;
		m_ioPacket->KeyboardData.Flags = Flags;
		m_ioPacket->KeyboardData.Reserved = Reserved;
		m_ioPacket->KeyboardData.ExtraInformation = ExtraInformation;


		InterlockedExchange(&m_ioPacket->kernel, 1); // 通知内核有新请求

		//用户请求完成
		while (InterlockedCompareExchange(&m_ioPacket->user, 0, 1) != 1) {
			_mm_pause(); // 自旋等待
		}
		InterlockedExchange(&m_ioPacket->user, 0); // 重置uready标志

		return !m_ioPacket->status;
	}

	static inline __forceinline bool SendMouseEvent(USHORT UnitId, USHORT Flags, USHORT ButtonFlags, USHORT ButtonData, ULONG RawButtons, LONG dx, LONG dy, ULONG ExtraInformation) {

		memset(m_ioPacket, 0, sizeof(Requests));
		m_ioPacket->Op = op_s;


		m_ioPacket->MouseData.UnitId = UnitId;
		m_ioPacket->MouseData.Flags = Flags;
		m_ioPacket->MouseData.ButtonFlags = ButtonFlags;
		m_ioPacket->MouseData.ButtonData = ButtonData;
		m_ioPacket->MouseData.RawButtons = RawButtons;
		m_ioPacket->MouseData.LastX = dx;
		m_ioPacket->MouseData.LastY = dy;
		m_ioPacket->MouseData.ExtraInformation = ExtraInformation;


		InterlockedExchange(&m_ioPacket->kernel, 1); // 通知内核有新请求

		//用户请求完成
		while (InterlockedCompareExchange(&m_ioPacket->user, 0, 1) != 1) {
			_mm_pause(); // 自旋等待
		}
		InterlockedExchange(&m_ioPacket->user, 0); // 重置uready标志

		return !m_ioPacket->status;

	}

	// --- 终止进程 ---
	static inline __forceinline bool TerminateTargetProcess(int pid) {
		memset(m_ioPacket, 0, sizeof(Requests));
		m_ioPacket->Op = op_z;
		m_ioPacket->TargetProcessId = pid;
		InterlockedExchange(&m_ioPacket->kernel, 1); // 通知内核有新请求

		//用户请求完成
		while (InterlockedCompareExchange(&m_ioPacket->user, 0, 1) != 1) {
			_mm_pause(); // 自旋等待
		}
		InterlockedExchange(&m_ioPacket->user, 0); // 重置uready标志

		return !m_ioPacket->status;
	}



	static inline __forceinline void IO() {
		memset(m_ioPacket, 0, sizeof(Requests));
		m_ioPacket->Op = op_o;
		InterlockedExchange(&m_ioPacket->kernel, 1); // 通知内核有新请求

		//用户请求完成
		while (InterlockedCompareExchange(&m_ioPacket->user, 0, 1) != 1) {
			_mm_pause(); // 自旋等待
		}
		InterlockedExchange(&m_ioPacket->user, 0); // 重置uready标志

	}
private:

	// 定义请求操作的枚举类型
	typedef enum _req_op {
		op_o = 0,           //空调用
		op_r = 1,           // 读操作
		op_w = 2,           // 写操作
		op_m = 3,				//获取模块基地址
		op_j = 4,				//键盘事件
		op_s = 5,				//鼠标事件
		op_z = 6,				//终止进程


		exit = 444//用户进程退出
	} req_op;

	// 将在队列中使用的请求实例结构体
	typedef struct _req_Obj {


		volatile LONG kernel;        //由用户模式设置 1 = 内核有待处理的请求, 0 = 请求已完成
		volatile LONG user;       //由内核模式设置 1 = 用户模式有待处理的请求, 0 = 请求已完成
		req_op Op;                  // 请求操作类型
		NTSTATUS status = -1;            // 操作状态

		//内存读取
		UINT32      TargetProcessId;
		PVOID       TargetAddress;
		char       UserBufferAddress[4096];
		ULONG       TransferSize;

		//模块基地址获取
		char	ModuleName[100];
		ULONG64     ModuleBaseAddress;
		ULONG64     ModuleSize;

		//模拟键盘和鼠标输入
		MOUSE_INPUT_DATA    MouseData;
		KEYBOARD_INPUT_DATA KeyboardData;

	} Requests;



	inline static std::wstring m_tempDir;
	inline static std::wstring m_driverPath;
	inline static const unsigned char* m_driverBin = nullptr;
	inline static size_t m_driverLen = 0;
	inline static Requests* m_ioPacket;

	inline static int TargetPid;




	// --- 底层内核通信封装 ---
	static inline __forceinline bool KernelReadMemory(PVOID address, PVOID buffer, SIZE_T size) {

		memset(m_ioPacket, 0, sizeof(Requests));
		m_ioPacket->Op = op_r;
		m_ioPacket->TargetProcessId = TargetPid;
		m_ioPacket->TargetAddress = address;

		m_ioPacket->TransferSize = static_cast<ULONG>(size);
		InterlockedExchange(&m_ioPacket->kernel, 1); // 通知内核有新请求


		while (InterlockedCompareExchange(&m_ioPacket->user, 0, 1) != 1) {
			_mm_pause();
		}
		InterlockedExchange(&m_ioPacket->user, 0);

		memcpy(buffer, m_ioPacket->UserBufferAddress, size);

		return !m_ioPacket->status;
	}
	static inline __forceinline bool KernelWriteMemory(PVOID address, PVOID buffer, SIZE_T size) {

		memset(m_ioPacket, 0, sizeof(Requests));
		m_ioPacket->Op = op_w;
		m_ioPacket->TargetProcessId = TargetPid;
		m_ioPacket->TargetAddress = address;
		memcpy(m_ioPacket->UserBufferAddress, buffer, size);

		m_ioPacket->TransferSize = static_cast<ULONG>(size);
		InterlockedExchange(&m_ioPacket->kernel, 1); // 通知内核有新请求


		while (InterlockedCompareExchange(&m_ioPacket->user, 0, 1) != 1) {
			_mm_pause();
		}
		InterlockedExchange(&m_ioPacket->user, 0);

		return !m_ioPacket->status;

	}
	static inline __forceinline uintptr_t KernelGetModuleBase(const char* moduleName) {

		memset(m_ioPacket, 0, sizeof(Requests));
		m_ioPacket->Op = op_m;
		m_ioPacket->TargetProcessId = TargetPid;

		strcpy_s(m_ioPacket->ModuleName, 100, moduleName);


		InterlockedExchange(&m_ioPacket->kernel, 1);

		while (InterlockedCompareExchange(&m_ioPacket->user, 0, 1) != 1) {
			_mm_pause();
		}
		InterlockedExchange(&m_ioPacket->user, 0);

		return m_ioPacket->ModuleBaseAddress;
	}

	// --- 服务与文件管理 ---
	static inline __forceinline bool writeDriverToFile() {
		std::ofstream file(m_driverPath, std::ios::binary);
		if (!file) return false;
		file.write(reinterpret_cast<const char*>(m_driverBin), m_driverLen);
		return file.good();
	}
	static inline __forceinline bool installAndStartService() {
		SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
		if (!scm) return false;

		SC_HANDLE service = OpenServiceW(scm, L"ataport", SERVICE_STOP | DELETE);
		if (service) {
			SERVICE_STATUS status;
			ControlService(service, SERVICE_CONTROL_STOP, &status);
			DeleteService(service);
			CloseServiceHandle(service);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		service = CreateService(scm, L"ataport", L"ataportService", SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, m_driverPath.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr);
		if (!service) {
			if (GetLastError() == ERROR_SERVICE_EXISTS) {
				service = OpenServiceW(scm, L"ataport", SERVICE_START);
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
	static inline __forceinline bool uninstallService() {
		SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
		if (!scm) return false;
		SC_HANDLE service = OpenServiceW(scm, L"ataport", SERVICE_STOP | DELETE);
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