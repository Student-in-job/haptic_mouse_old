#pragma once
#include <Windows.h>
#include <commctrl.h>
#include <strsafe.h>
#include <list>
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")

#include "helpers.h"

extern int Controls;
extern HINSTANCE Instance;

HWND CreateLabel(HWND hwnd, SWindow window);
HWND CreateLabel(HWND hwnd, SWindow window, DWORD dwStyle);
HWND CreateButton(HWND hwnd, SWindow window,const wchar_t* caption, int eventCode);
void ConfigureTrackbar(HWND hwnd, int min, int max, int pagesize, int position);
void SetComboValue(HWND hWnd, const wchar_t* value);
void SetComboValue(HWND hWnd, std::string value);
void SetComboValue(HWND hWnd, std::wstring value);
void SetLabel(HWND label, const wchar_t* value);
void SetLabel(HWND label, double value);
std::list<std::string> GetImagesNames(const wchar_t* directory);
std::list<std::string> GetImagesNames(std::string directory);
void DisplayError(const  wchar_t* errorMesssage);
void LoadPicture(HDC hdc, SWindow window, wchar_t* fileName);
void LoadPicture(HDC hdc, SWindow window, std::string fileName);
HWND CreateStatusbar(HWND hWnd);
void SetStatusValue(HWND statusBar, const wchar_t* value, int part);
