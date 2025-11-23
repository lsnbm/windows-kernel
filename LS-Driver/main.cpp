#pragma once  // 防止头文件重复包含
#include<ntifs.h>
#include <ntddmou.h>   // 鼠标 结构体 MOUSE_INPUT_DATA
#include <ntddkbd.h>   // 键盘 结构体KEYBOARD_INPUT_DATA
#include<ntstrsafe.h>
#include"Exporting Functions.hpp"
#include"Driver hiding.h"
#include "DirectPhysicalMemory.h"
#include"Simulate keyboard and mouse.h"
#include"Vad.h"

ULONG32 TargetProcessId = 0;

static  PEPROCESS TarGetprocess;
static  ULONG_PTR TarGetprocess_dirbase;




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


// 全局变量，用于保存MDL和映射后的地址
PMDL        g_pMdl = NULL;
PVOID       g_pKernelAddress = NULL;
HANDLE      g_hThread = NULL;             // 内核线程的句柄
HANDLE      g_hThread2 = NULL;             // 内核线程的句柄
volatile bool  PerocessExit = 1;			//用户线程默认退出状态  
 
Requests* req;                      // 指向共享请求对象的指针



VOID DriverMainLoop(PVOID StartContext) {
	UNREFERENCED_PARAMETER(StartContext);


	KdPrint(("[+] 派遣线程处理器索引: %d\n", KeGetCurrentProcessorIndex())); // 打印处理器索引

	//只要不是退出就不等待
	while (1) {
		//请求进程不是退出状态才可以执行
		if (!PerocessExit) {

			// 使用原子操作检查`Ready`字段是否为1。如果是，表示有新请求，并将其设置为0。
			if (InterlockedCompareExchange(&req->kernel, 0, 1) == 1) {
				InterlockedExchange(&req->kernel, 0);//请求已处理

				//如果目标进程更换了，重新获取
				if (TargetProcessId != req->TargetProcessId && req->TargetProcessId != 0) {

					TargetProcessId = req->TargetProcessId;
					KdPrint(("目标进程更换:%d\n", TargetProcessId));

					TarGetprocess = EPROCESS_MEMBER_OFFSET::FindEprocessByProcessId((HANDLE)TargetProcessId);
					KdPrint(("目标进程PEPROCESS:%p\n", TarGetprocess));

					TarGetprocess_dirbase = getprocessdirbase(TarGetprocess);
					KdPrint(("目标进程页目录基地址:%p\n", TarGetprocess_dirbase));
				}

				switch (req->Op) {
				case op_r: {

					//只能处理64位地址
					req->status = ReadPhysMemory(&g_TransferPage, TarGetprocess_dirbase, req->TargetAddress, req->UserBufferAddress, req->TransferSize, &rwCtx);
					break;
				}
				case op_w: {
					req->status = WritePhysMemory(&g_TransferPage, TarGetprocess_dirbase, req->TargetAddress, req->UserBufferAddress, req->TransferSize, &rwCtx);
					break;
				}
				case op_m: {

					if (SystemVersionInf.dwBuildNumber>=WINDOWS_24H2) {


						KdPrint(("ModuleName=%s\n", req->ModuleName));


						WIN11_24H2::GetModuleBaseByVad_NoAttach(TarGetprocess, req->ModuleName, &req->ModuleBaseAddress, &req->ModuleSize);

					}
					else {
						WIN11_23H2::GetModuleBaseByVad_NoAttach(TarGetprocess, req->ModuleName, &req->ModuleBaseAddress, &req->ModuleSize);
					}
	
					break;
				}
				case op_j: {
					KeyboardSimulation(&req->KeyboardData);
					break;
				}
				case op_s: {
					MouseSimulation(&req->MouseData);
					break;
				}
				case op_z: {

					// 根据PID查找EPROCESS结构
					PEPROCESS process = EPROCESS_MEMBER_OFFSET::FindEprocessByProcessId((HANDLE)TargetProcessId);;
					//必须增加引用计数因为，下面代码终止进程会释放引用计数，如果过度释放变-1会蓝屏
					ObReferenceObjectSafe(process);
					HANDLE processHandle = NULL;
					// 获取进程的内核句柄，以便使用ZwTerminateProcess
					req->status = L_ObOpenObjectByPointer(
						process,
						OBJ_KERNEL_HANDLE, // 指定创建的是内核句柄
						NULL,
						STANDARD_RIGHTS_ALL, // 所有访问权限
						*PsProcessType,
						KernelMode,
						&processHandle
					);

					if (!NT_SUCCESS(req->status)) {
						KdPrint(("Failed to get process handle. Status: %X\n", req->status));
						ObDereferenceObject(process);
						break;
					}

					// 使用获取到的句柄终止进程
					req->status = ZwTerminateProcess(processHandle, 0); // 0 表示成功的退出码

					// 不论终止是否成功，我们都需要关闭句柄并释放对process对象的引用
					L_ObCloseHandle(processHandle, KernelMode);
					ObDereferenceObject(process);

					if (!NT_SUCCESS(req->status)) {
						KdPrint(("Failed to terminate process. Status: %X\n", req->status));
					}
					else {
						KdPrint(("Process terminated successfully.\n"));
					}


					break;
				}
				case exit: {
					KdPrint(("开始清理旧进程资源\n"));

					// 清理共享内存映射和Mdl
					if (g_pKernelAddress && g_pMdl)
					{
						MmUnmapLockedPages(g_pKernelAddress, g_pMdl);
						g_pKernelAddress = NULL;
						IoFreeMdl(g_pMdl);
						g_pMdl = NULL;
					}

					PerocessExit = TRUE;//进程退出状态为真
					break;


				}
				default:break;
				}
				InterlockedExchange(&req->user, 1);//通知用户处理完成
			}

		}
		YieldProcessor();//自旋等

	}
}



