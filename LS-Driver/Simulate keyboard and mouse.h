//找到键鼠驱动的回调函数地址和设备对象地址，并调用实现模拟

#pragma once
#include<ntifs.h>
#include <ntddmou.h>   // 鼠标 结构体 MOUSE_INPUT_DATA
#include <ntddkbd.h>   // 键盘 结构体KEYBOARD_INPUT_DATA

extern "C"  POBJECT_TYPE* IoDriverObjectType;


typedef VOID(*MY_KEYBOARDCALLBACK) (PDEVICE_OBJECT, PKEYBOARD_INPUT_DATA, PKEYBOARD_INPUT_DATA, PULONG);

typedef VOID(*MY_MOUSECALLBACK) (PDEVICE_OBJECT, PMOUSE_INPUT_DATA, PMOUSE_INPUT_DATA, PULONG);


inline PDEVICE_OBJECT MouseDeviceObject = NULL;

inline PDEVICE_OBJECT KeyboardDeviceObject = NULL;

inline MY_MOUSECALLBACK MouseClassServiceCallback = NULL;

inline MY_KEYBOARDCALLBACK KeyboardClassServiceCallback = NULL;



inline NTSTATUS ZwReferenceObjectByName(PUNICODE_STRING ObjectName, ULONG Attributes, PACCESS_STATE PassedAccessState, ACCESS_MASK DesiredAccess, POBJECT_TYPE ObjectType, KPROCESSOR_MODE AccessMode, PVOID ParseContext, PDRIVER_OBJECT* Object) {

	typedef NTSTATUS(NTAPI* fn_ObReferenceObjectByName)(PUNICODE_STRING, ULONG, PACCESS_STATE, ACCESS_MASK, POBJECT_TYPE, KPROCESSOR_MODE, PVOID, PDRIVER_OBJECT*);

	static fn_ObReferenceObjectByName _ObReferenceObjectByName = NULL;

	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	if (!_ObReferenceObjectByName || !IoDriverObjectType) {
		UNICODE_STRING RoutineName;

		RtlInitUnicodeString(&RoutineName, L"ObReferenceObjectByName");

		_ObReferenceObjectByName = (fn_ObReferenceObjectByName)(MmGetSystemRoutineAddress(&RoutineName));
	}

	if (_ObReferenceObjectByName && IoDriverObjectType) {

		Status = _ObReferenceObjectByName(ObjectName, Attributes, PassedAccessState, DesiredAccess, ObjectType, AccessMode, ParseContext, Object);
	}

	return Status;
}

