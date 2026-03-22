#pragma once

#include<ntifs.h>
#include "Exporting Functions.hpp"




/*
遍历 VAD 树获取模块基地址和大小的步骤：

1.获取模块基地址（dll）

	修结构:
	执行 dt nt!_MMVAD，确认 MMVAD 结构体的定义。
	执行 dt nt!_MMVAD_SHORT		确认 MMVAD_SHORT 结构体的定义。
	执行 dt nt!_MM_AVL_NODE		确认 MM_AVL_NODE 结构体的定义。 可能没有这个结构体，没关系。
	执行 dt nt!_SUBSECTION		确认 SUBSECTION 结构体的定义。
	执行 dt nt!_CONTROL_AREA	确认 CONTROL_AREA 结构体的定义。
	执行 dt nt!_SEGMENT			确认 SEGMENT 结构体的定义。
	执行 dt nt!_EX_FAST_REF		确认 EX_FAST_REF 结构体的定义。
	执行 dt nt!_EX_PUSH_LOCK	确认 EX_PUSH_LOCK 结构体的定义。


*/




// 全局范围的前向声明
struct _FILE_OBJECT;
struct _EPROCESS;
struct _MMPTE;
extern C_MmIsAddressValid L_MmIsAddressValid;
extern C_RtlCompareUnicodeString L_RtlCompareUnicodeString ;

extern C_RtlInitAnsiString L_RtlInitAnsiString;
extern C_RtlInitUnicodeString L_RtlInitUnicodeString ;

extern C_RtlAnsiStringToUnicodeString L_RtlAnsiStringToUnicodeString;

extern C_RtlSuffixUnicodeString L_RtlSuffixUnicodeString ;

extern C_RtlFreeUnicodeString L_RtlFreeUnicodeString;

extern C_ExAllocatePool2 L_ExAllocatePool2 ;


extern C_ExFreePoolWithTag L_ExFreePoolWithTag;

namespace WIN11_24H2 {




	typedef struct _RTL_BALANCED_NODE {
		union {
			struct _RTL_BALANCED_NODE* Children[2];
			struct { struct _RTL_BALANCED_NODE* Left; struct _RTL_BALANCED_NODE* Right; } s;
		};
		union {
			::UCHAR Red : 1;
			::UCHAR Balance : 2;
			::ULONG_PTR ParentValue;
		} u;
	} RTL_BALANCED_NODE, * PRTL_BALANCED_NODE;

	typedef struct _RTL_AVL_TREE {
		PRTL_BALANCED_NODE Root;
	} RTL_AVL_TREE, * PRTL_AVL_TREE;

	typedef union _EX_PUSH_LOCK {
		struct { unsigned __int64 Locked : 1; unsigned __int64 Waiting : 1; unsigned __int64 Waking : 1; unsigned __int64 MultipleShared : 1; unsigned __int64 Shared : 60; };
		::ULONG_PTR Value;
		::PVOID Ptr;
	} EX_PUSH_LOCK, * PEX_PUSH_LOCK;

	typedef union _EX_FAST_REF {
		::PVOID Object;
		struct { unsigned __int64 RefCnt : 4; unsigned __int64 PtrValue : 60; };
		::ULONG_PTR Value;
	} EX_FAST_REF, * PEX_FAST_REF;

	typedef struct _MMVAD_SHORT {
		union {
			RTL_BALANCED_NODE VadNode;
			struct { ::PVOID NextVad; ::PVOID ExtraCreateInfo; };
		};
		::ULONG StartingVpn;
		::ULONG EndingVpn;
		::UCHAR StartingVpnHigh;
		::UCHAR EndingVpnHigh;
		::UCHAR CommitChargeHigh;
		::UCHAR SpareNT64VadUChar;
		volatile ::LONG ReferenceCount;
		EX_PUSH_LOCK PushLock;
		::ULONG32 u_flags;
		::ULONG CommitCharge;
		::ULONG64 u5_flags;
	} MMVAD_SHORT, * PMMVAD_SHORT;

	typedef struct _SEGMENT {
		struct _CONTROL_AREA* ControlArea;
		::ULONG TotalNumberOfPtes;
		::ULONG SegmentFlags_placeholder;
		::ULONG64 NumberOfCommittedPages;
		::ULONG64 SizeOfSegment;
		union {
			struct _MMEXTEND_INFO* ExtendInfo;
			::PVOID BasedAddress;
		};
		EX_PUSH_LOCK SegmentLock;
		::ULONG64 u1_placeholder;
		::ULONG64 u2_placeholder;
		struct _MMPTE* PrototypePte;
	} SEGMENT, * PSEGMENT;

	typedef struct _CONTROL_AREA {
		struct  _SEGMENT* Segment;
		union {
			::LIST_ENTRY ListHead;
			::PVOID AweContext;
		};
		::ULONG64 NumberOfSectionReferences;
		::ULONG64 NumberOfPfnReferences;
		::ULONG64 NumberOfMappedViews;
		::ULONG64 NumberOfUserReferences;
		::ULONG u_flags;
		::ULONG u1_flags;
		EX_FAST_REF FilePointer;
		volatile ::LONG ControlAreaLock;
		::ULONG ModifiedWriteCount;
		::PVOID WaitList;
		union {
			struct { ::PVOID Lookaside; ::PVOID PowerDomain; } u2_struct;
			::ULONG64 u2_quadwords[2];
		} u2_placeholder;
		EX_PUSH_LOCK FileObjectLock;
		volatile ::ULONG64 LockedPages;
		::ULONG64 u3_flags;
	} CONTROL_AREA, * PCONTROL_AREA;

