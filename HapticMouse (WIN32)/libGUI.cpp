#include "libGUI.h"

#include <tchar.h>

HWND CreateLabel(HWND hwnd, SWindow window)
{
    return CreateWindowW(L"static", L"", WS_CHILD | WS_VISIBLE,
        window.x, window.y, window.width, window.heigth, hwnd, (HMENU)Controls++, Instance, NULL);
}

HWND CreateLabel(HWND hwnd, SWindow window, DWORD dwStyle)
{
    return CreateWindowW(L"static", L"", WS_CHILD | WS_VISIBLE | dwStyle,
        window.x, window.y, window.width, window.heigth, hwnd, (HMENU)Controls++, Instance, NULL);
}

HWND CreateButton(HWND hwnd, SWindow window, const wchar_t* caption, int eventCode)
{
    return CreateWindowW(L"BUTTON", caption, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        window.x, window.y, window.width, window.heigth, hwnd, (HMENU)eventCode, Instance, NULL);
}

void ConfigureTrackbar(HWND hwnd, int min, int max, int pagesize, int position)
{
    if (!position)
        position = 0;
    SendMessageW(hwnd, TBM_SETRANGE, TRUE, MAKELONG(min, max));
    SendMessageW(hwnd, TBM_SETPAGESIZE, 0, pagesize);
    SendMessageW(hwnd, TBM_SETTICFREQ, pagesize, 0);
    SendMessageW(hwnd, TBM_SETPOS, TRUE, position);
}

void SetComboValue(HWND hWnd,const wchar_t* value)
{
    SendMessageW(hWnd, (UINT) CB_ADDSTRING, (WPARAM) 0, (LPARAM)value);
}

void SetComboValue(HWND hWnd, std::string value)
{
    wchar_t* pValue = ToWChar(value.c_str());
    SetComboValue(hWnd, pValue);
}

void SetComboValue(HWND hWnd, std::wstring value)
{
    wchar_t* pValue = ToWChar(value);
    SetComboValue(hWnd, pValue);
}

void SetLabel(HWND label,const wchar_t* value)
{
    wchar_t buffer[256];
    StringCbPrintfW(buffer, sizeof(buffer) / sizeof(wchar_t), L"%s", value);
    SetWindowTextW(label, buffer);
}

void SetLabel(HWND label, double value)
{
    wchar_t buffer[256];
    StringCbPrintfW(buffer, sizeof(buffer) / sizeof(wchar_t), L"%f", value);
    SetWindowTextW(label, buffer);
}

std::list<std::string> GetImagesNames(const wchar_t* directory)
{
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA ffd;
    TCHAR szDir[MAX_PATH];
    DWORD dwError = 0;

    StringCbCopyW(szDir, MAX_PATH, directory);
    StringCbCat(szDir, MAX_PATH, TEXT("*"));

    hFind = FindFirstFile(szDir, &ffd);
    std::list<std::string> imageNames;
    
    // List all the files in the directory with some info about them.
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                std::wstring ws(ffd.cFileName);
                // your new String
                std::string str(ws.begin(), ws.end()-4);
                imageNames.push_back(str);
            }
        } while (FindNextFile(hFind, &ffd) != 0);
    }
    FindClose(hFind);
    return imageNames;
}

std::list<std::string> GetImagesNames(std::string directory)
{
    std::wstring widestr = std::wstring(directory.begin(), directory.end());
    const wchar_t* wideDir = widestr.c_str();
    return GetImagesNames(wideDir);
}

void DisplayError(const wchar_t* errorMesssage)
{
    MessageBox(NULL, (LPCTSTR)errorMesssage, TEXT("Error"), MB_OK);
}

void LoadPicture(HDC hdc, SWindow window, wchar_t* fileName)
{
    Gdiplus::Graphics graphics(hdc);
    Gdiplus::Image image(fileName);
    graphics.DrawImage(&image, window.x, window.y, window.width, window.heigth);
}

void LoadPicture(HDC hdc, SWindow window, std::string fileName)
{
    wchar_t* imagePath = ToWChar(fileName);
    LoadPicture(hdc, window, imagePath);
}

HWND CreateStatusbar(HWND hWnd)
{
    return CreateWindowEx(0, STATUSCLASSNAME, L"Ready", WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
        hWnd, NULL, NULL, NULL);
}

void SetStatusValue(HWND statusBar, const wchar_t* value, int part)
{
    SendMessage(statusBar, SB_SETTEXT, part, (LPARAM)value);
}