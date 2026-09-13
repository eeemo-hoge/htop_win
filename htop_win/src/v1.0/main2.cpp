#include "memory2.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <tlhelp32.h>
#include <windows.h>
#include <iomanip>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include <conio.h>
#include <atomic>
#include <cctype>
#pragma comment(lib, "kernel32.lib")
using namespace std;

bool killProcess(DWORD pid) {
    HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (!hProc) return false;
    BOOL result = TerminateProcess(hProc, 1);
    CloseHandle(hProc);
    return result != FALSE;
}

int main() {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    // カーソルを非表示にしてチラつきを軽減
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(console, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(console, &cursorInfo);

    std::string statusMessage = "";
    std::string searchKeyword;
    int scrollOffset = 0;
    bool paused = false;

    while (true) {
        if (_kbhit()) {
            int ch = _getch();
            if (ch == 0 || ch == 224) { // 特殊キー（矢印キーなど）
                int arrow = _getch();
                if (arrow == 72) { // Up arrow
                    if (scrollOffset > 0) scrollOffset--;
                } else if (arrow == 80) { // Down arrow
                    scrollOffset++;
                } else if (arrow == 73) { // Page Up
                    scrollOffset = max(0, scrollOffset - 10);
                } else if (arrow == 81) { // Page Down
                    scrollOffset += 10;
                }
            } else if (ch == 'k' || ch == 'K') {
                // カーソルを再表示してPID入力
                cursorInfo.bVisible = TRUE;
                SetConsoleCursorInfo(console, &cursorInfo);

                cout << "\nEnter PID to kill (sudo kill): ";
                DWORD targetPid = 0;
                if (cin >> targetPid) {
                    if (killProcess(targetPid)) {
                        statusMessage = "Successfully killed process " + std::to_string(targetPid);
                    } else {
                        statusMessage = "Failed to kill process " + std::to_string(targetPid) + " (Error: " + std::to_string(GetLastError()) + ")";
                    }
                } else {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    statusMessage = "Invalid PID entered.";
                }

                cursorInfo.bVisible = FALSE;
                SetConsoleCursorInfo(console, &cursorInfo);
                system("cls");
            } else if (ch == 's' || ch == 'S') {
                cursorInfo.bVisible = TRUE;
                SetConsoleCursorInfo(console, &cursorInfo);

                cout << "\nSearch process keyword (empty to clear): ";
                std::getline(cin >> std::ws, searchKeyword);
                scrollOffset = 0;
                statusMessage = searchKeyword.empty()
                    ? "Process search cleared."
                    : "Searching processes for: " + searchKeyword;

                cursorInfo.bVisible = FALSE;
                SetConsoleCursorInfo(console, &cursorInfo);
                system("cls");
            } else if (ch == ' ') {
                paused = !paused;
            } else if (ch == 'q' || ch == 'Q') {
                break;
            }
        }

        // 画面の高さを取得して表示行数を決定
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(console, &csbi);
        int windowHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        int windowWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;

        // 画面全体を空白でクリアせず、カーソル位置を先頭に戻して上書き描画
        SetConsoleCursorPosition(console, { 0, 0 });

        std::ostringstream out;

        out << "--------------------" << "\n";
        out << "this program is \"htop\" of windows version " << (paused ? "[PAUSED]" : "") << "\n";

        if (!statusMessage.empty()) {
            out << "[Status] " << statusMessage << "\n";
        }

        out << "\n=== Basic Information ===" << "\n";
        MEMORYSTATUSEX memInfo;
        memInfo.dwLength = sizeof(memInfo);
        if (GlobalMemoryStatusEx(&memInfo)) {
            out << fixed << setprecision(2);
            out << "Total Memory : " << (memInfo.ullTotalPhys / (1024.0 * 1024.0 * 1024.0)) << " GB" << "\n";
            out << "Available     : " << (memInfo.ullAvailPhys / (1024.0 * 1024.0 * 1024.0)) << " GB" << "\n";
        }

        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        //cpu core count
        out << "Logical CPUs : " << sysInfo.dwNumberOfProcessors << "\n";
        //physical core count
        out << "Physical CPUs : " << sysInfo.dwNumberOfProcessors / 2 << "\n"; // assuming hyper-threading is enabled
        //cpu usage
        out << "CPU Usage : " << "N/A" << "\n"; // Placeholder for CPU usage as it requires more complex calculation   

        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32 pe32;
            pe32.dwSize = sizeof(pe32);
            std::vector<PROCESSENTRY32> processes;
            if (Process32First(hSnapshot, &pe32)) {
                do {
                    bool matches = searchKeyword.empty();
                    if (!matches) {
                        std::string executable = pe32.szExeFile;
                        std::string keyword = searchKeyword;
                        for (char& c : executable) {
                            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                        }
                        for (char& c : keyword) {
                            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
                        }
                        matches = executable.find(keyword) != std::string::npos;
                    }
                    if (matches) {
                        processes.push_back(pe32);
                    }
                } while (Process32Next(hSnapshot, &pe32));
            }
            CloseHandle(hSnapshot);

            out << "Process Count : " << processes.size();
            if (!searchKeyword.empty()) {
                out << " (matching: " << searchKeyword << ")";
            }
            out << "\n";

            out << "\n=== Processes (Offset: " << scrollOffset << ") ===" << "\n";
            out << left << setw(10) << "PID" << "| "
                << setw(10) << "PPID" << "| "
                << "Executable" << "\n";
            out << string(40, '-') << "\n";

            if (scrollOffset >= (int)processes.size()) {
                scrollOffset = max(0, (int)processes.size() - 1);
            }

            int headerLines = 14;
            int maxProcessLines = max(5, windowHeight - headerLines - 3);
            int displayCount = 0;

            for (size_t i = scrollOffset; i < processes.size() && displayCount < maxProcessLines; ++i, ++displayCount) {
                const auto& proc = processes[i];
                out << left << setw(10) << proc.th32ProcessID << "| "
                    << setw(10) << proc.th32ParentProcessID << "| "
                    << proc.szExeFile << "\n";
            }
        }

        out << "\n";
        out << "Controls: Up/Down/PgUp/PgDn: Scroll | s: Search | Space: Pause/Resume | k: Kill | q: Exit\n";

        cout << out.str() << flush;
        for (int i = 0; i < 20; ++i) {
            if (_kbhit()) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}