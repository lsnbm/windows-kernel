#pragma once
#include<ntifs.h>
#include"strXOR.h"


#define WINDOWS_1803 17134
#define WINDOWS_1809 17763
#define WINDOWS_1903 18362
#define WINDOWS_1909 18363
#define WINDOWS_2004 19041
#define WINDOWS_20H2 19569
#define WINDOWS_21H2 20180
#define WINDOWS_22H2 22621
#define WINDOWS_23H2 22631
#define WINDOWS_24H2 26100


inline  RTL_OSVERSIONINFOW SystemVersionInf;//系统版本信息

// 用于 ZwQuerySystemInformation 的信息类别枚举
typedef enum _SystemInformationClass
{
	SystemBasicInformation,
	SystemProcessorInformation,
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
	SystemPathInformation,
	SystemProcessInformation,
	SystemCallCountInformation,
	SystemDeviceInformation,
	SystemProcessorPerformanceInformation,
	SystemFlagsInformation,
	SystemCallTimeInformation,
	SystemModuleInformation = 0x0B
} SystemInformationClass, * PSystemInformationClass;

// 系统模块信息结构体
typedef struct _RtlProcessModuleInformation
{
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR  FullPathName[256];
} RtlProcessModuleInformation, * PRtlProcessModuleInformation;

// 包含所有系统模块信息的结构体
typedef struct _RtlProcessModules
{
	ULONG NumberOfModules;
	RtlProcessModuleInformation Modules[1];
} RtlProcessModules, * PRtlProcessModules;

// (未文档化) 加载器数据表条目结构体
typedef struct _LdrDataTableEntry {
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	LIST_ENTRY HashLinks;
	PVOID SectionPointer;
	ULONG CheckSum;
	ULONG TimeDateStamp;
} LdrDataTableEntry, * PLdrDataTableEntry;


typedef NTSTATUS(*C_ZwQuerySystemInformation)(ULONG systemInformationClass, PVOID systemInformation, ULONG systemInformationLength, PULONG returnLength);
inline C_ZwQuerySystemInformation L_ZwQuerySystemInformation = nullptr;


typedef BOOLEAN(*C_RtlEqualUnicodeString)(PCUNICODE_STRING String1, PCUNICODE_STRING String2, BOOLEAN CaseInSensitive);
inline C_RtlEqualUnicodeString L_RtlEqualUnicodeString = nullptr;

typedef void (*C_RtlInitUnicodeString)(PUNICODE_STRING DestinationString, PCWSTR SourceString);
inline C_RtlInitUnicodeString L_RtlInitUnicodeString = nullptr;

typedef VOID(*C_RtlInitAnsiString)(_Out_ PANSI_STRING DestinationString, _In_opt_z_ __drv_aliasesMem PCSZ SourceString);
inline C_RtlInitAnsiString L_RtlInitAnsiString = nullptr;

typedef NTSTATUS(*C_RtlAnsiStringToUnicodeString)(_When_(AllocateDestinationString, _Out_ _At_(DestinationString->Buffer, __drv_allocatesMem(Mem)))_When_(!AllocateDestinationString, _Inout_)PUNICODE_STRING DestinationString, _In_ PCANSI_STRING SourceString, _In_ BOOLEAN AllocateDestinationString);
inline C_RtlAnsiStringToUnicodeString L_RtlAnsiStringToUnicodeString = nullptr;

typedef BOOLEAN(*C_RtlSuffixUnicodeString)(_In_ PCUNICODE_STRING String1, _In_ PCUNICODE_STRING String2, _In_ BOOLEAN CaseInSensitive);
inline C_RtlSuffixUnicodeString L_RtlSuffixUnicodeString = nullptr;



typedef VOID(*C_RtlFreeUnicodeString)(_Inout_ _At_(UnicodeString->Buffer, _Frees_ptr_opt_)PUNICODE_STRING UnicodeString);
inline C_RtlFreeUnicodeString L_RtlFreeUnicodeString = nullptr;


typedef  PVOID(*C_ExAllocatePool2)(_In_ POOL_FLAGS Flags, _In_ SIZE_T NumberOfBytes, _In_ ULONG Tag);
inline C_ExAllocatePool2 L_ExAllocatePool2 = nullptr;

typedef VOID(*C_ExFreePoolWithTag)(_Pre_notnull_ __drv_freesMem(Mem) PVOID P, _In_ ULONG Tag);
inline C_ExFreePoolWithTag L_ExFreePoolWithTag = nullptr;


typedef NTSTATUS(*C_ObOpenObjectByPointer)(_In_ PVOID Object, _In_ ULONG HandleAttributes, _In_opt_ PACCESS_STATE PassedAccessState, _In_ ACCESS_MASK DesiredAccess, _In_opt_ POBJECT_TYPE ObjectType, _In_ KPROCESSOR_MODE AccessMode, _Out_ PHANDLE Handle);
inline C_ObOpenObjectByPointer L_ObOpenObjectByPointer = nullptr;

