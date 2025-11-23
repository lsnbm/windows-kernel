#pragma once
#include <ntifs.h>
#include <intrin.h>

// --- 结构体定义 ---

// 用于最终物理内存操作的中转页信息
struct PHYSICAL_PAGE_INFO
{
	PVOID BaseAddress;
	SIZE_T Size;
	PVOID PteAddress;
};

// 终极隐蔽方案的上下文结构体：缓存物理页表项的值
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

inline PHYSICAL_PAGE_INFO g_TransferPage;// 用于物理中转页
inline STEALTH_RW_CONTEXT  rwCtx;		//缓存物理页表项的值


// 检查物理页是否在有效物理内存范围内
inline bool IsPhysPageInRange(ULONG64 PhysAddress, ULONG64 Size)
{
	const ULONG64 PhysPageEnd = PhysAddress + Size - 1;
	// 缓存 MmGetPhysicalMemoryRanges 的结果以提高性能
	static PPHYSICAL_MEMORY_RANGE g_PhysicalMemoryRanges = NULL;

	if (!g_PhysicalMemoryRanges)
	{
		g_PhysicalMemoryRanges = MmGetPhysicalMemoryRanges();
		if (!g_PhysicalMemoryRanges) return false; // 无法获取物理内存范围
	}

	for (int i = 0; ; ++i)
	{
		const PHYSICAL_MEMORY_RANGE* range = &g_PhysicalMemoryRanges[i];
		// 列表以一个空的条目结束
		if (!range->BaseAddress.QuadPart && !range->NumberOfBytes.QuadPart) break;
		if (PhysAddress >= range->BaseAddress.QuadPart && PhysPageEnd < (range->BaseAddress.QuadPart + range->NumberOfBytes.QuadPart))
		{
			return true;
		}
	}
	return false;
}


// 分配用于映射的中转页
inline ULONG AllocatePhysicalPage(PHYSICAL_PAGE_INFO* PhysicalPageInfo)
{
	// PTE基址在驱动生命周期内是固定的，只需计算一次
	static ULONG64 g_PteBaseForAlloc = 0;

	if (!PhysicalPageInfo) return 22;
	memset(PhysicalPageInfo, 0, sizeof(PHYSICAL_PAGE_INFO));

	// --- 仅在第一次调用时计算PTE基址 ---
	if (g_PteBaseForAlloc == 0)
	{
		PHYSICAL_ADDRESS DirectoryTableBase;
		DirectoryTableBase.QuadPart = (__readcr3() & ~0xFFF);
		PULONG64 PML4Table = (PULONG64)MmGetVirtualForPhysical(DirectoryTableBase);
		if (!PML4Table) return 0x106; 

		for (ULONG64 index = 0; index < 512; ++index)
		{
			// 寻找自映射(self-referencing)的PML4E
			if ((PML4Table[index] & 1) && (((PML4Table[index] >> 12) & 0xFFFFFFFFFF) == (DirectoryTableBase.QuadPart >> 12)))
			{
				ULONG64 va_base = index << 39;
				// 使用符号扩展计算规范地址
				g_PteBaseForAlloc = (ULONG64)((INT64)(va_base << 16) >> 16);
				break;
			}
		}

		if (g_PteBaseForAlloc == 0) return 0x107; //未找到自映射PML4E
	}

	PVOID BaseAddress = MmAllocateMappingAddress(PAGE_SIZE, 'MyTg');
	if (!BaseAddress) return 0x119; 

	// 计算此映射地址对应的PTE的虚拟地址
	PVOID PteAddress = (PVOID)(g_PteBaseForAlloc + 8 * ((reinterpret_cast<ULONG64>(BaseAddress) & 0xFFFFFFFFFFFFi64) >> 12));

	// 使用 MmIsAddressValid 是检查内核虚拟地址是否有效的标准方法
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

// 释放中转页
inline void FreePhysicalPage(PHYSICAL_PAGE_INFO* PageInfo)
{
	if (PageInfo && PageInfo->BaseAddress)
	{
		MmFreeMappingAddress(PageInfo->BaseAddress, 'MyTg');
		memset(PageInfo, 0, sizeof(PHYSICAL_PAGE_INFO));
	}
}


inline ULONG ReadPhysicalSinglePage(const PHYSICAL_PAGE_INFO* TransferPageInfo, ULONG64 PhysAddress, PVOID Buffer, SIZE_T Size)
{

	// 修改PTE以映射到目标物理地址，保留原有的权限位（通常是可读写等）
	*(ULONG64*)TransferPageInfo->PteAddress = (PhysAddress & ~0xFFFULL) | (*(ULONG64*)TransferPageInfo->PteAddress & 0xFFF0000000000FFF) | 0x103;
	// 使TLB中的旧映射无效
	__invlpg(TransferPageInfo->BaseAddress);
	// 从映射了新物理地址的中转页中拷贝数据
	__movsb((PUCHAR)Buffer, (PUCHAR)TransferPageInfo->BaseAddress + (PhysAddress & 0xFFF), Size);
	return 0;
}
inline ULONG WritePhysicalSinglePage(const PHYSICAL_PAGE_INFO* TransferPageInfo, ULONG64 PhysAddress, PVOID Buffer, SIZE_T Size)
{

	*(ULONG64*)TransferPageInfo->PteAddress = (PhysAddress & ~0xFFFULL) | (*(ULONG64*)TransferPageInfo->PteAddress & 0xFFF0000000000FFF) | 0x103;
	__invlpg(TransferPageInfo->BaseAddress);
	__movsb((PUCHAR)TransferPageInfo->BaseAddress + (PhysAddress & 0xFFF), (PUCHAR)Buffer, Size);
	return 0;
}



// 转换为物理地址缓存页表信息并进行物理范围检查
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
		//在此进行物理范围检查
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
		//  在此进行物理范围检查
		if (!IsPhysPageInRange(*pPhysicalPageBase, *pPageSize)) return 267;
		return 0;
	}

	const ULONG64 pte_phys_addr = (pde_val & 0x000FFFFFFFFF000) + 8 * ((va_val >> 12) & 0x1FF);
	if (ReadPhysicalSinglePage(TransferPageInfo, pte_phys_addr, &pte_val, 8) || (pte_val & 1) == 0) return 265;

	*pPageSize = 0x1000; // 4KB 页
	*pPhysicalPageBase = (pte_val & 0x000FFFFFFFFF000);
	
	//在此进行物理范围检查
	if (!IsPhysPageInRange(*pPhysicalPageBase, *pPageSize)) return 268;
	return 0;
}