	typedef struct _SUBSECTION {
		struct _CONTROL_AREA* ControlArea;
		struct _MMPTE* SubsectionBase;
		struct _SUBSECTION* NextSubsection;
		union { RTL_AVL_TREE GlobalPerSessionHead; ::PVOID CreationWaitList; ::PVOID SessionDriverProtos; } u;
		::ULONG32 SubsectionFlags;
		::ULONG StartingSector;
		::ULONG NumberOfFullSectors;
		::ULONG PtesInSubsection;
		::ULONG u1_flags;
		::ULONG UnusedPtesAndFlags;
	} SUBSECTION, * PSUBSECTION;

	typedef struct _MMVAD {
		MMVAD_SHORT Core;
		::ULONG64 u2_flags;
		struct _SUBSECTION* Subsection;
		struct _MMPTE* FirstPrototypePte;
		struct _MMPTE* LastContiguousPte;
		::LIST_ENTRY ViewLinks;
		struct _EPROCESS* VadsProcess;
		::ULONG64 u4_flags_or_pAweInfo;
		struct _FILE_OBJECT* FileObject;
	} MMVAD, * PMMVAD;

	typedef struct _VAD_SEARCH_RESULT {
		::BOOLEAN Found;
		::ULONG_PTR BaseAddress;
		::ULONG_PTR ModuleSize;
	} VAD_SEARCH_RESULT, * PVAD_SEARCH_RESULT;



	//遍历 VAD
	inline VOID VadpTraverseTree(IN PRTL_BALANCED_NODE pNode, IN::PUNICODE_STRING pTargetModuleNameUnicode, IN PVAD_SEARCH_RESULT pResult) {
		if (!L_MmIsAddressValid(pNode) || pResult->Found) return;
		PMMVAD pVad = (PMMVAD)pNode;
		VadpTraverseTree(pVad->Core.VadNode.s.Left, pTargetModuleNameUnicode, pResult);
		if (pResult->Found) return;
		::PFILE_OBJECT pFile = NULL; ::PUNICODE_STRING pFullPath = NULL;
		if (pVad->FileObject != NULL) pFile = (::PFILE_OBJECT)pVad->FileObject;
		else if (pVad->Subsection != NULL && L_MmIsAddressValid(pVad->Subsection) && pVad->Subsection->ControlArea != NULL && L_MmIsAddressValid(pVad->Subsection->ControlArea)) {
			PCONTROL_AREA pCtrlArea = (PCONTROL_AREA)pVad->Subsection->ControlArea;
			::PVOID rawFilePointer = pCtrlArea->FilePointer.Object;
			if (rawFilePointer != NULL) pFile = (::PFILE_OBJECT)((::ULONG_PTR)rawFilePointer & ~((::ULONG_PTR)sizeof(::PVOID) - 1));
		}
		if (pFile != NULL && L_MmIsAddressValid(pFile)) {
			pFullPath = &pFile->FileName;
			if (pFullPath->Buffer != NULL && pFullPath->Length > 0 && L_MmIsAddressValid(pFullPath->Buffer)) {
				__try {
					::UNICODE_STRING currentFileName; RtlZeroMemory(&currentFileName, sizeof(::UNICODE_STRING));
					::USHORT i; for (i = (pFullPath->Length / sizeof(WCHAR)) - 1; i > 0; i--) { if (pFullPath->Buffer[i] == L'\\') break; }
					::USHORT nameStartIndex = (i > 0 && pFullPath->Buffer[i] == L'\\') ? i + 1 : 0; if (i == 0 && pFullPath->Buffer[0] == L'\\') nameStartIndex = 1;
					currentFileName.Buffer = &pFullPath->Buffer[nameStartIndex];
					currentFileName.Length = pFullPath->Length - (nameStartIndex * sizeof(WCHAR));
					currentFileName.MaximumLength = currentFileName.Length;




				// 计算当前VAD区域的基地址和大小
					::ULONG64 startingVpn = ((::ULONG64)pVad->Core.StartingVpnHigh << 32) | pVad->Core.StartingVpn;
					::ULONG64 endingVpn = ((::ULONG64)pVad->Core.EndingVpnHigh << 32) | pVad->Core.EndingVpn;
					::ULONG_PTR baseAddress = startingVpn << PAGE_SHIFT;
					::ULONG_PTR moduleSize = ((endingVpn + 1) - startingVpn) << PAGE_SHIFT;

					// 使用KdPrint输出每个模块的信息
					KdPrint(("[WIN11_24H2][VAD Traverse] Module: %wZ, Base: 0x%p, Size: 0x%p\n", &currentFileName, (PVOID)baseAddress, (PVOID)moduleSize));



					if (L_RtlCompareUnicodeString(&currentFileName, pTargetModuleNameUnicode, TRUE) == 0) {
						::ULONG64 startingVpn = ((::ULONG64)pVad->Core.StartingVpnHigh << 32) | pVad->Core.StartingVpn;
						::ULONG64 endingVpn = ((::ULONG64)pVad->Core.EndingVpnHigh << 32) | pVad->Core.EndingVpn;
						pResult->BaseAddress = startingVpn << PAGE_SHIFT;
						pResult->ModuleSize = ((endingVpn + 1) - startingVpn) << PAGE_SHIFT;
						pResult->Found = TRUE;
						return;
					}
				}
				__except (EXCEPTION_EXECUTE_HANDLER) {
					// 此处的异常通常是由于内存竞争或页换出导致，静默处理以保证驱动稳定性。
				}
			}
		}
		VadpTraverseTree(pVad->Core.VadNode.s.Right, pTargetModuleNameUnicode, pResult);
	}