typedef NTSTATUS(*C_ObCloseHandle)(_In_ _Post_ptr_invalid_ HANDLE Handle, _In_ KPROCESSOR_MODE PreviousMode);
inline C_ObCloseHandle L_ObCloseHandle = nullptr;

typedef PVOID(*C_MmGetSystemRoutineAddress)(PUNICODE_STRING SystemRoutineName);
inline C_MmGetSystemRoutineAddress L_MmGetSystemRoutineAddress = nullptr;

typedef PPEB(*C_PsGetProcessPeb)(PEPROCESS Process);
inline C_PsGetProcessPeb L_PsGetProcessPeb = nullptr;

typedef  PCHAR(*C_PsGetProcessImageFileName)(PEPROCESS Process);
inline C_PsGetProcessImageFileName L_PsGetProcessImageFileName = nullptr;

typedef NTSTATUS(*C_ObReferenceObjectByName)(PUNICODE_STRING ObjectName, ULONG Attributes, PACCESS_STATE PassedAccessState, ACCESS_MASK DesiredAccess, POBJECT_TYPE ObjectType, KPROCESSOR_MODE AccessMode, PVOID ParseContext, PDRIVER_OBJECT* Object);
inline C_ObReferenceObjectByName L_ObReferenceObjectByName = nullptr;

typedef PVOID(*C_RtlFindExportedRoutineByName)(PVOID ImageBase, PCCH RoutineName);
inline C_RtlFindExportedRoutineByName L_RtlFindExportedRoutineByName = nullptr;

typedef HANDLE(*C_PsGetProcessId)(PEPROCESS Process);
inline C_PsGetProcessId L_PsGetProcessId = nullptr;

typedef void(*C_IoRegisterDriverReinitialization)(PDRIVER_OBJECT DriverObject, PDRIVER_REINITIALIZE DriverReinitializationRoutine, PVOID Context);
inline C_IoRegisterDriverReinitialization L_IoRegisterDriverReinitialization = nullptr;

typedef NTSTATUS(*C_RtlDeleteRegistryValue)(ULONG RelativeTo, PCWSTR Path, PCWSTR ValueName);
inline C_RtlDeleteRegistryValue L_RtlDeleteRegistryValue = nullptr;

typedef LONG(*C_RtlCompareUnicodeString)(_In_ PCUNICODE_STRING String1, _In_ PCUNICODE_STRING String2, _In_ BOOLEAN CaseInSensitive);
inline C_RtlCompareUnicodeString L_RtlCompareUnicodeString = nullptr;

typedef NTSTATUS(*C_RtlGetVersion)(_Out_ _At_(lpVersionInformation->dwOSVersionInfoSize, _Pre_ _Valid_)_When_(lpVersionInformation->dwOSVersionInfoSize == sizeof(RTL_OSVERSIONINFOEXW), _At_((PRTL_OSVERSIONINFOEXW)lpVersionInformation, _Out_))PRTL_OSVERSIONINFOW lpVersionInformation);
inline C_RtlGetVersion L_RtlGetVersion = nullptr;

typedef BOOLEAN(*C_MmIsAddressValid)(_In_ PVOID VirtualAddress);
inline C_MmIsAddressValid L_MmIsAddressValid = nullptr;



typedef ULONG64 (*C_PsGetProcessSectionBaseAddress)(__in PEPROCESS Process);
inline C_PsGetProcessSectionBaseAddress L_PsGetProcessSectionBaseAddress = nullptr;



typedef NTSTATUS(__fastcall* C_MiProcessLoaderEntry)(PVOID pDriverSection, BOOLEAN bLoad);
inline C_MiProcessLoaderEntry L_MiProcessLoaderEntry = nullptr;





// 为了避免命名冲突，将结构体名称前加上 "MY_" 前缀
typedef struct _MY_SYSTEM_MODULE {
	PVOID  Reserved1;
	PVOID  Reserved2;
	PVOID  ImageBase;
	ULONG  ImageSize;
	ULONG  Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT PathLength;
	CHAR   ImageName[256];
} MY_SYSTEM_MODULE, * PMY_SYSTEM_MODULE;

// 同样，修改这个结构体的名称
typedef struct _MY_SYSTEM_MODULE_INFORMATION {
	ULONG             ModulesCount;
	MY_SYSTEM_MODULE  Modules[1];
} MY_SYSTEM_MODULE_INFORMATION, * PMY_SYSTEM_MODULE_INFORMATION;



namespace EPROCESS_MEMBER_OFFSET {