inline ULONG ReadPhysMemory(const PHYSICAL_PAGE_INFO* TransferPageInfo, ULONG64 DirectoryTableBase, PVOID Address, PVOID Buffer, ULONG TotalSize, STEALTH_RW_CONTEXT* Context)
{
	if (!Address || !Buffer || !TotalSize || !TransferPageInfo || !Context || !DirectoryTableBase) return 22;

	PUCHAR currentBuffer = (PUCHAR)Buffer;
	ULONG64 currentVa = (ULONG64)Address;
	ULONG bytesRemaining = TotalSize;
	ULONG status = 0;

	ULONG64 currentPagePhysBase = 0;
	ULONG64 currentPageSize = 0;
	// 追踪当前已知物理页所对应的虚拟地址范围的末尾
	ULONG64 currentPageVaEnd = currentVa;

	while (bytesRemaining > 0)
	{
		// 1. 检查当前VA是否已超出上一个已知物理页的虚拟地址范围
		if (currentVa >= currentPageVaEnd)
		{
			// 是，需要调用GetPhysPageInfoStealth重新获取物理页信息
			status = GetPhysPageInfoStealth(TransferPageInfo, DirectoryTableBase, (PVOID)currentVa, &currentPagePhysBase, &currentPageSize, Context);
			if (status != 0) return status;

			// 计算当前物理页对应的虚拟地址范围的结束地址
			currentPageVaEnd = (currentVa & ~(currentPageSize - 1)) + currentPageSize;
		}

		// 2. 计算当前VA在当前物理页中的最终物理地址
		const ULONG64 offsetInPage = currentVa & (currentPageSize - 1);
		const ULONG64 finalPhysAddr = currentPagePhysBase + offsetInPage;

		// 3. 计算本次可以在这个物理4KB页内处理多少字节
		ULONG bytesToProcess = min(bytesRemaining, PAGE_SIZE - (ULONG)(finalPhysAddr & 0xFFF));

		// 4. 执行单页物理读取
		status = ReadPhysicalSinglePage(TransferPageInfo, finalPhysAddr, currentBuffer, bytesToProcess);
		if (status != 0) return status;

		// 5. 更新计数器
		bytesRemaining -= bytesToProcess;
		currentBuffer += bytesToProcess;
		currentVa += bytesToProcess;
	}
	return 0;
}


// 统一高效循环的物理内存写入函数
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

