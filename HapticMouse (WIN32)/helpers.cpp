#include "helpers.h"
#include <chrono>

wchar_t* ToWChar(const char* input)
{
    const size_t cSize = strlen(input) + 1;
    wchar_t* output = new wchar_t[cSize];
    mbstowcs(output, input, cSize);

    return output;
}

wchar_t* ToWChar(std::wstring input)
{
    wchar_t* output = const_cast<wchar_t*>(input.c_str());
    return output;
}

wchar_t* ToWChar(std::string input)
{
    wchar_t* output = ToWChar(input.c_str());
    return output;
}

char* GetNameFromTime(std::string directory, time_t time)
{
    char timeString[100];
    strftime(timeString, 100, "%F_%H-%M-%S", localtime(&time));
    char* fileName = new char[directory.length() + 1];

    strcpy(fileName, directory.c_str());
    strcat(fileName, timeString);
    strcat(fileName, ".txt");
    return fileName;
}

char* GetNameFromTime(std::string directory)
{
    std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
    time_t time = std::chrono::system_clock::to_time_t(currentTime);
    return GetNameFromTime(directory, time);
}

std::wstring ToWideString(std::string input)
{
    std::wstring wsTmp(input.begin(), input.end());
    return wsTmp;
}

std::string ToString(std::wstring input)
{
    std::string sTmp = std::string(input.begin(), input.end());
    return sTmp; 
}

std::string ToString(wchar_t* input)
{
    std::wstring wsTmp(input);
    return ToString(wsTmp);
}