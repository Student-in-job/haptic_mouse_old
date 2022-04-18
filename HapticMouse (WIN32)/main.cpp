#pragma once

#include "helpers.h"
#include "main.h"
#include <Windows.h>
#include "libGUI.h"
//#include "libDAQmx.h"
#include "lib.h"
//
//#include <thread>
//#include <list>

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((unsigned short) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((unsigned short) 0x02)
#endif
//-------------------------------------------------------------
using namespace std;
using namespace std::chrono;
//-------------------------------------------------------------

#define IDB_START   21001
#define IDB_STOP    21002

#define SAMPLING_FREQUENCY 1000

//---------------------- Global variables ---------------------
bool      started = false;          // indicator of genaration signal thread is started
bool      running = false;          // indicator of genaration signal thread is running
double    vibrationsData[1000];     // Semples which being sent to NI DAQ
bool      modelsLoaded = false;     // Indicated did models were already loaded
string    imageDirectory;           // Directory which contains images of Materials
string    mainDir;                  // Application Directory
string    outputDirectory;          // Output directory
double    magVel;

//------------- Global WINAPI interface variables -------------
HINSTANCE Instance;
HWND      lPort;
HWND      lModel;
HWND      lVelosity;
HWND      hCombo;
HWND      status;
HWND      startButton;
int       Controls = 7000;

//----------------------- GDI+ Variables ----------------------
ULONG_PTR gdiplusToken;
Gdiplus::GdiplusStartupInput gdiplusStartupInput;

// Local variables for operations
int            frequency;                // Sampling frequency
const wchar_t* imgExt = L".jpg";         // Extensions of loaded images
string         imageFilePath = "";       // Path to the image that should be loaded

// Functions callback which receives messages from DAQ library
void SetProgress(const wchar_t* value);
void SetError(const wchar_t* value);

// Forward functions declaration
LRESULT CALLBACK Event_Callback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitPaths();
int EndThread();
int StartLoop(int mdl);
void WaitTillAllThreadsEnd();
double LinearVelosity(int rotations, double time);

/// <summary>
/// Initialize GUI controls' layout
/// </summary>
int InitInterfaceValues()
{
    margin            = 10;
    commonWidth       = windowWidth - 35;
    commonHeight      = 25;

    int staticWidth = 70;
    int labelHeight = 18;

    imageX            = margin;
    imageY            = margin;
    imageWidth        = commonWidth;
    imageHeight       = 350;

    panelX            = margin;
    panelY            = imageHeight + 2 * margin;
    panelWidth        = commonWidth;
    panelHeight       = 4 * labelHeight + 4 * margin;

    int panelWidth = commonWidth - 35;

    staticPortX       = panelX + margin;
    staticPortY       = panelY + 3 * margin;
    staticPortWidth   = staticWidth;
    staticPortHeight  = labelHeight;

    portLabelX        = staticPortX + staticPortWidth  + margin;
    portLabelY        = staticPortY;
    portLabelWidth    = panelWidth - (staticPortWidth + margin);
    portLabelHeight   = labelHeight;

    staticModelX      = panelX + margin;
    staticModelY      = staticPortY + staticPortHeight + margin;
    staticModelWidth  = staticWidth;
    staticModelHeight = labelHeight;

    modelLabelX       = staticModelX + staticModelWidth + margin;
    modelLabelY       = staticModelY;
    modelLabelWidth   = panelWidth - (staticModelWidth + margin);
    modelLabelHight   = labelHeight;

    staticVelocityX      = panelX + margin;
    staticVelocityY      = staticModelY + staticModelHeight + margin;
    staticVelocityWidth  = staticWidth;
    staticVelocityHeight = labelHeight;

    velocityLabelX       = staticVelocityX + staticVelocityWidth + margin;
    velocityLabelY       = staticVelocityY;
    velocityLabelWidth   = panelWidth - (staticVelocityWidth + margin);
    velocityLabelHeight   = labelHeight;

    int buttonsLine = panelY + panelHeight + margin;

    comboModelX          = margin;
    comboModelY          = buttonsLine;
    comboModelWidth      = commonWidth - (140 + 2 * margin);
    comboModelHeight     = 250;

    startButtonX         = comboModelX + comboModelWidth + margin;
    startButtonY         = buttonsLine;
    startButtonWidth     = 70;
    startButtonHeight    = commonHeight;

    stopButtonX          = startButtonX + startButtonWidth + margin;
    stopButtonY          = buttonsLine;
    stopButtonWidth      = 70;
    stopButtonHeight     = commonHeight;

    return 1;
}

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ INT nCmdShow)
{
    HWND hWnd;
    MSG msg;

    // Initialize GDI+.
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASS wc      = { 0 };
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = Event_Callback;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName = L"Haptic Mouse Class";

    Instance = hInstance;
    RegisterClass(&wc);

    hWnd = CreateWindow(
        wc.lpszClassName, L"Haptic Mouse App", WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX |WS_SYSMENU | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight, NULL, NULL, hInstance, NULL
    );

    RAWINPUTDEVICE rid[1];
    rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
    rid[0].dwFlags = RIDEV_CAPTUREMOUSE;
    rid[0].hwndTarget = hWnd;
    RegisterRawInputDevices(rid, 1, sizeof(rid[0]));

    ShowWindow(hWnd, nCmdShow);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 1;
}

