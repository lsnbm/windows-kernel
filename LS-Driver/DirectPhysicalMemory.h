#pragma once
#include <ntifs.h>
#include <intrin.h>

// --- 结构体定义 ---

struct PHYSICAL_PAGE_INFO
{
	PVOID BaseAddress;
	SIZE_T Size;
	PVOID PteAddress;
};

struct STEALTH_RW_CONTEXT
{
	ULONG64 TargetCr3;
	ULONG64 CachedPxeVal;
	ULONG64 CachedPxeVaRange;
	ULONG64 CachedPpeVal;
	ULONG64 CachedPpeVaRange;
	ULONG64 CachedPdeVal;
	ULONG64 CachedPdeVaRange;
};

// 全局变量 (无锁，请确保用户层是单线程/互斥访问)
inline PHYSICAL_PAGE_INFO g_TransferPage;
inline STEALTH_RW_CONTEXT rwCtx;

// --- 安全辅助函数 ---

// [核心安全机制] 使用 SEH 捕获内存访问异常
// 防止 User Buffer 无效/未分页/只读导致系统蓝屏
inline BOOLEAN SafeCopyMemory(PVOID Dest, const PVOID Src, SIZE_T Size)
{
	__try {
		// RtlCopyMemory 内部处理了重叠和对齐，比手动 __movsb 更稳健
		RtlCopyMemory(Dest, Src, Size);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		// 捕获到访问违规 (0xC0000005)，返回失败而不是蓝屏
		return FALSE;
	}
	return TRUE;
}

// 检查物理页是否在有效物理内存范围内
inline bool IsPhysPageInRange(ULONG64 PhysAddress, ULONG64 Size)
{
	const ULONG64 PhysPageEnd = PhysAddress + Size - 1;
	static PPHYSICAL_MEMORY_RANGE g_PhysicalMemoryRanges = NULL;

	if (!g_PhysicalMemoryRanges)
	{
		g_PhysicalMemoryRanges = MmGetPhysicalMemoryRanges();
		if (!g_PhysicalMemoryRanges) return false;
	}

	for (int i = 0; ; ++i)
	{
		const PHYSICAL_MEMORY_RANGE* range = &g_PhysicalMemoryRanges[i];
		if (!range->BaseAddress.QuadPart && !range->NumberOfBytes.QuadPart) break;

		if (PhysAddress >= (ULONG64)range->BaseAddress.QuadPart &&
			PhysPageEnd < ((ULONG64)range->BaseAddress.QuadPart + (ULONG64)range->NumberOfBytes.QuadPart))
		{
			return true;
		}
	}
	return false;
}

// --- 初始化与分配 ---

inline ULONG AllocatePhysicalPage(PHYSICAL_PAGE_INFO* PhysicalPageInfo)
{
	static ULONG64 g_PteBaseForAlloc = 0;

	if (!PhysicalPageInfo) return 22;
	memset(PhysicalPageInfo, 0, sizeof(PHYSICAL_PAGE_INFO));

	if (g_PteBaseForAlloc == 0)
	{
		PHYSICAL_ADDRESS DirectoryTableBase;
		DirectoryTableBase.QuadPart = (__readcr3() & ~0xFFF);
		PULONG64 PML4Table = (PULONG64)MmGetVirtualForPhysical(DirectoryTableBase);
		if (!PML4Table) return 0x106;

		for (ULONG64 index = 0; index < 512; ++index)
		{
			if ((PML4Table[index] & 1) && (((PML4Table[index] >> 12) & 0xFFFFFFFFFF) == (DirectoryTableBase.QuadPart >> 12)))
			{
				ULONG64 va_base = index << 39;
				g_PteBaseForAlloc = (ULONG64)((INT64)(va_base << 16) >> 16);
				break;
			}
		}
		if (g_PteBaseForAlloc == 0) return 0x107;
	}

	PVOID BaseAddress = MmAllocateMappingAddress(PAGE_SIZE, 'MyTg');
	if (!BaseAddress) return 0x119;

	PVOID PteAddress = (PVOID)(g_PteBaseForAlloc + 8 * ((reinterpret_cast<ULONG64>(BaseAddress) & 0xFFFFFFFFFFFFi64) >> 12));

	if (!MmIsAddressValid(PteAddress))
	{
		MmFreeMappingAddress(BaseAddress, 'MyTg');
		return 0x109;
	}

	PhysicalPageInfo->BaseAddress = BaseAddress;
	PhysicalPageInfo->Size = PAGE_SIZE;
	PhysicalPageInfo->PteAddress = PteAddress;
	return 0;
}

inline void FreePhysicalPage(PHYSICAL_PAGE_INFO* PageInfo)
{
	if (PageInfo && PageInfo->BaseAddress)
	{
		MmFreeMappingAddress(PageInfo->BaseAddress, 'MyTg');
		memset(PageInfo, 0, sizeof(PHYSICAL_PAGE_INFO));
	}
}

