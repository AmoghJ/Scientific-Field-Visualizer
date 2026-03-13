#pragma once

#ifndef CONSOLE_H
#define CONSOLE_H

#include "imgui.h"
#include <string>
#include <mutex>
#include <vector>

enum LogType {
    LOG,
    ERR,
    WAR
};

struct LogItem {
    std::string msg;
    LogType logtype;

    LogItem(std::string _msg, LogType _logType) {
        msg = _msg;
        logtype = _logType;
    }
};

/*Global singleton class that receives logs and renders the console*/
class Console {
public:

    static void Log(const std::string& message, LogType logType = LogType::LOG) {
        Instance().LogInstance(message, logType);
    }

    static void Draw(const char* title = "Console") {
        Instance().DrawInstance(title);
    }

    static void Clear() {
        Instance().ClearInstance();
    }

    // Prevent copy/move
    Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;
    Console(Console&&) = delete;
    Console& operator=(Console&&) = delete;

    ~Console() {}

private:

    Console() = default;

    static Console& Instance() {
        static Console instance;
        return instance;
    }

    std::vector<LogItem> Items;
    std::mutex mutex_;

    void LogInstance(const std::string& message, LogType logType);
    void ClearInstance();
    void DrawInstance(const char* title);
};

#endif //CONSOLE_H