	inline ::NTSTATUS GetModuleBaseByVad_NoAttach(IN::PEPROCESS pTargetProcess, IN const char* ModuleName, OUT::PULONG_PTR pBaseAddress, OUT::PULONG_PTR pModuleSize) {
		if (!pTargetProcess || !ModuleName || !pBaseAddress || !pModuleSize) {
			KdPrint(("[WIN11_24H2] [-] GetModuleBaseByVad_NoAttach: Invalid parameters.\n"));
			return STATUS_INVALID_PARAMETER;
		}

		*pBaseAddress = 0;
		*pModuleSize = 0;

		::NTSTATUS status = STATUS_NOT_FOUND;
		PVAD_SEARCH_RESULT pSearchResult = NULL;
		::UNICODE_STRING unicodeModuleName;
		::ANSI_STRING ansiModuleName;
		::BOOLEAN isUnicodeStringAllocated = FALSE;

		RtlZeroMemory(&unicodeModuleName, sizeof(::UNICODE_STRING));

		L_RtlInitAnsiString(&ansiModuleName, ModuleName);
		status = L_RtlAnsiStringToUnicodeString(&unicodeModuleName, &ansiModuleName, TRUE);
		if (!NT_SUCCESS(status)) {
			KdPrint(("[WIN11_24H2] [-] RtlAnsiStringToUnicodeString failed for '%s'. Status: 0x%X\n", ModuleName, status));
			return status;
		}
		isUnicodeStringAllocated = TRUE;

		UNICODE_STRING exeSuffix;
		L_RtlInitUnicodeString(&exeSuffix, L".exe");



		if (L_RtlSuffixUnicodeString(&exeSuffix, &unicodeModuleName, TRUE)) {
			*pBaseAddress = *(::PULONG_PTR)((::PUCHAR)pTargetProcess + EPROCESS_MEMBER_OFFSET::SectionBaseAddress);
			status = STATUS_SUCCESS;
			goto Cleanup;
		}
		else {
			pSearchResult = (PVAD_SEARCH_RESULT)L_ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(VAD_SEARCH_RESULT), 'VSad');
			if (!pSearchResult) {
				status = STATUS_INSUFFICIENT_RESOURCES;
				KdPrint(("[WIN11_24H2] [-] ExAllocatePool2 failed. Status: 0x%X\n", status));
				// 跳转到清理代码块
				goto Cleanup;
			}
			RtlZeroMemory(pSearchResult, sizeof(VAD_SEARCH_RESULT));

			__try {
				::PVOID pVadRootTable = (::PVOID)((::PUCHAR)pTargetProcess + EPROCESS_MEMBER_OFFSET::VAD_ROOT);
				if (L_MmIsAddressValid(pVadRootTable)) {
					PRTL_BALANCED_NODE pRootNode = *(PRTL_BALANCED_NODE*)pVadRootTable;
					if (pRootNode != NULL) {
						VadpTraverseTree(pRootNode, &unicodeModuleName, pSearchResult);
						if (pSearchResult->Found) {
							*pBaseAddress = pSearchResult->BaseAddress;
							*pModuleSize = pSearchResult->ModuleSize;
							status = STATUS_SUCCESS;
						}
						else {
							// 修正 2: 使用 %wZ 打印 UNICODE_STRING，更安全
							KdPrint(("[WIN11_24H2] [i] VAD tree traversed, but module '%wZ' was not found.\n", &unicodeModuleName));
							status = STATUS_NOT_FOUND;
						}
					}
					else {
						KdPrint(("[WIN11_24H2] [i] VAD root address is valid, but the tree is empty (root node is NULL).\n"));
						status = STATUS_NOT_FOUND;
					}
				}
				else {
					KdPrint(("[WIN11_24H2] [-] Calculated VAD root address (0x%p) is invalid. Check EPROCESS_VAD_ROOT_OFFSET (0x%X).\n", pVadRootTable, EPROCESS_MEMBER_OFFSET::VAD_ROOT));
					status = STATUS_INVALID_ADDRESS;
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				status = GetExceptionCode();
				KdPrint(("[WIN11_24H2] [-] An unhandled exception occurred during VAD traversal. Exception code: 0x%X\n", status));
			}
		}

	Cleanup:
		// 修正 3: 添加统一的清理代码块
		if (pSearchResult) {
			L_ExFreePoolWithTag(pSearchResult, 'VSad');
		}
		if (isUnicodeStringAllocated) {
			L_RtlFreeUnicodeString(&unicodeModuleName);
		}

		return status;
	}



}



namespace WIN11_23H2 {

	typedef struct _RTL_BALANCED_NODE {
		union {
			struct _RTL_BALANCED_NODE* Children[2];
			struct { struct _RTL_BALANCED_NODE* Left; struct _RTL_BALANCED_NODE* Right; } s;
		};
		union {
			::UCHAR Red : 1;
			::UCHAR Balance : 2;
			::ULONG_PTR ParentValue;
		} u;
	} RTL_BALANCED_NODE, * PRTL_BALANCED_NODE;

