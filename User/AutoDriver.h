#pragma once

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <intrin.h>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <windows.h>
#include <ntddkbd.h>
#include <ntddmou.h>

class AutoDriver
{
public:
	class SpinLock
	{
		std::atomic_flag m_locked = ATOMIC_FLAG_INIT;

		inline void pause() const noexcept
		{
			_mm_pause();
		}

	public:
		void lock() noexcept
		{
			for (int i = 0; i < 64; ++i)
			{
				if (!m_locked.test_and_set(std::memory_order_acquire))
				{
					return;
				}

				while (m_locked.test(std::memory_order_relaxed))
				{
					pause();
				}
			}

			while (m_locked.test_and_set(std::memory_order_acquire))
			{
				m_locked.wait(true, std::memory_order_relaxed);
			}
		}

		void unlock() noexcept
		{
			m_locked.clear(std::memory_order_release);
			m_locked.notify_one();
		}
	};

	enum req_op
	{
		op_o = 0,
		op_r = 1,
		op_w = 2,
		op_m = 3,
		op_j = 4,
		op_s = 5,
		op_z = 6,
		exit = 444
	};

	struct Requests
	{
		volatile LONG kernel = 0;
		volatile LONG user = 0;

		req_op Op = op_o;
		NTSTATUS status = -1;

		UINT32 TargetProcessId = 0;
		PVOID TargetAddress = nullptr;
		char UserBufferAddress[1024] = {};
		ULONG TransferSize = 0;

		char ModuleName[46] = {};
		ULONG64 ModuleBaseAddress = 0;
		ULONG64 ModuleSize = 0;

		MOUSE_INPUT_DATA MouseData = {};
		KEYBOARD_INPUT_DATA KeyboardData = {};
	};

	AutoDriver()
	{
		s_instance = this;
		InitCommunication();
	}

	~AutoDriver()
	{
		if (s_instance == this)
		{
			ExitCommunication();
			if (m_ioPacket != nullptr)
			{
				VirtualFree(m_ioPacket, 0, MEM_RELEASE);
				m_ioPacket = nullptr;
			}
			s_instance = nullptr;
		}
	}

	static void ExitCommunication()
	{
		AutoDriver *driver = GetInstance();
		if (driver == nullptr || driver->m_ioPacket == nullptr)
		{
			return;
		}

		std::scoped_lock<SpinLock> lock(driver->m_mutex);
		driver->m_ioPacket->Op = exit;
		driver->IoCommitAndWait();
	}

	static void SetTargetProcessId(int pid)
	{
		AutoDriver *driver = GetInstance();
		if (driver == nullptr)
		{
			return;
		}

		driver->m_targetPid = pid;
		if (driver->m_ioPacket == nullptr)
		{
			return;
		}

		std::printf("m_ioPacket=%p\n", static_cast<void *>(driver->m_ioPacket));
		std::printf("m_ioPacket->Op=%p\n", static_cast<void *>(&driver->m_ioPacket->Op));
		std::printf("m_ioPacket->kernel=%p\n", static_cast<void *>(const_cast<LONG *>(&driver->m_ioPacket->kernel)));
		std::printf("m_ioPacket->mouse=%p\n", static_cast<void *>(&driver->m_ioPacket->MouseData));
	}

	static int GetTargetProcessId()
	{
		AutoDriver *driver = GetInstance();
		return driver != nullptr ? driver->m_targetPid : 0;
	}

	template <typename T>
	static T Read(uintptr_t address)
	{
		T value = {};
		Read(address, &value, sizeof(T));
		return value;
	}

	static bool Read(uintptr_t address, PVOID buffer, size_t size)
	{
		AutoDriver *driver = GetInstance();
		return driver != nullptr && driver->KReadProcessMemory(address, buffer, size);
	}

	template <typename T>
	static bool Write(uintptr_t address, const T &value)
	{
		return Write(address, const_cast<T *>(&value), sizeof(T));
	}

