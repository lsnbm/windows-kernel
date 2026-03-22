#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <windows.h>
#include <tlhelp32.h>  // For CreateToolhelp32Snapshot

// 用于存储进程信息的辅助结构体
struct ProcessInfo {
    uint32_t pid;
    uint32_t parentPid;
};

//此函数的增强版能够处理一个进程（如启动器）启动另一个同名进程（如游戏主程序）的情况。
//它会优先返回那个作为子进程的PID。
uint32_t GetPidByName(const wchar_t* procName) {
    std::vector<ProcessInfo> matchingProcs;

    // 1. 获取所有进程的快照
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);

    // 2. 遍历所有进程，找出所有匹配的进程并存储信息
    if (Process32FirstW(hSnap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, procName) == 0) {
                matchingProcs.push_back({ pe.th32ProcessID, pe.th32ParentProcessID });
            }
        } while (Process32NextW(hSnap, &pe));
    }
    CloseHandle(hSnap);

    // 3. 分析结果
    if (matchingProcs.empty()) {
        return 0;
    }

    if (matchingProcs.size() == 1) {
        return matchingProcs[0].pid;
    }

    std::unordered_set<uint32_t> pids;
    for (const auto& proc : matchingProcs) {
        pids.insert(proc.pid);
    }

    for (const auto& proc : matchingProcs) {
        if (pids.count(proc.parentPid)) {
            return proc.pid;
        }
    }

    for (const auto& proc : matchingProcs) {
        bool isParent = false;
        for (const auto& otherProc : matchingProcs) {
            if (proc.pid == otherProc.parentPid) {
                isParent = true;
                break;
            }
        }
        if (!isParent) {
            return proc.pid;
        }
    }

    return 0;
}
