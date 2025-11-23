#pragma once

// --- C++ 标准库 ---
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <functional>
#include <map>
#include <chrono>
#include <thread>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <cctype>  // 用于 isprint
#include <cstdint>
#include <cstring>

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

// --- Windows & C 运行时库 ---
#include <windows.h>
#include <winuser.h>
#include <windows.h>
#include <windowsx.h> // 用于 GET_X_LPARAM, GET_Y_LPARAM
#include <deque>

// --- 第三方与项目库 ---
#include "AutoDriver.h" 
#include "../capstone/include/capstone/capstone.h"
#include <random>

#pragma comment(lib, "../capstone/lib/capstone.lib")

class DriverController {
private:
	// --- RAII 资源管理辅助类 ---
	class CapstoneManager {
	public:
		csh handle = 0; // 初始化为0，表示无效句柄

		// 构造函数：初始化 Capstone
		CapstoneManager() {
			if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
				fprintf(stderr, "[!] Capstone 致命错误: 无法初始化库。\n");
				handle = 0; // 确保句柄无效
				return;
			}
			cs_option(handle, CS_OPT_DETAIL, CS_OPT_ON);
			printf("[+] Capstone 引擎已成功初始化一次。\n");
		}

		// 析构函数：在程序结束时自动调用
		~CapstoneManager() {
			if (handle) {
				cs_close(&handle);
				printf("[+] Capstone 引擎已成功释放。\n");
			}
		}
	};