	static bool Write(uintptr_t address, PVOID buffer, size_t size)
	{
		AutoDriver *driver = GetInstance();
		return driver != nullptr && driver->KWriteProcessMemory(address, buffer, size);
	}

	static uintptr_t GetModuleBase(const std::string &moduleName)
	{
		AutoDriver *driver = GetInstance();
		return driver != nullptr ? driver->KGetModuleBase(moduleName.c_str()) : 0;
	}

	static std::string ReadRemoteAnsiString(uintptr_t remoteAddr, size_t maxLen = 256)
	{
		if (remoteAddr == 0 || maxLen == 0)
		{
			return "";
		}

		std::vector<char> buffer(maxLen + 1, 0);
		if (!Read(remoteAddr, buffer.data(), maxLen))
		{
			return "";
		}

		buffer[maxLen] = '\0';
		return std::string(buffer.data());
	}

	static bool SendKeyboardEvent(
		USHORT UnitId,
		USHORT MakeCode,
		USHORT Flags,
		USHORT Reserved,
		ULONG ExtraInformation)
	{
		AutoDriver *driver = GetInstance();
		if (driver == nullptr || driver->m_ioPacket == nullptr)
		{
			return false;
		}

		std::scoped_lock<SpinLock> lock(driver->m_mutex);
		driver->m_ioPacket->Op = op_j;
		driver->m_ioPacket->KeyboardData.UnitId = UnitId;
		driver->m_ioPacket->KeyboardData.MakeCode = MakeCode;
		driver->m_ioPacket->KeyboardData.Flags = Flags;
		driver->m_ioPacket->KeyboardData.Reserved = Reserved;
		driver->m_ioPacket->KeyboardData.ExtraInformation = ExtraInformation;
		driver->IoCommitAndWait();
		return driver->m_ioPacket->status == 0;
	}

	static bool SendMouseEvent(
		USHORT UnitId,
		USHORT Flags,
		USHORT ButtonFlags,
		USHORT ButtonData,
		ULONG RawButtons,
		LONG dx,
		LONG dy,
		ULONG ExtraInformation)
	{
		AutoDriver *driver = GetInstance();
		if (driver == nullptr || driver->m_ioPacket == nullptr)
		{
			return false;
		}

		std::scoped_lock<SpinLock> lock(driver->m_mutex);
		driver->m_ioPacket->Op = op_s;
		driver->m_ioPacket->MouseData.UnitId = UnitId;
		driver->m_ioPacket->MouseData.Flags = Flags;
		driver->m_ioPacket->MouseData.ButtonFlags = ButtonFlags;
		driver->m_ioPacket->MouseData.ButtonData = ButtonData;
		driver->m_ioPacket->MouseData.RawButtons = RawButtons;
		driver->m_ioPacket->MouseData.LastX = dx;
		driver->m_ioPacket->MouseData.LastY = dy;
		driver->m_ioPacket->MouseData.ExtraInformation = ExtraInformation;
		driver->IoCommitAndWait();
		return driver->m_ioPacket->status == 0;
	}

	static bool TerminateTargetProcess(int pid)
	{
		AutoDriver *driver = GetInstance();
		if (driver == nullptr || driver->m_ioPacket == nullptr)
		{
			return false;
		}

		std::scoped_lock<SpinLock> lock(driver->m_mutex);
		driver->m_ioPacket->Op = op_z;
		driver->m_ioPacket->TargetProcessId = pid;
		driver->IoCommitAndWait();
		return driver->m_ioPacket->status == 0;
	}

	static void IO()
	{
		AutoDriver *driver = GetInstance();
		if (driver == nullptr || driver->m_ioPacket == nullptr)
		{
			return;
		}

		std::scoped_lock<SpinLock> lock(driver->m_mutex);
		driver->m_ioPacket->Op = op_o;
		driver->IoCommitAndWait();
	}

public:
	SpinLock m_mutex;

private:
	static constexpr size_t kBufferSize = 1024;

	inline static AutoDriver *s_instance = nullptr;

	Requests *m_ioPacket = nullptr;
	int m_targetPid = 0;

