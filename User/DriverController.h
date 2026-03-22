#pragma once

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <thread>
#include <vector>
#include <windows.h>

#include "AutoDriver.h"

class DriverController {
public:
	DriverController() = default;

	void RunFunctionalTests() {
		if (!EnsureTargetProcess()) {
			return;
		}

		bool running = true;
		while (running) {
			system("cls");
			std::cout
				<< "==== Functional Tests ====\n"
				<< "Target PID: " << targetPid_ << "\n"
				<< "1. Read int\n"
				<< "2. Write int\n"
				<< "3. Terminate process\n"
				<< "4. Get module base\n"
				<< "0. Back\n"
				<< "Select: ";

			int choice = -1;
			if (!PromptValue("", choice)) {
				continue;
			}

			switch (choice) {
			case 1:
				HandleMemoryRead();
				break;
			case 2:
				HandleMemoryWrite();
				break;
			case 3:
				HandleProcessTermination();
				break;
			case 4:
				HandleGetModuleBase();
				break;
			case 0:
				running = false;
				break;
			default:
				std::cerr << "Invalid selection.\n";
				system("pause");
				break;
			}
		}
	}

	void RunAllPerformanceTests() {
		const uint32_t selfPid = GetCurrentProcessId();
		AutoDriver::SetTargetProcessId(static_cast<int>(selfPid));

		std::cout << "\nRunning basic performance tests against current process, PID " << selfPid << ".\n";

		const auto ioStart = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < 10000; ++i) {
			AutoDriver::IO();
		}
		const auto ioEnd = std::chrono::high_resolution_clock::now();

		const auto totalMs = std::chrono::duration<double, std::milli>(ioEnd - ioStart).count();
		std::cout << "IO test finished: 10000 calls in " << std::fixed << std::setprecision(2) << totalMs << " ms\n";

		int sample = 0x12345678;
		int readBack = AutoDriver::Read<int>(reinterpret_cast<uintptr_t>(&sample));
		std::cout << "Read-back check: expected 0x" << std::hex << sample
			<< ", got 0x" << readBack << std::dec << "\n";