	inline ULONG UniqueProcessId = 0;
	inline	ULONG ImageFileNameOffset = 0;
	inline ULONG ActiveProcessLinks = 0;
	inline ULONG Peb = 0;
	inline ULONG SectionBaseAddress = 0;
	inline ULONG VAD_ROOT = 0;


	// 初始化pid在eprocess的偏移量
	inline BOOLEAN InitializeUniqueProcessIdOffset() {

		PUCHAR functionCode = (PUCHAR)L_PsGetProcessId;

		if (functionCode == NULL) {
			KdPrint(("EPROCESS_OFFSETS: 错误: 无法解析 PsGetProcessId 函数地址。\n"));
			return FALSE;
		}

		for (ULONG i = 0; i < 64; ++i) {
			// 将两种模式的检查合并，减少代码重复
			// 模式 1: mov eax, [ecx+offset] (8B 81 ?? ?? ?? ??)
			if (functionCode[i] == 0x8B && functionCode[i + 1] == 0x81) {
				UniqueProcessId = *(PULONG)(functionCode + i + 2);
				// 成功时不输出，直接返回
				return TRUE;
			}
			// 模式 2: mov rax, [rcx+offset] (48 8B 81 ?? ?? ?? ??)
			if (functionCode[i] == 0x48 && functionCode[i + 1] == 0x8B && functionCode[i + 2] == 0x81) {
				UniqueProcessId = *(PULONG)(functionCode + i + 3);
				// 成功时不输出，直接返回
				return TRUE;
			}
		}

		KdPrint(("EPROCESS_OFFSETS: 错误: 未能找到 UniqueProcessId 的特征指令。\n"));
		return FALSE;
	}

	//初始化进程名在eprocess的偏移量
	inline BOOLEAN InitializeImageFileNameOffset() {

		PUCHAR functionCode =(PUCHAR)L_PsGetProcessImageFileName;

		if (functionCode == NULL) {
			KdPrint(("EPROCESS_OFFSETS: 错误: 无法解析 PsGetProcessImageFileName 函数地址。\n"));
			return FALSE;
		}

		// 在函数的前64个字节内搜索特征码
		for (ULONG i = 0; i < 64; ++i) {
			// 特征码: lea rax, [rcx+offset] (48 8D 81 ?? ?? ?? ??)
			if (functionCode[i] == 0x48 &&
				functionCode[i + 1] == 0x8D &&
				functionCode[i + 2] == 0x81)
			{
				// 找到了! 接下来的4个字节就是偏移量
				ImageFileNameOffset = *(PULONG)(functionCode + i + 3);
				KdPrint(("EPROCESS_OFFSETS: 成功找到 ImageFileName 偏移量: 0x%X\n", ImageFileNameOffset));
				return TRUE;
			}
		}

		KdPrint(("EPROCESS_OFFSETS: 错误: 未能找到 ImageFileName 的特征指令。\n"));
		return FALSE;
	}