void InitControls(HWND hwnd)
{
    HWND    hwndSinePanel = CreateWindowW(L"BUTTON", L"Main statistics", WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
        panelX, panelY, panelWidth, panelHeight, hwnd, (HMENU)Controls++, Instance, NULL);

    SWindow sPortSize = { staticPortX, staticPortY, staticPortWidth, staticPortHeight };
    HWND    staticPort = CreateLabel(hwnd, sPortSize, SS_RIGHT);
    SetLabel(staticPort, L"Port:");

    SWindow lPortSize = { portLabelX, portLabelY, portLabelWidth, portLabelHeight };
    lPort = CreateLabel(hwnd, lPortSize);

    SWindow sModelSize = { staticModelX, staticModelY, staticModelWidth, staticModelHeight };
    HWND    sModel = CreateLabel(hwnd, sModelSize, SS_RIGHT);
    SetLabel(sModel, L"Model:");

    SWindow modelSize = { modelLabelX, modelLabelY, modelLabelWidth, modelLabelHight };
    lModel = CreateLabel(hwnd, modelSize);
    
    SWindow sVelositySize = { staticVelocityX, staticVelocityY, staticVelocityWidth, staticVelocityHeight };
    HWND    sVelosity = CreateLabel(hwnd, sVelositySize, SS_RIGHT);
    SetLabel(sVelosity, L"Velocity:");

    SWindow VelositySize = { velocityLabelX, velocityLabelY, velocityLabelWidth, velocityLabelHeight };
    lVelosity = CreateLabel(hwnd, VelositySize);

    SWindow startButtonSize = { startButtonX, startButtonY, startButtonWidth, startButtonHeight };
    startButton = CreateButton(hwnd, startButtonSize, L"Start", IDB_START);

    SWindow stopButtonSize = { stopButtonX, stopButtonY, stopButtonWidth, stopButtonHeight };
    HWND stopButton = CreateButton(hwnd, stopButtonSize, L"Stop", IDB_STOP);

    hCombo = CreateWindowW(L"Combobox", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL | CBS_AUTOHSCROLL,
        comboModelX, comboModelY, comboModelWidth, comboModelHeight, hwnd, (HMENU)Controls++, Instance, NULL);

    status = CreateStatusbar(hwnd);
}

void InitCombobox()
{
    list<string> images = GetImagesNames(imageDirectory);
    for (std::list<string>::iterator it = images.begin(); it != images.end(); ++it)
    {
        SetComboValue(hCombo, *it);
    }
}