//共享内存通信
VOID InitializeAndMapCommunication(PVOID)
{
	KdPrint(("[+] 连接线程处理器索引: %d\n", KeGetCurrentProcessorIndex())); // 打印处理器索引

	//固定3秒等待
	for (;;)
	{
		//请求进程退出，开始查找
		if (PerocessExit) {

			ULONG pid = EPROCESS_MEMBER_OFFSET::FindEprocessByProcessName("Lark.exe");
			KdPrint(("请求进程PID:%d\n", pid));
			if (pid)
			{
				PEPROCESS eproc = EPROCESS_MEMBER_OFFSET::FindEprocessByProcessId((HANDLE)pid);
				KdPrint(("请求进程PEPROCESS:%p\n", eproc));
				if (eproc) {
					ULONG64 dirbase = getprocessdirbase(eproc);
					KdPrint(("请求进程页目录基地址:%llx\n", dirbase));
					if (dirbase) {

						//由用户分配固定虚拟内存
						ULONG64 buffer = 0x20258270000;
						NTSTATUS status;

						// 为了清晰，修改了此处的描述
						KdPrint(("读取成功: 进程全局共享内存指针指向的地址是: %llx\n", buffer));

						// --- [开始MDL映射流程] ---
						const SIZE_T Size = sizeof(Requests); // 假设 'Requests' 是一个已定义的结构体或类型
						KdPrint(("[MDL] 准备为大小为 %zu 字节的共享内存创建MDL。\n", Size));
						KdPrint(("[MDL] 共享内存的起始虚拟地址为: %llx\n", buffer));

						if (buffer && Size) {
							KdPrint(("[MDL] 地址和大小有效，开始分配MDL...\n"));
							PMDL mdl = IoAllocateMdl(NULL, (ULONG)Size, FALSE, FALSE, NULL);
							if (mdl) {
								KdPrint(("[MDL] IoAllocateMdl 成功, MDL at: %p\n", mdl));

								// 设置MDL字段
								mdl->StartVa = (PVOID)(buffer & ~(PAGE_SIZE - 1));
								mdl->ByteOffset = (USHORT)(buffer & (PAGE_SIZE - 1));
								mdl->ByteCount = (ULONG)Size;
								KdPrint(("[MDL] MDL 设置: StartVa=%p, ByteOffset=%u, ByteCount=%u\n", mdl->StartVa, mdl->ByteOffset, mdl->ByteCount));

								PPFN_NUMBER pfns = (PPFN_NUMBER)(mdl + 1);
								ULONG pageCount = (ULONG)ADDRESS_AND_SIZE_TO_SPAN_PAGES(buffer, Size);
								KdPrint(("[MDL] 内存跨越 %u 个物理页，开始手动转换PFN...\n", pageCount));

								BOOLEAN translationSuccess = TRUE;
								for (ULONG i = 0; i < pageCount; i++) {
									ULONG_PTR currentVa = buffer + (i * PAGE_SIZE);
									ULONG_PTR pa = 0; //转换后的物理地址
									ULONG_PTR currentPageSize = 0; //返回值物理页有多大

									KdPrint(("[MDL] [Page %u/%u] 正在转换虚拟地址: %llx\n", i + 1, pageCount, currentVa));

									GetPhysPageInfoStealth(&g_TransferPage, dirbase, (void*)currentVa, &pa, &currentPageSize, &rwCtx);

									if (!pa) {
										KdPrint(("[MDL] !! 错误: [Page %u/%u] 虚拟地址 %llx 转换为物理地址失败!\n", i + 1, pageCount, currentVa));
										translationSuccess = FALSE;
										break;
									}

									pfns[i] = (PFN_NUMBER)(pa >> PAGE_SHIFT);
									KdPrint(("[MDL] [Page %u/%u] 转换成功: VA=%llx -> PA=%llx (PFN=%llx)\n", i + 1, pageCount, currentVa, pa, pfns[i]));
								}

								if (translationSuccess) {
									KdPrint(("[MDL] 所有页面的PFN转换成功。准备映射MDL到内核空间...\n"));
									PVOID kernelAddress = NULL;
									NTSTATUS mapStatus = STATUS_SUCCESS;
									__try {
										kernelAddress = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority);
									}
									__except (EXCEPTION_EXECUTE_HANDLER) {
										mapStatus = GetExceptionCode();
										KdPrint(("[MDL] !! 严重错误: MmMapLockedPagesSpecifyCache 触发异常，代码: 0x%X\n", mapStatus));
									}

									if (NT_SUCCESS(mapStatus) && kernelAddress) {
										KdPrint(("[MDL] MmMapLockedPagesSpecifyCache 成功! 内核映射地址: %p\n", kernelAddress));
										g_pMdl = mdl;
										g_pKernelAddress = kernelAddress;
										req = (Requests*)kernelAddress;
										status = STATUS_SUCCESS;
										KdPrint(("[MDL] 共享内存映射设置完毕，通信已准备就绪。\n"));


										req->status = 0x1344D60;
										PerocessExit = FALSE;//进程退出状态为假
										InterlockedExchange(&req->user, 1);//通知用户处理连接完成
									}
									else {
										KdPrint(("[MDL] !! 错误: MmMapLockedPagesSpecifyCache 失败。Status: 0x%X, KernelAddress: %p。正在释放MDL。\n", mapStatus, kernelAddress));
										IoFreeMdl(mdl);
										status = (NT_SUCCESS(mapStatus)) ? STATUS_INSUFFICIENT_RESOURCES : mapStatus;
									}
								}
								else {
									KdPrint(("[MDL] !! 错误: PFN转换过程中发生错误，正在释放MDL。\n"));
									IoFreeMdl(mdl);
									status = STATUS_UNSUCCESSFUL;
								}
							}
							else {
								KdPrint(("[MDL] !! 错误: IoAllocateMdl 失败，内存不足。\n"));
								status = STATUS_INSUFFICIENT_RESOURCES;
							}
						}
						else {
							KdPrint(("[MDL] !! 错误: 共享内存地址为NULL或大小为0，无法创建MDL。Buffer=%llx, Size=%zu\n", buffer, Size));
							status = STATUS_INVALID_PARAMETER;
						}



					}

				}
			}


		}



		LARGE_INTEGER interval;
		interval.QuadPart = -(LONGLONG)3 * 1000 * 10000; // -3 秒
		KeDelayExecutionThread(KernelMode, FALSE, &interval);
	}
}