inline NTSTATUS SearchServiceFromMouExt(PDRIVER_OBJECT MouDriverObject, PDEVICE_OBJECT pPortDev) {

	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	PDEVICE_OBJECT pTargetDeviceObject = NULL;

	UCHAR* DeviceExt = NULL;

	PVOID KbdDriverStart = NULL;

	ULONG KbdDriverSize = NULL;

	PDEVICE_OBJECT pTmpDev = NULL;

	UNICODE_STRING kbdDriName = { NULL };

	KbdDriverStart = MouDriverObject->DriverStart;

	KbdDriverSize = MouDriverObject->DriverSize;

	RtlInitUnicodeString(&kbdDriName, L"\\Driver\\mouclass");

	pTmpDev = pPortDev;

	while (pTmpDev->AttachedDevice != NULL) {

		if (RtlCompareUnicodeString(&pTmpDev->AttachedDevice->DriverObject->DriverName, &kbdDriName, TRUE)) {

			pTmpDev = pTmpDev->AttachedDevice;
		}
		else
			break;
	}

	if (pTmpDev->AttachedDevice != NULL) {

		pTargetDeviceObject = MouDriverObject->DeviceObject;

		while (pTargetDeviceObject) {

			if (pTmpDev->AttachedDevice != pTargetDeviceObject) {

				pTargetDeviceObject = pTargetDeviceObject->NextDevice;

				continue;
			}

			DeviceExt = (UCHAR*)pTmpDev->DeviceExtension;

			MouseDeviceObject = NULL;

			for (ULONG i = NULL; i < PAGE_SIZE; i++, DeviceExt++) {

				if (MmIsAddressValid(DeviceExt)) {

					PVOID pTemp = *(PVOID*)DeviceExt;

					if (MouseDeviceObject && MouseClassServiceCallback) {

						Status = STATUS_SUCCESS;

						break;
					}

					if (pTemp == pTargetDeviceObject) {

						MouseDeviceObject = pTargetDeviceObject;

						continue;
					}

					if (pTemp > KbdDriverStart && pTemp < (PVOID)((UCHAR*)KbdDriverStart + KbdDriverSize) && MmIsAddressValid(pTemp)) {

						MouseClassServiceCallback = (MY_MOUSECALLBACK)pTemp;

						Status = STATUS_SUCCESS;
					}
				}
				else
					break;
			}

			if (Status == STATUS_SUCCESS) {

				break;
			}

			pTargetDeviceObject = pTargetDeviceObject->NextDevice;
		}
	}

	return Status;
}
inline NTSTATUS SearchServiceFromKdbExt(PDRIVER_OBJECT KbdDriverObject, PDEVICE_OBJECT pPortDev) {

	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	PDEVICE_OBJECT pTargetDeviceObject = NULL;

	UCHAR* DeviceExt = NULL;

	PVOID KbdDriverStart = NULL;

	ULONG KbdDriverSize = NULL;

	PDEVICE_OBJECT pTmpDev = NULL;

	UNICODE_STRING kbdDriName = { NULL };

	KbdDriverStart = KbdDriverObject->DriverStart;

	KbdDriverSize = KbdDriverObject->DriverSize;

	RtlInitUnicodeString(&kbdDriName, L"\\Driver\\kbdclass");

	pTmpDev = pPortDev;

	while (pTmpDev->AttachedDevice != NULL) {

		if (RtlCompareUnicodeString(&pTmpDev->AttachedDevice->DriverObject->DriverName, &kbdDriName, TRUE)) {

			pTmpDev = pTmpDev->AttachedDevice;
		}
		else
			break;
	}

	if (pTmpDev->AttachedDevice != NULL) {

		pTargetDeviceObject = KbdDriverObject->DeviceObject;

		while (pTargetDeviceObject) {

			if (pTmpDev->AttachedDevice != pTargetDeviceObject) {

				pTargetDeviceObject = pTargetDeviceObject->NextDevice;

				continue;
			}

			DeviceExt = (UCHAR*)pTmpDev->DeviceExtension;

			KeyboardDeviceObject = NULL;

			for (ULONG i = NULL; i < PAGE_SIZE; i++, DeviceExt++) {

				if (MmIsAddressValid(DeviceExt)) {

					PVOID pTemp = *(PVOID*)DeviceExt;

					if (KeyboardDeviceObject && KeyboardClassServiceCallback) {

						Status = STATUS_SUCCESS;

						break;
					}

					if (pTemp == pTargetDeviceObject) {

						KeyboardDeviceObject = pTargetDeviceObject;

						continue;
					}

					if (pTemp > KbdDriverStart && pTemp < (PVOID)((UCHAR*)KbdDriverStart + KbdDriverSize) && MmIsAddressValid(pTemp)) {

						KeyboardClassServiceCallback = (MY_KEYBOARDCALLBACK)pTemp;
					}
				}
				else
					break;
			}

			if (Status == STATUS_SUCCESS) {

				break;
			}

			pTargetDeviceObject = pTargetDeviceObject->NextDevice;
		}
	}

	return Status;
}
inline NTSTATUS SearchMouServiceCallBack() {

	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	PDRIVER_OBJECT ClassObject = NULL;

	PDRIVER_OBJECT DriverObject = NULL;

	PDEVICE_OBJECT DeviceObject = NULL;

	UNICODE_STRING DeviceName[] = { RTL_CONSTANT_STRING(L"\\Driver\\mouhid"), RTL_CONSTANT_STRING(L"\\Driver\\i8042prt") };

	for (size_t i = NULL; i < ARRAYSIZE(DeviceName); i++) {

		Status = ZwReferenceObjectByName(&DeviceName[i], OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL, &DriverObject);

		if (NT_SUCCESS(Status)) {

			ObfDereferenceObject(DriverObject);

			break;
		}
	}

	if (DriverObject != NULL) {

		UNICODE_STRING ClassName = RTL_CONSTANT_STRING(L"\\Driver\\mouclass");

		Status = ZwReferenceObjectByName(&ClassName, OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL, &ClassObject);

		if (NT_SUCCESS(Status)) {

			DeviceObject = DriverObject->DeviceObject;

			while (DeviceObject) {

				Status = SearchServiceFromMouExt(ClassObject, DeviceObject);

				if (!NT_SUCCESS(Status)) {

					DeviceObject = DeviceObject->NextDevice;
				}
				else
					break;
			}

			ObfDereferenceObject(ClassObject);
		}
	}

	return Status;
}
inline NTSTATUS SearchKdbServiceCallBack() {

	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	PDRIVER_OBJECT ClassObject = NULL;

	PDRIVER_OBJECT DriverObject = NULL;

	PDEVICE_OBJECT DeviceObject = NULL;

	UNICODE_STRING DeviceName[] = { RTL_CONSTANT_STRING(L"\\Driver\\kbdhid"), RTL_CONSTANT_STRING(L"\\Driver\\i8042prt") };

	for (size_t i = NULL; i < ARRAYSIZE(DeviceName); i++) {

		Status = ZwReferenceObjectByName(&DeviceName[i], OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL, &DriverObject);

		if (NT_SUCCESS(Status)) {

			ObfDereferenceObject(DriverObject);

			break;
		}
	}

	if (DriverObject != NULL) {

		UNICODE_STRING ClassName = RTL_CONSTANT_STRING(L"\\Driver\\kbdclass");

		Status = ZwReferenceObjectByName(&ClassName, OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL, &ClassObject);

		if (NT_SUCCESS(Status)) {

			DeviceObject = DriverObject->DeviceObject;

			while (DeviceObject) {

				Status = SearchServiceFromKdbExt(ClassObject, DeviceObject);

				if (!NT_SUCCESS(Status)) {

					DeviceObject = DeviceObject->NextDevice;
				}
				else
					break;
			}

			ObfDereferenceObject(ClassObject);
		}
	}

	return Status;
}