// --- 核心单页读写 (无锁 + SEH保护) ---

inline ULONG ReadPhysicalSinglePage(const PHYSICAL_PAGE_INFO* TransferPageInfo, ULONG64 PhysAddress, PVOID Buffer, SIZE_T Size)
{
	// 1. 修改 PTE 映射到目标物理地址
	// 保留原有属性位，强制设置为 Present(1) | RW(2) = 3 (Kernel Mode)
	// 如果需要 User 位，可用 0x7，但在驱动中 0x3 足矣
	volatile ULONG64* pPte = (volatile ULONG64*)TransferPageInfo->PteAddress;
	*pPte = (PhysAddress & ~0xFFFULL) | (*pPte & 0xFFF0000000000FFF) | 0x3;

	// 2. 刷新 TLB
	__invlpg(TransferPageInfo->BaseAddress);

	// 3. 安全拷贝 (从 映射页 读到 Buffer)
	// 如果 Buffer 无效，这里会捕获异常并返回错误，而不是蓝屏
	if (!SafeCopyMemory(Buffer, (PUCHAR)TransferPageInfo->BaseAddress + (PhysAddress & 0xFFF), Size))
	{
		return 0xC0000005; // STATUS_ACCESS_VIOLATION
	}

	return 0;
}

inline ULONG WritePhysicalSinglePage(const PHYSICAL_PAGE_INFO* TransferPageInfo, ULONG64 PhysAddress, PVOID Buffer, SIZE_T Size)
{
	// 1. 修改 PTE
	volatile ULONG64* pPte = (volatile ULONG64*)TransferPageInfo->PteAddress;
	*pPte = (PhysAddress & ~0xFFFULL) | (*pPte & 0xFFF0000000000FFF) | 0x3;

	// 2. 刷新 TLB
	__invlpg(TransferPageInfo->BaseAddress);

	// 3. 安全拷贝 (从 Buffer 写到 映射页)
	if (!SafeCopyMemory((PUCHAR)TransferPageInfo->BaseAddress + (PhysAddress & 0xFFF), Buffer, Size))
	{
		return 0xC0000005;
	}

	return 0;
}

// --- 页表遍历逻辑 (保持原样，调用带保护的单页读写) ---

inline ULONG GetPhysPageInfoStealth(const PHYSICAL_PAGE_INFO* TransferPageInfo, ULONG64 Cr3, PVOID Va, PULONG64 pPhysicalPageBase, PULONG64 pPageSize, STEALTH_RW_CONTEXT* Context)
{
	if (!pPhysicalPageBase || !pPageSize || !Context) return 22;

	const ULONG64 va_val = (ULONG64)Va;
	const ULONG64 pxe_va_range = va_val >> 39;
	const ULONG64 ppe_va_range = va_val >> 30;
	const ULONG64 pde_va_range = va_val >> 21;

	ULONG64 pxe_val, ppe_val, pde_val, pte_val;

	if (Context->TargetCr3 != Cr3)
	{
		memset(Context, 0, sizeof(STEALTH_RW_CONTEXT));
		Context->TargetCr3 = Cr3;
	}

	if (Context->CachedPxeVaRange != pxe_va_range)
	{
		if (ReadPhysicalSinglePage(TransferPageInfo, (Cr3 & ~0xFFF) + 8 * (pxe_va_range & 0x1FF), &pxe_val, 8) || (pxe_val & 1) == 0) return 262;
		Context->CachedPxeVal = pxe_val;
		Context->CachedPxeVaRange = pxe_va_range;
		Context->CachedPpeVaRange = (ULONG64)-1;
		Context->CachedPdeVaRange = (ULONG64)-1;
	}
	pxe_val = Context->CachedPxeVal;

	if (Context->CachedPpeVaRange != ppe_va_range)
	{
		const ULONG64 ppe_phys_addr = (pxe_val & 0x000FFFFFFFFF000) + 8 * (ppe_va_range & 0x1FF);
		if (ReadPhysicalSinglePage(TransferPageInfo, ppe_phys_addr, &ppe_val, 8) || (ppe_val & 1) == 0) return 263;
		Context->CachedPpeVal = ppe_val;
		Context->CachedPpeVaRange = ppe_va_range;
		Context->CachedPdeVaRange = (ULONG64)-1;
	}
	ppe_val = Context->CachedPpeVal;

	if (ppe_val & 0x80) // 1GB 大页
	{
		*pPageSize = 0x40000000;
		*pPhysicalPageBase = (ppe_val & 0x000FFFFFC0000000);
		if (!IsPhysPageInRange(*pPhysicalPageBase, *pPageSize)) return 266;
		return 0;
	}

	if (Context->CachedPdeVaRange != pde_va_range)
	{
		const ULONG64 pde_phys_addr = (ppe_val & 0x000FFFFFFFFF000) + 8 * (pde_va_range & 0x1FF);
		if (ReadPhysicalSinglePage(TransferPageInfo, pde_phys_addr, &pde_val, 8) || (pde_val & 1) == 0) return 264;
		Context->CachedPdeVal = pde_val;
		Context->CachedPdeVaRange = pde_va_range;
	}
	pde_val = Context->CachedPdeVal;

	if (pde_val & 0x80) // 2MB 大页
	{
		*pPageSize = 0x200000;
		*pPhysicalPageBase = (pde_val & 0x000FFFFFFE00000);
		if (!IsPhysPageInRange(*pPhysicalPageBase, *pPageSize)) return 267;
		return 0;
	}

	const ULONG64 pte_phys_addr = (pde_val & 0x000FFFFFFFFF000) + 8 * ((va_val >> 12) & 0x1FF);
	if (ReadPhysicalSinglePage(TransferPageInfo, pte_phys_addr, &pte_val, 8) || (pte_val & 1) == 0) return 265;

	*pPageSize = 0x1000; // 4KB 页
	*pPhysicalPageBase = (pte_val & 0x000FFFFFFFFF000);

	if (!IsPhysPageInRange(*pPhysicalPageBase, *pPageSize)) return 268;
	return 0;
}