extern "C" NTSTATUS DriverEntry(
	PDRIVER_OBJECT  DriverObject,/*加载驱动会实例化这个驱动对象：用来描述驱动结构*/
	PUNICODE_STRING RegistryPath/*驱动注册表路径*/
)
{
	KdPrint(("驱动入口内存=%llx \n", DriverObject->DriverStart));
	KdPrint(("注册表路径=%wZ \n", RegistryPath));
	KdPrint(("PerocessExit=%llx \n", &PerocessExit));

	//初始化所有导出函数指针
	initFun();

	//////驱动隐藏
	L_IoRegisterDriverReinitialization(DriverObject, &Reinitialize, NULL);

	//初始化物理传输中转页
	AllocatePhysicalPage(&g_TransferPage);

	//初始化键鼠
	InitMouseAndKeyboard();

	//初始化EPROSESS一系列成员偏移
	if (!EPROCESS_MEMBER_OFFSET::InitializeUniqueProcessIdOffset()) {
		KdPrint((("初始化 UniqueProcessId偏移失败\n")));
		return STATUS_UNSUCCESSFUL;
	}
	if (!EPROCESS_MEMBER_OFFSET::InitializeImageFileNameOffset()) {
		KdPrint((("初始化 processName偏移失败\n")));
		return STATUS_UNSUCCESSFUL;
	}
	if (!EPROCESS_MEMBER_OFFSET::InitializeActiveProcessLinksOffset()) {
		KdPrint((("初始化 ActiveProcessLinks偏移 失败\n")));
		return STATUS_UNSUCCESSFUL;
	}
	if (!EPROCESS_MEMBER_OFFSET::InitializePebOffset()) {
		KdPrint((("初始化 Peb 偏移 失败\n")));
		return STATUS_UNSUCCESSFUL;
	}
	if (!EPROCESS_MEMBER_OFFSET::InitializeSectionBaseAddressOffset()) {
		KdPrint((("初始化 SectionBaseAddress 偏移 失败\n")));
		return STATUS_UNSUCCESSFUL;
	}
	if (!EPROCESS_MEMBER_OFFSET::InitializeVAD_ROOTOffset()) {
		KdPrint((("初始化 VAD_ROOTO 偏移 失败\n")));
		return STATUS_UNSUCCESSFUL;
	}
	
	//初始化共享内存通信线程
	PsCreateSystemThread(
		&g_hThread,              // 返回的线程句柄
		THREAD_ALL_ACCESS,       // 访问权限
		NULL,                    // 对象属性
		NULL,                    // 进程句柄 (NULL = 系统进程)
		NULL,                    // ClientId
		InitializeAndMapCommunication,          // 线程的起始函数
		NULL                     // 传递给线程的上下文参数
	);

	//创建派遣线程
	PsCreateSystemThread(
		&g_hThread2,              // 返回的线程句柄
		THREAD_ALL_ACCESS,       // 访问权限
		NULL,                    // 对象属性
		NULL,                    // 进程句柄 (NULL = 系统进程)
		NULL,                    // ClientId
		DriverMainLoop,          // 线程的起始函数
		NULL                     // 传递给线程的上下文参数
	);

	//注册表清理
	L_RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, RegistryPath->Buffer, OBFUSCATE(L"DisplayName").decrypt());
	L_RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, RegistryPath->Buffer, OBFUSCATE(L"ErrorControl").decrypt());
	L_RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, RegistryPath->Buffer, OBFUSCATE(L"ImagePath").decrypt());
	L_RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, RegistryPath->Buffer, OBFUSCATE(L"Start").decrypt());
	L_RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, RegistryPath->Buffer, OBFUSCATE(L"Type").decrypt());
	L_RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, RegistryPath->Buffer, OBFUSCATE(L"WOW64").decrypt());

	// 删除 Enum 子键
	UNICODE_STRING enumKeyPath;
	WCHAR enumKeyPathBuffer[260];
	//处理最内层的 Enum 子键
	RtlInitEmptyUnicodeString(&enumKeyPath, enumKeyPathBuffer, sizeof(enumKeyPathBuffer));
	if (NT_SUCCESS(RtlUnicodeStringCopy(&enumKeyPath, RegistryPath))) {
		UNICODE_STRING suffixString;
		L_RtlInitUnicodeString(&suffixString, OBFUSCATE(L"\\Enum").decrypt());

		// 如果路径构建成功，则先删除其内部的值
		if (NT_SUCCESS(RtlUnicodeStringCat(&enumKeyPath, &suffixString))) {
			L_RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, enumKeyPath.Buffer, OBFUSCATE(L"Count").decrypt());
			L_RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, enumKeyPath.Buffer, OBFUSCATE(L"NextInstance").decrypt());
			L_RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, enumKeyPath.Buffer, OBFUSCATE(L"INITSTARTFAILED").decrypt());

			//  然后删除 Enum 键本身
			HANDLE hEnumKey;
			OBJECT_ATTRIBUTES enumObjAttr;
			InitializeObjectAttributes(&enumObjAttr, &enumKeyPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
			if (NT_SUCCESS(ZwOpenKey(&hEnumKey, DELETE, &enumObjAttr))) {
				ZwDeleteKey(hEnumKey);
				ZwClose(hEnumKey);
			}
		}
	}

	// 处理外层的主服务键
	L_RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, RegistryPath->Buffer, OBFUSCATE(L"DisplayName").decrypt());
	L_RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, RegistryPath->Buffer, OBFUSCATE(L"ErrorControl").decrypt());
	L_RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, RegistryPath->Buffer, OBFUSCATE(L"ImagePath").decrypt());
	L_RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, RegistryPath->Buffer, OBFUSCATE(L"Start").decrypt());
	L_RtlDeleteRegistryValue(RTL_REGISTRY_ABSOLUTE, RegistryPath->Buffer, OBFUSCATE(L"Type").decrypt());

	//最后删除主服务键本身
	HANDLE hServiceKey;
	OBJECT_ATTRIBUTES serviceObjAttr;
	InitializeObjectAttributes(&serviceObjAttr, RegistryPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	if (NT_SUCCESS(ZwOpenKey(&hServiceKey, DELETE, &serviceObjAttr))) {
		ZwDeleteKey(hServiceKey);
		ZwClose(hServiceKey);
	}

	// 删除加载驱动服务项
	NTSTATUS status;
	UNICODE_STRING serviceName;


	L_RtlInitUnicodeString(&serviceName, OBFUSCATE(L"ataport").decrypt());

	KdPrint(("正在卸载驱动并尝试删除服务项 '%wZ'。\n", &serviceName));

	{
	
		HANDLE hServicesKey = NULL;
		HANDLE hTargetServiceKey = NULL;
		UNICODE_STRING servicesPath;
		OBJECT_ATTRIBUTES objAttributes;

		//  构造 Services 注册表项的内核路径
		L_RtlInitUnicodeString(&servicesPath, L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services");

		// 初始化对象属性结构体，并打开 Services 键
		InitializeObjectAttributes(&objAttributes,
			&servicesPath,
			OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
			NULL,
			NULL);

		status = ZwOpenKey(&hServicesKey, KEY_ALL_ACCESS, &objAttributes);
		if (!NT_SUCCESS(status))
		{
			KdPrint(("打开 Services 注册表项失败，状态码: 0x%X\n", status));
		}
		else
		{
			// 基于 Services 键的句柄，打开要删除的目标服务键
			InitializeObjectAttributes(&objAttributes,
				&serviceName, // 使用上面定义的 serviceName 变量
				OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
				hServicesKey, // 此处指定父项句柄
				NULL);

			// 请求 DELETE 权限
			status = ZwOpenKey(&hTargetServiceKey, DELETE, &objAttributes);
			if (!NT_SUCCESS(status))
			{
				KdPrint(("打开服务项 '%wZ' 失败，状态码: 0x%X\n", &serviceName, status));
				ZwClose(hServicesKey);
			}
			else
			{
				// 执行删除操作
				status = ZwDeleteKey(hTargetServiceKey);
				if (!NT_SUCCESS(status))
				{
					KdPrint(("删除服务项 '%wZ' 失败，状态码: 0x%X\n", &serviceName, status));
				}

				// 关闭所有打开的句柄，释放资源
				ZwClose(hTargetServiceKey);
				ZwClose(hServicesKey);
			}
		}
	}

	if (NT_SUCCESS(status))
	{
		KdPrint(("服务项已成功删除。\n"));
	}
	else
	{
		KdPrint(("删除服务项失败(由于服务句柄存在重启自动删除)，状态码: 0x%X\n", status));
	}


	return STATUS_UNSUCCESSFUL;
}















































//{


	//	//////---------------------------------------------------------------------------
	//	//////蓝屏是因为上面修改机器码的驱动bios和NIC函数变量偏移不对，系统定制
	//	//////---------------------------------------------------------------------------

	//	//if (SystemVersionInf.dwBuildNumber == WINDOWS_24H2) {

	//	//	if (NT_SUCCESS(WIN11_24H2::SpoofREG::SpoofIdentifiers())) {
	//	//		KdPrint(("[+] Successfully deceived the GUID\n"));
	//	//	}

	//	//	if (NT_SUCCESS(WIN11_24H2::SpoofHDD::SpoofHDD())) {
	//	//		KdPrint(("[+] Successfully deceived the HDD\n"));
	//	//	}

	//	//	if (NT_SUCCESS(WIN11_24H2::SpoofSMBIOS::SpoofSMBIOS())) {
	//	//		KdPrint(("[+] Successfully deceived the SMBIOS\n"));
	//	//	}


	//	//	if (NT_SUCCESS(WIN11_24H2::SpoofNIC::SpoofNIC())) {
	//	//		KdPrint(("[+] Successfully deceived the NIC\n"));
	//	//	}

	//	//	//////蓝屏
	//	////	//if (NT_SUCCESS(WIN11_24H2::SpoofGPU::SpoofGPU())) {
	//	////	//	KdPrint(("[+] Successfully deceived the GPU\n"));
	//	////	//}

	//	//	/////挂钩
	//	////if (NT_SUCCESS(WIN11_24H2::SpoofPART::SpoofPART())) {
	//	////		KdPrint(("[+] Successfully deceived the PART\n"));
	//	////}


	//	//	//if (NT_SUCCESS(WIN11_24H2::SpoofVOLUMES::SpoofVOLUMES())) {
	//	//		//KdPrint(("[+] Successfully deceived the VOLUMES\n"));
	//	//	//}



	//	//}
	//	//else {

	//	//	if (NT_SUCCESS(WIN11_23H2::SpoofREG::SpoofIdentifiers())) {
	//	//		KdPrint(("[+] Successfully deceived the GUID\n"));
	//	//	}

	//	//	if (NT_SUCCESS(WIN11_23H2::SpoofHDD::SpoofHDD())) {
	//	//		KdPrint(("[+] Successfully deceived the HDD\n"));
	//	//	}


	//	//	if (NT_SUCCESS(WIN11_23H2::SpoofSMBIOS::SpoofSMBIOS1())) {
	//	//		KdPrint(("[+] Successfully deceived the SMBIOS (Method 1)\n"));
	//	//	}
	//	//	if (NT_SUCCESS(WIN11_23H2::SpoofSMBIOS::SpoofSMBIOS2())) {
	//	//		KdPrint(("[+] Successfully deceived the SMBIOS (Method 2)\n"));
	//	//	}


	//	//	if (NT_SUCCESS(WIN11_23H2::SpoofNIC::SpoofNIC())) {
	//	//		KdPrint(("[+] Successfully deceived the NIC\n"));
	//	//	}


	//	//	//if (NT_SUCCESS(WIN11_23H2::SpoofGPU::SpoofGPU())) {
	//	//	//	KdPrint(("[+] Successfully deceived the GPU\n"));
	//	//	//}


	//	//	//if (NT_SUCCESS(WIN11_23H2::SpoofPART::SpoofPART())) {
	//	//	//	KdPrint(("[+] Successfully deceived the PART\n"));
	//	//	//}


	//	//	//if (NT_SUCCESS(WIN11_23H2::SpoofVOLUMES::SpoofVOLUMES())) {
	//	//		//KdPrint(("[+] Successfully deceived the VOLUMES FILE\n"));
	//	//	//}


	//	//}

	//	//// 在所有伪造操作完成后，杀掉 WMI 服务进程以可能应用某些更改
	//	//Utils::ZwKillProcess(L"WmiPrvSE.exe");


	//}

