#pragma once

#include <windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <functional>
#include <atomic>
#include <filesystem>

/*
* Monitors files for changes
* Useful to hot-reload shaders during debugging
*/
class FileMonitor {
public:
    FileMonitor() {}

    FileMonitor(const std::wstring& filePath, std::function<void()> onFileChanged)
        : running(true), callback(onFileChanged)
    {
        monitoredFileName = std::filesystem::path(filePath).filename().wstring();
        monitoredDirectory = std::filesystem::path(filePath).parent_path().wstring();

        monitorThread = std::jthread(&FileMonitor::monitorLoop, this);
    }

    void beginMonitoring(const std::wstring& filePath, std::function<void()> onFileChanged) {
        running = true;
        callback = onFileChanged;

        monitoredFileName = std::filesystem::path(filePath).filename().wstring();
        monitoredDirectory = std::filesystem::path(filePath).parent_path().wstring();

        monitorThread = std::jthread(&FileMonitor::monitorLoop, this);
    }

    ~FileMonitor() {
        stop();
    }

    void stop() {
        if (running.exchange(false)) {
            CancelIo(hDir);
            if (monitorThread.joinable()) {
                monitorThread.join();
            }
        }
    }

private:
    void monitorLoop() {
        hDir = CreateFileW(
            monitoredDirectory.c_str(),
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            NULL
        );

        if (hDir == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to open directory handle.\n";
            return;
        }

        char buffer[1024];
        DWORD bytesReturned;
        OVERLAPPED overlapped = {};
        HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        overlapped.hEvent = hEvent;

        while (running) {
            ResetEvent(hEvent);

            if (!ReadDirectoryChangesW(
                hDir,
                &buffer,
                sizeof(buffer),
                FALSE, // non-recursive
                FILE_NOTIFY_CHANGE_LAST_WRITE,
                &bytesReturned,
                &overlapped,
                NULL))
            {
                std::cerr << "ReadDirectoryChangesW failed.\n";
                break;
            }

            DWORD waitStatus = WaitForSingleObject(hEvent, 5000); // Timeout in case of exit

            if (waitStatus == WAIT_OBJECT_0) {
                FILE_NOTIFY_INFORMATION* pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
                do {
                    std::wstring fileName(pNotify->FileName, pNotify->FileNameLength / sizeof(WCHAR));
                    if (fileName == monitoredFileName) {
                        callback(); // File modified
                    }
                } while (pNotify->NextEntryOffset != 0 &&
                    (pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(
                        reinterpret_cast<BYTE*>(pNotify) + pNotify->NextEntryOffset)));
            }
        }

        CloseHandle(hEvent);
        CloseHandle(hDir);
    }

    std::atomic<bool> running;
    std::function<void()> callback;
    std::jthread monitorThread;
    HANDLE hDir;
    std::wstring monitoredDirectory;
    std::wstring monitoredFileName;
};
