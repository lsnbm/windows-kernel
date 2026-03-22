//
////windbg加载公共符号服务器				.sympath srv*c:\symbols*https://msdl.microsoft.com/download/symbols
////强制重新加载内核符号					.reload /f nt
////强制加载 ndis.sys 的符号				.reload /f ndis.sys
//
//#pragma once
//#include <ata.h>
//#include <ntddscsi.h>
//#include <ntifs.h>
//#include <ntstrsafe.h>
//#include <ntimage.h>
//#include <mountdev.h>
//#include <mountmgr.h>
//#include <ntdddisk.h>
//
//
//
//extern "C" POBJECT_TYPE* IoDriverObjectType;
//
//extern C_ZwQuerySystemInformation L_ZwQuerySystemInformation;
//extern  C_ObReferenceObjectByName L_ObReferenceObjectByName;
//extern  C_PsGetProcessImageFileName L_PsGetProcessImageFileName;
//
////==================================================================================
//// 1. Windows 内核结构和函数定义 (Kernel Definitions)
////==================================================================================
//namespace WinKernel
//{
//
//	typedef enum _SYSTEM_INFORMATION_CLASS {
//		SystemBasicInformation = 0x0,
//		SystemProcessorInformation = 0x1,
//		SystemPerformanceInformation = 0x2,
//		SystemTimeOfDayInformation = 0x3,
//		SystemPathInformation = 0x4,
//		SystemProcessInformation = 0x5,
//		SystemCallCountInformation = 0x6,
//		SystemDeviceInformation = 0x7,
//		SystemProcessorPerformanceInformation = 0x8,
//		SystemFlagsInformation = 0x9,
//		SystemCallTimeInformation = 0xa,
//		SystemModuleInformation = 0xb,
//		SystemLocksInformation = 0xc,
//		SystemStackTraceInformation = 0xd,
//		SystemNonPagedPoolInformation = 0xe,
//		SystemNonNonPagedPoolInformation = 0xf,
//		SystemHandleInformation = 0x10,
//		SystemObjectInformation = 0x11,
//		SystemPageFileInformation = 0x12,
//		SystemVdmInstemulInformation = 0x13,
//		SystemVdmBopInformation = 0x14,
//		SystemFileCacheInformation = 0x15,
//		SystemPoolTagInformation = 0x16,
//		SystemInterruptInformation = 0x17,
//		SystemDpcBehaviorInformation = 0x18,
//		SystemFullMemoryInformation = 0x19,
//		SystemLoadGdiDriverInformation = 0x1a,
//		SystemUnloadGdiDriverInformation = 0x1b,
//		SystemTimeAdjustmentInformation = 0x1c,
//		SystemSummaryMemoryInformation = 0x1d,
//		SystemMirrorMemoryInformation = 0x1e,
//		SystemPerformanceTraceInformation = 0x1f,
//		SystemObsolete0 = 0x20,
//		SystemExceptionInformation = 0x21,
//		SystemCrashDumpStateInformation = 0x22,
//		SystemKernelDebuggerInformation = 0x23,
//		SystemContextSwitchInformation = 0x24,
//		SystemRegistryQuotaInformation = 0x25,
//		SystemExtendServiceTableInformation = 0x26,
//		SystemPrioritySeperation = 0x27,
//		SystemVerifierAddDriverInformation = 0x28,
//		SystemVerifierRemoveDriverInformation = 0x29,
//		SystemProcessorIdleInformation = 0x2a,
//		SystemLegacyDriverInformation = 0x2b,
//		SystemCurrentTimeZoneInformation = 0x2c,
//		SystemLookasideInformation = 0x2d,
//		SystemTimeSlipNotification = 0x2e,
//		SystemSessionCreate = 0x2f,
//		SystemSessionDetach = 0x30,
//		SystemSessionInformation = 0x31,
//		SystemRangeStartInformation = 0x32,
//		SystemVerifierInformation = 0x33,
//		SystemVerifierThunkExtend = 0x34,
//		SystemSessionProcessInformation = 0x35,
//		SystemLoadGdiDriverInSystemSpace = 0x36,
//		SystemNumaProcessorMap = 0x37,
//		SystemPrefetcherInformation = 0x38,
//		SystemExtendedProcessInformation = 0x39,
//		SystemRecommendedSharedDataAlignment = 0x3a,
//		SystemComPlusPackage = 0x3b,
//		SystemNumaAvailableMemory = 0x3c,
//		SystemProcessorPowerInformation = 0x3d,
//		SystemEmulationBasicInformation = 0x3e,
//		SystemEmulationProcessorInformation = 0x3f,
//		SystemExtendedHandleInformation = 0x40,
//		SystemLostDelayedWriteInformation = 0x41,
//		SystemBigPoolInformation = 0x42,
//		SystemSessionPoolTagInformation = 0x43,
//		SystemSessionMappedViewInformation = 0x44,
//		SystemHotpatchInformation = 0x45,
//		SystemObjectSecurityMode = 0x46,
//		SystemWatchdogTimerHandler = 0x47,
//		SystemWatchdogTimerInformation = 0x48,
//		SystemLogicalProcessorInformation = 0x49,
//		SystemWow64SharedInformationObsolete = 0x4a,
//		SystemRegisterFirmwareTableInformationHandler = 0x4b,
//		SystemFirmwareTableInformation = 0x4c,
//		SystemModuleInformationEx = 0x4d,
//		SystemVerifierTriageInformation = 0x4e,
//		SystemSuperfetchInformation = 0x4f,
//		SystemMemoryListInformation = 0x50,
//		SystemFileCacheInformationEx = 0x51,
//		SystemThreadPriorityClientIdInformation = 0x52,
//		SystemProcessorIdleCycleTimeInformation = 0x53,
//		SystemVerifierCancellationInformation = 0x54,
//		SystemProcessorPowerInformationEx = 0x55,
//		SystemRefTraceInformation = 0x56,
//		SystemSpecialPoolInformation = 0x57,
//		SystemProcessIdInformation = 0x58,
//		SystemErrorPortInformation = 0x59,
//		SystemBootEnvironmentInformation = 0x5a,
//		SystemHypervisorInformation = 0x5b,
//		SystemVerifierInformationEx = 0x5c,
//		SystemTimeZoneInformation = 0x5d,
//		SystemImageFileExecutionOptionsInformation = 0x5e,
//		SystemCoverageInformation = 0x5f,
//		SystemPrefetchPatchInformation = 0x60,
//		SystemVerifierFaultsInformation = 0x61,
//		SystemSystemPartitionInformation = 0x62,
//		SystemSystemDiskInformation = 0x63,
//		SystemProcessorPerformanceDistribution = 0x64,
//		SystemNumaProximityNodeInformation = 0x65,
//		SystemDynamicTimeZoneInformation = 0x66,
//		SystemCodeIntegrityInformation = 0x67,
//		SystemProcessorMicrocodeUpdateInformation = 0x68,
//		SystemProcessorBrandString = 0x69,
//		SystemVirtualAddressInformation = 0x6a,
//		SystemLogicalProcessorAndGroupInformation = 0x6b,
//		SystemProcessorCycleTimeInformation = 0x6c,
//		SystemStoreInformation = 0x6d,
//		SystemRegistryAppendString = 0x6e,
//		SystemAitSamplingValue = 0x6f,
//		SystemVhdBootInformation = 0x70,
//		SystemCpuQuotaInformation = 0x71,
//		SystemNativeBasicInformation = 0x72,
//		SystemErrorPortTimeouts = 0x73,
//		SystemLowPriorityIoInformation = 0x74,
//		SystemBootEntropyInformation = 0x75,
//		SystemVerifierCountersInformation = 0x76,
//		SystemNonPagedPoolInformationEx = 0x77,
//		SystemSystemPtesInformationEx = 0x78,
//		SystemNodeDistanceInformation = 0x79,
//		SystemAcpiAuditInformation = 0x7a,
//		SystemBasicPerformanceInformation = 0x7b,
//		SystemQueryPerformanceCounterInformation = 0x7c,
//		SystemSessionBigPoolInformation = 0x7d,
//		SystemBootGraphicsInformation = 0x7e,
//		SystemScrubPhysicalMemoryInformation = 0x7f,
//		SystemBadPageInformation = 0x80,
//		SystemProcessorProfileControlArea = 0x81,
//		SystemCombinePhysicalMemoryInformation = 0x82,
//		SystemEntropyInterruptTimingInformation = 0x83,
//		SystemConsoleInformation = 0x84,
//		SystemPlatformBinaryInformation = 0x85,
//		SystemThrottleNotificationInformation = 0x86,
//		SystemHypervisorProcessorCountInformation = 0x87,
//		SystemDeviceDataInformation = 0x88,
//		SystemDeviceDataEnumerationInformation = 0x89,
//		SystemMemoryTopologyInformation = 0x8a,
//		SystemMemoryChannelInformation = 0x8b,
//		SystemBootLogoInformation = 0x8c,
//		SystemProcessorPerformanceInformationEx = 0x8d,
//		SystemSpare0 = 0x8e,
//		SystemSecureBootPolicyInformation = 0x8f,
//		SystemPageFileInformationEx = 0x90,
//		SystemSecureBootInformation = 0x91,
//		SystemEntropyInterruptTimingRawInformation = 0x92,
//		SystemPortableWorkspaceEfiLauncherInformation = 0x93,
//		SystemFullProcessInformation = 0x94,
//		SystemKernelDebuggerInformationEx = 0x95,
//		SystemBootMetadataInformation = 0x96,
//		SystemSoftRebootInformation = 0x97,
//		SystemElamCertificateInformation = 0x98,
//		SystemOfflineDumpConfigInformation = 0x99,
//		SystemProcessorFeaturesInformation = 0x9a,
//		SystemRegistryReconciliationInformation = 0x9b,
//		SystemSupportedProcessArchitectures = 0xb5,
//	} SYSTEM_INFORMATION_CLASS;
//
//	typedef struct _SYSTEM_THREAD_INFORMATION {
//		LARGE_INTEGER KernelTime;
//		LARGE_INTEGER UserTime;
//		LARGE_INTEGER CreateTime;
//		ULONG WaitTime;
//		PVOID StartAddress;
//		CLIENT_ID ClientId;
//		KPRIORITY Priority;
//		LONG BasePriority;
//		ULONG ContextSwitches;
//		ULONG ThreadState;
//		ULONG WaitReason;
//		ULONG PadPadAlignment;
//	} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;
//
//	typedef struct _SYSTEM_MODULE {
//		ULONG_PTR Reserved[2];
//		PVOID Base;
//		ULONG Size;
//		ULONG Flags;
//		USHORT Index;
//		USHORT Unknown;
//		USHORT LoadCount;
//		USHORT ModuleNameOffset;
//		CHAR ImageName[256];
//	} SYSTEM_MODULE, * PSYSTEM_MODULE;
//
//	typedef struct _SYSTEM_MODULE_INFORMATION {
//		ULONG_PTR ulModuleCount;
//		SYSTEM_MODULE Modules[1];
//	} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;
//
//	typedef struct _SYSTEM_PROCESS_INFORMATION {
//		ULONG NextEntryOffset;
//		ULONG NumberOfThreads;
//		LARGE_INTEGER WorkingSetPrivateSize;
//		ULONG HardFaultCount;
//		ULONG NumberOfThreadsHighWatermark;
//		ULONGLONG CycleTime;
//		LARGE_INTEGER CreateTime;
//		LARGE_INTEGER UserTime;
//		LARGE_INTEGER KernelTime;
//		UNICODE_STRING ImageName;
//		KPRIORITY BasePriority;
//		HANDLE hProcessId;
//		HANDLE UniqueProcessId;
//		ULONG HandleCount;
//		ULONG SessionId;
//		ULONG_PTR UniqueProcessKey;
//		SIZE_T PeakVirtualSize;
//		SIZE_T VirtualSize;
//		ULONG PageFaultCount;
//		SIZE_T PeakWorkingSetSize;
//		SIZE_T WorkingSetSize;
//		SIZE_T QuotaPeakPagedPoolUsage;
//		SIZE_T QuotaPagedPoolUsage;
//		SIZE_T QuotaPeakNonPagedPoolUsage;
//		SIZE_T QuotaNonPagedPoolUsage;
//		SIZE_T PagefileUsage;
//		SIZE_T PeakPagefileUsage;
//		SIZE_T PrivatePageCount;
//		LARGE_INTEGER ReadOperationCount;
//		LARGE_INTEGER WriteOperationCount;
//		LARGE_INTEGER OtherOperationCount;
//		LARGE_INTEGER ReadTransferCount;
//		LARGE_INTEGER WriteTransferCount;
//		LARGE_INTEGER OtherTransferCount;
//		SYSTEM_THREAD_INFORMATION Threads[1];
//	} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;
//}
//
//
////==================================================================================
//// 2. 通用工具函数 (Utility Functions)
////==================================================================================
//namespace Utils
//{
//
//
//	//杀死进程
//	NTSTATUS ZwKillProcess(LPCWSTR ProcessName) {
//
//		NTSTATUS Result = STATUS_UNSUCCESSFUL;
//
//		ULONG Size = NULL;
//
//		Result = L_ZwQuerySystemInformation(WinKernel::SystemProcessInformation, NULL, Size, &Size);
//
//		if (!NT_SUCCESS(Result) && Size != NULL) {
//
//
//			WinKernel::PSYSTEM_PROCESS_INFORMATION pBuffer = (WinKernel::PSYSTEM_PROCESS_INFORMATION)(ExAllocatePoolWithTag(NonPagedPool, Size, 'SG'));
//
//			if (pBuffer != NULL) {
//
//				Result = L_ZwQuerySystemInformation(WinKernel::SystemProcessInformation, pBuffer, Size, &Size);
//
//				if (NT_SUCCESS(Result)) {
//
//					for (WinKernel::PSYSTEM_PROCESS_INFORMATION pInfo = (WinKernel::PSYSTEM_PROCESS_INFORMATION)(((PUCHAR)pBuffer) + pBuffer->NextEntryOffset); pInfo->NextEntryOffset; pInfo = (WinKernel::PSYSTEM_PROCESS_INFORMATION)(((PUCHAR)pInfo) + pInfo->NextEntryOffset)) {
//
//						if (pInfo->ImageName.Buffer && !_wcsicmp(pInfo->ImageName.Buffer, ProcessName)) {
//
//							HANDLE hProcess = NULL;
//
//							CLIENT_ID ClientId = { NULL };
//
//							OBJECT_ATTRIBUTES Object = { NULL };
//
//							Object.Length = sizeof(Object);
//
//							ClientId.UniqueProcess = pInfo->hProcessId;
//
//							Result = ZwOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &Object, &ClientId);
//
//							if (NT_SUCCESS(Result)) {
//
//								Result = ZwTerminateProcess(hProcess, STATUS_SUCCESS);
//
//								ZwClose(hProcess);
//							}
//						}
//					}
//				}
//
//				ExFreePoolWithTag(pBuffer, 'SG');
//			}
//		}
//
//		return Result;
//	}
//
//	//获取内核驱动模块基址
//	PVOID GetKernelModuleBase(const char* moduleName) {
//		PVOID address = nullptr;
//		ULONG size = 0;
//
//		NTSTATUS status = L_ZwQuerySystemInformation(WinKernel::SystemModuleInformation, nullptr, 0, &size);
//		if (status != STATUS_INFO_LENGTH_MISMATCH) {
//			return nullptr;
//		}
//
//		if (size == 0) {
//			return nullptr;
//		}
//
//		WinKernel::PSYSTEM_MODULE_INFORMATION moduleList = static_cast<WinKernel::PSYSTEM_MODULE_INFORMATION>(ExAllocatePoolWithTag(NonPagedPool, size, 'oamL'));
//		if (!moduleList) {
//			return nullptr;
//		}
//
//		status = L_ZwQuerySystemInformation(WinKernel::SystemModuleInformation, moduleList, size, nullptr);
//		if (!NT_SUCCESS(status)) {
//			ExFreePoolWithTag(moduleList, 'oamL');
//			return nullptr;
//		}
//
//		for (ULONG i = 0; i < moduleList->ulModuleCount; i++) {
//			WinKernel::SYSTEM_MODULE module = moduleList->Modules[i];
//			const char* currentModuleName = strrchr(module.ImageName, '\\');
//			if (currentModuleName == nullptr) {
//				currentModuleName = module.ImageName;
//			}
//			else {
//				currentModuleName++;
//			}
//
//			if (_stricmp(currentModuleName, moduleName) == 0) {
//				address = module.Base;
//				break;
//			}
//		}
//
//		ExFreePoolWithTag(moduleList, 'oamL');
//		return address;
//	}
//
//	//特征码扫描
//	PVOID FindPatternImage(PVOID base, const char* pattern, const char* mask)
//	{
//		const size_t maskLength = strlen(mask);
//		if (maskLength == 0) {
//			return nullptr;
//		}
//
//		PIMAGE_NT_HEADERS ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>((PUCHAR)base + ((PIMAGE_DOS_HEADER)base)->e_lfanew);
//		PIMAGE_SECTION_HEADER sectionHeader = ((PIMAGE_SECTION_HEADER)((ULONG_PTR)(ntHeaders)+((LONG)__builtin_offsetof(IMAGE_NT_HEADERS64, OptionalHeader)) + ((ntHeaders))->FileHeader.SizeOfOptionalHeader));
//
//		for (USHORT i = 0; i < ntHeaders->FileHeader.NumberOfSections; ++i, ++sectionHeader)
//		{
//			if (memcmp(sectionHeader->Name, ".text", 5) == 0 || *reinterpret_cast<PULONG>(sectionHeader->Name) == 'EGAP')
//			{
//				char* sectionStart = (char*)base + sectionHeader->VirtualAddress;
//				size_t sectionSize = sectionHeader->Misc.VirtualSize;
//
//				if (sectionSize < maskLength) {
//					continue;
//				}
//
//				for (size_t j = 0; j <= sectionSize - maskLength; ++j)
//				{
//					const char* scanAddress = sectionStart + j;
//					bool found = true;
//					for (size_t k = 0; k < maskLength; ++k)
//					{
//						if (mask[k] == 'x' && scanAddress[k] != pattern[k])
//						{
//							found = false;
//							break;
//						}
//					}
//
//					if (found)
//					{
//						return (PVOID)scanAddress;
//					}
//				}
//			}
//		}
//
//		return nullptr;
//	}
//}
//
//
//namespace WIN11_23H2 {
//
//
//	//==================================================================================
//	// 3. 伪造注册表与标识文件 (Spoof Registry and Identifier Files)
//	//==================================================================================
//	namespace SpoofREG
//	{
//		// 伪造注册表键值和特定的标识符文件
//		NTSTATUS SpoofIdentifiers() {
//			// 使用静态布尔值确保此函数的核心逻辑只成功执行一次
//			static BOOLEAN Enable = FALSE;
//			if (Enable) {
//				return STATUS_SUCCESS;
//			}
//
//			NTSTATUS Result = STATUS_UNSUCCESSFUL;
//			BOOLEAN bAnySuccess = FALSE; // 标志：是否有任何一个操作成功
//
//			//================================================
//			// 部分 1: 伪造注册表中的 GUID
//			//================================================
//			KdPrint(("[*] Spoofing registry GUIDs...\n"));
//
//			GUID newGuid;
//			Result = ExUuidCreate(&newGuid);
//			if (!NT_SUCCESS(Result)) {
//				KdPrint(("[-] CRITICAL: Failed to create UUID. Error: 0x%X\n", Result));
//				// UUID创建失败是致命的，后续依赖它的操作无法进行
//			}
//			else {
//				WCHAR guidBuffer[40];
//				UNICODE_STRING guidString;
//				RtlInitEmptyUnicodeString(&guidString, guidBuffer, sizeof(guidBuffer));
//
//				// 将GUID结构体格式化为UNICODE字符串
//				Result = RtlUnicodeStringPrintf(
//					&guidString,
//					L"%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
//					newGuid.Data1, newGuid.Data2, newGuid.Data3,
//					newGuid.Data4[0], newGuid.Data4[1], newGuid.Data4[2], newGuid.Data4[3],
//					newGuid.Data4[4], newGuid.Data4[5], newGuid.Data4[6], newGuid.Data4[7]
//				);
//
//				if (!NT_SUCCESS(Result)) {
//					KdPrint(("[-] CRITICAL: Failed to format GUID to string. Error: 0x%X\n", Result));
//				}
//				else {
//					// 待伪造的注册表键值列表
//					const WCHAR* szKeysToSpoof[][2] = {
//						{ L"\\Registry\\Machine\\Software\\Microsoft\\SQMClient", L"MachineId" },
//						{ L"\\Registry\\Machine\\Software\\Microsoft\\Cryptography", L"MachineGuid" },
//						{ L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\IDConfigDB\\Hardware Profiles\\0001", L"HwProfileGuid" },
//						{ L"\\Registry\\Machine\\Software\\Microsoft\\Windows\\CurrentVersion\\OneSettings\\WSD\\UpdateAgent\\QueryParameters", L"deviceId" },
//						{ L"\\Registry\\Machine\\Software\\Microsoft\\Windows\\CurrentVersion\\OneSettings\\appcompat\\runtimesdbincloud\\QueryParameters", L"deviceid" }
//					};
//
//					for (SIZE_T i = 0; i < ARRAYSIZE(szKeysToSpoof); i++) {
//						// 写入GUID字符串
//						if (NT_SUCCESS(RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE, szKeysToSpoof[i][0], szKeysToSpoof[i][1], REG_SZ, guidString.Buffer, guidString.Length + sizeof(WCHAR)))) {
//							bAnySuccess = TRUE;
//						}
//					}
//				}
//			}
//
//			//================================================
//			// 部分 2: 伪造注册表中的其他标识符
//			//================================================
//
//			// 伪造二进制类型的注册表值
//			{
//				UCHAR randomBinaryData[32];
//				ULONG seed = KeQueryTimeIncrement();
//
//				for (SIZE_T i = 0; i < sizeof(randomBinaryData); i++) {
//					randomBinaryData[i] = (UCHAR)(RtlRandomEx(&seed) & 0xFF);
//				}
//
//				if (NT_SUCCESS(RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE, L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\TPM\\WMI", L"WindowsAIKHash", REG_BINARY, randomBinaryData, sizeof(randomBinaryData)))) {
//					bAnySuccess = TRUE;
//				}
//			}
//
//			// 清理挂载设备信息
//			{
//				const WCHAR* volumeLetters[] = { L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J", L"K", L"L", L"M", L"N", L"O", L"P", L"Q", L"R", L"S", L"T", L"U", L"V", L"W", L"X", L"Y", L"Z" };
//				WCHAR volumeValueName[32];
//
//				for (SIZE_T i = 0; i < ARRAYSIZE(volumeLetters); i++) {
//					RtlStringCbPrintfW(volumeValueName, sizeof(volumeValueName), L"\\DosDevices\\%ws:", volumeLetters[i]);
//					if (NT_SUCCESS(RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, L"\\Registry\\Machine\\System\\MountedDevices", volumeValueName))) {
//						bAnySuccess = TRUE;
//					}
//				}
//			}
//
//			//================================================
//			// 部分 3: 伪造 MachineGuid.txt 文件 (原 SpoofFile 逻辑)
//			//================================================
//			KdPrint(("[*] Spoofing identifier files...\n"));
//
//			PWCHAR pFileBuffer = (PWCHAR)ExAllocatePoolWithTag(NonPagedPool, PAGE_SIZE, 'LS');
//			if (pFileBuffer == NULL) {
//				KdPrint(("[-] SpoofIdentifiers: Failed to allocate memory for file spoofing.\n"));
//			}
//			else {
//				ULONG fileSeeds[] = { 0x18547856, 0x74569821, 0x56741359, 0x12347865, 0x75234785, 0x35132475, 0x23547856 };
//
//				NTSTATUS fileStatus = RtlStringCchPrintfW(pFileBuffer,
//					PAGE_SIZE / sizeof(WCHAR),
//					L"{%08X-%04X-%04X-%04X-%04X%04X%04X}",
//					RtlRandomEx(&fileSeeds[0]), (USHORT)(RtlRandomEx(&fileSeeds[1]) & 0xFFFF), (USHORT)(RtlRandomEx(&fileSeeds[2]) & 0xFFFF),
//					(USHORT)(RtlRandomEx(&fileSeeds[3]) & 0xFFFF), (USHORT)(RtlRandomEx(&fileSeeds[4]) & 0xFFFF), (USHORT)(RtlRandomEx(&fileSeeds[5]) & 0xFFFF),
//					(USHORT)(RtlRandomEx(&fileSeeds[6]) & 0xFFFF)
//				);
//
//				if (NT_SUCCESS(fileStatus)) {
//					UNICODE_STRING usFileName;
//					RtlInitUnicodeString(&usFileName, L"\\SystemRoot\\System32\\Restore\\MachineGuid.txt");
//					OBJECT_ATTRIBUTES objectAttributes;
//					InitializeObjectAttributes(&objectAttributes, &usFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
//
//					HANDLE hFile = NULL;
//					IO_STATUS_BLOCK ioStatusBlock;
//
//					fileStatus = ZwCreateFile(&hFile, GENERIC_WRITE, &objectAttributes, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_SUPERSEDE, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
//
//					if (NT_SUCCESS(fileStatus)) {
//						const ULONG bufferSizeInBytes = 38 * sizeof(WCHAR);
//						LARGE_INTEGER byteOffset = { 0 };
//						fileStatus = ZwWriteFile(hFile, NULL, NULL, NULL, &ioStatusBlock, pFileBuffer, bufferSizeInBytes, &byteOffset, NULL);
//						ZwClose(hFile);
//
//						if (NT_SUCCESS(fileStatus)) {
//							bAnySuccess = TRUE; // 文件写入成功也算整体成功
//						}
//					}
//				}
//				ExFreePoolWithTag(pFileBuffer, 'LS');
//			}
//
//			//================================================
//			// 最终状态判断
//			//================================================
//			if (bAnySuccess) {
//				KdPrint(("[+] SpoofIdentifiers: One or more identifiers were successfully spoofed.\n"));
//				Enable = TRUE;      // 标记已成功执行，下次不再进入
//				return STATUS_SUCCESS; // 只要有任意一个成功，就返回整体成功
//			}
//
//			// 如果没有任何操作成功，则返回最后一个遇到的错误代码或默认失败代码
//			KdPrint(("[-] SpoofIdentifiers: All spoofing attempts failed.\n"));
//			return (NT_SUCCESS(Result) ? STATUS_UNSUCCESSFUL : Result);
//		}
//	}
//
//
//	//==================================================================================
//	// 3. 伪造磁盘序列号
//	//==================================================================================
//	namespace SpoofHDD
//	{
//		using namespace Utils;
//
//		//------------------------------------------------
//		// storport.sys
//		//------------------------------------------------
//		typedef struct _STOR_SCSI_IDENTITY
//		{
//			char Space[0x8];
//			STRING SerialNumber;
//		} STOR_SCSI_IDENTITY, * PSTOR_SCSI_IDENTITY;
//
//		typedef struct _TELEMETRY_UNIT_EXTENSION
//		{
//			int SmartMask;//监控开启标志
//		} TELEMETRY_UNIT_EXTENSION, * PTELEMETRY_UNIT_EXTENSION;
//
//		typedef struct _RAID_UNIT_EXTENSION
//		{
//			union
//			{
//				struct
//				{
//					char Space[0x68];
//					STOR_SCSI_IDENTITY Identity;
//				} _Identity;
//
//				struct
//				{
//					char Space[0x7c8];
//					TELEMETRY_UNIT_EXTENSION Telemetry;
//				} _Smart;
//			};
//		} RAID_UNIT_EXTENSION, * PRAID_UNIT_EXTENSION;
//
//		typedef __int64(__fastcall* RaidUnitRegisterInterfaces)(PRAID_UNIT_EXTENSION a1);
//
//		// 伪造硬盘序列号
//		NTSTATUS SpoofHDD()
//		{
//			PVOID base = GetKernelModuleBase("storport.sys");
//			if (!base)
//			{
//				KdPrint(("[-] Failed to find storport.sys base!\n"));
//				return STATUS_UNSUCCESSFUL;
//			}
//
//			RaidUnitRegisterInterfaces  registerInterfaces = static_cast<RaidUnitRegisterInterfaces>(FindPatternImage(base, "\x48\x89\x5C\x24\x00\x55\x56\x57\x48\x83\xEC\x50", "xxxx?xxxxxxx"));
//
//			if (!registerInterfaces)
//			{
//				KdPrint(("[-] Failed to  find RaidUnitRegisterInterfaces!\n"));
//
//				return STATUS_UNSUCCESSFUL;
//			}
//
//
//
//			NTSTATUS status = STATUS_NOT_FOUND;
//			for (int i = 0; i < 2; i++)
//			{
//				PDEVICE_OBJECT raidDeviceChain = nullptr;
//				{
//					wchar_t raidBuffer[32];
//					RtlStringCbPrintfW(raidBuffer, sizeof(raidBuffer), L"\\Device\\RaidPort%d", i);
//
//					UNICODE_STRING raidPortUnicode;
//					RtlInitUnicodeString(&raidPortUnicode, raidBuffer);
//
//					PFILE_OBJECT fileObject = nullptr;
//					PDEVICE_OBJECT deviceObject = nullptr;
//					NTSTATUS getDeviceStatus = IoGetDeviceObjectPointer(&raidPortUnicode, FILE_READ_DATA, &fileObject, &deviceObject);
//
//					if (NT_SUCCESS(getDeviceStatus))
//					{
//						raidDeviceChain = deviceObject->DriverObject->DeviceObject;
//						ObDereferenceObject(fileObject);
//					}
//				}
//
//				if (!raidDeviceChain)
//				{
//					continue;
//				}
//
//
//
//				PDEVICE_OBJECT currentDevice = raidDeviceChain;
//				while (currentDevice)
//				{
//					if (currentDevice->DeviceType == FILE_DEVICE_DISK)
//					{
//						PRAID_UNIT_EXTENSION extension = static_cast<PRAID_UNIT_EXTENSION>(currentDevice->DeviceExtension);
//						if (extension)
//						{
//							USHORT length = extension->_Identity.Identity.SerialNumber.Length;
//							if (length > 0)
//							{
//
//
//
//								char original[256];
//								RtlCopyMemory(original, extension->_Identity.Identity.SerialNumber.Buffer, min(length, 255));
//
//								original[min(length, 255)] = '\0';
//
//
//
//								char* newSerialBuffer = static_cast<char*>(ExAllocatePoolWithTag(NonPagedPool, length + 1, 'oamL'));
//
//
//								if (newSerialBuffer)
//								{
//									{
//										static const char hexChars[] = "0123456789ABCDEF";
//										auto seed = KeQueryTimeIncrement();
//
//										for (int n = 0; n < length; ++n)
//										{
//											if (n > 0 && (n + 1) % 5 == 0)
//											{
//												newSerialBuffer[n] = '_';
//											}
//											else
//											{
//												auto key = RtlRandomEx(&seed) % (sizeof(hexChars) - 1);
//												newSerialBuffer[n] = hexChars[key];
//											}
//										}
//										newSerialBuffer[length] = '\0';
//									}
//
//									RtlInitString(&extension->_Identity.Identity.SerialNumber, newSerialBuffer);
//									KdPrint(("[+] Changed disk serial from '%s' to '%s'.\n", original, newSerialBuffer));
//									status = STATUS_SUCCESS;
//									ExFreePoolWithTag(newSerialBuffer, 'oamL');
//								}
//
//								extension->_Smart.Telemetry.SmartMask = 0;
//								registerInterfaces(extension);
//							}
//						}
//					}
//					currentDevice = currentDevice->NextDevice;
//				}
//
//
//
//
//			}
//			return status;
//		}
//	}
//
//
//
//	//==================================================================================
//	// 5. 伪造SMBIOS (Spoof SMBIOS)
//	//==================================================================================
//	namespace SpoofSMBIOS
//	{
//		using namespace Utils;
//
//
//		// SMBIOS  的单表头
//		typedef struct
//		{
//			UINT8   Type;
//			UINT8   Length;
//			UINT8   Handle[2];
//		} SMBIOS_HEADER;
//
//		typedef UINT8   SMBIOS_STRING;
//
//		typedef struct
//		{
//			SMBIOS_HEADER   Hdr;
//			SMBIOS_STRING   Vendor;
//			SMBIOS_STRING   BiosVersion;
//			UINT8           BiosSegment[2];
//			SMBIOS_STRING   BiosReleaseDate;
//			UINT8           BiosSize;
//			UINT8           BiosCharacteristics[8];
//		} SMBIOS_TYPE0;
//
//		typedef struct
//		{
//			SMBIOS_HEADER   Hdr;
//			SMBIOS_STRING   Manufacturer;
//			SMBIOS_STRING   ProductName;
//			SMBIOS_STRING   Version;
//			SMBIOS_STRING   SerialNumber;
//			GUID			Uuid;
//			UINT8           WakeUpType;
//		} SMBIOS_TYPE1;
//
//		typedef struct
//		{
//			SMBIOS_HEADER   Hdr;
//			SMBIOS_STRING   Manufacturer;
//			SMBIOS_STRING   ProductName;
//			SMBIOS_STRING   Version;
//			SMBIOS_STRING   SerialNumber;
//		} SMBIOS_TYPE2;
//
//		typedef struct
//		{
//			SMBIOS_HEADER   Hdr;
//			SMBIOS_STRING   Manufacturer;
//			UINT8           Type;
//			SMBIOS_STRING   Version;
//			SMBIOS_STRING   SerialNumber;
//			SMBIOS_STRING   AssetTag;
//			UINT8           BootupState;
//			UINT8           PowerSupplyState;
//			UINT8           ThermalState;
//			UINT8           SecurityStatus;
//			UINT8           OemDefined[4];
//		} SMBIOS_TYPE3;
//
//		typedef struct
//		{
//			SMBIOS_HEADER   Hdr;
//			UINT8           Socket;
//			UINT8           ProcessorType;
//			UINT8           ProcessorFamily;
//			SMBIOS_STRING   ProcessorManufacture;
//			UINT8           ProcessorId[8];
//			SMBIOS_STRING   ProcessorVersion;
//			UINT8           Voltage;
//			UINT8           ExternalClock[2];
//			UINT8           MaxSpeed[2];
//			UINT8           CurrentSpeed[2];
//			UINT8           Status;
//			UINT8           ProcessorUpgrade;
//			UINT8           L1CacheHandle[2];
//			UINT8           L2CacheHandle[2];
//			UINT8           L3CacheHandle[2];
//		} SMBIOS_TYPE4;
//
//		typedef union
//		{
//			SMBIOS_HEADER* Hdr;
//			SMBIOS_TYPE0* Type0;
//			SMBIOS_TYPE1* Type1;
//			SMBIOS_TYPE2* Type2;
//			SMBIOS_TYPE3* Type3;
//			SMBIOS_TYPE4* Type4;
//			UINT8* Raw;
//		} SMBIOS_STRUCTURE_POINTER;
//
//		typedef struct
//		{
//			UINT8   AnchorString[4];
//			UINT8   EntryPointStructureChecksum;
//			UINT8   EntryPointLength;
//			UINT8   MajorVersion;
//			UINT8   MinorVersion;
//			UINT16  MaxStructureSize;
//			UINT8   EntryPointRevision;
//			UINT8   FormattedArea[5];
//			UINT8   IntermediateAnchorString[5];
//			UINT8   IntermediateChecksum;
//			UINT16  TableLength;
//			UINT32  TableAddress;
//			UINT16  NumberOfSmbiosStructures;
//			UINT8   SmbiosBcdRevision;
//		} SMBIOS_STRUCTURE_TABLE;
//
//		typedef struct _RAW_SMBIOS
//		{
//			UINT8	Unknown;
//			UINT8	MajorVersion;
//			UINT8	MinorVersion;
//			UINT8	DmiRevision;
//			UINT32	Size;
//			UINT8* Entry;
//		} RAW_SMBIOS;
//
//		//修改SMBIOS 表内存
//		NTSTATUS SpoofSMBIOS1()
//		{
//			PVOID base = GetKernelModuleBase("ntoskrnl.exe");
//			if (!base)
//			{
//				KdPrint(("[-] Failed to find ntoskrnl.exe base!\n"));
//				return STATUS_UNSUCCESSFUL;
//			}
//
//			auto* physicalAddressPtr = static_cast<PPHYSICAL_ADDRESS>(FindPatternImage(base, "\x48\x8B\x0D\x00\x00\x00\x00\x48\x85\xC9\x74\x00\x8B\x15", "xxx????xxxx?xx"));
//			if (!physicalAddressPtr)
//			{
//				KdPrint(("[-] Failed to find SMBIOS physical address pointer!\n"));
//				return STATUS_UNSUCCESSFUL;
//			}
//
//			physicalAddressPtr = reinterpret_cast<PPHYSICAL_ADDRESS>((PUCHAR)physicalAddressPtr + 7 + *reinterpret_cast<int*>((PUCHAR)physicalAddressPtr + 3));
//			if (!physicalAddressPtr)
//			{
//				KdPrint(("[-] Physical address pointer is null after calculation!\n"));
//				return STATUS_UNSUCCESSFUL;
//			}
//
//			PHYSICAL_ADDRESS physicalAddress = *physicalAddressPtr;
//
//			auto* sizeScan = FindPatternImage(base, "\x8B\x1D\x00\x00\x00\x00\x48\x8B\xD0\x44\x8B\xC3\x48\x8B\xCD\xE8\x00\x00\x00\x00\x8B\xD3\x48\x8B", "xx????xxxxxxxxxx????xxxx");
//			if (!sizeScan)
//			{
//				KdPrint(("[-] Failed to find SMBIOS size pointer!\n"));
//				return STATUS_UNSUCCESSFUL;
//			}
//
//			const auto size = *reinterpret_cast<ULONG*>((PUCHAR)sizeScan + 6 + *reinterpret_cast<int*>((PUCHAR)sizeScan + 2));
//			if (!size)
//			{
//				KdPrint(("[-] SMBIOS size is null!\n"));
//				return STATUS_UNSUCCESSFUL;
//			}
//
//			PVOID mappedBase = MmMapIoSpace(physicalAddress, size, MmNonCached);
//			if (!mappedBase)
//			{
//				KdPrint(("[-] Failed to map SMBIOS structures!\n"));
//				return STATUS_UNSUCCESSFUL;
//			}
//
//			PVOID currentMappedPtr = mappedBase;
//			const PUCHAR endAddress = (PUCHAR)mappedBase + size;
//
//			while ((PUCHAR)currentMappedPtr < endAddress)
//			{
//				auto* header = static_cast<SMBIOS_HEADER*>(currentMappedPtr);
//				if (header->Type == 127 && header->Length == 4)
//				{
//					break;
//				}
//
//				if (header->Length > 0)
//				{
//					auto RandomizeSmbiosString = [&](SMBIOS_STRING stringIndex) {
//						if (!stringIndex) return;
//						char* stringToModify = nullptr;
//						{
//							const char* start = (const char*)header + header->Length;
//							if (*start != 0) {
//								SMBIOS_STRING currentIndex = stringIndex;
//								while (--currentIndex) {
//									start += strlen(start) + 1;
//								}
//								stringToModify = const_cast<char*>(start);
//							}
//						}
//						if (!stringToModify) return;
//						{
//							const auto length = static_cast<int>(strlen(stringToModify));
//							if (length == 0) return;
//							char* buffer = static_cast<char*>(ExAllocatePoolWithTag(NonPagedPool, length + 1, 'oamL'));
//							if (buffer)
//							{
//								{
//									static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
//									auto seed = KeQueryTimeIncrement();
//									for (int n = 0; n < length; n++)
//									{
//										auto key = RtlRandomEx(&seed) % (sizeof(alphanum) - 1);
//										buffer[n] = alphanum[key];
//									}
//									buffer[length] = '\0';
//								}
//								memcpy(stringToModify, buffer, length);
//								ExFreePool(buffer);
//							}
//						}
//						};
//
//					if (header->Type == 0) {
//						auto* type0 = reinterpret_cast<SMBIOS_TYPE0*>(header);
//						RandomizeSmbiosString(type0->Vendor);
//					}
//					else if (header->Type == 1) {
//						auto* type1 = reinterpret_cast<SMBIOS_TYPE1*>(header);
//						RandomizeSmbiosString(type1->Manufacturer);
//						RandomizeSmbiosString(type1->ProductName);
//						RandomizeSmbiosString(type1->SerialNumber);
//
//						GUID newUuid;
//						if (NT_SUCCESS(ExUuidCreate(&newUuid)))
//						{
//							memcpy(&type1->Uuid, &newUuid, sizeof(type1->Uuid));
//						}
//					}
//					else if (header->Type == 2) {
//						auto* type2 = reinterpret_cast<SMBIOS_TYPE2*>(header);
//						RandomizeSmbiosString(type2->Manufacturer);
//						RandomizeSmbiosString(type2->ProductName);
//						RandomizeSmbiosString(type2->SerialNumber);
//					}
//					else if (header->Type == 3) {
//						auto* type3 = reinterpret_cast<SMBIOS_TYPE3*>(header);
//						RandomizeSmbiosString(type3->Manufacturer);
//						RandomizeSmbiosString(type3->SerialNumber);
//					}
//				}
//
//				PUCHAR nextTableStart = (PUCHAR)currentMappedPtr + header->Length;
//				while (nextTableStart < endAddress - 1 && (*nextTableStart != 0 || *(nextTableStart + 1) != 0))
//				{
//					nextTableStart++;
//				}
//				currentMappedPtr = nextTableStart + 2;
//			}
//
//			MmUnmapIoSpace(mappedBase, size);
//
//			return STATUS_SUCCESS;
//		}
//
//		//破坏表结构
//		NTSTATUS SpoofSMBIOS2() {
//			static BOOLEAN Enable = FALSE;
//			NTSTATUS Result = STATUS_SUCCESS;
//
//			if (Enable == FALSE) {
//				PVOID base = GetKernelModuleBase("ntoskrnl.exe");
//				if (!base) {
//					return STATUS_NOT_FOUND;
//				}
//
//				PVOID ExpBootEnvironmentInformation = NULL;
//				PPHYSICAL_ADDRESS WmipSMBiosTablePhysicalAddress = NULL;
//
//				PVOID pAddress1 = FindPatternImage(base, "\x0F\x10\x05\x00\x00\x00\x00\x0F\x11\x00\x8B", "xxx????xx?x");
//				if (pAddress1 != NULL) {
//					ExpBootEnvironmentInformation = (PVOID)((ULONG64)pAddress1 + *(INT*)((ULONG64)pAddress1 + 3) + 3 + 4);
//				}
//
//				if (ExpBootEnvironmentInformation != NULL) {
//					PVOID SignAddress = FindPatternImage(base, "\x48\x8B\x0D\x00\x00\x00\x00\x48\x85\xC9\x0F\x84\x00\x00\x00\x00\x48\x8B\xD0", "xxx????xxxxx????xxx");
//					if (SignAddress != NULL) {
//						WmipSMBiosTablePhysicalAddress = (PPHYSICAL_ADDRESS)((ULONG64)SignAddress + *(INT*)((ULONG64)SignAddress + 3) + 3 + 4);
//					}
//				}
//
//				if (ExpBootEnvironmentInformation != NULL && WmipSMBiosTablePhysicalAddress != NULL) {
//					for (SIZE_T i = 0; i < 16; i++) {
//						ULONG Seed = (ULONG)(123456 * i);
//						((char*)ExpBootEnvironmentInformation)[i] = (char)(RtlRandomEx(&Seed) % 255);
//					}
//
//					for (SIZE_T i = 0; i < sizeof(PHYSICAL_ADDRESS); i++) {
//						((char*)WmipSMBiosTablePhysicalAddress)[i] = (char)0;
//					}
//
//					Enable = TRUE;
//				}
//			}
//
//			if (Enable != FALSE) {
//				Result = STATUS_SUCCESS;
//			}
//			else {
//				Result = STATUS_UNSUCCESSFUL;
//			}
//
//			return Result;
//		}
//
//
//	}
//
//
//	//==================================================================================
//	// 6. 伪造网卡 (Spoof NIC)
//	//==================================================================================
//	namespace SpoofNIC
//	{
//		using namespace Utils;
//		using namespace WinKernel;
//
//		//------------------------------------------------
//		// 结构体定义 (已精简，只保留内存修改所需)
//		//------------------------------------------------
//
//		// 物理地址 (如MAC地址) 结构
//		typedef struct _IF_PHYSICAL_ADDRESS_LH {
//			USHORT Length;
//			UCHAR Address[32];
//		} IF_PHYSICAL_ADDRESS_LH, * PIF_PHYSICAL_ADDRESS_LH;
//
//		// NDIS 接口块，包含物理地址信息
//		typedef struct _NDIS_IF_BLOCK {
//			char _padding_0[0x464];
//			IF_PHYSICAL_ADDRESS_LH ifPhysAddress;        // 0x464
//			IF_PHYSICAL_ADDRESS_LH PermanentPhysAddress; // 0x486
//		} NDIS_IF_BLOCK, * PNDIS_IF_BLOCK;
//
//		// 内核字符串结构 (来自ndis.sys的定义)
//		typedef struct _KSTRING {
//			char _padding_0[0x10];
//			WCHAR Buffer[1]; // 0x10, 实际大小可变
//		} KSTRING, * PKSTRING;
//
//		// NDIS 过滤器块，用于遍历系统中的网络适配器
//		typedef struct _NDIS_FILTER_BLOCK {
//			char _padding_0[0x8];
//			struct _NDIS_FILTER_BLOCK* NextFilter; // 0x8
//			char _padding_1[0x18];
//			PKSTRING FilterInstanceName;           // 0x28
//		} NDIS_FILTER_BLOCK, * PNDIS_FILTER_BLOCK;
//
//
//		//------------------------------------------------
//		// 模块级静态/全局变量 (已精简)
//		//------------------------------------------------
//		static DWORD g_Seed = 0;
//
//
//		//------------------------------------------------
//		// 内部辅助函数 (只保留核心算法)
//		//------------------------------------------------
//		static DWORD Internal_Random(DWORD* seed) {
//			DWORD s = *seed * 1103515245 + 12345;
//			*seed = s;
//			return (s / 65536) % 32768;
//		}
//
//		static DWORD Internal_Hash(unsigned char* buffer, DWORD length) {
//			if (!length) return 0;
//			DWORD h = (*buffer ^ 0x4B9ACE2F) * 0x1000193;
//			for (DWORD i = 1; i < length; ++i) {
//				h = (buffer[i] ^ h) * 0x1000193;
//			}
//			return h;
//		}
//
//		static VOID Internal_SpoofBuffer(DWORD seed, unsigned char* buffer, DWORD length) {
//			seed ^= Internal_Hash(buffer, length);
//			for (DWORD i = 0; i < length; ++i) {
//				buffer[i] ^= (char)Internal_Random(&seed);
//			}
//		}
//
//
//		NTSTATUS SpoofNIC() {
//			// --- 确保种子只在首次调用时被动态初始化 ---
//			static BOOLEAN g_SeedInitialized = FALSE;
//			if (!g_SeedInitialized)
//			{
//				LARGE_INTEGER time;
//				KeQuerySystemTime(&time); // 获取当前高精度系统时间
//				g_Seed = time.LowPart ^ time.HighPart;
//				g_SeedInitialized = TRUE; // 标记种子已经初始化，防止重复设置
//			}
//
//			// 查找并修改 ndis.sys 中的网卡信息
//			PVOID base = GetKernelModuleBase(OBFUSCATE("ndis.sys").decrypt());
//			if (!base) {
//				KdPrint((OBFUSCATE("SpoofNIC: ! Failed to get \"ndis.sys\" base address!\n").decrypt()));
//				return STATUS_NOT_FOUND;
//			}
//
//			// 通过特征码扫描定位 ndisGlobalFilterList
//			PNDIS_FILTER_BLOCK ndisGlobalFilterList = (PNDIS_FILTER_BLOCK)FindPatternImage(base,
//				OBFUSCATE("\x40\x8A\xF0\x48\x8B\x05").decrypt(), OBFUSCATE("xxxxxx").decrypt());
//			if (!ndisGlobalFilterList) {
//				KdPrint((OBFUSCATE("SpoofNIC: ! Failed to find ndisGlobalFilterList pattern!\n").decrypt()));
//				return STATUS_NOT_FOUND;
//			}
//
//			// 通过特征码扫描定位 IfBlock 的偏移量
//			DWORD* ndisFilter_IfBlock_pattern = (DWORD*)FindPatternImage(base,
//				OBFUSCATE("\x48\x85\x00\x0F\x84\x00\x00\x00\x00\x00\x8B\x00\x00\x00\x00\x00\x33").decrypt(), OBFUSCATE("xx?xx?????x???xxx").decrypt());
//			if (!ndisFilter_IfBlock_pattern) {
//				KdPrint((OBFUSCATE("SpoofNIC: ! Failed to find ndisFilter_IfBlock pattern!\n").decrypt()));
//				return STATUS_NOT_FOUND;
//			}
//
//			// 从特征码中提取偏移量并计算最终地址
//			DWORD ndisFilter_IfBlock_offset = *(DWORD*)((char*)ndisFilter_IfBlock_pattern + 12);
//			ndisGlobalFilterList = (PNDIS_FILTER_BLOCK)((char*)ndisGlobalFilterList + 3);
//			ndisGlobalFilterList = *(PNDIS_FILTER_BLOCK*)((char*)ndisGlobalFilterList + 7 + *(int*)((char*)ndisGlobalFilterList + 3));
//
//			for (PNDIS_FILTER_BLOCK filter = ndisGlobalFilterList; filter; filter = filter->NextFilter) {
//				PNDIS_IF_BLOCK block = *(PNDIS_IF_BLOCK*)((char*)filter + ndisFilter_IfBlock_offset);
//				if (block && filter->FilterInstanceName && filter->FilterInstanceName->Buffer) {
//					// --- 核心逻辑: 直接修改内存中的 MAC 地址 ---
//					Internal_SpoofBuffer(g_Seed, block->ifPhysAddress.Address, block->ifPhysAddress.Length);
//					Internal_SpoofBuffer(g_Seed, block->PermanentPhysAddress.Address, block->PermanentPhysAddress.Length);
//				}
//			}
//
//			return STATUS_SUCCESS;
//		}
//	}
//
//	//==================================================================================
//	// 7. 伪造GPU (Spoof GPU)
//	//==================================================================================
//	namespace SpoofGPU {
//
//		NTSTATUS SpoofGpuControl(PDEVICE_OBJECT pDevice, PIRP IRP) {
//			// 首先，将IRP原封不动地传递给原始的驱动程序函数
//			NTSTATUS Result = pDevice->DriverObject->MajorFunction[28](pDevice, IRP);
//
//			PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(IRP);
//
//			// 确保 pIoStack 有效，并且是我们想要Hook的目标IOCTL
//			if (pIoStack != NULL && pIoStack->Parameters.DeviceIoControl.IoControlCode == 0x8DE0008 /*IOCTL_NVIDIA_SMIL*/) {
//
//				// 只有在原始调用成功且缓冲区有效时才进行处理
//				if (NT_SUCCESS(Result) && IRP->UserBuffer != NULL) {
//
//					char* pKernelBuffer = (char*)ExAllocatePoolWithTag(NonPagedPool, 512 /*IOCTL_NVIDIA_SMIL_MAX*/, 'LS');
//					if (pKernelBuffer == NULL) {
//						// 错误：内核内存分配失败
//						KdPrint(("[-] SpoofGpuControl: Failed to allocate kernel buffer.\n"));
//					}
//					else {
//						__try {
//							// 探针检查用户缓冲区是否可读
//							ProbeForRead(IRP->UserBuffer, 512, sizeof(UCHAR));
//
//							// 将用户缓冲区的内容安全地拷贝到我们的内核缓冲区
//							RtlCopyMemory(pKernelBuffer, IRP->UserBuffer, 512);
//
//							// 在我们自己的内核缓冲区中安全地搜索 "GPU-" 字符串
//							for (INT Index = 0; Index < (512 - 4); Index++) {
//								if (memcmp(pKernelBuffer + Index, "GPU-", 4) == 0) {
//									// 探针检查用户缓冲区是否可写
//									ProbeForWrite(IRP->UserBuffer, 512, sizeof(UCHAR));
//
//									// 直接在用户缓冲区中将 "GPU-" 的 'G' 替换为 NULL 结束符，实现截断
//									((char*)IRP->UserBuffer)[Index] = '\0';
//
//									// 任务完成，退出循环
//									break;
//								}
//							}
//						}
//						__except (EXCEPTION_EXECUTE_HANDLER) {
//							// 错误：访问用户模式内存时发生异常
//							KdPrint(("[-] SpoofGpuControl: Exception 0x%X caught while accessing UserBuffer.\n", GetExceptionCode()));
//						}
//
//						// 释放内核内存
//						ExFreePoolWithTag(pKernelBuffer, 'LS');
//					}
//				}
//			}
//
//			// 返回原始函数的执行结果
//			return Result;
//		}
//
//		NTSTATUS SpoofGPU() {
//			static BOOLEAN Enable = FALSE;
//			NTSTATUS Result = STATUS_UNSUCCESSFUL; // 初始化为失败状态
//
//			if (Enable == FALSE) {
//				UNICODE_STRING ObjeName = RTL_CONSTANT_STRING(L"\\Driver\\nvlddmkm");
//				PDRIVER_OBJECT DriverObj = NULL;
//
//				// 尝试获取NVIDIA驱动对象
//				Result = L_ObReferenceObjectByName(&ObjeName, OBJ_CASE_INSENSITIVE, 0, 0, *IoDriverObjectType, KernelMode, 0, &DriverObj);
//				if (!NT_SUCCESS(Result)) {
//					// 错误：找不到或无法引用NVIDIA驱动对象
//					KdPrint(("[-] SpoofGPU: Failed to get reference to %wZ. Status: 0x%X\n", &ObjeName, Result));
//					return Result; // 直接返回失败
//				}
//
//				// 成功获取对象，现在进行Hook
//				*(PVOID*)&DriverObj->MajorFunction[28] = InterlockedExchangePointer((PVOID*)&DriverObj->MajorFunction[14], (PVOID)SpoofGpuControl);
//
//				// 解除对驱动对象的引用
//				ObfDereferenceObject(DriverObj);
//
//				// 标记为已启用，并设置最终返回值为成功
//				Enable = TRUE;
//				Result = STATUS_SUCCESS;
//			}
//			else {
//				// 如果已经启用，直接返回成功
//				Result = STATUS_SUCCESS;
//			}
//
//			return Result;
//		}
//
//	}
//
//
//
//	//==================================================================================
//	// 8. 伪造分区 GUID (Spoof PART)
//	//==================================================================================
//	namespace SpoofPART {
//
//		static ULONG g_PartSeed = 0;
//
//		// 前向声明，因为 SpoofVOLUMES 可能需要调用
//		NTSTATUS BuildCompletion(PIO_STACK_LOCATION pIoStack, PIRP IRP, PIO_COMPLETION_ROUTINE Routine);
//
//		// 完成例程：专门用于修改 IOCTL_DISK_GET_DRIVE_LAYOUT_EX
//		NTSTATUS PartLayoutCompletion(PDEVICE_OBJECT pDevice, PIRP IRP, PVOID Context) {
//			UNREFERENCED_PARAMETER(pDevice);
//			struct _COMPLETION_CONTEXT { PVOID Buffer; ULONG BufferLength; PVOID OldContext; PIO_COMPLETION_ROUTINE OldRoutine; } Request;
//
//			if (NT_SUCCESS(IRP->IoStatus.Status) && Context != NULL) {
//				RtlCopyMemory(&Request, Context, sizeof(Request));
//				ExFreePoolWithTag(Context, 'LS');
//
//				if (Request.Buffer != NULL && Request.BufferLength >= sizeof(DRIVE_LAYOUT_INFORMATION_EX)) {
//					PDRIVE_LAYOUT_INFORMATION_EX pLayout = (PDRIVE_LAYOUT_INFORMATION_EX)Request.Buffer;
//					if (pLayout->PartitionStyle == PARTITION_STYLE_GPT) {
//						ExUuidCreate((GUID*)&pLayout->Gpt.DiskId);
//					}
//					else if (pLayout->PartitionStyle == PARTITION_STYLE_MBR) {
//						pLayout->Mbr.Signature = RtlRandomEx(&g_PartSeed);
//					}
//				}
//
//				if (Request.OldRoutine != NULL) {
//					return Request.OldRoutine(pDevice, IRP, Request.OldContext);
//				}
//			}
//
//			if (IRP->PendingReturned) IoMarkIrpPending(IRP);
//			return STATUS_MORE_PROCESSING_REQUIRED;
//		}
//
//		// 完成例程：专门用于修改 IOCTL_DISK_GET_PARTITION_INFO_EX
//		NTSTATUS PartInfoCompletion(PDEVICE_OBJECT pDevice, PIRP IRP, PVOID Context) {
//			UNREFERENCED_PARAMETER(pDevice);
//			struct _COMPLETION_CONTEXT { PVOID Buffer; ULONG BufferLength; PVOID OldContext; PIO_COMPLETION_ROUTINE OldRoutine; } Request;
//
//			if (NT_SUCCESS(IRP->IoStatus.Status) && Context != NULL) {
//				RtlCopyMemory(&Request, Context, sizeof(Request));
//				ExFreePoolWithTag(Context, 'LS');
//
//				if (Request.Buffer != NULL && Request.BufferLength >= sizeof(PARTITION_INFORMATION_EX)) {
//					PPARTITION_INFORMATION_EX pPartInfo = (PPARTITION_INFORMATION_EX)Request.Buffer;
//					if (pPartInfo->PartitionStyle == PARTITION_STYLE_GPT) {
//						ExUuidCreate((GUID*)&pPartInfo->Gpt.PartitionId);
//					}
//				}
//
//				if (Request.OldRoutine != NULL) {
//					return Request.OldRoutine(pDevice, IRP, Request.OldContext);
//				}
//			}
//
//			if (IRP->PendingReturned) IoMarkIrpPending(IRP);
//			return STATUS_MORE_PROCESSING_REQUIRED;
//		}
//
//		NTSTATUS BuildCompletion(PIO_STACK_LOCATION pIoStack, PIRP IRP, PIO_COMPLETION_ROUTINE Routine) {
//			struct _COMPLETION_CONTEXT { PVOID Buffer; ULONG BufferLength; PVOID OldContext; PIO_COMPLETION_ROUTINE OldRoutine; } Request, * pRequestContext;
//
//			pRequestContext = (decltype(pRequestContext))ExAllocatePoolWithTag(NonPagedPool, sizeof(Request), 'LS');
//			if (!pRequestContext) {
//				KdPrint(("[-] BuildCompletion: Failed to allocate context memory.\n"));
//				return STATUS_INSUFFICIENT_RESOURCES;
//			}
//
//			Request.Buffer = IRP->AssociatedIrp.SystemBuffer;
//			Request.BufferLength = pIoStack->Parameters.DeviceIoControl.OutputBufferLength;
//			Request.OldContext = pIoStack->Context;
//			Request.OldRoutine = pIoStack->CompletionRoutine;
//			RtlCopyMemory(pRequestContext, &Request, sizeof(Request));
//
//			pIoStack->Control = SL_INVOKE_ON_SUCCESS | SL_INVOKE_ON_ERROR | SL_INVOKE_ON_CANCEL;
//			pIoStack->Context = pRequestContext;
//			pIoStack->CompletionRoutine = Routine;
//
//			return STATUS_SUCCESS;
//		}
//
//		NTSTATUS SpoofPartControl(PDEVICE_OBJECT pDevice, PIRP IRP) {
//			PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(IRP);
//			if (pIoStack != NULL) {
//				switch (pIoStack->Parameters.DeviceIoControl.IoControlCode) {
//				case IOCTL_DISK_GET_DRIVE_LAYOUT_EX:
//					BuildCompletion(pIoStack, IRP, PartLayoutCompletion);
//					break;
//				case IOCTL_DISK_GET_PARTITION_INFO_EX:
//					BuildCompletion(pIoStack, IRP, PartInfoCompletion);
//					break;
//				}
//			}
//			return pDevice->DriverObject->MajorFunction[28](pDevice, IRP);
//		}
//
//		NTSTATUS SpoofPART() {
//			static BOOLEAN Enable = FALSE;
//			if (Enable) return STATUS_SUCCESS;
//			if (g_PartSeed == 0) { LARGE_INTEGER t; KeQuerySystemTime(&t); g_PartSeed = t.LowPart; }
//
//			UNICODE_STRING ObjeName = RTL_CONSTANT_STRING(L"\\Driver\\partmgr");
//			PDRIVER_OBJECT DriverObj = NULL;
//			NTSTATUS Result = L_ObReferenceObjectByName(&ObjeName, OBJ_CASE_INSENSITIVE, 0, 0, *IoDriverObjectType, KernelMode, 0, &DriverObj);
//
//			if (NT_SUCCESS(Result)) {
//				*(PVOID*)&DriverObj->MajorFunction[28] = InterlockedExchangePointer((PVOID*)&DriverObj->MajorFunction[14], (PVOID)SpoofPartControl);
//				ObfDereferenceObject(DriverObj);
//				Enable = TRUE;
//			}
//			else {
//				KdPrint(("[-] SpoofPART: Failed to hook %wZ. Status: 0x%X\n", &ObjeName, Result));
//			}
//			return Result;
//		}
//	}
//
//
//	//==================================================================================
//	// 9. 伪造卷 GUID (Spoof VOLUMES)
//	//==================================================================================
//	namespace SpoofVOLUMES
//	{
//
//		NTSTATUS SpoofVOLUMES() {
//
//			static BOOLEAN Enable = FALSE;
//
//			NTSTATUS Result = STATUS_SUCCESS;
//
//			if (Enable == FALSE) {
//
//
//
//				PCHAR pSpoofBuffer = (PCHAR)ExAllocatePoolWithTag(NonPagedPool, PAGE_SIZE, 'LS');
//
//				if (pSpoofBuffer != NULL) {
//
//					PCWCHAR VolumeIndex[] = { L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J", L"K", L"L", L"M", L"N", L"O", L"P", L"Q", L"R", L"S", L"T", L"U", L"V", L"W", L"X", L"Y", L"Z" };
//
//					for (SIZE_T Index = NULL; Index < ARRAYSIZE(VolumeIndex); Index++) {
//
//						WCHAR VolumeGuidPath[260] = { NULL };
//
//
//						Result = RtlStringCbPrintfW(VolumeGuidPath, sizeof(VolumeGuidPath), L"\\??\\%ws:\\System Volume Information\\IndexerVolumeGuid", VolumeIndex[Index]);
//
//						if (NT_SUCCESS(Result)) {
//
//
//							IO_STATUS_BLOCK IoStatus = { NULL };
//
//							OBJECT_ATTRIBUTES FileAttrib = { NULL };
//
//							FILE_NETWORK_OPEN_INFORMATION FileInfo = { NULL };
//
//							UNICODE_STRING usFileName = { NULL };
//
//							RtlInitUnicodeString(&usFileName, VolumeGuidPath);
//
//							InitializeObjectAttributes(&FileAttrib, &usFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, 0);
//
//							if (IoFastQueryNetworkAttributes(&FileAttrib, DELETE, 0, &IoStatus, &FileInfo)) {
//
//								if (NT_SUCCESS(IoStatus.Status) && IoStatus.Information != FILE_DOES_NOT_EXIST) {
//
//									Result = STATUS_SUCCESS;
//								}
//							}
//
//
//
//
//							if (NT_SUCCESS(Result)) {
//
//								// 生成新的GUID
//								GUID newGuid;
//								Result = ExUuidCreate(&newGuid);
//
//								WCHAR guidBuffer[40];
//								UNICODE_STRING guidString;
//								RtlInitEmptyUnicodeString(&guidString, guidBuffer, sizeof(guidBuffer));
//
//								// 将GUID结构体格式化为UNICODE字符串
//								Result = RtlUnicodeStringPrintf(
//									&guidString,
//									L"%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
//									newGuid.Data1, newGuid.Data2, newGuid.Data3,
//									newGuid.Data4[0], newGuid.Data4[1], newGuid.Data4[2], newGuid.Data4[3],
//									newGuid.Data4[4], newGuid.Data4[5], newGuid.Data4[6], newGuid.Data4[7]
//								);
//
//
//
//								HANDLE hFile = NULL;
//
//								OBJECT_ATTRIBUTES ObjectAttributes = { NULL };
//
//								IO_STATUS_BLOCK IoStatusBlock = { NULL };
//
//								UNICODE_STRING usFileName = { NULL };
//
//								RtlInitUnicodeString(&usFileName, VolumeGuidPath);
//
//								InitializeObjectAttributes(&ObjectAttributes, &usFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
//
//								Result = ZwCreateFile(&hFile, GENERIC_ALL, &ObjectAttributes, &IoStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
//
//								if (NT_SUCCESS(Result)) {
//
//									LARGE_INTEGER liFileOff = { NULL };
//
//									Result = ZwWriteFile(hFile, NULL, NULL, NULL, &IoStatusBlock, pSpoofBuffer, (ULONG)strlen(pSpoofBuffer), &liFileOff, NULL);
//
//									ZwClose(hFile);
//								}
//
//
//
//
//								Enable = TRUE;
//							}
//						}
//					}
//
//					for (SIZE_T Index = NULL; Index < ARRAYSIZE(VolumeIndex); Index++) {
//
//						WCHAR VolumeGuidPath[260] = { NULL };
//
//
//						Result = RtlStringCbPrintfW(VolumeGuidPath, sizeof(VolumeGuidPath), L"\\??\\%ws:\\System Volume Information\\WPSSettings.data", VolumeIndex[Index]);
//
//						if (NT_SUCCESS(Result)) {
//
//							IO_STATUS_BLOCK IoStatus = { NULL };
//
//							OBJECT_ATTRIBUTES FileAttrib = { NULL };
//
//							FILE_NETWORK_OPEN_INFORMATION FileInfo = { NULL };
//
//							UNICODE_STRING usFileName = { NULL };
//
//							RtlInitUnicodeString(&usFileName, VolumeGuidPath);
//
//							InitializeObjectAttributes(&FileAttrib, &usFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, 0);
//
//							if (IoFastQueryNetworkAttributes(&FileAttrib, DELETE, 0, &IoStatus, &FileInfo)) {
//
//								if (NT_SUCCESS(IoStatus.Status) && IoStatus.Information != FILE_DOES_NOT_EXIST) {
//
//									Result = STATUS_SUCCESS;
//								}
//							}
//
//
//
//							if (NT_SUCCESS(Result)) {
//
//
//
//								// 生成新的GUID
//								GUID newGuid;
//								Result = ExUuidCreate(&newGuid);
//
//								WCHAR guidBuffer[40];
//								UNICODE_STRING guidString;
//								RtlInitEmptyUnicodeString(&guidString, guidBuffer, sizeof(guidBuffer));
//
//								// 将GUID结构体格式化为UNICODE字符串
//								Result = RtlUnicodeStringPrintf(
//									&guidString,
//									L"%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
//									newGuid.Data1, newGuid.Data2, newGuid.Data3,
//									newGuid.Data4[0], newGuid.Data4[1], newGuid.Data4[2], newGuid.Data4[3],
//									newGuid.Data4[4], newGuid.Data4[5], newGuid.Data4[6], newGuid.Data4[7]
//								);
//
//
//
//								HANDLE hFile = NULL;
//
//								OBJECT_ATTRIBUTES ObjectAttributes = { NULL };
//
//								IO_STATUS_BLOCK IoStatusBlock = { NULL };
//
//								UNICODE_STRING usFileName = { NULL };
//
//								RtlInitUnicodeString(&usFileName, VolumeGuidPath);
//
//								InitializeObjectAttributes(&ObjectAttributes, &usFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
//
//								Result = ZwCreateFile(&hFile, GENERIC_ALL, &ObjectAttributes, &IoStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
//
//								if (NT_SUCCESS(Result)) {
//
//									LARGE_INTEGER liFileOff = { NULL };
//
//									Result = ZwWriteFile(hFile, NULL, NULL, NULL, &IoStatusBlock, pSpoofBuffer, (ULONG)strlen(pSpoofBuffer), &liFileOff, NULL);
//
//									ZwClose(hFile);
//								}
//
//
//
//
//								Enable = TRUE;
//
//
//								Enable = TRUE;
//							}
//						}
//					}
//
//					ExFreePoolWithTag(pSpoofBuffer, 'LS');
//				}
//			}
//
//			if (Enable != FALSE) {
//
//				Result = STATUS_SUCCESS;
//			}
//
//			return Result;
//		}
//
//	}
//
//
//
//}
//
//
//
//
//
//
//namespace WIN11_24H2 {
//
//
//	//==================================================================================
//	// 3. 伪造注册表与标识文件 (Spoof Registry and Identifier Files)
//	//==================================================================================
//	namespace SpoofREG
//	{
//		// 伪造注册表键值和特定的标识符文件
//		NTSTATUS SpoofIdentifiers() {
//			// 使用静态布尔值确保此函数的核心逻辑只成功执行一次
//			static BOOLEAN Enable = FALSE;
//			if (Enable) {
//				return STATUS_SUCCESS;
//			}
//
//			NTSTATUS Result = STATUS_UNSUCCESSFUL;
//			BOOLEAN bAnySuccess = FALSE; // 标志：是否有任何一个操作成功
//
//			//================================================
//			// 部分 1: 伪造注册表中的 GUID
//			//================================================
//			KdPrint(("[*] Spoofing registry GUIDs...\n"));
//
//			GUID newGuid;
//			Result = ExUuidCreate(&newGuid);
//			if (!NT_SUCCESS(Result)) {
//				KdPrint(("[-] CRITICAL: Failed to create UUID. Error: 0x%X\n", Result));
//				// UUID创建失败是致命的，后续依赖它的操作无法进行
//			}
//			else {
//				WCHAR guidBuffer[40];
//				UNICODE_STRING guidString;
//				RtlInitEmptyUnicodeString(&guidString, guidBuffer, sizeof(guidBuffer));
//
//				// 将GUID结构体格式化为UNICODE字符串
//				Result = RtlUnicodeStringPrintf(
//					&guidString,
//					L"%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
//					newGuid.Data1, newGuid.Data2, newGuid.Data3,
//					newGuid.Data4[0], newGuid.Data4[1], newGuid.Data4[2], newGuid.Data4[3],
//					newGuid.Data4[4], newGuid.Data4[5], newGuid.Data4[6], newGuid.Data4[7]
//				);
//
//				if (!NT_SUCCESS(Result)) {
//					KdPrint(("[-] CRITICAL: Failed to format GUID to string. Error: 0x%X\n", Result));
//				}
//				else {
//					// 待伪造的注册表键值列表
//					const WCHAR* szKeysToSpoof[][2] = {
//						{ L"\\Registry\\Machine\\Software\\Microsoft\\SQMClient", L"MachineId" },
//						{ L"\\Registry\\Machine\\Software\\Microsoft\\Cryptography", L"MachineGuid" },
//						{ L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\IDConfigDB\\Hardware Profiles\\0001", L"HwProfileGuid" },
//						{ L"\\Registry\\Machine\\Software\\Microsoft\\Windows\\CurrentVersion\\OneSettings\\WSD\\UpdateAgent\\QueryParameters", L"deviceId" },
//						{ L"\\Registry\\Machine\\Software\\Microsoft\\Windows\\CurrentVersion\\OneSettings\\appcompat\\runtimesdbincloud\\QueryParameters", L"deviceid" }
//					};
//
//					for (SIZE_T i = 0; i < ARRAYSIZE(szKeysToSpoof); i++) {
//						// 写入GUID字符串
//						if (NT_SUCCESS(RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE, szKeysToSpoof[i][0], szKeysToSpoof[i][1], REG_SZ, guidString.Buffer, guidString.Length + sizeof(WCHAR)))) {
//							bAnySuccess = TRUE;
//						}
//					}
//				}
//			}
//
//			//================================================
//			// 部分 2: 伪造注册表中的其他标识符
//			//================================================
//
//			// 伪造二进制类型的注册表值
//			{
//				UCHAR randomBinaryData[32];
//				ULONG seed = KeQueryTimeIncrement();
//
//				for (SIZE_T i = 0; i < sizeof(randomBinaryData); i++) {
//					randomBinaryData[i] = (UCHAR)(RtlRandomEx(&seed) & 0xFF);
//				}
//
//				if (NT_SUCCESS(RtlWriteRegistryValue(RTL_REGISTRY_ABSOLUTE, L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\TPM\\WMI", L"WindowsAIKHash", REG_BINARY, randomBinaryData, sizeof(randomBinaryData)))) {
//					bAnySuccess = TRUE;
//				}
//			}
//
//			// 清理挂载设备信息
//			{
//				const WCHAR* volumeLetters[] = { L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J", L"K", L"L", L"M", L"N", L"O", L"P", L"Q", L"R", L"S", L"T", L"U", L"V", L"W", L"X", L"Y", L"Z" };
//				WCHAR volumeValueName[32];
//
//				for (SIZE_T i = 0; i < ARRAYSIZE(volumeLetters); i++) {
//					RtlStringCbPrintfW(volumeValueName, sizeof(volumeValueName), L"\\DosDevices\\%ws:", volumeLetters[i]);
//					if (NT_SUCCESS(RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, L"\\Registry\\Machine\\System\\MountedDevices", volumeValueName))) {
//						bAnySuccess = TRUE;
//					}
//				}
//			}
//
//			//================================================
//			// 部分 3: 伪造 MachineGuid.txt 文件 (原 SpoofFile 逻辑)
//			//================================================
//			KdPrint(("[*] Spoofing identifier files...\n"));
//
//			PWCHAR pFileBuffer = (PWCHAR)ExAllocatePoolWithTag(NonPagedPool, PAGE_SIZE, 'LS');
//			if (pFileBuffer == NULL) {
//				KdPrint(("[-] SpoofIdentifiers: Failed to allocate memory for file spoofing.\n"));
//			}
//			else {
//				ULONG fileSeeds[] = { 0x18547856, 0x74569821, 0x56741359, 0x12347865, 0x75234785, 0x35132475, 0x23547856 };
//
//				NTSTATUS fileStatus = RtlStringCchPrintfW(pFileBuffer,
//					PAGE_SIZE / sizeof(WCHAR),
//					L"{%08X-%04X-%04X-%04X-%04X%04X%04X}",
//					RtlRandomEx(&fileSeeds[0]), (USHORT)(RtlRandomEx(&fileSeeds[1]) & 0xFFFF), (USHORT)(RtlRandomEx(&fileSeeds[2]) & 0xFFFF),
//					(USHORT)(RtlRandomEx(&fileSeeds[3]) & 0xFFFF), (USHORT)(RtlRandomEx(&fileSeeds[4]) & 0xFFFF), (USHORT)(RtlRandomEx(&fileSeeds[5]) & 0xFFFF),
//					(USHORT)(RtlRandomEx(&fileSeeds[6]) & 0xFFFF)
//				);
//
//				if (NT_SUCCESS(fileStatus)) {
//					UNICODE_STRING usFileName;
//					RtlInitUnicodeString(&usFileName, L"\\SystemRoot\\System32\\Restore\\MachineGuid.txt");
//					OBJECT_ATTRIBUTES objectAttributes;
//					InitializeObjectAttributes(&objectAttributes, &usFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
//
//					HANDLE hFile = NULL;
//					IO_STATUS_BLOCK ioStatusBlock;
//
//					fileStatus = ZwCreateFile(&hFile, GENERIC_WRITE, &objectAttributes, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_SUPERSEDE, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
//
//					if (NT_SUCCESS(fileStatus)) {
//						const ULONG bufferSizeInBytes = 38 * sizeof(WCHAR);
//						LARGE_INTEGER byteOffset = { 0 };
//						fileStatus = ZwWriteFile(hFile, NULL, NULL, NULL, &ioStatusBlock, pFileBuffer, bufferSizeInBytes, &byteOffset, NULL);
//						ZwClose(hFile);
//
//						if (NT_SUCCESS(fileStatus)) {
//							bAnySuccess = TRUE; // 文件写入成功也算整体成功
//						}
//					}
//				}
//				ExFreePoolWithTag(pFileBuffer, 'LS');
//			}
//
//			//================================================
//			// 最终状态判断
//			//================================================
//			if (bAnySuccess) {
//				KdPrint(("[+] SpoofIdentifiers: One or more identifiers were successfully spoofed.\n"));
//				Enable = TRUE;      // 标记已成功执行，下次不再进入
//				return STATUS_SUCCESS; // 只要有任意一个成功，就返回整体成功
//			}
//
//			// 如果没有任何操作成功，则返回最后一个遇到的错误代码或默认失败代码
//			KdPrint(("[-] SpoofIdentifiers: All spoofing attempts failed.\n"));
//			return (NT_SUCCESS(Result) ? STATUS_UNSUCCESSFUL : Result);
//		}
//	}
//
//
//	//==================================================================================
//	// 3. 伪造磁盘序列号  ida逆向获取结构体和函数特征
//	//==================================================================================
//	namespace SpoofHDD
//	{
//		using namespace Utils;
//
//		typedef struct _STOR_IDENTITY {
//			PVOID InquiryData;
//			_STRING SerialNumber;
//			USHORT Flags;
//			char Padding[6];
//			PVOID DeviceId;
//			PVOID AtaDeviceId;
//			PVOID RichDeviceDescription;
//		} STOR_IDENTITY, * PSTOR_IDENTITY;
//
//		__declspec(align(4))
//			typedef struct _TELEMETRY_UNIT_EXTENSION {
//			ULONG32 Flags;
//			unsigned __int8 NvmeCriticalWarning;
//			unsigned __int8 DeviceHealthPollCount;
//			unsigned char Reserved[2];
//		} TELEMETRY_UNIT_EXTENSION, * PTELEMETRY_UNIT_EXTENSION;
//
//		__declspec(align(32))
//			typedef struct _RAID_UNIT_EXTENSION {
//			union {
//				struct {
//					char Space[0x70];
//					STOR_IDENTITY Identity;
//				} _Identity;
//
//				struct {
//					char Space2[0x848];
//					TELEMETRY_UNIT_EXTENSION Telemetry;
//				} _Telemetry;
//			};
//			char _tail[0x5F0];
//		} RAID_UNIT_EXTENSION, * PRAID_UNIT_EXTENSION;
//
//		// 函数指针类型定义
//		typedef __int64(__fastcall* RaidUnitRegisterInterfaces_t)(PRAID_UNIT_EXTENSION a1);
//
//
//		NTSTATUS SpoofHDD()
//		{
//
//			PVOID base = GetKernelModuleBase("storport.sys");
//			if (!base) {
//				KdPrint(("[-] Failed to find storport.sys base!\n"));
//				return STATUS_UNSUCCESSFUL;
//			}
//
//			RaidUnitRegisterInterfaces_t registerInterfaces = static_cast<RaidUnitRegisterInterfaces_t>(FindPatternImage(base, "\x48\x89\x5C\x24\x00\x55\x56\x57\x48\x83\xEC\x50", "xxxx?xxxxxxx"));
//			if (!registerInterfaces) {
//				KdPrint(("[-] Failed to find RaidUnitRegisterInterfaces!\n"));
//				return STATUS_UNSUCCESSFUL;
//			}
//
//
//			NTSTATUS status = STATUS_NOT_FOUND;
//			for (int i = 0; i < 2; i++) { // 遍历 RaidPort0 和 RaidPort1
//				PDEVICE_OBJECT raidDeviceChain = nullptr;
//				{
//					wchar_t raidBuffer[32];
//					RtlStringCbPrintfW(raidBuffer, sizeof(raidBuffer), L"\\Device\\RaidPort%d", i);
//
//					UNICODE_STRING raidPortUnicode;
//					RtlInitUnicodeString(&raidPortUnicode, raidBuffer);
//
//					PFILE_OBJECT fileObject = nullptr;
//					PDEVICE_OBJECT deviceObject = nullptr;
//					NTSTATUS getDeviceStatus = IoGetDeviceObjectPointer(&raidPortUnicode, FILE_READ_DATA, &fileObject, &deviceObject);
//					if (NT_SUCCESS(getDeviceStatus)) {
//						raidDeviceChain = deviceObject->DriverObject->DeviceObject;
//						ObDereferenceObject(fileObject);
//					}
//				}
//
//				if (!raidDeviceChain) {
//					continue;
//				}
//
//				PDEVICE_OBJECT currentDevice = raidDeviceChain;
//				while (currentDevice) {
//					// 确保我们操作的是磁盘设备
//					if (currentDevice->DeviceType == FILE_DEVICE_DISK) {
//						PRAID_UNIT_EXTENSION extension = static_cast<PRAID_UNIT_EXTENSION>(currentDevice->DeviceExtension);
//						if (extension) {
//							// 获取原始序列号的长度和缓冲区指针
//							USHORT length = extension->_Identity.Identity.SerialNumber.Length;
//							char* serialBuffer = extension->_Identity.Identity.SerialNumber.Buffer;
//
//							if (length > 0 && serialBuffer) {
//
//								// 1. (可选) 备份原始序列号以便打印日志
//								char originalSerial[256];
//								RtlZeroMemory(originalSerial, sizeof(originalSerial));
//								RtlCopyMemory(originalSerial, serialBuffer, min(length, 255));
//
//								// 2. 准备随机数种子和字符集
//								static const char hexChars[] = "0123456789ABCDEF";
//								auto seed = KeQueryTimeIncrement();
//
//								// 3. 遍历并修改原始缓冲区中的每一个字符
//								for (USHORT n = 0; n < length; ++n) {
//									// VMware虚拟磁盘的序列号可能包含点(.)或下划线(_)，我们可以选择跳过它们
//									if (serialBuffer[n] == '.' || serialBuffer[n] == '_') {
//										continue;
//									}
//									// 也可以跳过空格，因为某些物理硬盘用空格填充末尾
//									if (serialBuffer[n] == ' ') {
//										continue;
//									}
//
//									auto key = RtlRandomEx(&seed) % (sizeof(hexChars) - 1);
//									serialBuffer[n] = hexChars[key];
//								}
//
//								// 4. 打印修改前后的对比日志
//								KdPrint(("[+] Changed disk serial from '%s' to '%s'.\n", originalSerial, serialBuffer));
//								status = STATUS_SUCCESS;
//
//
//								// 重置其他可能被监控的遥测数据
//								extension->_Telemetry.Telemetry.NvmeCriticalWarning = 0;
//
//								// 调用接口使其重新应用或重新缓存我们修改过的信息
//								registerInterfaces(extension);
//							}
//						}
//					}
//					currentDevice = currentDevice->NextDevice;
//				}
//			}
//			return status;
//		}
//	}
//
//
//	//==================================================================================
//	// 5. 伪造SMBIOS (Spoof SMBIOS)
//	//==================================================================================
//	namespace SpoofSMBIOS
//	{
//		NTSTATUS SpoofSMBIOS() {
//
//			using namespace Utils;
//
//			static BOOLEAN Spoofed = FALSE;
//			if (Spoofed == TRUE) {
//				return STATUS_SUCCESS;
//			}
//
//			PVOID ntosBase = GetKernelModuleBase("ntoskrnl.exe");
//			if (!ntosBase) {
//				// 错误输出: 获取内核基址失败 (保留)
//				DbgPrint("[-] SpoofSMBIOS: 获取 ntoskrnl.exe 基址失败。\n");
//				return STATUS_NOT_FOUND;
//			}
//			//每个系统不一样根据系统定制偏移!!!
//			/*
//			 * [WmipSMBiosTablePhysicalAddress 的偏移量]
//			 * 该值是使用 WinDbg 内核调试器在特定 Windows 内核版本上计算得出的。
//			 * 计算公式: 偏移量 = 变量地址 - 模块基地址
//			 * ---------------------------------------------------------------------
//			 * 变量地址 (   x nt!*WmipSMBiosTablePhysicalAddress*    ): fffff802`d4dd60e8
//			 * 模块基地址 (   lm m nt   ):                      fffff802`d3e00000
//			 * ---------------------------------------------------------------------
//			 * 计算结果 (? fffff802`d4dd60e8 - fffff802`d3e00000): 0xFD60E8
//			 */
//
//			//ida:加载ntoskrnl,Names搜索WmipSMBiosTablePhysicalAddress获得偏移
//			ULONGLONG offset_WmipSMBios = 0xFD60E8;
//
//			/*
//			 * [ExpBootEnvironmentInformation 的偏移量]
//			 * 该值是使用 WinDbg 内核调试器在特定 Windows 内核版本上计算得出的。
//			 * 计算公式: 偏移量 = 变量地址 - 模块基地址
//			 * ---------------------------------------------------------------------
//			 * 变量地址 (x nt!*ExpBootEnvironmentInformation*): fffff802`d4cfe0a0
//			 * 模块基地址 (lm m nt):                         fffff802`d3e00000
//			 * ---------------------------------------------------------------------
//			 * 计算结果 (? fffff802`d4cfe0a0 - fffff802`d3e00000): 0xEFE0A0
//			 */
//			 //ida:加载ntoskrnl,Names搜索ExpBootEnvironmentInformation获得偏移
//			ULONGLONG offset_ExpBoot = 0xEFE0A0;
//
//
//			// 计算两个目标的最终虚拟地址
//			PPHYSICAL_ADDRESS pWmipSMBiosTablePhysicalAddress = (PPHYSICAL_ADDRESS)((PUCHAR)ntosBase + offset_WmipSMBios);
//			PVOID pExpBootEnvironmentInformation = (PVOID)((PUCHAR)ntosBase + offset_ExpBoot);
//
//
//			// 执行所有修改操作
//			__try {
//				// 操作 1: 随机化启动环境信息
//				ULONG seed = (ULONG)KeQueryInterruptTime();
//				for (SIZE_T i = 0; i < 16; i++) {
//					seed = (seed * 0x343FD + 0x269EC3) + (ULONG)i;
//					((PUCHAR)pExpBootEnvironmentInformation)[i] = (UCHAR)(RtlRandomEx(&seed) & 0xFF);
//				}
//
//				// 操作 2: 清零 SMBIOS 表的物理地址
//				pWmipSMBiosTablePhysicalAddress->QuadPart = 0;
//
//				Spoofed = TRUE;
//			}
//			__except (EXCEPTION_EXECUTE_HANDLER) {
//				// 错误输出: 内存修改时发生异常 (保留)
//				DbgPrint("[-] SpoofSMBIOS: 在修改内存期间发生异常。\n");
//				return GetExceptionCode();
//			}
//
//			return STATUS_SUCCESS;
//		}
//
//	}
//
//	//==================================================================================
//	// 6. 伪造网卡 (Spoof NIC)
//	//==================================================================================
//
//	namespace SpoofNIC
//	{
//		using namespace Utils;
//		using namespace WinKernel;
//
//		// --- 您原来的结构体定义 (保持不变) ---
//		typedef struct _IF_PHYSICAL_ADDRESS_LH {
//			USHORT Length;
//			UCHAR Address[32];
//		} IF_PHYSICAL_ADDRESS_LH, * PIF_PHYSICAL_ADDRESS_LH;
//
//		typedef struct _NDIS_IF_BLOCK {
//			char _padding_0[0x464];
//			IF_PHYSICAL_ADDRESS_LH ifPhysAddress;
//			IF_PHYSICAL_ADDRESS_LH PermanentPhysAddress;
//		} NDIS_IF_BLOCK, * PNDIS_IF_BLOCK;
//
//		typedef struct _KSTRING {
//			char _padding_0[0x10];
//			WCHAR Buffer[1];
//		} KSTRING, * PKSTRING;
//
//		typedef struct _NDIS_FILTER_BLOCK {
//			char _padding_0[0x8];
//			struct _NDIS_FILTER_BLOCK* NextFilter;
//			char _padding_1[0x18];
//			PKSTRING FilterInstanceName;
//		} NDIS_FILTER_BLOCK, * PNDIS_FILTER_BLOCK;
//
//
//		// --- 您原来的辅助函数 (保持不变) ---
//		static DWORD g_Seed = 0;
//
//		static DWORD Internal_Random(DWORD* seed) {
//			DWORD s = *seed * 1103515245 + 12345;
//			*seed = s;
//			return (s / 65536) % 32768;
//		}
//
//		static DWORD Internal_Hash(unsigned char* buffer, DWORD length) {
//			if (!length) return 0;
//			DWORD h = (*buffer ^ 0x4B9ACE2F) * 0x1000193;
//			for (DWORD i = 1; i < length; ++i) {
//				h = (buffer[i] ^ h) * 0x1000193;
//			}
//			return h;
//		}
//
//		static VOID Internal_SpoofBuffer(DWORD seed, unsigned char* buffer, DWORD length) {
//			seed ^= Internal_Hash(buffer, length);
//			for (DWORD i = 0; i < length; ++i) {
//				buffer[i] ^= (char)Internal_Random(&seed);
//			}
//		}
//
//
//		// --- 经过增强的 SpoofNIC 调试函数 ---
//		NTSTATUS SpoofNIC()
//		{
//			KdPrint(("[SpoofNIC] ==> Function entered.\n"));
//
//			static BOOLEAN g_SeedInitialized = FALSE;
//			if (!g_SeedInitialized)
//			{
//				LARGE_INTEGER time;
//				KeQuerySystemTime(&time);
//				g_Seed = time.LowPart ^ time.HighPart;
//				g_SeedInitialized = TRUE;
//				KdPrint(("[SpoofNIC] Seed initialized with value: 0x%X\n", g_Seed));
//			}
//
//			PVOID base = GetKernelModuleBase("ndis.sys");
//			if (!base) {
//				KdPrint(("[SpoofNIC] !!! CRITICAL: Failed to get \"ndis.sys\" base address!\n"));
//				return STATUS_NOT_FOUND;
//			}
//			KdPrint(("[SpoofNIC] ndis.sys base address found at: %p\n", base));
//
//			//IDA：Names搜索ndisGlobalFilterList获取偏移
//			ULONGLONG ndisGlobalFilterList_Offset = 0x123638;
//			//IDA：Loacal Types搜索_NDIS_FILTER_BLOCK获取成员IfBlock偏移
//			DWORD IfBlock_Offset = 0x2B0;
//
//			KdPrint(("[SpoofNIC] Using ndisGlobalFilterList_Offset: 0x%llX\n", ndisGlobalFilterList_Offset));
//			KdPrint(("[SpoofNIC] Using IfBlock_Offset: 0x%X\n", IfBlock_Offset));
//
//			// 使用偏移量直接计算出 ndisGlobalFilterList 全局变量自身的地址
//			PNDIS_FILTER_BLOCK* p_ndisGlobalFilterList = (PNDIS_FILTER_BLOCK*)((PUCHAR)base + ndisGlobalFilterList_Offset);
//			KdPrint(("[SpoofNIC] Calculated address of ndisGlobalFilterList pointer variable: %p\n", p_ndisGlobalFilterList));
//
//			// 保护内存访问，防止因地址无效导致蓝屏
//			__try
//			{
//				// 解引用一次，获取链表的第一个节点的地址
//				PNDIS_FILTER_BLOCK ndisGlobalFilterList = *p_ndisGlobalFilterList;
//
//				// 这是最关键的检查点之一！如果这里为NULL，循环将不会执行。
//				KdPrint(("[SpoofNIC] Value of ndisGlobalFilterList (list head): %p\n", ndisGlobalFilterList));
//
//				if (!ndisGlobalFilterList) {
//					KdPrint(("[SpoofNIC] !!! WARNING: Filter list head is NULL. No network adapters found or offset is incorrect. Exiting.\n"));
//					return STATUS_SUCCESS;
//				}
//
//				int filterCount = 0;
//				for (PNDIS_FILTER_BLOCK filter = ndisGlobalFilterList; filter; filter = filter->NextFilter)
//				{
//					filterCount++;
//					KdPrint(("[SpoofNIC] [%d] Processing filter block at: %p\n", filterCount, filter));
//
//					// 使用我们找到的结构体成员偏移量来定位 IfBlock
//					PNDIS_IF_BLOCK IfBlock = *(PNDIS_IF_BLOCK*)((PUCHAR)filter + IfBlock_Offset);
//
//
//					KdPrint(("[SpoofNIC]   -> Potential IfBlock pointer value: %p\n", IfBlock));
//
//					if (IfBlock && filter->FilterInstanceName && filter->FilterInstanceName->Buffer && IfBlock->ifPhysAddress.Length == 6)
//					{
//						KdPrint(("[SpoofNIC]   -> SUCCESS: Found valid IfBlock for instance: %ws\n", filter->FilterInstanceName->Buffer));
//						KdPrint(("[SpoofNIC]   -> Original MAC: %02x-%02x-%02x-%02x-%02x-%02x\n",
//							IfBlock->ifPhysAddress.Address[0], IfBlock->ifPhysAddress.Address[1],
//							IfBlock->ifPhysAddress.Address[2], IfBlock->PermanentPhysAddress.Address[3],
//							IfBlock->ifPhysAddress.Address[4], IfBlock->ifPhysAddress.Address[5]));
//
//						// --- 核心逻辑: 直接修改内存中的 MAC 地址 ---
//						Internal_SpoofBuffer(g_Seed, IfBlock->ifPhysAddress.Address, IfBlock->ifPhysAddress.Length);
//						Internal_SpoofBuffer(g_Seed, IfBlock->PermanentPhysAddress.Address, IfBlock->PermanentPhysAddress.Length);
//
//						//确保生成的MAC地址是有效的单播、本地管理地址
//						// 防止生成多播地址（最低位为1）导致网络异常
//						IfBlock->ifPhysAddress.Address[0] &= 0xFE; // 将最低位置为0 (单播)
//						IfBlock->ifPhysAddress.Address[0] |= 0x02; // 将倒数第二位置为1 (本地管理)
//						// 对永久地址也做同样操作
//						IfBlock->PermanentPhysAddress.Address[0] &= 0xFE;
//						IfBlock->PermanentPhysAddress.Address[0] |= 0x02;
//
//						KdPrint(("[SpoofNIC]   -> Spoofed MAC:  %02x-%02x-%02x-%02x-%02x-%02x\n",
//							IfBlock->ifPhysAddress.Address[0], IfBlock->ifPhysAddress.Address[1],
//							IfBlock->ifPhysAddress.Address[2], IfBlock->ifPhysAddress.Address[3],
//							IfBlock->ifPhysAddress.Address[4], IfBlock->ifPhysAddress.Address[5]));
//					}
//					else {
//						KdPrint(("[SpoofNIC]   -> INFO: Skipped this filter block. IfBlock was NULL, name was NULL, or MAC length was not 6.\n"));
//					}
//				}
//
//				if (filterCount == 0) {
//					KdPrint(("[SpoofNIC] !!! WARNING: The filter list was traversed, but zero items were processed. The list might be empty.\n"));
//				}
//			}
//			__except (EXCEPTION_EXECUTE_HANDLER)
//			{
//				KdPrint(("[SpoofNIC] !!! CRITICAL: An exception (0x%X) occurred while accessing memory. Offsets are likely incorrect!\n", GetExceptionCode()));
//				return GetExceptionCode();
//			}
//
//			KdPrint(("[SpoofNIC] ==> Function finished successfully.\n"));
//			return STATUS_SUCCESS;
//		}
//	}
//	//==================================================================================
//	// 7. 伪造GPU (Spoof GPU)
//	//==================================================================================
//	namespace SpoofGPU {
//
//		NTSTATUS SpoofGpuControl(PDEVICE_OBJECT pDevice, PIRP IRP) {
//			// 首先，将IRP原封不动地传递给原始的驱动程序函数
//			NTSTATUS Result = pDevice->DriverObject->MajorFunction[28](pDevice, IRP);
//
//			PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(IRP);
//
//			// 确保 pIoStack 有效，并且是我们想要Hook的目标IOCTL
//			if (pIoStack != NULL && pIoStack->Parameters.DeviceIoControl.IoControlCode == 0x8DE0008 /*IOCTL_NVIDIA_SMIL*/) {
//
//				// 只有在原始调用成功且缓冲区有效时才进行处理
//				if (NT_SUCCESS(Result) && IRP->UserBuffer != NULL) {
//
//					char* pKernelBuffer = (char*)ExAllocatePoolWithTag(NonPagedPool, 512 /*IOCTL_NVIDIA_SMIL_MAX*/, 'LS');
//					if (pKernelBuffer == NULL) {
//						// 错误：内核内存分配失败
//						KdPrint(("[-] SpoofGpuControl: Failed to allocate kernel buffer.\n"));
//					}
//					else {
//						__try {
//							// 探针检查用户缓冲区是否可读
//							ProbeForRead(IRP->UserBuffer, 512, sizeof(UCHAR));
//
//							// 将用户缓冲区的内容安全地拷贝到我们的内核缓冲区
//							RtlCopyMemory(pKernelBuffer, IRP->UserBuffer, 512);
//
//							// 在我们自己的内核缓冲区中安全地搜索 "GPU-" 字符串
//							for (INT Index = 0; Index < (512 - 4); Index++) {
//								if (memcmp(pKernelBuffer + Index, "GPU-", 4) == 0) {
//									// 探针检查用户缓冲区是否可写
//									ProbeForWrite(IRP->UserBuffer, 512, sizeof(UCHAR));
//
//									// 直接在用户缓冲区中将 "GPU-" 的 'G' 替换为 NULL 结束符，实现截断
//									((char*)IRP->UserBuffer)[Index] = '\0';
//
//									// 任务完成，退出循环
//									break;
//								}
//							}
//						}
//						__except (EXCEPTION_EXECUTE_HANDLER) {
//							// 错误：访问用户模式内存时发生异常
//							KdPrint(("[-] SpoofGpuControl: Exception 0x%X caught while accessing UserBuffer.\n", GetExceptionCode()));
//						}
//
//						// 释放内核内存
//						ExFreePoolWithTag(pKernelBuffer, 'LS');
//					}
//				}
//			}
//
//			// 返回原始函数的执行结果
//			return Result;
//		}
//
//		NTSTATUS SpoofGPU() {
//			static BOOLEAN Enable = FALSE;
//			NTSTATUS Result = STATUS_UNSUCCESSFUL; // 初始化为失败状态
//
//			if (Enable == FALSE) {
//				UNICODE_STRING ObjeName = RTL_CONSTANT_STRING(L"\\Driver\\nvlddmkm");
//				PDRIVER_OBJECT DriverObj = NULL;
//
//				// 尝试获取NVIDIA驱动对象
//				Result = L_ObReferenceObjectByName(&ObjeName, OBJ_CASE_INSENSITIVE, 0, 0, *IoDriverObjectType, KernelMode, 0, &DriverObj);
//				if (!NT_SUCCESS(Result)) {
//					// 错误：找不到或无法引用NVIDIA驱动对象
//					KdPrint(("[-] SpoofGPU: Failed to get reference to %wZ. Status: 0x%X\n", &ObjeName, Result));
//					return Result; // 直接返回失败
//				}
//
//				// 成功获取对象，现在进行Hook
//				*(PVOID*)&DriverObj->MajorFunction[28] = InterlockedExchangePointer((PVOID*)&DriverObj->MajorFunction[14], (PVOID)SpoofGpuControl);
//
//				// 解除对驱动对象的引用
//				ObfDereferenceObject(DriverObj);
//
//				// 标记为已启用，并设置最终返回值为成功
//				Enable = TRUE;
//				Result = STATUS_SUCCESS;
//			}
//			else {
//				// 如果已经启用，直接返回成功
//				Result = STATUS_SUCCESS;
//			}
//
//			return Result;
//		}
//
//	}
//
//
//
//	//==================================================================================
//	// 8. 伪造分区 GUID (Spoof PART)
//	//==================================================================================
//	namespace SpoofPART {
//
//		static ULONG g_PartSeed = 0;
//
//
//		// 完成例程：专门用于修改 IOCTL_DISK_GET_DRIVE_LAYOUT_EX
//		NTSTATUS PartLayoutCompletion(PDEVICE_OBJECT pDevice, PIRP IRP, PVOID Context) {
//			UNREFERENCED_PARAMETER(pDevice);
//			struct _COMPLETION_CONTEXT { PVOID Buffer; ULONG BufferLength; PVOID OldContext; PIO_COMPLETION_ROUTINE OldRoutine; } Request;
//
//			if (NT_SUCCESS(IRP->IoStatus.Status) && Context != NULL) {
//				RtlCopyMemory(&Request, Context, sizeof(Request));
//				ExFreePoolWithTag(Context, 'LS');
//
//				if (Request.Buffer != NULL && Request.BufferLength >= sizeof(DRIVE_LAYOUT_INFORMATION_EX)) {
//					PDRIVE_LAYOUT_INFORMATION_EX pLayout = (PDRIVE_LAYOUT_INFORMATION_EX)Request.Buffer;
//					if (pLayout->PartitionStyle == PARTITION_STYLE_GPT) {
//						ExUuidCreate((GUID*)&pLayout->Gpt.DiskId);
//					}
//					else if (pLayout->PartitionStyle == PARTITION_STYLE_MBR) {
//						pLayout->Mbr.Signature = RtlRandomEx(&g_PartSeed);
//					}
//				}
//
//				if (Request.OldRoutine != NULL) {
//					return Request.OldRoutine(pDevice, IRP, Request.OldContext);
//				}
//			}
//
//			if (IRP->PendingReturned) IoMarkIrpPending(IRP);
//			return STATUS_MORE_PROCESSING_REQUIRED;
//		}
//
//		// 完成例程：专门用于修改 IOCTL_DISK_GET_PARTITION_INFO_EX
//		NTSTATUS PartInfoCompletion(PDEVICE_OBJECT pDevice, PIRP IRP, PVOID Context) {
//			UNREFERENCED_PARAMETER(pDevice);
//			struct _COMPLETION_CONTEXT { PVOID Buffer; ULONG BufferLength; PVOID OldContext; PIO_COMPLETION_ROUTINE OldRoutine; } Request;
//
//			if (NT_SUCCESS(IRP->IoStatus.Status) && Context != NULL) {
//				RtlCopyMemory(&Request, Context, sizeof(Request));
//				ExFreePoolWithTag(Context, 'LS');
//
//				if (Request.Buffer != NULL && Request.BufferLength >= sizeof(PARTITION_INFORMATION_EX)) {
//					PPARTITION_INFORMATION_EX pPartInfo = (PPARTITION_INFORMATION_EX)Request.Buffer;
//					if (pPartInfo->PartitionStyle == PARTITION_STYLE_GPT) {
//						ExUuidCreate((GUID*)&pPartInfo->Gpt.PartitionId);
//					}
//				}
//
//				if (Request.OldRoutine != NULL) {
//					return Request.OldRoutine(pDevice, IRP, Request.OldContext);
//				}
//			}
//
//			if (IRP->PendingReturned) IoMarkIrpPending(IRP);
//			return STATUS_MORE_PROCESSING_REQUIRED;
//		}
//
//		NTSTATUS BuildCompletion(PIO_STACK_LOCATION pIoStack, PIRP IRP, PIO_COMPLETION_ROUTINE Routine) {
//			struct _COMPLETION_CONTEXT { PVOID Buffer; ULONG BufferLength; PVOID OldContext; PIO_COMPLETION_ROUTINE OldRoutine; } Request, * pRequestContext;
//
//			pRequestContext = (decltype(pRequestContext))ExAllocatePoolWithTag(NonPagedPool, sizeof(Request), 'LS');
//			if (!pRequestContext) {
//				KdPrint(("[-] BuildCompletion: Failed to allocate context memory.\n"));
//				return STATUS_INSUFFICIENT_RESOURCES;
//			}
//
//			Request.Buffer = IRP->AssociatedIrp.SystemBuffer;
//			Request.BufferLength = pIoStack->Parameters.DeviceIoControl.OutputBufferLength;
//			Request.OldContext = pIoStack->Context;
//			Request.OldRoutine = pIoStack->CompletionRoutine;
//			RtlCopyMemory(pRequestContext, &Request, sizeof(Request));
//
//			pIoStack->Control = SL_INVOKE_ON_SUCCESS | SL_INVOKE_ON_ERROR | SL_INVOKE_ON_CANCEL;
//			pIoStack->Context = pRequestContext;
//			pIoStack->CompletionRoutine = Routine;
//
//			return STATUS_SUCCESS;
//		}
//
//		NTSTATUS SpoofPartControl(PDEVICE_OBJECT pDevice, PIRP IRP) {
//			PIO_STACK_LOCATION pIoStack = IoGetCurrentIrpStackLocation(IRP);
//			if (pIoStack != NULL) {
//				switch (pIoStack->Parameters.DeviceIoControl.IoControlCode) {
//				case IOCTL_DISK_GET_DRIVE_LAYOUT_EX:
//					BuildCompletion(pIoStack, IRP, PartLayoutCompletion);
//					break;
//				case IOCTL_DISK_GET_PARTITION_INFO_EX:
//					BuildCompletion(pIoStack, IRP, PartInfoCompletion);
//					break;
//				}
//			}
//			return pDevice->DriverObject->MajorFunction[28](pDevice, IRP);
//		}
//
//		NTSTATUS SpoofPART() {
//			static BOOLEAN Enable = FALSE;
//			if (Enable) return STATUS_SUCCESS;
//			if (g_PartSeed == 0) { LARGE_INTEGER t; KeQuerySystemTime(&t); g_PartSeed = t.LowPart; }
//
//			UNICODE_STRING ObjeName = RTL_CONSTANT_STRING(L"\\Driver\\partmgr");
//			PDRIVER_OBJECT DriverObj = NULL;
//			NTSTATUS Result = L_ObReferenceObjectByName(&ObjeName, OBJ_CASE_INSENSITIVE, 0, 0, *IoDriverObjectType, KernelMode, 0, &DriverObj);
//
//			if (NT_SUCCESS(Result)) {
//				*(PVOID*)&DriverObj->MajorFunction[28] = InterlockedExchangePointer((PVOID*)&DriverObj->MajorFunction[14], (PVOID)SpoofPartControl);
//				ObfDereferenceObject(DriverObj);
//				Enable = TRUE;
//			}
//			else {
//				KdPrint(("[-] SpoofPART: Failed to hook %wZ. Status: 0x%X\n", &ObjeName, Result));
//			}
//			return Result;
//		}
//	}
//
//
//
//
//	//==================================================================================
//	// 9. 伪造卷 GUID (Spoof VOLUMES)
//	//==================================================================================
//	namespace SpoofVOLUMES
//	{
//
//		NTSTATUS SpoofVOLUMES() {
//
//			static BOOLEAN Enable = FALSE;
//
//			NTSTATUS Result = STATUS_SUCCESS;
//
//			if (Enable == FALSE) {
//
//
//
//				PCHAR pSpoofBuffer = (PCHAR)ExAllocatePoolWithTag(NonPagedPool, PAGE_SIZE, 'LS');
//
//				if (pSpoofBuffer != NULL) {
//
//					PCWCHAR VolumeIndex[] = { L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J", L"K", L"L", L"M", L"N", L"O", L"P", L"Q", L"R", L"S", L"T", L"U", L"V", L"W", L"X", L"Y", L"Z" };
//
//					for (SIZE_T Index = NULL; Index < ARRAYSIZE(VolumeIndex); Index++) {
//
//						WCHAR VolumeGuidPath[260] = { NULL };
//
//
//						Result = RtlStringCbPrintfW(VolumeGuidPath, sizeof(VolumeGuidPath), L"\\??\\%ws:\\System Volume Information\\IndexerVolumeGuid", VolumeIndex[Index]);
//
//						if (NT_SUCCESS(Result)) {
//
//
//							IO_STATUS_BLOCK IoStatus = { NULL };
//
//							OBJECT_ATTRIBUTES FileAttrib = { NULL };
//
//							FILE_NETWORK_OPEN_INFORMATION FileInfo = { NULL };
//
//							UNICODE_STRING usFileName = { NULL };
//
//							RtlInitUnicodeString(&usFileName, VolumeGuidPath);
//
//							InitializeObjectAttributes(&FileAttrib, &usFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, 0);
//
//							if (IoFastQueryNetworkAttributes(&FileAttrib, DELETE, 0, &IoStatus, &FileInfo)) {
//
//								if (NT_SUCCESS(IoStatus.Status) && IoStatus.Information != FILE_DOES_NOT_EXIST) {
//
//									Result = STATUS_SUCCESS;
//								}
//							}
//
//
//
//
//							if (NT_SUCCESS(Result)) {
//
//								// 生成新的GUID
//								GUID newGuid;
//								Result = ExUuidCreate(&newGuid);
//
//								WCHAR guidBuffer[40];
//								UNICODE_STRING guidString;
//								RtlInitEmptyUnicodeString(&guidString, guidBuffer, sizeof(guidBuffer));
//
//								// 将GUID结构体格式化为UNICODE字符串
//								Result = RtlUnicodeStringPrintf(
//									&guidString,
//									L"%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
//									newGuid.Data1, newGuid.Data2, newGuid.Data3,
//									newGuid.Data4[0], newGuid.Data4[1], newGuid.Data4[2], newGuid.Data4[3],
//									newGuid.Data4[4], newGuid.Data4[5], newGuid.Data4[6], newGuid.Data4[7]
//								);
//
//
//
//								HANDLE hFile = NULL;
//
//								OBJECT_ATTRIBUTES ObjectAttributes = { NULL };
//
//								IO_STATUS_BLOCK IoStatusBlock = { NULL };
//
//								UNICODE_STRING usFileName = { NULL };
//
//								RtlInitUnicodeString(&usFileName, VolumeGuidPath);
//
//								InitializeObjectAttributes(&ObjectAttributes, &usFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
//
//								Result = ZwCreateFile(&hFile, GENERIC_ALL, &ObjectAttributes, &IoStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
//
//								if (NT_SUCCESS(Result)) {
//
//									LARGE_INTEGER liFileOff = { NULL };
//
//									Result = ZwWriteFile(hFile, NULL, NULL, NULL, &IoStatusBlock, pSpoofBuffer, (ULONG)strlen(pSpoofBuffer), &liFileOff, NULL);
//
//									ZwClose(hFile);
//								}
//
//
//
//
//								Enable = TRUE;
//							}
//						}
//					}
//
//					for (SIZE_T Index = NULL; Index < ARRAYSIZE(VolumeIndex); Index++) {
//
//						WCHAR VolumeGuidPath[260] = { NULL };
//
//
//						Result = RtlStringCbPrintfW(VolumeGuidPath, sizeof(VolumeGuidPath), L"\\??\\%ws:\\System Volume Information\\WPSSettings.data", VolumeIndex[Index]);
//
//						if (NT_SUCCESS(Result)) {
//
//							IO_STATUS_BLOCK IoStatus = { NULL };
//
//							OBJECT_ATTRIBUTES FileAttrib = { NULL };
//
//							FILE_NETWORK_OPEN_INFORMATION FileInfo = { NULL };
//
//							UNICODE_STRING usFileName = { NULL };
//
//							RtlInitUnicodeString(&usFileName, VolumeGuidPath);
//
//							InitializeObjectAttributes(&FileAttrib, &usFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, 0);
//
//							if (IoFastQueryNetworkAttributes(&FileAttrib, DELETE, 0, &IoStatus, &FileInfo)) {
//
//								if (NT_SUCCESS(IoStatus.Status) && IoStatus.Information != FILE_DOES_NOT_EXIST) {
//
//									Result = STATUS_SUCCESS;
//								}
//							}
//
//
//
//							if (NT_SUCCESS(Result)) {
//
//
//
//								// 生成新的GUID
//								GUID newGuid;
//								Result = ExUuidCreate(&newGuid);
//
//								WCHAR guidBuffer[40];
//								UNICODE_STRING guidString;
//								RtlInitEmptyUnicodeString(&guidString, guidBuffer, sizeof(guidBuffer));
//
//								// 将GUID结构体格式化为UNICODE字符串
//								Result = RtlUnicodeStringPrintf(
//									&guidString,
//									L"%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
//									newGuid.Data1, newGuid.Data2, newGuid.Data3,
//									newGuid.Data4[0], newGuid.Data4[1], newGuid.Data4[2], newGuid.Data4[3],
//									newGuid.Data4[4], newGuid.Data4[5], newGuid.Data4[6], newGuid.Data4[7]
//								);
//
//
//
//								HANDLE hFile = NULL;
//
//								OBJECT_ATTRIBUTES ObjectAttributes = { NULL };
//
//								IO_STATUS_BLOCK IoStatusBlock = { NULL };
//
//								UNICODE_STRING usFileName = { NULL };
//
//								RtlInitUnicodeString(&usFileName, VolumeGuidPath);
//
//								InitializeObjectAttributes(&ObjectAttributes, &usFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
//
//								Result = ZwCreateFile(&hFile, GENERIC_ALL, &ObjectAttributes, &IoStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
//
//								if (NT_SUCCESS(Result)) {
//
//									LARGE_INTEGER liFileOff = { NULL };
//
//									Result = ZwWriteFile(hFile, NULL, NULL, NULL, &IoStatusBlock, pSpoofBuffer, (ULONG)strlen(pSpoofBuffer), &liFileOff, NULL);
//
//									ZwClose(hFile);
//								}
//
//
//
//
//								Enable = TRUE;
//
//
//								Enable = TRUE;
//							}
//						}
//					}
//
//					ExFreePoolWithTag(pSpoofBuffer, 'LS');
//				}
//			}
//
//			if (Enable != FALSE) {
//
//				Result = STATUS_SUCCESS;
//			}
//
//			return Result;
//		}
//
//	}
//
//
//
//}
//
