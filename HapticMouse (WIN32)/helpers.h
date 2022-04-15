#pragma once
#include <wchar.h>
#include <string>

#define TEXT_LINE_HEIGHT 19

struct SWindow {
	int x;
	int y;
	int width;
	int heigth;
};

wchar_t* ToWChar(const char* c);
wchar_t* ToWChar(std::wstring input);
wchar_t* ToWChar(std::string input);
char* GetNameFromTime(std::string directory);
char* GetNameFromTime(std::string directory, time_t time);
std::wstring ToWideString(std::string input);
std::string ToString(std::wstring input);
std::string ToString(wchar_t* input);