	// 初始化 ActiveProcessLinks 偏移量
	inline BOOLEAN InitializeActiveProcessLinksOffset() {
		// 依赖检查保持不变，这是正确的逻辑
		if (UniqueProcessId == 0) {
			KdPrint(("EPROCESS_OFFSETS: 错误: 必须先成功初始化 UniqueProcessId 偏移量。\n"));
			return FALSE;
		}

		PEPROCESS systemProcess = PsInitialSystemProcess;

		for (ULONG offset = 0; offset < 2048; offset += sizeof(PVOID)) {
			// 跳过 UniqueProcessId 偏移量，避免误判
			if (offset == UniqueProcessId) {
				continue;
			}

			__try {
				PLIST_ENTRY links = (PLIST_ENTRY)((PUCHAR)systemProcess + offset);

				// 优化验证逻辑：先做最快、最不可能通过的检查
				// 1. 检查指针是否看起来像一个有效的双向链表
				if (links->Flink->Blink != links || links->Blink->Flink != links) {
					continue; // 如果不是，立即进行下一次循环
				}

				// 2. 只有通过了初步检查，才进行更耗费资源的下一步验证
				PEPROCESS nextProcess = (PEPROCESS)((PUCHAR)links->Flink - offset);

				// 检查 nextProcess 是否是一个有效的 EPROCESS 指针，且不是自身
				if (nextProcess == systemProcess || !MmIsAddressValid(nextProcess)) {
					continue;
				}

				HANDLE nextPid = *(PHANDLE)((PUCHAR)nextProcess + UniqueProcessId);

				// 最后的确认：下一个进程的PID是否有效
				if (HandleToUlong(nextPid) != 0) {
					ActiveProcessLinks = offset;
					// 成功时不输出
					return TRUE;
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				// 访问异常是扫描过程中的正常现象，直接继续
			}
		}

		KdPrint(("EPROCESS_OFFSETS: 错误: 未能找到 ActiveProcessLinks 的有效偏移量。\n"));
		return FALSE;
	}

	// 初始化 PEB 偏移量
	inline BOOLEAN InitializePebOffset() {
		PEPROCESS explorerProcess = NULL;

		__try {
			PEPROCESS currentProcess = PsInitialSystemProcess;
			do {
				if (_stricmp(L_PsGetProcessImageFileName(currentProcess), "explorer.exe") == 0) {
					explorerProcess = currentProcess;
					break;
				}
				PLIST_ENTRY listEntry = (PLIST_ENTRY)((PUCHAR)currentProcess + ActiveProcessLinks);
				currentProcess = (PEPROCESS)((PUCHAR)listEntry->Flink - ActiveProcessLinks);
			} while (currentProcess != PsInitialSystemProcess);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			// 异常发生时，explorerProcess 保持为 NULL
		}

		if (explorerProcess == NULL) {
			KdPrint(("EPROCESS_OFFSETS: 错误: 未能找到 explorer.exe 进程。\n"));
			return FALSE;
		}

		PPEB explorerPeb = L_PsGetProcessPeb(explorerProcess);
		if (explorerPeb == NULL) {
			KdPrint(("EPROCESS_OFFSETS: 错误: PsGetProcessPeb 未能返回 explorer.exe 的 PEB。\n"));
			return FALSE;
		}

		// 扫描逻辑保持不变，它已经很高效
		for (ULONG offset = 0; offset < 4096; offset += sizeof(PVOID)) {
			__try {
				if (*(PVOID*)((PUCHAR)explorerProcess + offset) == explorerPeb) {
					Peb = offset;
					// 成功时不输出
					return TRUE;
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				// 忽略无效内存访问
			}
		}

		KdPrint(("EPROCESS_OFFSETS: 错误: 在 EPROCESS 中扫描未找到 Peb 地址。\n"));
		return FALSE;
	}

	//初始化进程加载地址偏移
	inline BOOLEAN InitializeSectionBaseAddressOffset() {
		PUCHAR functionCode = (PUCHAR)L_PsGetProcessSectionBaseAddress;

		if (functionCode == NULL) {
			KdPrint(("EPROCESS_OFFSETS: 错误: PsGetProcessSectionBaseAddress 函数地址为空。\n"));
			return FALSE;
		}

		// 搜索特征码: 48 8B 81 ?? ?? ?? ?? 
		// 这是 "mov rax, [rcx+offset]" 指令
		for (ULONG i = 0; i < 32; ++i) {
			if (functionCode[i] == 0x48 &&
				functionCode[i + 1] == 0x8B &&
				functionCode[i + 2] == 0x81) {

				// 如果找到特征码，那么接下来的4个字节就是我们需要的偏移量
				SectionBaseAddress = *(PULONG)(functionCode + i + 3);


				KdPrint(("EPROCESS_OFFSETS: 成功定位 SectionBaseAddress 偏移: 0x%p\n", SectionBaseAddress));

				return TRUE;
			}
		}

		KdPrint(("EPROCESS_OFFSETS: 错误: 未能在 PsGetProcessSectionBaseAddress 中找到特征指令。\n"));
		return FALSE;
	}


	//初始化VAD_ROOT偏移量
	inline BOOLEAN InitializeVAD_ROOTOffset() {

		PVOID ntoskrnlBase = NULL;
		SIZE_T ntoskrnlSize = 0;

		// --- 内联 GetNtoskrnlInfo 的逻辑 ---
		NTSTATUS status = STATUS_INSUFFICIENT_RESOURCES;
		ULONG neededSize = 0;
		PMY_SYSTEM_MODULE_INFORMATION pModuleInfo = NULL;

		L_ZwQuerySystemInformation(11, NULL, 0, &neededSize);
		if (neededSize == 0)
		{
		
			return 0;
		}

		pModuleInfo = (PMY_SYSTEM_MODULE_INFORMATION)ExAllocatePool2(POOL_FLAG_PAGED, neededSize, 'VADT');
		if (!pModuleInfo)
		{
		
			return 0;
		}

		status = L_ZwQuerySystemInformation(11, pModuleInfo, neededSize, NULL);
		if (!NT_SUCCESS(status))
		{
			
			ExFreePoolWithTag(pModuleInfo, 'VADT');
			return 0;
		}

		if (pModuleInfo->ModulesCount > 0)
		{
			ntoskrnlBase = pModuleInfo->Modules[0].ImageBase;
			ntoskrnlSize = pModuleInfo->Modules[0].ImageSize;
			
		}
		else
		{
			
			ExFreePoolWithTag(pModuleInfo, 'VADT');
			return 0;
		}

		ExFreePoolWithTag(pModuleInfo, 'VADT');


		// --- 内联 FindPattern 的逻辑 ---
		// 特征码: lea r11, [rbx + offset]
		// 机器码: 4C 8D 9B XX XX XX XX
		UCHAR pattern[] = { 0x4C, 0x8D, 0x9B };
		PVOID foundAddress = NULL;

		if (!ntoskrnlBase || !ntoskrnlSize)
		{
			
			return 0;
		}

		PUCHAR searchBase = (PUCHAR)ntoskrnlBase;
		PUCHAR searchEnd = searchBase + ntoskrnlSize - sizeof(pattern);

		for (PUCHAR current = searchBase; current < searchEnd; ++current)
		{
			if (RtlCompareMemory(current, pattern, sizeof(pattern)) == sizeof(pattern))
			{
				foundAddress = current;
				break; // 找到后立即退出循环
			}
		}

		// --- 提取偏移量 ---
		if (foundAddress)
		{
		

			// 偏移量是紧跟在特征码后面的4个字节 (32位有符号整数)
			VAD_ROOT = *(PULONG)((PUCHAR)foundAddress + sizeof(pattern));
			KdPrint(("EPROCESS_OFFSETS: 成功定位 VAD_ROOT 偏移: 0x%p\n", VAD_ROOT));
		}


		return VAD_ROOT;
		
	}








	//返回指定PID的EPROCESS结构体
	inline PEPROCESS FindEprocessByProcessId(IN HANDLE targetPid) {
		if (targetPid==0) {
			return 0;
		}
		//直接使用命名空间内的偏移量，避免创建局部变量
		PEPROCESS startProcess = PsInitialSystemProcess;
		PEPROCESS currentProcess = startProcess;

		__try {
			do {
				// 直接比较 HANDLE，比转换成 ULONG 更高效、更安全
				if (*(PHANDLE)((PUCHAR)currentProcess + UniqueProcessId) == targetPid) {
					return currentProcess; // 找到后立即返回
				}

				PLIST_ENTRY processLinksEntry = (PLIST_ENTRY)((PUCHAR)currentProcess + ActiveProcessLinks);
				currentProcess = (PEPROCESS)((PUCHAR)processLinksEntry->Flink - ActiveProcessLinks);

			} while (currentProcess != startProcess);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			KdPrint(("严重错误: 在遍历进程链表时发生内存访问异常。\n"));
			return NULL; // 发生异常时返回 NULL
		}

		// 循环走完都没找到，说明进程不存在
		return NULL;
	}

	//用进程名获取pid
	inline ULONG FindEprocessByProcessName(IN PCSTR pProcessName) {
		if (pProcessName == NULL) {
			KdPrint(("错误: 输入的进程名指针 (pProcessName) 为 NULL。\n"));
			return NULL;
		}
		PEPROCESS startProcess = PsInitialSystemProcess;
		PEPROCESS currentProcess = startProcess;

		__try {
			do {
				// 获取当前进程的名称指针
				PCHAR currentName = (PCHAR)((PUCHAR)currentProcess + ImageFileNameOffset);

				// 比较进程名
				if (_stricmp(currentName, pProcessName) == 0) {
					return *(ULONG*)((PUCHAR)currentProcess + UniqueProcessId);
				}

				// 通过 ActiveProcessLinks 链表移动到下一个进程
				PLIST_ENTRY processLinksEntry = (PLIST_ENTRY)((PUCHAR)currentProcess + ActiveProcessLinks);
				// 从链表项指针反向计算出下一个 EPROCESS 结构的基地址
				currentProcess = (PEPROCESS)((PUCHAR)processLinksEntry->Flink - ActiveProcessLinks);

				// 循环直到回到起点，表示已遍历完所有活动进程
			} while (currentProcess != startProcess);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {
			// 捕获到任何内存访问异常
			KdPrint(("严重错误: 在遍历进程链表时发生内存访问异常。可能原因：链表损坏或并发修改。\n"));
			return NULL; 
		}

		KdPrint(("信息: 未能找到名为 \"%s\" 的进程。\n", pProcessName));
		return NULL;
	}


}


// 从指定的内核模块中查找导出函数的地址
inline PVOID GetSystemModuleExport(LPCWSTR moduleName, LPCSTR routineName)
{
	PVOID pModuleBase = NULL;
	UNICODE_STRING name;
	L_RtlInitUnicodeString(&name, OBFUSCATE(L"PsLoadedModuleList").decrypt());
	// 获取 PsLoadedModuleList 的地址
	PLIST_ENTRY pModuleList = (PLIST_ENTRY)L_MmGetSystemRoutineAddress(&name);
	if (!pModuleList)
	{
		return NULL;
	}

	UNICODE_STRING searchModuleName;
	L_RtlInitUnicodeString(&searchModuleName, moduleName);

	// 遍历链表
	for (PLIST_ENTRY pLink = pModuleList->Flink; pLink != pModuleList; pLink = pLink->Flink)
	{
		// 从链表项获取LDR_DATA_TABLE_ENTRY结构
		PLdrDataTableEntry pEntry = CONTAINING_RECORD(pLink, LdrDataTableEntry, InLoadOrderModuleList);

		// 比较模块名 (不区分大小写)
		if (L_RtlEqualUnicodeString(&pEntry->BaseDllName, &searchModuleName, TRUE))
		{
			pModuleBase = pEntry->DllBase;
			break; // 找到后退出循环
		}
	}

	if (pModuleBase)
	{
		// 如果找到了模块基址，则从中查找导出函数
		return L_RtlFindExportedRoutineByName(pModuleBase, routineName);
	}

	return NULL;
}


inline ULONG_PTR getprocessdirbase(PEPROCESS targetprocess)
{
	if (!targetprocess) return 0;

	// 主要偏移
	ULONG_PTR dirbase = *(PULONG_PTR)((PUCHAR)targetprocess + 0x28);
	if (dirbase == 0)
	{
		static UINT32 offset = 0;
		if (offset == 0) // 只在第一次调用时计算
		{
			switch (SystemVersionInf.dwBuildNumber)
			{
			case WINDOWS_1803:
			case WINDOWS_1809:
				offset = 0x0278;
				break;
			case WINDOWS_1903:
			case WINDOWS_1909:
				offset = 0x0280;
				break;
			default: // WINDOWS_2004 and newer
				offset = 0x0388;
				break;
			}
		}

		// 回退偏移
		dirbase = *(PULONG_PTR)((PUCHAR)targetprocess + offset);
	}
	return dirbase;
}




inline void initFun() {

	UNICODE_STRING routineName;

	// 基础函数直接赋值
	L_RtlInitUnicodeString = &RtlInitUnicodeString;
	KdPrint((OBFUSCATE("L_RtlInitUnicodeString=%p\n").decrypt(), L_RtlInitUnicodeString));

	L_MmGetSystemRoutineAddress = &MmGetSystemRoutineAddress;
	KdPrint((OBFUSCATE("L_MmGetSystemRoutineAddress=%p\n").decrypt(), L_MmGetSystemRoutineAddress));


	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"1").decrypt());
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"2").decrypt());
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"3").decrypt());
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"4").decrypt());
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"5").decrypt());
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"6").decrypt());
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"7").decrypt());


	// 初始化 RtlInitAnsiString 函数指针
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"RtlInitAnsiString").decrypt());
	L_RtlInitAnsiString = (C_RtlInitAnsiString)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint((OBFUSCATE("L_RtlInitAnsiString=%p\n").decrypt(), L_RtlInitAnsiString));

	// 初始化 RtlAnsiStringToUnicodeString 函数指针
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"RtlAnsiStringToUnicodeString").decrypt());
	L_RtlAnsiStringToUnicodeString = (C_RtlAnsiStringToUnicodeString)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint((OBFUSCATE("L_RtlAnsiStringToUnicodeString=%p\n").decrypt(), L_RtlAnsiStringToUnicodeString));

	// 初始化 RtlSuffixUnicodeString 函数指针
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"RtlSuffixUnicodeString").decrypt());
	L_RtlSuffixUnicodeString = (C_RtlSuffixUnicodeString)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint((OBFUSCATE("L_RtlSuffixUnicodeString=%p\n").decrypt(), L_RtlSuffixUnicodeString));

	// 初始化 RtlFreeUnicodeString 函数指针
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"RtlFreeUnicodeString").decrypt());
	L_RtlFreeUnicodeString = (C_RtlFreeUnicodeString)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint((OBFUSCATE("L_RtlFreeUnicodeString=%p\n").decrypt(), L_RtlFreeUnicodeString));

	// 初始化 ExAllocatePool2 函数指针
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"ExAllocatePool2").decrypt());
	L_ExAllocatePool2 = (C_ExAllocatePool2)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint((OBFUSCATE("L_ExAllocatePool2=%p\n").decrypt(), L_ExAllocatePool2));

	// 初始化 ExFreePoolWithTag 函数指针
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"ExFreePoolWithTag").decrypt());
	L_ExFreePoolWithTag = (C_ExFreePoolWithTag)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint((OBFUSCATE("L_ExFreePoolWithTag=%p\n").decrypt(), L_ExFreePoolWithTag));

	// 初始化 ObOpenObjectByPointer 函数指针
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"ObOpenObjectByPointer").decrypt());
	L_ObOpenObjectByPointer = (C_ObOpenObjectByPointer)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint((OBFUSCATE("L_ObOpenObjectByPointer=%p\n").decrypt(), L_ObOpenObjectByPointer));

	// 初始化 ObCloseHandle 函数指针
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"ObCloseHandle").decrypt());
	L_ObCloseHandle = (C_ObCloseHandle)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint((OBFUSCATE("L_ObCloseHandle=%p\n").decrypt(), L_ObCloseHandle));


	// 获取系统信息
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"ZwQuerySystemInformation").decrypt());
	L_ZwQuerySystemInformation = (C_ZwQuerySystemInformation)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint(("L_ZwQuerySystemInformation=%p\n", L_ZwQuerySystemInformation));


	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"RtlEqualUnicodeString").decrypt());
	L_RtlEqualUnicodeString = (C_RtlEqualUnicodeString)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint(("L_RtlEqualUnicodeString=%p\n", L_RtlEqualUnicodeString));

	// 获取指定进程的 PEB
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"PsGetProcessPeb").decrypt());
	L_PsGetProcessPeb = (C_PsGetProcessPeb)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint(("L_PsGetProcessPeb=%p\n", L_PsGetProcessPeb));


	// 获取指定进程名
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"PsGetProcessImageFileName").decrypt());
	L_PsGetProcessImageFileName = (C_PsGetProcessImageFileName)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint(("L_PsGetProcessImageFileName=%p\n", L_PsGetProcessImageFileName));


	// 获取驱动对象
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"ObReferenceObjectByName").decrypt());
	L_ObReferenceObjectByName = (C_ObReferenceObjectByName)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint(("L_ObReferenceObjectByName=%p\n", L_ObReferenceObjectByName));


	// 获取进程pid
	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"PsGetProcessId").decrypt());
	L_PsGetProcessId = (C_PsGetProcessId)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint(("L_PsGetProcessId=%p\n", L_PsGetProcessId));


	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"IoRegisterDriverReinitialization").decrypt());
	L_IoRegisterDriverReinitialization = (C_IoRegisterDriverReinitialization)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint(("L_IoRegisterDriverReinitialization=%p\n", L_IoRegisterDriverReinitialization));


	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"RtlDeleteRegistryValue").decrypt());
	L_RtlDeleteRegistryValue = (C_RtlDeleteRegistryValue)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint(("L_RtlDeleteRegistryValue=%p\n", L_RtlDeleteRegistryValue));


	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"RtlFindExportedRoutineByName").decrypt());
	L_RtlFindExportedRoutineByName = (C_RtlFindExportedRoutineByName)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint(("L_RtlFindExportedRoutineByName=%p\n", L_RtlFindExportedRoutineByName));


	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"RtlGetVersion").decrypt());
	L_RtlGetVersion = (C_RtlGetVersion)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint(("L_RtlGetVersion = % p\n", L_RtlGetVersion));


	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"MmIsAddressValid").decrypt());
	L_MmIsAddressValid = (C_MmIsAddressValid)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint(("L_MmIsAddressValid=%p\n", L_MmIsAddressValid));


	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"RtlCompareUnicodeString").decrypt());
	L_RtlCompareUnicodeString = (C_RtlCompareUnicodeString)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint(("L_RtlCompareUnicodeString=%p\n", L_RtlCompareUnicodeString));



	L_RtlInitUnicodeString(&routineName, OBFUSCATE(L"PsGetProcessSectionBaseAddress").decrypt());
	L_PsGetProcessSectionBaseAddress = (C_PsGetProcessSectionBaseAddress)L_MmGetSystemRoutineAddress(&routineName);
	KdPrint(("L_PsGetProcessSectionBaseAddress=%p\n", L_PsGetProcessSectionBaseAddress));


	


	// 获取系统版本信息Windows 
	L_RtlGetVersion(&SystemVersionInf);

	// 仅当是 Windows 10 或 11 时执行
	if (SystemVersionInf.dwMajorVersion == 10 || SystemVersionInf.dwMajorVersion == 11)
	{
		// 使用 do-while(false) 结构方便错误处理
		do
		{
			// 定义要搜索的特征码 (使用 UCHAR/BYTE 更标准)
			const UCHAR MmUnloadSystemImage_Code[] = { 0x40, 0xB7, 0x01, 0x48, 0x8B, 0xCE, 0xE8 };
			const ULONG SearchLength1 = sizeof(MmUnloadSystemImage_Code);

			ULONG_PTR MmUnloadSystemImageAddress = 0;
			ULONG_PTR MiUnloadSystemImageAddress = 0;
			ULONG_PTR StartAddress = 0;

			// 默认将结果设为失败
			L_MiProcessLoaderEntry = nullptr;

			// 获取 MmUnloadSystemImage 的地址
			UNICODE_STRING u_FuncName;
			L_RtlInitUnicodeString(&u_FuncName, OBFUSCATE(L"MmUnloadSystemImage").decrypt());
			MmUnloadSystemImageAddress = (ULONG_PTR)L_MmGetSystemRoutineAddress(&u_FuncName);
			if (MmUnloadSystemImageAddress == 0)
			{
				KdPrint((OBFUSCATE("[-] [Win10/11] Failed to get MmUnloadSystemImage address.\n").decrypt()));
				break; // 失败，跳出 do-while
			}

			//  在 MmUnloadSystemImage 中扫描特征码以定位 MiUnloadSystemImage
			StartAddress = MmUnloadSystemImageAddress;
			while (StartAddress < MmUnloadSystemImageAddress + 0x500)
			{
				if (RtlCompareMemory((VOID*)StartAddress, MmUnloadSystemImage_Code, SearchLength1) == SearchLength1)
				{
					StartAddress += SearchLength1; // 跳到 call 指令的相对地址部分
					MiUnloadSystemImageAddress = (*(LONG*)StartAddress) + StartAddress + 4;
					break; // 找到后立即跳出 while 循环
				}
				++StartAddress;
			}

			if (MiUnloadSystemImageAddress == 0)
			{
				KdPrint((OBFUSCATE("[-] [Win10/11] Step 1: Feature scan for MiUnloadSystemImage failed.\n").decrypt()));
				break; // 失败，跳出 do-while
			}

			// 在 MiUnloadSystemImage 中扫描特征码以定位 MiProcessLoaderEntry
			StartAddress = MiUnloadSystemImageAddress;
			while (StartAddress < MiUnloadSystemImageAddress + 0x600)
			{
				// 特征码: call; ...; mov eax, [rip+...]
				if (*(UCHAR*)StartAddress == 0xE8 &&
					*(UCHAR*)(StartAddress + 5) == 0x8B && *(UCHAR*)(StartAddress + 6) == 0x05)
				{
					StartAddress++; // 跳过 call 的 0xE8
					L_MiProcessLoaderEntry = (C_MiProcessLoaderEntry)((*(LONG*)StartAddress) + StartAddress + 4);
					break;
				}
				++StartAddress;
			}

		} while (false); // 这个循环只会执行一次

		// 关键修复：在所有操作完成后，根据 L_MiProcessLoaderEntry 的最终值来判断并打印信息
		if (L_MiProcessLoaderEntry != nullptr)
		{
			KdPrint((OBFUSCATE("[+] [Win10/11] Successfully found MiProcessLoaderEntry at: %p\n").decrypt(), L_MiProcessLoaderEntry));
		}
		else
		{
			KdPrint((OBFUSCATE("[-] [Win10/11] Final check: Failed to find MiProcessLoaderEntry.\n").decrypt()));
		}
	}

}


