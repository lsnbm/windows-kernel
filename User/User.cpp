#include <exception>
#include <iostream>
#include <string>
#include <windows.h>

#include "AutoDriver.h"
#include "DriverController.h"

static void ShowMainMenu() {
	system("cls");
	std::cout
		<< "==== LS Driver Console ====\n"
		<< "1. Functional tests\n"
		<< "2. Performance tests\n"
		<< "3. Live input test\n"
		<< "4. Memory inspection\n"
		<< "0. Exit\n"
		<< "Select: ";
}

static BOOL WINAPI ConsoleHandler(DWORD eventType) {
	switch (eventType) {
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
		std::cout << "Console interrupt ignored.\n";
		return TRUE;
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		std::cout << "Console is closing, exiting driver communication.\n";
		AutoDriver::ExitCommunication();
		return FALSE;
	default:
		return FALSE;
	}
}

int main() {
	AutoDriver driver;

	if (!SetConsoleCtrlHandler(ConsoleHandler, TRUE)) {
		std::cerr << "Failed to install console handler.\n";
		return 1;
	}

	try {
		DriverController controller;
		bool running = true;

		while (running) {
			ShowMainMenu();

			std::string line;
			std::getline(std::cin, line);

			int choice = -1;
			try {
				if (!line.empty()) {
					choice = std::stoi(line);
				}
			}
			catch (...) {
				choice = -1;
			}

			switch (choice) {
			case 1:
				controller.RunFunctionalTests();
				break;
			case 2:
				controller.RunAllPerformanceTests();
				break;
			case 3:
				controller.RunLiveInputTest();
				break;
			case 4:
				controller.RunMemoryInspectionSession();
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
	catch (const std::exception& ex) {
		std::cerr << "Unhandled exception: " << ex.what() << '\n';
		system("pause");
		return 1;
	}

	std::cout << "Exiting.\n";
	return 0;
}