// 初始化键鼠模拟模块
inline NTSTATUS InitMouseAndKeyboard()
{
	//鼠标设备和回调函数指针初始化
	if (MouseDeviceObject == NULL || MouseClassServiceCallback == NULL) {

		NTSTATUS mouStatus = SearchMouServiceCallBack();
		if (NT_SUCCESS(mouStatus))
		{
			KdPrint(("[+] Mouse simulation initialized successfully.\n"));
		}
		else
		{
			KdPrint(("[-] Failed to initialize mouse simulation. Status: 0x%X\n", mouStatus));
		}
	}
	//键盘设备和回调函数指针初始化
	if (KeyboardDeviceObject == NULL || KeyboardClassServiceCallback == NULL) {

		NTSTATUS kbdStatus = SearchKdbServiceCallBack();
		if (NT_SUCCESS(kbdStatus))
		{
			KdPrint(("[+] Keyboard simulation initialized successfully.\n"));
		}
		else
		{
			KdPrint(("[-] Failed to initialize keyboard simulation. Status: 0x%X\n", kbdStatus));
		}


	}

	return (MouseDeviceObject && MouseClassServiceCallback && KeyboardDeviceObject && KeyboardClassServiceCallback) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}




// 键盘模拟函数
inline void KeyboardSimulation(PKEYBOARD_INPUT_DATA Data)
{
	if (!KeyboardClassServiceCallback || !KeyboardDeviceObject || !Data) {
		return;
	}

	ULONG InputDataConsumed = 0;

	// [修正] 直接使用调用者传入的指针，而不是创建局部拷贝
	PKEYBOARD_INPUT_DATA KbdInputDataStart = Data;
	PKEYBOARD_INPUT_DATA KbdInputDataEnd = KbdInputDataStart + 1;

	// 调用回调函数
	KeyboardClassServiceCallback(KeyboardDeviceObject, KbdInputDataStart, KbdInputDataEnd, &InputDataConsumed);
}

// 鼠标模拟函数
inline void MouseSimulation(PMOUSE_INPUT_DATA Data)
{
	if (!MouseClassServiceCallback || !MouseDeviceObject || !Data) {
		return;
	}

	ULONG InputDataConsumed = 0;

	// [修正] 直接使用调用者传入的指针
	PMOUSE_INPUT_DATA MouseInputDataStart = Data;
	PMOUSE_INPUT_DATA MouseInputDataEnd = MouseInputDataStart + 1;

	// 调用回调函数
	MouseClassServiceCallback(MouseDeviceObject, MouseInputDataStart, MouseInputDataEnd, &InputDataConsumed);
}