	typedef struct _RTL_AVL_TREE {
		PRTL_BALANCED_NODE Root;
	} RTL_AVL_TREE, * PRTL_AVL_TREE;

	typedef union _EX_PUSH_LOCK {
		struct { unsigned __int64 Locked : 1; unsigned __int64 Waiting : 1; unsigned __int64 Waking : 1; unsigned __int64 MultipleShared : 1; unsigned __int64 Shared : 60; };
		::ULONG_PTR Value;
		::PVOID Ptr;
	} EX_PUSH_LOCK, * PEX_PUSH_LOCK;

	typedef union _EX_FAST_REF {
		::PVOID Object;
		struct { unsigned __int64 RefCnt : 4; unsigned __int64 PtrValue : 60; };
		::ULONG_PTR Value;
	} EX_FAST_REF, * PEX_FAST_REF;

	typedef struct _MMVAD_SHORT {
		union {
			RTL_BALANCED_NODE VadNode;
			struct { ::PVOID NextVad; ::PVOID ExtraCreateInfo; };
		};
		::ULONG StartingVpn;
		::ULONG EndingVpn;
		::UCHAR StartingVpnHigh;
		::UCHAR EndingVpnHigh;
		::UCHAR CommitChargeHigh;
		::UCHAR SpareNT64VadUChar;
		volatile ::LONG ReferenceCount;
		EX_PUSH_LOCK PushLock;
		::ULONG32 u_flags;
		::ULONG32 u1_flags;
		::ULONG64 u5_flags;
	} MMVAD_SHORT, * PMMVAD_SHORT;

	typedef struct _CONTROL_AREA {
		struct _SEGMENT* Segment;
		union { ::LIST_ENTRY ListHead; ::PVOID AweContext; };
		::ULONG64 NumberOfSectionReferences;
		::ULONG64 NumberOfPfnReferences;
		::ULONG64 NumberOfMappedViews;
		::ULONG64 NumberOfUserReferences;
		::ULONG32 u_flags;
		::ULONG32 u1_flags;
		EX_FAST_REF FilePointer;
		volatile ::LONG ControlAreaLock;
		::ULONG ModifiedWriteCount;
		::PVOID WaitList;
		::ULONG64 u2_flags;
		EX_PUSH_LOCK FileObjectLock;
		volatile ::ULONG64 LockedPages;
		::ULONG64 u3_flags;
	} CONTROL_AREA, * PCONTROL_AREA;

	typedef struct _SUBSECTION {
		struct _CONTROL_AREA* ControlArea;
		struct _MMPTE* SubsectionBase;
		struct _SUBSECTION* NextSubsection;
		union { RTL_AVL_TREE GlobalPerSessionHead; ::PVOID CreationWaitList; ::PVOID SessionDriverProtos; } u_internal;
		::ULONG32 u_flags;
		::ULONG StartingSector;
		::ULONG NumberOfFullSectors;
		::ULONG PtesInSubsection;
		::ULONG u1_flags;
		::ULONG UnusedPtesAndFlags;
	} SUBSECTION, * PSUBSECTION;

	typedef struct _MMVAD {
		MMVAD_SHORT Core;
		::ULONG64 u2_flags;
		struct _SUBSECTION* Subsection;
		struct _MMPTE* FirstPrototypePte;
		struct _MMPTE* LastContiguousPte;
		::LIST_ENTRY ViewLinks;
		struct _EPROCESS* VadsProcess;
		::ULONG64 u4_flags_or_pAweInfo;
		struct _FILE_OBJECT* FileObject;
	} MMVAD, * PMMVAD;

	typedef struct _VAD_SEARCH_RESULT {
		::BOOLEAN Found;
		::ULONG_PTR BaseAddress;
		::ULONG_PTR ModuleSize;
	} VAD_SEARCH_RESULT, * PVAD_SEARCH_RESULT;