public:
	// --- 构造函数 ---
	DriverController() = default;


	///  [公共接口] 运行一个精简后的基础功能交互式子菜单。
	void RunFunctionalTests() {
		if (!assureTargetProcess_()) return;

		// 精简后的菜单，移除了多余的输入模拟选项
		const std::map<int, std::pair<std::string, std::function<void()>>> menu_actions = {
			{1, {"读取内存",         [this]() { handleMemoryRead_(); }}},
			{2, {"写入内存",         [this]() { handleMemoryWrite_(); }}},
			{3, {"终止进程",         [this]() { handleProcessTermination_(); }}},
			{4, {"获取模块基址",     [this]() { handleGetModuleBase_(); }}},
		};

		bool is_running = true;
		while (is_running) {
			system("cls");
			std::cout << "================== 功能测试子菜单 ==================\n"
				<< " 目标进程 PID: " << targetPid_ << "\n\n";
			for (const auto& item : menu_actions) {
				std::cout << " [" << std::setw(2) << item.first << "] " << item.second.first << std::endl;
			}
			std::cout << " [0] 返回主菜单\n"
				<< "====================================================\n";

			int choice = 0;
			if (!promptForValue_("请选择一个选项: ", choice)) {
				system("pause");
				continue;
			}

			if (choice == 0) {
				is_running = false;
				continue;
			}

			auto it = menu_actions.find(choice);
			if (it != menu_actions.end()) {
				it->second.second();
			}
			else {
				std::cerr << "无效的选择!" << std::endl;
			}
			system("pause");
		}
	}

	///  [公共接口] 运行所有性能测试。
	void RunAllPerformanceTests() {
		const uint32_t self_pid = GetCurrentProcessId();
		std::cout << "\n正在对当前进程 (PID: " << self_pid << ") 运行性能与数据一致性测试...\n" << std::endl;
		AutoDriver::SetTargetProcessId(self_pid);

		std::random_device rd;
		std::mt19937 gen(rd());

		auto run_test = [](const std::string& name, int iterations, const std::function<void()>& test_logic, const std::function<void()>& validation = nullptr) {
			std::cout << "--- 开始测试: " << name << " (" << iterations << "次) ---\n";
			auto start_time = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < iterations; ++i) test_logic();
			auto end_time = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double, std::milli> total_ms = end_time - start_time;
			double avg_us = total_ms.count() * 1000.0 / iterations;
			std::cout << "总计耗时: " << std::fixed << std::setprecision(2) << total_ms.count() << " 毫秒\n";
			std::cout << "平均延迟: " << std::fixed << std::setprecision(4) << avg_us << " 微秒/操作\n";
			if (validation) validation();
			std::cout << "--------------------------------------------------\n\n";
			};

		run_test("空IOCTL调用 (基准)", 1000000, []() { AutoDriver::IO(); });

		const size_t ALLOC_SIZE = 1024 * 1024 * 100;
		int* test_data = (int*)(VirtualAlloc(NULL, ALLOC_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));

		if (test_data) {
			std::cout << ">>> 成功分配 " << ALLOC_SIZE / (1024 * 1024) << " MB 测试内存...\n";
			const int num_integers = ALLOC_SIZE / sizeof(int);
			std::uniform_int_distribution<int> data_distrib(INT_MIN, INT_MAX);
			for (int i = 0; i < num_integers; ++i) test_data[i] = data_distrib(gen);
			std::cout << ">>> 内存填充完毕。\n\n";

			long long mismatch_count = 0;
			run_test("随机4字节读取", 1000000, [&]() {
				std::uniform_int_distribution<int> index_distrib(0, num_integers - 1);
				int random_index = index_distrib(gen);
				uintptr_t target_address = reinterpret_cast<uintptr_t>(test_data + random_index);
				if (AutoDriver::Read<int>(target_address) != test_data[random_index]) mismatch_count++;
				}, [&]() {
					if (mismatch_count > 0) std::cerr << "!!! 验证失败: " << mismatch_count << " 次数据不匹配!!!\n";
					else std::cout << "验证成功: 所有数据均被正确读取。\n";
					});

				VirtualFree(test_data, 0, MEM_RELEASE);
		}
		else {
			std::cerr << "!!! VirtualAlloc 分配失败。无法执行内存读取相关的性能测试。\n\n";
		}

		std::cout << "所有性能测试完成。\n";
		system("pause");
	}

	/// [公共接口] 启动实时输入测试。
	void RunLiveInputTest() {
		if (!assureTargetProcess_()) return;

		std::cout << "\n--- 实时输入测试已激活 (按 'Q' 退出) ---\n"
			<< "  [左方向键]  - 模拟左键单击\n"
			<< "  [右方向键]  - 模拟右键单击\n"
			<< "  [上方向键]  - 模拟滚轮向上\n"
			<< "  [下方向键]  - 模拟滚轮向下\n"
			<< "  [K 键]        - 模拟空格键按下\n"
			<< "  [Y 键]        - 模拟鼠标拖拽 (右下方向)\n"
			<< "----------------------------------------\n";

		while (GetAsyncKeyState('Q')) {} // 确保 Q 键不是一开始就被按下的状态
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
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
			if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
				AutoDriver::SendMouseEvent(0, 0, MOUSE_WHEEL, static_cast<USHORT>(-120), 0, 0, 0, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
			if (GetAsyncKeyState('K') & 0x8000) {
				USHORT scan_code = MapVirtualKeyA(VK_SPACE, MAPVK_VK_TO_VSC);
				AutoDriver::SendKeyboardEvent(0, scan_code, KEY_MAKE, 0, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				AutoDriver::SendKeyboardEvent(0, scan_code, KEY_BREAK, 0, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			if (GetAsyncKeyState('Y') & 0x0001) {
				AutoDriver::SendMouseEvent(0, 0, MOUSE_LEFT_BUTTON_DOWN, 0, 0, 0, 0, 0);
				for (int i = 0; i < 100; i++) {
					AutoDriver::SendMouseEvent(0, MOUSE_MOVE_RELATIVE, 0, 0, 0, 1, 1, 0);
					std::this_thread::sleep_for(std::chrono::milliseconds(2));
				}
				AutoDriver::SendMouseEvent(0, 0, MOUSE_LEFT_BUTTON_UP, 0, 0, 0, 0, 0);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		std::cout << "\n实时输入测试结束。\n";
	}


	///[公共接口 ] 运行一个统一的交互式会话，用于查看和反汇编内存。
	void RunMemoryInspectionSession() {
		if (!assureTargetProcess_()) return;

		while (true) {
			system("cls");
			std::cout << "-- 内存查看/反汇编工具 (在地址提示处输入 'exit' 退出) --\n";

			uint64_t target_address;

			if (!promptForAddressAndSize_(target_address)) {
				break; // 用户输入了 'exit'
			}

			disassembleMemory_(target_address);

		}
	}

private:
	uint32_t targetPid_{ 0 };
	CapstoneManager capstone_;


	bool stringsAreEqualIgnoreCase_(const std::string& a, const std::string& b) {
		return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin(), b.end(), [](char ac, char bc) {
			return std::tolower(ac) == std::tolower(bc);
			});
	}

	template<typename T>
	bool promptForValue_(const std::string& prompt, T& value, bool is_hex_input = false) {
		std::cout << prompt;
		std::string line;
		std::getline(std::cin, line);
		if (line.empty()) {
			return false;
		}

		std::stringstream ss(line);
		std::istream& stream = is_hex_input ? ss >> std::hex : ss >> std::dec;
		stream >> value;

		if (ss.fail() || !ss.eof()) {
			std::cerr << "错误: 无效的输入格式。" << std::endl;
			return false;
		}
		return true;
	}

	bool assureTargetProcess_() {

		uint32_t pid = 0;
		if (promptForValue_("请输入目标 PID: ", pid) && pid > 0) {
			targetPid_ = pid;
			AutoDriver::SetTargetProcessId(targetPid_);
			std::cout << "目标进程已设置为: " << targetPid_ << std::endl;
			return true;
		}
		std::cerr << "错误: 未能设置有效的进程ID。\n";
		system("pause");

		return true;
	}

	bool promptForAddressAndSize_(uint64_t& address) {
		std::string line;
		std::cout << "请输入目标地址 (例如 0x7FF6B2A310C0): ";
		std::getline(std::cin, line);
		if (stringsAreEqualIgnoreCase_(line, "exit") || stringsAreEqualIgnoreCase_(line, "quit")) {
			return false;
		}

		try {
			address = std::stoull(line, nullptr, 16);
			return true;
		}
		catch (const std::exception&) {
			std::cerr << "\n错误: 无效的输入。请确保地址是十六进制，大小是数字。\n";
			return false;
		}
	}

	// 功能测试子菜单
	void handleMemoryRead_() {
		uintptr_t target_address = 0;
		if (!promptForValue_("请输入要读取的地址 (十六进制): 0x", target_address, true)) return;
		int type_choice = 0;
		if (!promptForValue_("请选择数据类型: [1] int [2] long long [3] float [4] double: ", type_choice)) return;

		std::cout << std::uppercase << std::hex;
		switch (type_choice) {
		case 1: { int val = AutoDriver::Read<int>(target_address); std::cout << "值 (int): " << std::dec << val << std::endl; break; }
		case 2: { long long val = AutoDriver::Read<long long>(target_address); std::cout << "值 (long long): " << std::dec << val << std::endl; break; }
		case 3: { float val = AutoDriver::Read<float>(target_address); std::cout << "值 (float): " << val << std::endl; break; }
		case 4: { double val = AutoDriver::Read<double>(target_address); std::cout << "值 (double): " << val << std::endl; break; }
		default: std::cerr << "无效的类型选择!" << std::endl;
		}
		std::cout << std::dec;
	}

	void handleMemoryWrite_() {
		uintptr_t target_address = 0;
		int integer_value = 0;
		if (promptForValue_("请输入要写入的地址 (十六进制): 0x", target_address, true) &&
			promptForValue_("请输入要写入的4字节整数值 (十进制): ", integer_value)) {
			std::cout << (AutoDriver::Write<int>(target_address, integer_value) ? "写入成功!" : "写入失败!") << std::endl;
		}
	}

	void handleProcessTermination_() {
		std::cout << "您确定要终止进程 " << targetPid_ << "? (y/n): ";
		std::string confirm;
		std::getline(std::cin, confirm);
		if (!confirm.empty() && std::tolower(confirm[0]) == 'y') {
			std::cout << (AutoDriver::TerminateTargetProcess(targetPid_) ? "进程已终止!" : "终止失败!") << std::endl;
		}
	}

	void handleGetModuleBase_() {
		std::string module_name;
		std::cout << "请输入模块名称: ";
		std::getline(std::cin, module_name);
		uintptr_t base_address = AutoDriver::GetModuleBase(module_name);
		if (base_address) {
			std::cout << "模块基地址: 0x" << std::hex << std::uppercase << base_address << std::dec << std::endl;
		}
		else {
			std::cerr << "获取模块基地址失败!" << std::endl;
		}
	}



	// 反汇编和内存查看器函数
	void disassembleMemory_(uint64_t address) {


		// 定义内存数据显示模式的枚举
		enum MemoryDisplayMode {
			MODE_BYTE_HEX = 1001, // 使用大于1000的ID以避免与系统命令冲突
			MODE_BYTE_DEC,
			MODE_2BYTE_HEX,
			MODE_2BYTE_DEC,
			MODE_4BYTE_HEX,
			MODE_4BYTE_DEC,
			MODE_8BYTE_HEX,
			MODE_8BYTE_DEC,
			MODE_FLOAT,
			MODE_DOUBLE
		};
		// 1. 将所有状态封装在本地结构体中
		struct AppState {
			csh capstoneHandle = 0;
			HFONT hFont = nullptr;
			uint64_t currentAddress = 0;
			size_t bufferSize = 256;
			std::vector<std::string> disassemblyText;
			const int SCROLL_INSTRUCTIONS = 2;
			std::deque<uint64_t> addressHistory;
			const int LINE_HEIGHT = 18;
			const int AVG_BYTES_PER_INSTRUCTION = 8;

			int splitterY = 300;
			bool isDraggingSplitter = false;
			std::vector<uint8_t> memoryDumpBuffer;
			const int MEM_DUMP_COLS = 16;

			// 新增状态：显示模式和菜单句柄
			MemoryDisplayMode displayMode = MODE_BYTE_HEX; // 默认模式
			HMENU hContextMenu = NULL;
		};

		// 2. 将反汇编和内存读取逻辑定义为一个 static lambda
		static auto disassemble = [](AppState& state) {
			// --- 反汇编部分 (上半部分) ---
			state.disassemblyText.clear();
			if (state.bufferSize == 0) state.bufferSize = 128;
			std::vector<uint8_t> instruction_bytes(state.bufferSize);
			if (!AutoDriver::Read(state.currentAddress, instruction_bytes.data(), instruction_bytes.size())) {
				state.disassemblyText.push_back("错误: 读取进程内存失败");
				return;
			}
			cs_insn* instructions;
			size_t count = cs_disasm(state.capstoneHandle, instruction_bytes.data(), state.bufferSize, state.currentAddress, 0, &instructions);
			if (count > 0) {
				for (size_t i = 0; i < count; ++i) {
					std::stringstream line_ss;
					line_ss << "0x" << std::hex << instructions[i].address << ":\t";
					std::stringstream bytes_ss;
					for (size_t j = 0; j < instructions[i].size; ++j) {
						bytes_ss << std::setw(2) << std::setfill('0') << std::hex << (int)instructions[i].bytes[j] << " ";
					}
					line_ss << std::left << std::setw(30) << bytes_ss.str();
					line_ss << instructions[i].mnemonic << " " << instructions[i].op_str;
					state.disassemblyText.push_back(line_ss.str());
				}
				cs_free(instructions, count);
			}
			else {
				state.disassemblyText.push_back("错误: 反汇编失败!");
			}

			// --- 读取内存数据 (下半部分) ---
			if (!state.memoryDumpBuffer.empty()) {
				if (!AutoDriver::Read(state.currentAddress, state.memoryDumpBuffer.data(), state.memoryDumpBuffer.size())) {
					std::fill(state.memoryDumpBuffer.begin(), state.memoryDumpBuffer.end(), 0x00);
				}
			}
			};

		// 3. 将窗口过程定义为一个 static lambda 
		static auto wndProc = [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT {
			AppState* pState = (AppState*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

			switch (msg) {
			case WM_CREATE: {
				CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
				pState = (AppState*)pCreate->lpCreateParams;
				SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pState);
				pState->hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Courier New"));
				if (pState->addressHistory.empty()) {
					pState->addressHistory.push_back(pState->currentAddress);
				}
				// 创建右键菜单
				pState->hContextMenu = CreatePopupMenu();
				AppendMenu(pState->hContextMenu, MF_STRING, MODE_BYTE_HEX, TEXT("字节 十六进制"));
				AppendMenu(pState->hContextMenu, MF_STRING, MODE_BYTE_DEC, TEXT("字节 十进制"));
				AppendMenu(pState->hContextMenu, MF_SEPARATOR, 0, NULL);
				AppendMenu(pState->hContextMenu, MF_STRING, MODE_2BYTE_HEX, TEXT("2字节 十六进制"));
				AppendMenu(pState->hContextMenu, MF_STRING, MODE_2BYTE_DEC, TEXT("2字节 十进制"));
				AppendMenu(pState->hContextMenu, MF_SEPARATOR, 0, NULL);
				AppendMenu(pState->hContextMenu, MF_STRING, MODE_4BYTE_HEX, TEXT("4字节 十六进制"));
				AppendMenu(pState->hContextMenu, MF_STRING, MODE_4BYTE_DEC, TEXT("4字节 十进制"));
				AppendMenu(pState->hContextMenu, MF_SEPARATOR, 0, NULL);
				AppendMenu(pState->hContextMenu, MF_STRING, MODE_8BYTE_HEX, TEXT("8字节 十六进制"));
				AppendMenu(pState->hContextMenu, MF_STRING, MODE_8BYTE_DEC, TEXT("8字节 十进制"));
				AppendMenu(pState->hContextMenu, MF_SEPARATOR, 0, NULL);
				AppendMenu(pState->hContextMenu, MF_STRING, MODE_FLOAT, TEXT("浮点"));
				AppendMenu(pState->hContextMenu, MF_STRING, MODE_DOUBLE, TEXT("双精度浮点"));
				return 0;
			}

			case WM_SIZE: {
				if (!pState) break;
				RECT clientRect;
				GetClientRect(hwnd, &clientRect);
				if (!pState->isDraggingSplitter) {
					pState->splitterY = clientRect.bottom / 2;
				}
				int topPaneHeight = pState->splitterY;
				if (topPaneHeight > pState->LINE_HEIGHT) {
					pState->bufferSize = ((topPaneHeight / pState->LINE_HEIGHT) + 2) * pState->AVG_BYTES_PER_INSTRUCTION;
				}
				int bottomPaneHeight = clientRect.bottom - pState->splitterY;
				if (bottomPaneHeight > pState->LINE_HEIGHT) {
					pState->memoryDumpBuffer.resize((bottomPaneHeight / pState->LINE_HEIGHT) * pState->MEM_DUMP_COLS);
				}
				else {
					pState->memoryDumpBuffer.clear();
				}
				disassemble(*pState);
				InvalidateRect(hwnd, NULL, TRUE);
				return 0;
			}

			case WM_CONTEXTMENU: {
				if (!pState) break;
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				ScreenToClient(hwnd, &pt);
				if (pt.y > pState->splitterY) {
					for (int i = MODE_BYTE_HEX; i <= MODE_DOUBLE; ++i) {
						CheckMenuItem(pState->hContextMenu, i, MF_BYCOMMAND | ((pState->displayMode == i) ? MF_CHECKED : MF_UNCHECKED));
					}
					TrackPopupMenu(pState->hContextMenu, TPM_LEFTALIGN | TPM_TOPALIGN, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, hwnd, NULL);
				}
				return 0;
			}

			case WM_COMMAND: {
				if (!pState) break;
				int wmId = LOWORD(wParam);
				if (wmId >= MODE_BYTE_HEX && wmId <= MODE_DOUBLE) {
					pState->displayMode = (MemoryDisplayMode)wmId;
					InvalidateRect(hwnd, NULL, TRUE);
				}
				return 0;
			}

			case WM_PAINT: {
				if (!pState) break;
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);
				SelectObject(hdc, pState->hFont);
				SetBkMode(hdc, TRANSPARENT);
				RECT clientRect;
				GetClientRect(hwnd, &clientRect);

				// 1. 绘制上半部分 (反汇编)
				int y = 10;
				for (const auto& line : pState->disassemblyText) {
					if (y + pState->LINE_HEIGHT > pState->splitterY) break;
					TextOutA(hdc, 10, y, line.c_str(), line.length());
					y += pState->LINE_HEIGHT;
				}

				// 2. 绘制分割线
				HPEN hPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
				HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
				MoveToEx(hdc, 0, pState->splitterY, NULL);
				LineTo(hdc, clientRect.right, pState->splitterY);
				SelectObject(hdc, hOldPen);
				DeleteObject(hPen);

				// 3. 绘制下半部分 (内存视图)
				y = pState->splitterY + 10;

				for (size_t i = 0; i < pState->memoryDumpBuffer.size(); i += pState->MEM_DUMP_COLS) {
					if (y + pState->LINE_HEIGHT > clientRect.bottom) break;

					//line_ss写循环内部，每次都初始化流，杜绝了状态污染的问题。
					std::stringstream line_ss;
					line_ss.str("");
					line_ss.clear();

					uint64_t lineAddress = pState->currentAddress + i;
					line_ss << "0x" << std::hex << std::setw(16) << std::setfill('0') << lineAddress << ":  ";

					std::stringstream data_ss;
					size_t type_size = 1;
					switch (pState->displayMode) {
					case MODE_2BYTE_HEX: case MODE_2BYTE_DEC: type_size = 2; break;
					case MODE_4BYTE_HEX: case MODE_4BYTE_DEC: case MODE_FLOAT: type_size = 4; break;
					case MODE_8BYTE_HEX: case MODE_8BYTE_DEC: case MODE_DOUBLE: type_size = 8; break;
					}

					for (int j = 0; j < pState->MEM_DUMP_COLS; j += type_size) {
						if (i + j + type_size > pState->memoryDumpBuffer.size()) break;
						uint8_t* data_ptr = &pState->memoryDumpBuffer[i + j];
						switch (pState->displayMode) {
						case MODE_BYTE_HEX:  data_ss << std::hex << std::setw(2) << std::setfill('0') << (int)*data_ptr << " "; break;
						case MODE_BYTE_DEC:  data_ss << std::dec << std::setw(3) << std::setfill(' ') << (int)*data_ptr << " "; break;
						case MODE_2BYTE_HEX: data_ss << std::hex << std::setw(4) << std::setfill('0') << *reinterpret_cast<uint16_t*>(data_ptr) << " "; break;
						case MODE_2BYTE_DEC: data_ss << std::dec << std::setw(5) << std::setfill(' ') << *reinterpret_cast<uint16_t*>(data_ptr) << " "; break;
						case MODE_4BYTE_HEX: data_ss << std::hex << std::setw(8) << std::setfill('0') << *reinterpret_cast<uint32_t*>(data_ptr) << "  "; break;
						case MODE_4BYTE_DEC: data_ss << std::dec << std::setw(10) << std::setfill(' ') << *reinterpret_cast<uint32_t*>(data_ptr) << "  "; break;
						case MODE_8BYTE_HEX: data_ss << std::hex << std::setw(16) << std::setfill('0') << *reinterpret_cast<uint64_t*>(data_ptr) << "  "; break;
						case MODE_8BYTE_DEC: data_ss << std::dec << std::setw(20) << std::setfill(' ') << *reinterpret_cast<uint64_t*>(data_ptr) << "  "; break;
						case MODE_FLOAT:     data_ss << std::fixed << std::setprecision(4) << std::setw(12) << std::setfill(' ') << *reinterpret_cast<float*>(data_ptr) << "  "; break;
						case MODE_DOUBLE:    data_ss << std::fixed << std::setprecision(8) << std::setw(18) << std::setfill(' ') << *reinterpret_cast<double*>(data_ptr) << "  "; break;
						}
					}
					line_ss << std::left << std::setw(50) << data_ss.str();

					if (pState->displayMode == MODE_BYTE_HEX || pState->displayMode == MODE_BYTE_DEC) {
						std::string ascii_repr;
						for (int j = 0; j < pState->MEM_DUMP_COLS && (i + j < pState->memoryDumpBuffer.size()); ++j) {
							char c = pState->memoryDumpBuffer[i + j];
							ascii_repr += isprint(static_cast<unsigned char>(c)) ? c : '.';
						}
						line_ss << ascii_repr;
					}

					std::string line = line_ss.str();
					TextOutA(hdc, 10, y, line.c_str(), line.length());
					y += pState->LINE_HEIGHT;
				}

				EndPaint(hwnd, &ps);
				return 0;
			}

			case WM_MOUSEWHEEL: {
				if (!pState) break;
				short delta = GET_WHEEL_DELTA_WPARAM(wParam);
				if (delta > 0) {
					if (pState->addressHistory.size() > 1) {
						pState->addressHistory.pop_back();
						pState->currentAddress = pState->addressHistory.back();
					}
				}
				else if (delta < 0) {
					std::vector<uint8_t> temp_buffer(64);
					AutoDriver::Read(pState->currentAddress, temp_buffer.data(), temp_buffer.size());
					cs_insn* instructions;
					size_t count = cs_disasm(pState->capstoneHandle, temp_buffer.data(), temp_buffer.size(), pState->currentAddress, pState->SCROLL_INSTRUCTIONS + 1, &instructions);
					if (count > pState->SCROLL_INSTRUCTIONS) {
						pState->currentAddress = instructions[pState->SCROLL_INSTRUCTIONS].address;
						pState->addressHistory.push_back(pState->currentAddress);
						if (pState->addressHistory.size() > 1000) pState->addressHistory.pop_front();
					}
					cs_free(instructions, count);
				}
				disassemble(*pState);
				InvalidateRect(hwnd, NULL, TRUE);
				return 0;
			}

			case WM_LBUTTONDOWN: {
				if (!pState) break;
				int yPos = HIWORD(lParam);
				if (abs(yPos - pState->splitterY) < 5) {
					pState->isDraggingSplitter = true;
					SetCapture(hwnd);
					SetCursor(LoadCursor(NULL, IDC_SIZENS));
				}
				return 0;
			}

			case WM_LBUTTONUP: {
				if (pState && pState->isDraggingSplitter) {
					pState->isDraggingSplitter = false;
					ReleaseCapture();
				}
				return 0;
			}

			case WM_MOUSEMOVE: {
				if (pState && pState->isDraggingSplitter) {
					int yPos = HIWORD(lParam);
					RECT clientRect;
					GetClientRect(hwnd, &clientRect);
					if (yPos > pState->LINE_HEIGHT * 2 && yPos < clientRect.bottom - pState->LINE_HEIGHT * 2) {
						pState->splitterY = yPos;
						SendMessage(hwnd, WM_SIZE, 0, MAKELPARAM(clientRect.right, clientRect.bottom));
					}
				}
				else if (pState) {
					int yPos = HIWORD(lParam);
					if (abs(yPos - pState->splitterY) < 5) {
						SetCursor(LoadCursor(NULL, IDC_SIZENS));
					}
				}
				return 0;
			}

			case WM_DESTROY: {
				if (pState) {
					DeleteObject(pState->hFont);
					if (pState->hContextMenu) {
						DestroyMenu(pState->hContextMenu);
					}
				}
				PostQuitMessage(0);
				return 0;
			}
			}
			return DefWindowProc(hwnd, msg, wParam, lParam);
			};

		// --- 函数的主体逻辑 ---
		AppState state;
		state.currentAddress = address;

		state.capstoneHandle = capstone_.handle;


		HINSTANCE hInstance = GetModuleHandle(NULL);
		const wchar_t CLASS_NAME[] = L"DynamicDisassemblyWindowClass";

		WNDCLASS wc = { };
		wc.lpfnWndProc = wndProc;
		wc.hInstance = hInstance;
		wc.lpszClassName = CLASS_NAME;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		RegisterClass(&wc);



		std::wstringstream ss;
		ss << std::hex << std::uppercase << AutoDriver::GetModuleBase(".exe");  // (uppercase)大写十六进制
		std::wstring windowTitle = L"动态反汇编 & 内存查看器   镜像基地址:" + ss.str();

		HWND hwnd = CreateWindowEx(
			WS_EX_COMPOSITED,
			CLASS_NAME, windowTitle.c_str(), WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT, CW_USEDEFAULT, 950, 600,
			NULL, NULL, hInstance, &state
		);

		if (hwnd == NULL) {
			cs_close(&state.capstoneHandle);
			UnregisterClass(CLASS_NAME, hInstance);
			return;
		}


		MSG msg = { };
		while (GetMessage(&msg, NULL, 0, 0) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		cs_close(&state.capstoneHandle);
		UnregisterClass(CLASS_NAME, hInstance);
	}


};