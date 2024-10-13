#ifndef PORTABLE_CANVAS_WINDOWS_ERROR_2024_02_13_H
#define PORTABLE_CANVAS_WINDOWS_ERROR_2024_02_13_H

#include <windows.h>
#include <string>
#include <string_view>

namespace pc {

std::string lastWindowsError();

void logMessage(std::string_view source, UINT message, HWND hwnd, WPARAM wParam, LPARAM lParam);

std::string windowsMessageToString(UINT message);

}

#define LOG_MESSAGE(source,message,hwnd,wParam,lParam)
#define LOG_MESSAGE(source,message,hwnd,wParam,lParam) logMessage(source,message,hwnd,wParam,lParam)
#define LOG_APP_MESSAGE(source,message,hwnd,wParam,lParam)

#endif