	static AutoDriver *GetInstance()
	{
		return s_instance;
	}

	void IoCommitAndWait()
	{
		InterlockedExchange(&m_ioPacket->kernel, 1);

		while (InterlockedCompareExchange(&m_ioPacket->user, 0, 1) != 1)
		{
			std::this_thread::yield();
		}

		InterlockedExchange(&m_ioPacket->user, 0);
	}

	void InitCommunication()
	{
		m_ioPacket = static_cast<Requests *>(
			VirtualAlloc(reinterpret_cast<void *>(0x20258270000), sizeof(Requests), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));

		if (m_ioPacket == nullptr)
		{
			std::printf("[-] 分配共享内存失败，错误码: %lu\n", GetLastError());
			return;
		}

		memset(m_ioPacket, 0, sizeof(Requests));

		std::printf("[+] 分配虚拟地址成功，地址: %p 大小: %zu\n", static_cast<void *>(m_ioPacket), sizeof(Requests));
		std::printf("等待驱动握手...\n");

		while (InterlockedCompareExchange(&m_ioPacket->user, 0, 1) != 1)
		{
			std::this_thread::yield();
		}

		InterlockedExchange(&m_ioPacket->user, 0);
		std::printf("驱动已经连接\n");
	}

	bool KReadProcessMemory(uintptr_t address, void *buffer, size_t size)
	{
		if (m_ioPacket == nullptr)
		{
			return false;
		}
		if (size == 0)
		{
			return true;
		}
		if (buffer == nullptr)
		{
			return false;
		}

		std::scoped_lock<SpinLock> lock(m_mutex);

		size_t processed = 0;
		while (processed < size)
		{
			const size_t chunk = (std::min)(kBufferSize, size - processed);
			m_ioPacket->Op = op_r;
			m_ioPacket->TargetProcessId = static_cast<UINT32>(m_targetPid);
			m_ioPacket->TargetAddress = reinterpret_cast<PVOID>(address + processed);
			m_ioPacket->TransferSize = static_cast<ULONG>(chunk);
			IoCommitAndWait();

			if (m_ioPacket->status != 0)
			{
				return false;
			}

			memcpy(static_cast<unsigned char *>(buffer) + processed, m_ioPacket->UserBufferAddress, chunk);
			processed += chunk;
		}

		return true;
	}

	bool KWriteProcessMemory(uintptr_t address, void *buffer, size_t size)
	{
		if (m_ioPacket == nullptr)
		{
			return false;
		}
		if (size == 0)
		{
			return true;
		}
		if (buffer == nullptr)
		{
			return false;
		}

		std::scoped_lock<SpinLock> lock(m_mutex);

		size_t processed = 0;
		while (processed < size)
		{
			const size_t chunk = (std::min)(kBufferSize, size - processed);
			m_ioPacket->Op = op_w;
			m_ioPacket->TargetProcessId = static_cast<UINT32>(m_targetPid);
			m_ioPacket->TargetAddress = reinterpret_cast<PVOID>(address + processed);
			m_ioPacket->TransferSize = static_cast<ULONG>(chunk);
			memcpy(m_ioPacket->UserBufferAddress, static_cast<unsigned char *>(buffer) + processed, chunk);
			IoCommitAndWait();

			if (m_ioPacket->status != 0)
			{
				return false;
			}

			processed += chunk;
		}

		return true;
	}

	uintptr_t KGetModuleBase(const char *moduleName)
	{
		if (m_ioPacket == nullptr)
		{
			return 0;
		}

		std::scoped_lock<SpinLock> lock(m_mutex);
		m_ioPacket->Op = op_m;
		m_ioPacket->TargetProcessId = static_cast<UINT32>(m_targetPid);
		strcpy_s(m_ioPacket->ModuleName, sizeof(m_ioPacket->ModuleName), moduleName != nullptr ? moduleName : "");
		IoCommitAndWait();
		return static_cast<uintptr_t>(m_ioPacket->ModuleBaseAddress);
	}
};