//绑定到指定核心
NTSTATUS BindThreadToLogicalCore(ULONG targetCoreIndex)
{
	NTSTATUS status;
	PROCESSOR_NUMBER procNumber; // 用于存储转换后的处理器组号和组内编号
	GROUP_AFFINITY newAffinity, oldAffinity;

	//  检查目标核心索引是否有效
	// 首先获取系统中所有活动逻辑核心的总数
	ULONG totalLogicalProcessors = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);

	if (targetCoreIndex >= totalLogicalProcessors) {
		// 如果目标索引超出了范围，则返回参数错误
		KdPrint(("错误：目标核心索引 %u 超出范围 (总核心数: %u)\n", targetCoreIndex, totalLogicalProcessors));
		return STATUS_INVALID_PARAMETER;
	}

	// 将全局核心索引转换为处理器组号和组内编号
	status = KeGetProcessorNumberFromIndex(targetCoreIndex, &procNumber);
	if (!NT_SUCCESS(status)) {
		KdPrint(("错误：KeGetProcessorNumberFromIndex 失败，状态码: 0x%X\n", status));
		return status;
	}

	//  构造新的 GROUP_AFFINITY 结构体
	RtlZeroMemory(&newAffinity, sizeof(GROUP_AFFINITY)); // 清空结构体
	newAffinity.Group = procNumber.Group; // 设置组号
	// 设置亲和性掩码，只允许在目标核心上运行
	// (KAFFINITY)1 << procNumber.Number 的结果是一个只有一位为1的64位掩码
	newAffinity.Mask = (KAFFINITY)1 << procNumber.Number;

	// 应用新的亲和性设置，并保存旧的设置
	KeSetSystemGroupAffinityThread(&newAffinity, &oldAffinity);

	// --- 绑定成功 ---
	// 从此刻起，当前线程将只会在你指定的核心上运行。
	// 你可以在这里执行需要与特定核心绑定的代码。
	KdPrint(("线程已成功绑定到逻辑核心 %u (组: %d, 组内编号: %d)\n", targetCoreIndex, procNumber.Group, procNumber.Number));


	return STATUS_SUCCESS;
}