		system("pause");
	}

	void RunLiveInputTest() {
		if (!EnsureTargetProcess()) {
			return;
		}

		std::cout
			<< "\nLive input test started.\n"
			<< "Left Arrow: left click\n"
			<< "Right Arrow: right click\n"
			<< "Up Arrow: wheel up\n"
			<< "Down Arrow: wheel down\n"
			<< "K: press Space\n"
			<< "Q: quit\n";

		while (GetAsyncKeyState('Q') & 0x8000) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		while (!(GetAsyncKeyState('Q') & 0x8000)) {
			if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
				AutoDriver::SendMouseEvent(0, 0, MOUSE_LEFT_BUTTON_DOWN, 0, 0, 0, 0, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
				AutoDriver::SendMouseEvent(0, 0, MOUSE_LEFT_BUTTON_UP, 0, 0, 0, 0, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}

			if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
				AutoDriver::SendMouseEvent(0, 0, MOUSE_RIGHT_BUTTON_DOWN, 0, 0, 0, 0, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
				AutoDriver::SendMouseEvent(0, 0, MOUSE_RIGHT_BUTTON_UP, 0, 0, 0, 0, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}

			if (GetAsyncKeyState(VK_UP) & 0x8000) {
				AutoDriver::SendMouseEvent(0, 0, MOUSE_WHEEL, 120, 0, 0, 0, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(60));
			}

			if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
				AutoDriver::SendMouseEvent(0, 0, MOUSE_WHEEL, static_cast<USHORT>(-120), 0, 0, 0, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(60));
			}

			if (GetAsyncKeyState('K') & 0x8000) {
				USHORT scanCode = static_cast<USHORT>(MapVirtualKeyA(VK_SPACE, MAPVK_VK_TO_VSC));
				AutoDriver::SendKeyboardEvent(0, scanCode, KEY_MAKE, 0, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(40));
				AutoDriver::SendKeyboardEvent(0, scanCode, KEY_BREAK, 0, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	void RunMemoryInspectionSession() {
		if (!EnsureTargetProcess()) {
			return;
		}

		while (true) {
			system("cls");
			std::cout << "==== Memory Inspection ====\n";
			std::cout << "Enter address in hex, or type exit: ";

			std::string line;
			std::getline(std::cin, line);
			if (line == "exit" || line == "quit") {
				break;
			}

			uint64_t address = 0;
			try {
				address = std::stoull(line, nullptr, 16);
			}
			catch (...) {
				std::cerr << "Invalid address.\n";
				system("pause");
				continue;
			}

			size_t size = 0;
			if (!PromptValue("Size to read: ", size) || size == 0) {
				continue;
			}

			std::vector<unsigned char> buffer(size, 0);
			if (!AutoDriver::Read(static_cast<uintptr_t>(address), buffer.data(), buffer.size())) {
				std::cerr << "Read failed.\n";
				system("pause");
				continue;
			}

			std::cout << std::hex << std::setfill('0');
			for (size_t i = 0; i < buffer.size(); ++i) {
				if (i % 16 == 0) {
					std::cout << "\n0x" << std::setw(16) << (address + i) << ": ";
				}
				std::cout << std::setw(2) << static_cast<int>(buffer[i]) << ' ';
			}
			std::cout << std::dec << "\n\n";
			system("pause");
		}
	}

private:
	uint32_t targetPid_ = 0;

	template<typename T>
	bool PromptValue(const std::string& prompt, T& value) {
		if (!prompt.empty()) {
			std::cout << prompt;
		}

		std::string line;
		std::getline(std::cin, line);
		if (line.empty()) {
			return false;
		}

		try {
			if constexpr (std::is_integral_v<T>) {
				if constexpr (std::is_signed_v<T>) {
					value = static_cast<T>(std::stoll(line, nullptr, 10));
				}
				else {
					value = static_cast<T>(std::stoull(line, nullptr, 10));
				}
			}
			else {
				return false;
			}
		}
		catch (...) {
			std::cerr << "Invalid input.\n";
			return false;
		}

		return true;
	}

	bool EnsureTargetProcess() {
		uint32_t pid = 0;
		if (!PromptValue("Target PID: ", pid) || pid == 0) {
			std::cerr << "Invalid PID.\n";
			system("pause");
			return false;
		}

		targetPid_ = pid;
		AutoDriver::SetTargetProcessId(static_cast<int>(targetPid_));
		return true;
	}

	void HandleMemoryRead() {
		std::cout << "Address (hex): ";
		std::string line;
		std::getline(std::cin, line);

		uintptr_t address = 0;
		try {
			address = static_cast<uintptr_t>(std::stoull(line, nullptr, 16));
		}
		catch (...) {
			std::cerr << "Invalid address.\n";
			system("pause");
			return;
		}

		int value = AutoDriver::Read<int>(address);
		std::cout << "Value: " << value << " (0x" << std::hex << value << std::dec << ")\n";
		system("pause");
	}

	void HandleMemoryWrite() {
		std::cout << "Address (hex): ";
		std::string line;
		std::getline(std::cin, line);

		uintptr_t address = 0;
		try {
			address = static_cast<uintptr_t>(std::stoull(line, nullptr, 16));
		}
		catch (...) {
			std::cerr << "Invalid address.\n";
			system("pause");
			return;
		}

		int value = 0;
		if (!PromptValue("Value (int): ", value)) {
			system("pause");
			return;
		}

		const bool ok = AutoDriver::Write<int>(address, value);
		std::cout << (ok ? "Write succeeded.\n" : "Write failed.\n");
		system("pause");
	}

	void HandleProcessTermination() {
		std::string confirm;
		std::cout << "Terminate PID " << targetPid_ << "? (y/n): ";
		std::getline(std::cin, confirm);
		if (!confirm.empty() && (confirm[0] == 'y' || confirm[0] == 'Y')) {
			const bool ok = AutoDriver::TerminateTargetProcess(static_cast<int>(targetPid_));
			std::cout << (ok ? "Terminate succeeded.\n" : "Terminate failed.\n");
		}
		system("pause");
	}

	void HandleGetModuleBase() {
		std::string moduleName;
		std::cout << "Module name: ";
		std::getline(std::cin, moduleName);

		const uintptr_t base = AutoDriver::GetModuleBase(moduleName);
		if (base != 0) {
			std::cout << "Module base: 0x" << std::hex << base << std::dec << "\n";
		}
		else {
			std::cout << "Module not found or query failed.\n";
		}
		system("pause");
	}
};
