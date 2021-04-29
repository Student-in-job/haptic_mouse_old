#pragma once
#include <Strsafe.h>
#include "libHaptics.h"
#include "libCSV.h"
#include <chrono>
#include <time.h>
#include <string>
#include <vector>
#include <Windows.h>
#include <stdio.h>
#include <gdiplus.h>


using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

class MainWindow
{
	public:
		static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static void CALLBACK ComSearch(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
		static std::vector<std::wstring> GetCOMPortNames(HWND hWnd);
		MainWindow();
		BOOL Create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle = 0, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
			int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT, HWND hWndParent = 0, HMENU hMenu = 0);
		HWND Window() const;
		HWND GetHWND();
	protected:
		PCWSTR ClassName() const;
		void InitTimer(HWND hWnd);
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		// GUI
		HWND m_hwnd;
		HWND label1;
		HWND label2;
		HWND labelStat;
		HWND labelMouseX;
		HWND labelMouseY;
		HMENU hMenuBar;
		HANDLE hComm;

		ULONG_PTR gdiplusToken;
		GdiplusStartupInput gdiplusStartupInput;
		// Fields
		std::chrono::high_resolution_clock::time_point timepoint;
		std::map<int, int> serialPorts;
		bool COMactivated;
		int currentPort;
		VibroFeedback vibroFeedback;
		FrequencyRange vibroRange;
		// Methods
		void CheckPortMenuItems();
		void CreateLabels(HWND hWnd);
		void CreateMenus(HWND hWnd);
		void CreateButtons(HWND hWnd);
		void SetLabelValue(HWND label, LONG value);
		int ConnectPort(int port);
		void SendMessageToCOM(char message[]);
		void OnLoadPicture(HDC hdc);
		void OnTestClick();
};