// --- 最终的读写接口 (循环处理) ---

inline ULONG ReadPhysMemory(const PHYSICAL_PAGE_INFO* TransferPageInfo, ULONG64 DirectoryTableBase, PVOID Address, PVOID Buffer, ULONG TotalSize, STEALTH_RW_CONTEXT* Context)
{
	if (!Address || !Buffer || !TotalSize || !TransferPageInfo || !Context || !DirectoryTableBase) return 22;

	PUCHAR currentBuffer = (PUCHAR)Buffer;
	ULONG64 currentVa = (ULONG64)Address;
	ULONG bytesRemaining = TotalSize;
	ULONG status = 0;

	ULONG64 currentPagePhysBase = 0;
	ULONG64 currentPageSize = 0;
	ULONG64 currentPageVaEnd = currentVa;

	while (bytesRemaining > 0)
	{
		if (currentVa >= currentPageVaEnd)
		{
			status = GetPhysPageInfoStealth(TransferPageInfo, DirectoryTableBase, (PVOID)currentVa, &currentPagePhysBase, &currentPageSize, Context);
			if (status != 0) return status;
			currentPageVaEnd = (currentVa & ~(currentPageSize - 1)) + currentPageSize;
		}

		const ULONG64 offsetInPage = currentVa & (currentPageSize - 1);
		const ULONG64 finalPhysAddr = currentPagePhysBase + offsetInPage;

		ULONG bytesToProcess = min(bytesRemaining, PAGE_SIZE - (ULONG)(finalPhysAddr & 0xFFF));

		status = ReadPhysicalSinglePage(TransferPageInfo, finalPhysAddr, currentBuffer, bytesToProcess);
		if (status != 0) return status;

		bytesRemaining -= bytesToProcess;
		currentBuffer += bytesToProcess;
		currentVa += bytesToProcess;
	}
	return 0;
}

inline ULONG WritePhysMemory(const PHYSICAL_PAGE_INFO* TransferPageInfo, ULONG64 DirectoryTableBase, PVOID Address, PVOID Buffer, ULONG TotalSize, STEALTH_RW_CONTEXT* Context)
{
	if (!Address || !Buffer || !TotalSize || !TransferPageInfo || !Context || !DirectoryTableBase) return 22;

	PUCHAR currentBuffer = (PUCHAR)Buffer;
	ULONG64 currentVa = (ULONG64)Address;
	ULONG bytesRemaining = TotalSize;
	ULONG status = 0;

	ULONG64 currentPagePhysBase = 0;
	ULONG64 currentPageSize = 0;
	ULONG64 currentPageVaEnd = currentVa;

	while (bytesRemaining > 0)
	{
		if (currentVa >= currentPageVaEnd)
		{
			status = GetPhysPageInfoStealth(TransferPageInfo, DirectoryTableBase, (PVOID)currentVa, &currentPagePhysBase, &currentPageSize, Context);
			if (status != 0) return status;
			currentPageVaEnd = (currentVa & ~(currentPageSize - 1)) + currentPageSize;
		}

		const ULONG64 offsetInPage = currentVa & (currentPageSize - 1);
		const ULONG64 finalPhysAddr = currentPagePhysBase + offsetInPage;

		ULONG bytesToProcess = min(bytesRemaining, PAGE_SIZE - (ULONG)(finalPhysAddr & 0xFFF));

		status = WritePhysicalSinglePage(TransferPageInfo, finalPhysAddr, currentBuffer, bytesToProcess);
		if (status != 0) return status;

		bytesRemaining -= bytesToProcess;
		currentBuffer += bytesToProcess;
		currentVa += bytesToProcess;
	}
	return 0;
}