LRESULT CALLBACK Event_Callback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
        case WM_CREATE: // Initialization of interface and application
        {
            InitPaths();
            InitInterfaceValues();
            InitControls(hWnd);
            InitCombobox();
            break;
        }
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
                case IDB_START:
                {
                    int model = SendMessage(hCombo, CB_GETCURSEL, NULL, NULL);
                    if (model == CB_ERR)
                        DisplayError(L"Model has not being chosen!\nPlease choose model.");
                    else
                        StartLoop(model + 1);
                    EnableWindow(startButton, false);
                    EnableWindow(hCombo, false);
                    break;
                }
                case IDB_STOP:
                {
                    EndThread();
                    EnableWindow(startButton, true);
                    EnableWindow(hCombo, true);
                    break;
                }
            }
            if (HIWORD(wParam) == CBN_SELCHANGE)
            {
                wchar_t strText[100];
                int index = SendMessage(hCombo, CB_GETCURSEL, NULL, NULL);
                SendMessage(hCombo, CB_GETLBTEXT, index, (LPARAM)strText);
                string selectedString = ToString(strText);
                size_t found = selectedString.find("-");
                int len = selectedString.size() - found;
                SetLabel(lModel, ToWChar(selectedString.substr(found + 1, len)));
                imageFilePath = imageDirectory + selectedString + ToString(imgExt);
                RECT rect = { imageX, imageY, imageX + imageWidth, imageY + imageHeight };
                InvalidateRect(hWnd, &rect, TRUE);
            }
            break;
        }
        case WM_PAINT:
        {
            if (!imageFilePath.empty()) {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);
                SWindow imageSize = { imageX, imageY, imageWidth, imageHeight };
                LoadPicture(hdc, imageSize, imageFilePath);
                EndPaint(hWnd, &ps);
            }
            break;
        }
        case WM_MOUSEWHEEL: //Aproximation of speed of scrolling the mouse wheel
        {
            short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            int rotations = abs((int)(zDelta / WHEEL_DELTA));
            
            // Detect number of scrolled sections of mouse wheel
            high_resolution_clock::time_point timepoint = high_resolution_clock::now();
            int randomInc = rand() % 100 + 1;
            Sleep(50 + randomInc);
            high_resolution_clock::time_point now = high_resolution_clock::now();
            duration<double, milli> elapsed = now - timepoint;

            //Change the magniture Velocity
            magVel = LinearVelosity(rotations, elapsed.count());
            SetLabel(lVelosity, magVel);

            break;
        }
        case WM_DESTROY:
        {
            if (started)
                EndThread();
            WaitTillAllThreadsEnd();
            WaitForLibClose();
            PostQuitMessage(0);
            Gdiplus::GdiplusShutdown(gdiplusToken);
            break;
        }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void InitPaths()
{
    wchar_t directory[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, directory);
    mainDir = ToString(directory);

    outputDirectory = mainDir + "\\output\\";
    imageDirectory = mainDir + "\\resources\\images\\";
}

int EndThread()
{
    started = false;
    StopLib();
    return 1;
}

int StartLoop(int mdl)
{
    try
    {
        if (started)
            return 0;
        else
        {
            thread libData(startLib, (char*)mainDir.c_str(), mdl);
            libData.detach();
            SetStatusValue(status, L"Started haptics Lab", 0);
        }        
    }
    catch (const std::exception&)
    {
        return 0;
    }
    return 1;
}

void WaitTillAllThreadsEnd()
{
    while (running) {}
}

double LinearVelosity(int rotations, double time)
{
    double rad_angle = (rotations * ROTATION_ANGLE) / LOGITECH_STEPS_COUNT * RADIAN_ANGLE;
    double angle_velosity = rad_angle / time;
    double velosity = MOUSE_DIAMETER / 2 * angle_velosity;
    return velosity;
};

void SetProgress(const wchar_t* value)
{
    SetStatusValue(status, value, 0);
}
void SetError(const wchar_t* value)
{
    DisplayError(value);
}