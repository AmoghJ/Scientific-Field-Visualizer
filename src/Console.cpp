#include "Console.h"
#include <iostream>

void Console::ClearInstance() {
    Items.clear();
}

//Mutex locking to enable logging from another thread
void Console::LogInstance(const std::string& str, LogType logType) {
    mutex_.lock();
    Items.push_back(LogItem(str, logType));
    mutex_.unlock();
}

//Helper function to get color for message based on logtype
ImVec4 getMsgColor(LogType lt) {

    switch (lt) {
    case LOG:
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        break;

    case ERR:
        return ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        break;

    case WAR:
        return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
        break;

    default:
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        break;
    }
}

//Helper function to get color for status based on logtype
ImVec4 getStatColor(LogType lt) {

    switch (lt) {
    case LOG:
        return ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        break;

    case ERR:
        return ImVec4(0.75f, 0.0f, 0.0f, 1.0f);
        break;

    case WAR:
        return ImVec4(0.75f, 0.75f, 0.0f, 1.0f);
        break;

    default:
        return ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        break;
    }
}

//Prefixes for log output based on type
std::string getStatMessage(LogType lt) {

    switch (lt) {
    case LOG:
        return "[LOG] ";
        break;

    case ERR:
        return "[ERROR] ";
        break;

    case WAR:
        return "[WARNING] ";
        break;

    default:
        return "[LOG] ";
        break;
    }
}

void Console::DrawInstance(const char* title) {

    mutex_.lock();
    ImGui::Begin(title);

    ImGui::SameLine();
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate); //Display FPS in console
    ImGui::Separator();

    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    for (int i = 0; i < Items.size(); i++) {

        ImGui::PushStyleColor(ImGuiCol_Text, getStatColor(Items[i].logtype));
        ImGui::SameLine(0.0f, 0.0f); // No spacing between segments
        ImGui::TextUnformatted(getStatMessage(Items[i].logtype).c_str());
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Text, getMsgColor(Items[i].logtype));
        ImGui::SameLine(0.0f, 0.0f); // No spacing between segments
        ImGui::TextUnformatted(Items[i].msg.c_str());
        ImGui::PopStyleColor();

        ImGui::NewLine();
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();

    ImGui::End();

    mutex_.unlock();
}