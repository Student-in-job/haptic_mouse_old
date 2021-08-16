#pragma once
#include <Strsafe.h>
#include "libHaptics.h"
#include "libCSV.h"
#include <chrono>
#include <string>
#include <time.h>
#include <vector>
#include <Windows.h>
#include <stdio.h>
#include <gdiplus.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

class MainWindow
{
	public:
		static HANDLE hComm;
		static std::thread* comThread;
		static bool comThreadInitialized;
		static bool comThreadRunning;
		static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static void CALLBACK ComSearch(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
		static std::vector<std::wstring> GetCOMPortNames(HWND hWnd);
		static void SendMessageToCOM(char message[]);
		static void ComThreadRun(void);
		MainWindow();
		BOOL Create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle = 0, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
			int nWidth = CW_USEDEFAULT, int nHeight = CW_USEDEFAULT, HWND hWndParent = 0, HMENU hMenu = 0);
		HWND Window() const;
	protected:
		PCWSTR ClassName() const;
		void InitTimer(HWND hWnd);
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		// GUI
		HWND m_hwnd;
		HWND labelPort;
		HWND labelModel;
		HWND labelSpeed;
		HWND labelMouseX;
		HWND labelMouseY;
		HMENU hMenuBar;
		HWND comboBox;
		HWND statusBar;		

		ULONG_PTR gdiplusToken;
		GdiplusStartupInput gdiplusStartupInput;
		// Fields
		std::chrono::high_resolution_clock::time_point timepoint;
		std::map<int, int> serialPorts;
		bool COMactivated;
		int currentPort;
		std::map<int, std::wstring> materialPictures;
		std::string hapticMdlPath;
		std::wstring imageDirectory;
		std::string outputDirectory;
		
		// Methods
		void CheckPortMenuItems();
		void CreateLabels(HWND hWnd);
		void CreateMenus(HWND hWnd);
		void CreateCombobox(HWND hWnd);
		void CreateStatusbar(HWND hWnd);
		void CreateButtons(HWND hWnd);
		void SetLabelValue(HWND label, LONG value);
		void SetLabelValue(HWND label, double value);
		void SetLabelValue(HWND label, wchar_t* value);
		void SetComboValue(HWND hWnd, std::wstring value);
		void SetStatusValue(std::wstring value, int part);
		int ConnectPort(int port);
		void InitWindow();
		void OnLoadPicture(HDC hdc);
		void OnStartClick();
		void OnStopClick();
		void InitPaths();
};