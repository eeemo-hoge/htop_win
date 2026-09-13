#ifndef MEMORY_H
#define MEMORY_H

#include <string>
#include <vector>
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

class Process {
public:
    DWORD processId;
    std::string processName;
    SIZE_T processMemory;

    Process(DWORD pid, const std::string& processName, SIZE_T processMemory)
        : processId(pid), processName(processName), processMemory(processMemory) {}

    void updateMemory() {
        HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (handle == nullptr) {
            processMemory = 0;
            return;
        }

        PROCESS_MEMORY_COUNTERS pmc{};
        if (GetProcessMemoryInfo(handle, &pmc, sizeof(pmc))) {
            processMemory = pmc.WorkingSetSize;
        }

        CloseHandle(handle);
    }

    SIZE_T getMemory() const {
        return processMemory;
    }

    void showMemory() const {
        double memoryInMB = static_cast<double>(processMemory) / (1024.0 * 1024.0);
        std::cout << "Process ID: " << processId << std::endl;
        std::cout << "Process Name: " << processName << std::endl;
        std::cout << "Process Memory: " << processMemory << " bytes (" << memoryInMB << " MB)" << std::endl;
    }
};

class ProcessManager {
private:
    std::vector<Process> processes;

public:
    void scanProcesses() {
        processes.clear();
        HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hProcessSnap == INVALID_HANDLE_VALUE) {
            return;
        }

        PROCESSENTRY32 pe32{};
        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hProcessSnap, &pe32)) {
            do {
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
                SIZE_T memory = 0;

                if (hProcess) {
                    PROCESS_MEMORY_COUNTERS pmc{};
                    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                        memory = pmc.WorkingSetSize;
                    }
                    CloseHandle(hProcess);
                }

                processes.emplace_back(pe32.th32ProcessID, pe32.szExeFile, memory);
            } while (Process32Next(hProcessSnap, &pe32));
        }

        CloseHandle(hProcessSnap);
    }

    void updateAllProcessesMemory() {
        for (auto& process : processes) {
            process.updateMemory();
        }
    }

    void showAllProcesses() const {
        for (const auto& process : processes) {
            process.showMemory();
        }
    }

    void showSeparator() const {
        std::cout << "--------------------" << std::endl;
    }
};

#endif