	inline VOID VadpTraverseTree(IN PRTL_BALANCED_NODE pNode, IN::PUNICODE_STRING pTargetModuleNameUnicode, IN PVAD_SEARCH_RESULT pResult) {
		// (此函数逻辑与 24H2 版本完全相同)
		if (!L_MmIsAddressValid(pNode) || pResult->Found) return;
		PMMVAD pVad = (PMMVAD)pNode;
		VadpTraverseTree(pVad->Core.VadNode.s.Left, pTargetModuleNameUnicode, pResult);
		if (pResult->Found) return;
		::PFILE_OBJECT pFile = NULL; ::PUNICODE_STRING pFullPath = NULL;
		if (pVad->FileObject != NULL) pFile = (::PFILE_OBJECT)pVad->FileObject;
		else if (pVad->Subsection != NULL && L_MmIsAddressValid(pVad->Subsection) && pVad->Subsection->ControlArea != NULL && L_MmIsAddressValid(pVad->Subsection->ControlArea)) {
			PCONTROL_AREA pCtrlArea = (PCONTROL_AREA)pVad->Subsection->ControlArea;
			::PVOID rawFilePointer = pCtrlArea->FilePointer.Object;
			if (rawFilePointer != NULL) pFile = (::PFILE_OBJECT)((::ULONG_PTR)rawFilePointer & ~((::ULONG_PTR)sizeof(::PVOID) - 1));
		}
		if (pFile != NULL && L_MmIsAddressValid(pFile)) {
			pFullPath = &pFile->FileName;
			if (pFullPath->Buffer != NULL && pFullPath->Length > 0 && L_MmIsAddressValid(pFullPath->Buffer)) {
				__try {
					::UNICODE_STRING currentFileName; RtlZeroMemory(&currentFileName, sizeof(::UNICODE_STRING));
					::USHORT i; for (i = (pFullPath->Length / sizeof(WCHAR)) - 1; i > 0; i--) { if (pFullPath->Buffer[i] == L'\\') break; }
					::USHORT nameStartIndex = (i > 0 && pFullPath->Buffer[i] == L'\\') ? i + 1 : 0; if (i == 0 && pFullPath->Buffer[0] == L'\\') nameStartIndex = 1;
					currentFileName.Buffer = &pFullPath->Buffer[nameStartIndex];
					currentFileName.Length = pFullPath->Length - (nameStartIndex * sizeof(WCHAR));
					currentFileName.MaximumLength = currentFileName.Length;


					// 计算当前VAD区域的基地址和大小
					::ULONG64 startingVpn = ((::ULONG64)pVad->Core.StartingVpnHigh << 32) | pVad->Core.StartingVpn;
					::ULONG64 endingVpn = ((::ULONG64)pVad->Core.EndingVpnHigh << 32) | pVad->Core.EndingVpn;
					::ULONG_PTR baseAddress = startingVpn << PAGE_SHIFT;
					::ULONG_PTR moduleSize = ((endingVpn + 1) - startingVpn) << PAGE_SHIFT;

					// 使用KdPrint输出每个模块的信息
					KdPrint(("[WIN11_24H2][VAD Traverse] Module: %wZ, Base: 0x%p, Size: 0x%p\n", &currentFileName, (PVOID)baseAddress, (PVOID)moduleSize));

					if (L_RtlCompareUnicodeString(&currentFileName, pTargetModuleNameUnicode, TRUE) == 0) {
						::ULONG64 startingVpn = ((::ULONG64)pVad->Core.StartingVpnHigh << 32) | pVad->Core.StartingVpn;
						::ULONG64 endingVpn = ((::ULONG64)pVad->Core.EndingVpnHigh << 32) | pVad->Core.EndingVpn;
						pResult->BaseAddress = startingVpn << PAGE_SHIFT;
						pResult->ModuleSize = ((endingVpn + 1) - startingVpn) << PAGE_SHIFT;
						pResult->Found = TRUE;
						return;
					}
				}
				__except (EXCEPTION_EXECUTE_HANDLER) {}
			}
		}
		VadpTraverseTree(pVad->Core.VadNode.s.Right, pTargetModuleNameUnicode, pResult);
	}

	inline ::NTSTATUS GetModuleBaseByVad_NoAttach(IN::PEPROCESS pTargetProcess, IN const char* ModuleName, OUT::PULONG_PTR pBaseAddress, OUT::PULONG_PTR pModuleSize) {
		if (!pTargetProcess || !ModuleName || !pBaseAddress || !pModuleSize) {
			KdPrint(("[WIN11_23H2] [-] GetModuleBaseByVad_NoAttach: Invalid parameters.\n"));
			return STATUS_INVALID_PARAMETER;
		}

		*pBaseAddress = 0;
		*pModuleSize = 0;

		::NTSTATUS status = STATUS_NOT_FOUND;
		PVAD_SEARCH_RESULT pSearchResult = NULL;
		::UNICODE_STRING unicodeModuleName;
		::ANSI_STRING ansiModuleName;
		::BOOLEAN isUnicodeStringAllocated = FALSE;

		RtlZeroMemory(&unicodeModuleName, sizeof(::UNICODE_STRING));

		L_RtlInitAnsiString(&ansiModuleName, ModuleName);
		status = L_RtlAnsiStringToUnicodeString(&unicodeModuleName, &ansiModuleName, TRUE);
		if (!NT_SUCCESS(status)) {
			KdPrint(("[WIN11_23H2] [-] RtlAnsiStringToUnicodeString failed for '%s'. Status: 0x%X\n", ModuleName, status));
			// 此处无需清理，因为还没有其他资源被分配
			return status;
		}
		isUnicodeStringAllocated = TRUE;

		UNICODE_STRING exeSuffix;
		L_RtlInitUnicodeString(&exeSuffix, L".exe");

		if (L_RtlSuffixUnicodeString(&exeSuffix, &unicodeModuleName, TRUE)) {
			// 如果是 .exe 文件，直接获取 SectionBaseAddress
			*pBaseAddress = *(::PULONG_PTR)((::PUCHAR)pTargetProcess + EPROCESS_MEMBER_OFFSET::SectionBaseAddress);
			status = STATUS_SUCCESS;
			// 修正 1: 跳转到清理代码块以释放 unicodeModuleName
			goto Cleanup;
		}
		else {
			// 如果是 DLL 文件，遍历 VAD
			pSearchResult = (PVAD_SEARCH_RESULT)L_ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(VAD_SEARCH_RESULT), 'VSad');
			if (!pSearchResult) {
				status = STATUS_INSUFFICIENT_RESOURCES;
				KdPrint(("[WIN11_23H2] [-] ExAllocatePool2 failed. Status: 0x%X\n", status));
				// 跳转到清理代码块
				goto Cleanup;
			}
			RtlZeroMemory(pSearchResult, sizeof(VAD_SEARCH_RESULT));

			__try {
				::PVOID pVadRootTable = (::PVOID)((::PUCHAR)pTargetProcess + EPROCESS_MEMBER_OFFSET::VAD_ROOT);
				if (L_MmIsAddressValid(pVadRootTable)) {
					PRTL_BALANCED_NODE pRootNode = *(PRTL_BALANCED_NODE*)pVadRootTable;
					if (pRootNode != NULL) {
						VadpTraverseTree(pRootNode, &unicodeModuleName, pSearchResult);
						if (pSearchResult->Found) {
							*pBaseAddress = pSearchResult->BaseAddress;
							*pModuleSize = pSearchResult->ModuleSize;
							status = STATUS_SUCCESS;
						}
						else {
							// 修正 2: 使用 %wZ 打印 UNICODE_STRING，这才是安全和正确的方式
							KdPrint(("[WIN11_23H2] [i] VAD tree traversed, but module '%wZ' was not found.\n", &unicodeModuleName));
							status = STATUS_NOT_FOUND;
						}
					}
					else {
						KdPrint(("[WIN11_23H2] [i] VAD root address is valid, but the tree is empty (root node is NULL).\n"));
						status = STATUS_NOT_FOUND;
					}
				}
				else {
					KdPrint(("[WIN11_23H2] [-] Calculated VAD root address (0x%p) is invalid. Check EPROCESS_VAD_ROOT_OFFSET (0x%X).\n", pVadRootTable, EPROCESS_MEMBER_OFFSET::VAD_ROOT));
					status = STATUS_INVALID_ADDRESS;
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				status = GetExceptionCode();
				KdPrint(("[WIN11_23H2] [-] An unhandled exception occurred during VAD traversal. Exception code: 0x%X\n", status));
			}
		}

	Cleanup:
		// 修正 3: 添加统一的清理代码块，释放所有已分配的资源
		if (pSearchResult) {
			L_ExFreePoolWithTag(pSearchResult, 'VSad');
		}
		if (isUnicodeStringAllocated) {
			L_RtlFreeUnicodeString(&unicodeModuleName);
		}

		return status;
	}



}




