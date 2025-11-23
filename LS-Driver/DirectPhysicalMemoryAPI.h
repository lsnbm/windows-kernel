#pragma once
#include<ntifs.h>
#include <ntstrsafe.h> 



// 标准的物理地址掩码，用于从页表项(PTE/PDE等)中提取页帧号
// 适用于PAE和x64，覆盖了高达52位的物理地址空间
constexpr ULONG64 PFN_MASK = 0x000FFFFFFFFFF000;

//引入一个上下文结构体来缓存页表信息，避免重复的物理读取
struct VA_TRANSLATION_CONTEXT
{
	ULONG64 TargetCr3;         // 缓存目标的CR3
	ULONG64 CachedPdpPhysAddr; // 缓存的页目录指针表的物理基址
	ULONG64 CachedPdPhysAddr;  // 缓存的页目录的物理基址
	ULONG64 CachedPtPhysAddr;  // 缓存的页表的物理基址

	// 用于判断VA是否还在缓存范围内
	ULONG64 CachedPdpVaRange;
	ULONG64 CachedPdVaRange;
	ULONG64 CachedPtVaRange;
};


// 从物理地址复制数据
__forceinline NTSTATUS readphysaddress(PVOID address, PVOID buffer, SIZE_T size, SIZE_T* read)
{
	MM_COPY_ADDRESS addr;
	addr.PhysicalAddress.QuadPart = (LONGLONG)address;
	return MmCopyMemory(buffer, addr, size, MM_COPY_MEMORY_PHYSICAL, read);
}