namespace WIN11_22H2 {

	// 基础结构定义
	typedef struct _RTL_BALANCED_NODE {
		union {
			struct _RTL_BALANCED_NODE* Children[2];
			struct { struct _RTL_BALANCED_NODE* Left; struct _RTL_BALANCED_NODE* Right; } s;
		};
		union {
			::UCHAR Red : 1;
			::UCHAR Balance : 2;
			::ULONG_PTR ParentValue;
		} u;
	} RTL_BALANCED_NODE, * PRTL_BALANCED_NODE;

	typedef struct _RTL_AVL_TREE {
		PRTL_BALANCED_NODE Root;
	} RTL_AVL_TREE, * PRTL_AVL_TREE;

	typedef union _EX_PUSH_LOCK {
		struct { unsigned __int64 Locked : 1; unsigned __int64 Waiting : 1; unsigned __int64 Waking : 1; unsigned __int64 MultipleShared : 1; unsigned __int64 Shared : 60; };
		::ULONG_PTR Value;
		::PVOID Ptr;
	} EX_PUSH_LOCK, * PEX_PUSH_LOCK;

	typedef union _EX_FAST_REF {
		::PVOID Object;
		struct { unsigned __int64 RefCnt : 4; unsigned __int64 PtrValue : 60; };
		::ULONG_PTR Value;
	} EX_FAST_REF, * PEX_FAST_REF;

	// 【关键修正】MMVAD_SHORT
	// 在 Win11 22H2 中，此结构体大小应为 0x40
	typedef struct _MMVAD_SHORT {
		union {
			RTL_BALANCED_NODE VadNode; // 0x00 - 0x18
			struct { ::PVOID NextVad; ::PVOID ExtraCreateInfo; };
		};
		::ULONG StartingVpn;           // 0x18
		::ULONG EndingVpn;             // 0x1C
		::UCHAR StartingVpnHigh;       // 0x20
		::UCHAR EndingVpnHigh;         // 0x21
		::UCHAR CommitChargeHigh;      // 0x22
		::UCHAR SpareNT64VadUChar;     // 0x23
		volatile ::LONG ReferenceCount;// 0x24
		EX_PUSH_LOCK PushLock;         // 0x28 - 0x30
		::ULONG32 u_flags;             // 0x30
		::ULONG32 u1_flags;            // 0x34
		struct _MI_VAD_EVENT_BLOCK* EventList; // 0x38 - 0x40 (用于对齐到 0x40)
	} MMVAD_SHORT, * PMMVAD_SHORT;

	// SEGMENT 结构
	typedef struct _SEGMENT {
		struct _CONTROL_AREA* ControlArea;
		::ULONG TotalNumberOfPtes;
		::ULONG SegmentFlags_placeholder;
		::ULONG64 NumberOfCommittedPages;
		::ULONG64 SizeOfSegment;
		union {
			struct _MMEXTEND_INFO* ExtendInfo;
			::PVOID BasedAddress;
		};
		EX_PUSH_LOCK SegmentLock;
		::ULONG64 u1_placeholder;
		::ULONG64 u2_placeholder;
		struct _MMPTE* PrototypePte;
	} SEGMENT, * PSEGMENT;

	// CONTROL_AREA 结构
	typedef struct _CONTROL_AREA {
		struct _SEGMENT* Segment;
		union {
			::LIST_ENTRY ListHead;
			::PVOID AweContext;
		};
		::ULONG64 NumberOfSectionReferences;
		::ULONG64 NumberOfPfnReferences;
		::ULONG64 NumberOfMappedViews;
		::ULONG64 NumberOfUserReferences;
		::ULONG u_flags;
		::ULONG u1_flags;
		EX_FAST_REF FilePointer;       // 偏移通常为 0x40
		volatile ::LONG ControlAreaLock;
		::ULONG ModifiedWriteCount;
		::PVOID WaitList;
		union {
			struct { ::PVOID Lookaside; ::PVOID PowerDomain; } u2_struct;
			::ULONG64 u2_quadwords[2];
		} u2_placeholder;
		EX_PUSH_LOCK FileObjectLock;
		volatile ::ULONG64 LockedPages;
		::ULONG64 u3_flags;
	} CONTROL_AREA, * PCONTROL_AREA;

	// SUBSECTION 结构
	typedef struct _SUBSECTION {
		struct _CONTROL_AREA* ControlArea;
		struct _MMPTE* SubsectionBase;
		struct _SUBSECTION* NextSubsection;
		union { RTL_AVL_TREE GlobalPerSessionHead; ::PVOID CreationWaitList; ::PVOID SessionDriverProtos; } u;
		::ULONG32 SubsectionFlags;
		::ULONG StartingSector;
		::ULONG NumberOfFullSectors;
		::ULONG PtesInSubsection;
		::ULONG u1_flags;
		::ULONG UnusedPtesAndFlags;
	} SUBSECTION, * PSUBSECTION;

	// 【关键修正】MMVAD 结构体
	// 确保 Subsection 在 0x48 偏移处
	typedef struct _MMVAD {
		MMVAD_SHORT Core;           // 0x00 - 0x40
		union {
			::ULONG32 LongFlags;
			::ULONG32 VadFlags;
		} u2;                       // 0x40 - 0x44
		::ULONG32 Padding_u2;       // 0x44 - 0x48 (填充)

		struct _SUBSECTION* Subsection; // 0x48 <-- 关键偏移

		struct _MMPTE* FirstPrototypePte;
		struct _MMPTE* LastContiguousPte;
		::LIST_ENTRY ViewLinks;
		struct _EPROCESS* VadsProcess;
		::ULONG64 u4_flags_or_pAweInfo;
		struct _FILE_OBJECT* FileObject;
	} MMVAD, * PMMVAD;

	typedef struct _VAD_SEARCH_RESULT {
		::BOOLEAN Found;
		::ULONG_PTR BaseAddress;
		::ULONG_PTR ModuleSize;
	} VAD_SEARCH_RESULT, * PVAD_SEARCH_RESULT;

	// ---------------------------------------------------------
	// VAD 遍历函数
	// ---------------------------------------------------------
	inline VOID VadpTraverseTree(IN PRTL_BALANCED_NODE pNode, IN::PUNICODE_STRING pTargetModuleNameUnicode, IN PVAD_SEARCH_RESULT pResult) {
		if (!L_MmIsAddressValid(pNode) || pResult->Found) return;

		PMMVAD pVad = (PMMVAD)pNode;

		// 递归左子树
		VadpTraverseTree(pVad->Core.VadNode.s.Left, pTargetModuleNameUnicode, pResult);
		if (pResult->Found) return;

		::PFILE_OBJECT pFile = NULL;
		::PUNICODE_STRING pFullPath = NULL;

		// 1. 尝试直接获取 FileObject
		if (pVad->FileObject != NULL && L_MmIsAddressValid(pVad->FileObject)) {
			pFile = (::PFILE_OBJECT)pVad->FileObject;
		}
		// 2. 如果没有，尝试通过 Subsection -> ControlArea 获取
		else if (pVad->Subsection != NULL && L_MmIsAddressValid(pVad->Subsection)) {
			// 这里如果结构体定义错误，会导致读取错误的内存地址
			if (pVad->Subsection->ControlArea != NULL && L_MmIsAddressValid(pVad->Subsection->ControlArea)) {
				PCONTROL_AREA pCtrlArea = (PCONTROL_AREA)pVad->Subsection->ControlArea;
				::PVOID rawFilePointer = pCtrlArea->FilePointer.Object;

				// 去除低位的引用计数位 (EX_FAST_REF)
				if (rawFilePointer != NULL) {
					pFile = (::PFILE_OBJECT)((::ULONG_PTR)rawFilePointer & ~((::ULONG_PTR)sizeof(::PVOID) - 1));
				}
			}
		}

		if (pFile != NULL && L_MmIsAddressValid(pFile)) {
			pFullPath = &pFile->FileName;
			if (pFullPath->Buffer != NULL && pFullPath->Length > 0 && L_MmIsAddressValid(pFullPath->Buffer)) {
				__try {
					::UNICODE_STRING currentFileName;
					RtlZeroMemory(&currentFileName, sizeof(::UNICODE_STRING));

					// 简单的提取文件名逻辑 (从最后一个 \ 开始)
					::USHORT i;
					for (i = (pFullPath->Length / sizeof(WCHAR)) - 1; i > 0; i--) {
						if (pFullPath->Buffer[i] == L'\\') break;
					}
					::USHORT nameStartIndex = (i > 0 && pFullPath->Buffer[i] == L'\\') ? i + 1 : 0;
					if (i == 0 && pFullPath->Buffer[0] == L'\\') nameStartIndex = 1;

					currentFileName.Buffer = &pFullPath->Buffer[nameStartIndex];
					currentFileName.Length = pFullPath->Length - (nameStartIndex * sizeof(WCHAR));
					currentFileName.MaximumLength = currentFileName.Length;

					// 调试输出
					// ::ULONG64 startingVpn = ((::ULONG64)pVad->Core.StartingVpnHigh << 32) | pVad->Core.StartingVpn;
					// ::ULONG_PTR baseAddress = startingVpn << PAGE_SHIFT;
					// KdPrint(("[WIN11_22H2] Checking: %wZ at 0x%p\n", &currentFileName, (PVOID)baseAddress));

					if (L_RtlCompareUnicodeString(&currentFileName, pTargetModuleNameUnicode, TRUE) == 0) {
						::ULONG64 startingVpn = ((::ULONG64)pVad->Core.StartingVpnHigh << 32) | pVad->Core.StartingVpn;
						::ULONG64 endingVpn = ((::ULONG64)pVad->Core.EndingVpnHigh << 32) | pVad->Core.EndingVpn;

						pResult->BaseAddress = startingVpn << PAGE_SHIFT;
						pResult->ModuleSize = ((endingVpn + 1) - startingVpn) << PAGE_SHIFT;
						pResult->Found = TRUE;
						return;
					}
				}
				__except (EXCEPTION_EXECUTE_HANDLER) {
					// 忽略访问异常
				}
			}
		}

		// 递归右子树
		VadpTraverseTree(pVad->Core.VadNode.s.Right, pTargetModuleNameUnicode, pResult);
	}