// 从缓冲区复制数据到物理地址 (无变动，但其低效性是已知问题)
__forceinline NTSTATUS writephysaddress(PVOID address, PVOID buffer, SIZE_T size, SIZE_T* written)
{
	PHYSICAL_ADDRESS addr;
	addr.QuadPart = (LONGLONG)address;
	PVOID mapped_mem = MmMapIoSpaceEx(addr, size, PAGE_READWRITE);
	if (!mapped_mem)
	{
		if (written) *written = 0;
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	memcpy(mapped_mem, buffer, size);
	if (written) *written = size;
	MmUnmapIoSpace(mapped_mem, size);
	return STATUS_SUCCESS;
}

// 使用缓存的地址翻译函数
__forceinline ULONG64 translateaddress_cached(ULONG64 processdirbase, ULONG64 address, VA_TRANSLATION_CONTEXT* context)
{
	if (!context) return 0; // 必须提供上下文

	// 检查目标进程是否已更改，如果更改则清空缓存
	if (context->TargetCr3 != processdirbase)
	{
		memset(context, 0, sizeof(VA_TRANSLATION_CONTEXT));
		context->TargetCr3 = processdirbase;
	}

	const ULONG64 page_offset = address & (PAGE_SIZE - 1);
	const ULONG64 pte_index = (address >> 12) & 0x1FF;
	const ULONG64 pt_index = (address >> 21) & 0x1FF;
	const ULONG64 pd_index = (address >> 30) & 0x1FF;
	const ULONG64 pdp_index = (address >> 39) & 0x1FF;

	const ULONG64 pdp_va_range = address >> 39;
	const ULONG64 pd_va_range = address >> 30;
	const ULONG64 pt_va_range = address >> 21;

	SIZE_T read_size;

	// 1. PML4E -> PDPT 转换
	if (context->CachedPdpVaRange != pdp_va_range)
	{
		ULONG64 pdpe;
		NTSTATUS status = readphysaddress((void*)((processdirbase & ~0xf) + pdp_index * 8), &pdpe, sizeof(pdpe), &read_size);
		if (!NT_SUCCESS(status) || (pdpe & 1) == 0) return 0;
		context->CachedPdpPhysAddr = pdpe & PFN_MASK;
		context->CachedPdpVaRange = pdp_va_range;
		// 下级缓存必须失效
		context->CachedPdVaRange = (ULONG64)-1;
		context->CachedPtVaRange = (ULONG64)-1;
	}

	// 2. PDPTE -> PD 转换
	if (context->CachedPdVaRange != pd_va_range)
	{
		ULONG64 pde;
		NTSTATUS status = readphysaddress((void*)(context->CachedPdpPhysAddr + pd_index * 8), &pde, sizeof(pde), &read_size);
		if (!NT_SUCCESS(status) || (pde & 1) == 0) return 0;
		if ((pde & 0x80)) return (pde & 0x000FFFFFC0000000) + (address & ~0x3FFFFFFF); // 1GB大页
		context->CachedPdPhysAddr = pde & PFN_MASK;
		context->CachedPdVaRange = pd_va_range;
		// 下级缓存必须失效
		context->CachedPtVaRange = (ULONG64)-1;
	}

	// 3. PDE -> PT 转换
	if (context->CachedPtVaRange != pt_va_range)
	{
		ULONG64 pte;
		NTSTATUS status = readphysaddress((void*)(context->CachedPdPhysAddr + pt_index * 8), &pte, sizeof(pte), &read_size);
		if (!NT_SUCCESS(status) || (pte & 1) == 0) return 0;
		if ((pte & 0x80)) return (pte & 0x000FFFFFFE00000) + (address & ~0x1FFFFF); // 2MB大页
		context->CachedPtPhysAddr = pte & PFN_MASK;
		context->CachedPtVaRange = pt_va_range;
	}

	// 4. PTE -> Page 转换 (PTE本身不缓存，因为访问频率最高)
	ULONG64 final_pte;
	NTSTATUS status = readphysaddress((void*)(context->CachedPtPhysAddr + pte_index * 8), &final_pte, sizeof(final_pte), &read_size);
	if (!NT_SUCCESS(status) || (final_pte & 1) == 0) return 0;

	return (final_pte & PFN_MASK) + page_offset;
}


// 使用缓存和高效循环逻辑读函数
inline NTSTATUS ReadPhysMemoryAPI(ULONG_PTR process_dirbase, PVOID address, PVOID buffer, SIZE_T size, SIZE_T* bytes_read)
{
	if (bytes_read) *bytes_read = 0;
	if (!address || !buffer || !size || !process_dirbase) return STATUS_INVALID_PARAMETER;

	// 为此操作初始化一个缓存上下文
	VA_TRANSLATION_CONTEXT context = { 0 };

	PUCHAR current_buffer = (PUCHAR)buffer;
	ULONG64 current_va = (ULONG64)address;
	SIZE_T remaining_size = size;

	while (remaining_size > 0)
	{
		// 将当前虚拟地址翻译为物理地址
		const ULONG64 physical_address = translateaddress_cached(process_dirbase, current_va, &context);
		if (physical_address == 0) return STATUS_PARTIAL_COPY; // 表示部分成功

		//  计算可以在当前4KB页内连续读取多少字节
		SIZE_T bytes_in_page = PAGE_SIZE - (physical_address & 0xFFF);
		SIZE_T bytes_to_read = min(remaining_size, bytes_in_page);

		// 执行物理读取
		SIZE_T read_return = 0;
		NTSTATUS status = readphysaddress((void*)physical_address, current_buffer, bytes_to_read, &read_return);

		if (!NT_SUCCESS(status) || read_return == 0)
		{
			// 如果发生错误，更新总读取字节数并返回
			if (bytes_read) *bytes_read = size - remaining_size;
			return STATUS_PARTIAL_COPY;
		}

		// 更新计数器
		remaining_size -= read_return;
		current_buffer += read_return;
		current_va += read_return;
	}

	if (bytes_read) *bytes_read = size;
	return STATUS_SUCCESS;
}


inline NTSTATUS WritePhysMemoryAPI(ULONG_PTR process_dirbase, PVOID address, PVOID buffer, SIZE_T size, SIZE_T* bytes_written)
{

	if (bytes_written) *bytes_written = 0;
	if (!address || !buffer || !size || !process_dirbase) return STATUS_INVALID_PARAMETER;

	VA_TRANSLATION_CONTEXT context = { 0 };

	PUCHAR current_buffer = (PUCHAR)buffer;
	ULONG64 current_va = (ULONG64)address;
	SIZE_T remaining_size = size;

	while (remaining_size > 0)
	{
		const ULONG64 physical_address = translateaddress_cached(process_dirbase, current_va, &context);
		if (physical_address == 0) return STATUS_PARTIAL_COPY;

		SIZE_T bytes_in_page = PAGE_SIZE - (physical_address & 0xFFF);
		SIZE_T bytes_to_write = min(remaining_size, bytes_in_page);

		SIZE_T write_return = 0;
		NTSTATUS status = writephysaddress((void*)physical_address, current_buffer, bytes_to_write, &write_return);

		if (!NT_SUCCESS(status) || write_return == 0)
		{
			if (bytes_written) *bytes_written = size - remaining_size;
			return STATUS_PARTIAL_COPY;
		}

		remaining_size -= write_return;
		current_buffer += write_return;
		current_va += write_return;
	}

	if (bytes_written) *bytes_written = size;
	return STATUS_SUCCESS;
}