	inline ::NTSTATUS GetModuleBaseByVad_NoAttach(IN::PEPROCESS pTargetProcess, IN const char* ModuleName, OUT::PULONG_PTR pBaseAddress, OUT::PULONG_PTR pModuleSize) {
		if (!pTargetProcess || !ModuleName || !pBaseAddress || !pModuleSize) {
			return STATUS_INVALID_PARAMETER;
		}

		*pBaseAddress = 0;
		*pModuleSize = 0;

		::NTSTATUS status = STATUS_NOT_FOUND;
		PVAD_SEARCH_RESULT pSearchResult = NULL;
		::UNICODE_STRING unicodeModuleName;
		::ANSI_STRING ansiModuleName;
		::BOOLEAN isUnicodeStringAllocated = FALSE;

		RtlZeroMemory(&unicodeModuleName, sizeof(::UNICODE_STRING));

		L_RtlInitAnsiString(&ansiModuleName, ModuleName);
		status = L_RtlAnsiStringToUnicodeString(&unicodeModuleName, &ansiModuleName, TRUE);
		if (!NT_SUCCESS(status)) {
			return status;
		}
		isUnicodeStringAllocated = TRUE;

		UNICODE_STRING exeSuffix;
		L_RtlInitUnicodeString(&exeSuffix, L".exe");

		// 如果是 .exe，直接从 SectionBaseAddress 取
		if (L_RtlSuffixUnicodeString(&exeSuffix, &unicodeModuleName, TRUE)) {
			*pBaseAddress = *(::PULONG_PTR)((::PUCHAR)pTargetProcess + EPROCESS_MEMBER_OFFSET::SectionBaseAddress);
			status = STATUS_SUCCESS;
			goto Cleanup;
		}
		else {
			// 如果是 DLL，遍历 VAD
			pSearchResult = (PVAD_SEARCH_RESULT)L_ExAllocatePool2(POOL_FLAG_NON_PAGED, sizeof(VAD_SEARCH_RESULT), 'VSad');
			if (!pSearchResult) {
				status = STATUS_INSUFFICIENT_RESOURCES;
				goto Cleanup;
			}
			RtlZeroMemory(pSearchResult, sizeof(VAD_SEARCH_RESULT));

			__try {
				::PVOID pVadRootTable = (::PVOID)((::PUCHAR)pTargetProcess + EPROCESS_MEMBER_OFFSET::VAD_ROOT);
				if (L_MmIsAddressValid(pVadRootTable)) {
					PRTL_BALANCED_NODE pRootNode = *(PRTL_BALANCED_NODE*)pVadRootTable;
					if (pRootNode != NULL) {
						VadpTraverseTree(pRootNode, &unicodeModuleName, pSearchResult);
						if (pSearchResult->Found) {
							*pBaseAddress = pSearchResult->BaseAddress;
							*pModuleSize = pSearchResult->ModuleSize;
							status = STATUS_SUCCESS;
						}
						else {
							KdPrint(("[WIN11_22H2] Module '%wZ' not found in VAD.\n", &unicodeModuleName));
							status = STATUS_NOT_FOUND;
						}
					}
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {
				status = GetExceptionCode();
			}
		}

	Cleanup:
		if (pSearchResult) {
			L_ExFreePoolWithTag(pSearchResult, 'VSad');
		}
		if (isUnicodeStringAllocated) {
			L_RtlFreeUnicodeString(&unicodeModuleName);
		}

		return status;
	}
}
