#include "MainWindow.h"
#include "Shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")
#include <atlstr.h>

#define IDM_FILE_NEW   1
#define IDM_FILE_OPEN  2
#define IDM_FILE_CLOSE 3
#define IDT_TIMER  11001
#define IDB_TEST   21001

LRESULT CALLBACK MainWindow::WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    MainWindow *pThis = NULL;

    if (uMsg == WM_NCCREATE)
    {
        CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
        pThis = (MainWindow*)pCreate->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

        pThis->m_hwnd = hWnd;
    }
    else
    {
        pThis = (MainWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        
        switch (uMsg) {
            case WM_CREATE:
            {
                pThis->CreateMenus(pThis->m_hwnd);
                pThis->CreateLabels(pThis->m_hwnd);
                pThis->CreateButtons(pThis->m_hwnd);
                pThis->InitTimer(pThis->m_hwnd);
                break;
            }
            case WM_MOUSEWHEEL:
            {
                short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
                int rotations = abs((int)(zDelta / WHEEL_DELTA));
                wchar_t buffer[256];
                pThis->SetLabelValue(pThis->labelStat, rotations);
                
                // Detect number of scrolled sections of mouse wheel
                pThis->timepoint = std::chrono::high_resolution_clock::now();
                int randomInc = rand() % 100 + 1;
                Sleep(50 + randomInc);
                std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now(); 
                std::chrono::duration<double, std::milli> elapsed = now - pThis->timepoint;
                pThis->SetLabelValue(pThis->labelMouseY, Velosity::LinearVelosity(rotations, elapsed.count()));
                pThis->timepoint = std::chrono::high_resolution_clock::now();

                char message[20] = "#01";
                pThis->SendMessageToCOM(message);
                break;
            }
            case WM_COMMAND:
            {
                /*wchar_t buf[40];
                StringCbPrintfW(buf, sizeof(buf) / sizeof(wchar_t), L"%s %d", L"Menu:", LOWORD(wParam));
                MessageBoxW(0, buf, L"text", MB_OK);*/
                if ((UINT(wParam) > 1000) && (UINT(wParam) < 2000) && (UINT(wParam) != pThis->currentPort))
                {
                    int result = pThis->ConnectPort(UINT(wParam));
                    if (result == 1)
                    {
                        pThis->currentPort = UINT(wParam);
                        pThis->CheckPortMenuItems();
                        pThis->COMactivated = true;
                    }
                }

                switch (LOWORD(wParam))
                {
                    case IDM_FILE_NEW:
                    case IDM_FILE_OPEN:
                        MessageBeep(MB_ICONINFORMATION);
                        break;
                    case IDM_FILE_CLOSE:
                        SendMessage(hWnd, WM_CLOSE, 0, 0);
                        break;
                    case IDB_TEST:
                        //MessageBoxW(NULL, L"Button clicked", L"TEST", MB_OK);
                        pThis->OnTestClick();
                        break;
                    default:
                        break;
                }
                break;
            }
        }
    }
    if (pThis)
    {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

MainWindow::MainWindow() :m_hwnd(NULL)
{
    this->COMactivated = false;
    this->vibroFeedback = VibroFeedback();
}

BOOL MainWindow::Create(PCWSTR lpWindowName, DWORD dwStyle, DWORD dwExStyle, int x, int y,
    int nWidth, int nHeight, HWND hWndParent, HMENU hMenu)
{
    WNDCLASS wc = { 0 };
    
    // Initialize GDI+.
    GdiplusStartup(&this->gdiplusToken, &this->gdiplusStartupInput, NULL);

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainWindow::WindowProcedure;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName = ClassName();

    RegisterClass(&wc);

    m_hwnd = CreateWindowEx(
        dwExStyle, ClassName(), lpWindowName, dwStyle, x, y,
        nWidth, nHeight, hWndParent, hMenu, GetModuleHandle(NULL), this
    );

    return (m_hwnd ? TRUE : FALSE);
}

HWND MainWindow::Window() const { return m_hwnd; }

PCWSTR  MainWindow::ClassName() const
{
    return L"DesktopApp";
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT  ps;
    
    switch (uMsg)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            GdiplusShutdown(this->gdiplusToken);
            return 0;
        }
        case WM_PAINT:
        {
            hdc = BeginPaint(this->m_hwnd, &ps);
            this->OnLoadPicture(hdc);
            EndPaint(this->m_hwnd, &ps);
            return 0;
        }
        case WM_MOVE:
        {
            RECT rect;
            wchar_t buf[10];
            GetWindowRect(this->m_hwnd, &rect);

            StringCbPrintfW(buf, sizeof(buf) / sizeof(wchar_t), L"%ld", rect.left);
            SetWindowTextW(this->label1, buf);

            StringCbPrintfW(buf, sizeof(buf) / sizeof(wchar_t), L"%ld", rect.top);
            SetWindowTextW(this->label2, buf);
            break;
        }
        default:
            return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
}

HWND MainWindow::GetHWND()
{
    return this->m_hwnd;
}

void MainWindow::CreateLabels(HWND hWnd)
{
    CreateWindowW(L"static", L"x: ", WS_CHILD | WS_VISIBLE, 10, 300, 125, 25, hWnd, (HMENU) 1, NULL, NULL);
    this->label1 = CreateWindowW(L"static", L"150", WS_CHILD | WS_VISIBLE, 150, 300, 290, 25, hWnd, (HMENU)2, NULL, NULL);
    
    CreateWindowW(L"static", L"y: ", WS_CHILD | WS_VISIBLE, 10, 330, 125, 25, hWnd, (HMENU) 3, NULL, NULL);
    this->label2 = CreateWindowW(L"static", L"150", WS_CHILD | WS_VISIBLE, 150, 330, 290, 25, hWnd, (HMENU)4, NULL, NULL);

    CreateWindowW(L"static", L"Speed: ", WS_CHILD | WS_VISIBLE, 10, 360, 125, 25, hWnd, (HMENU)5, NULL, NULL);
    this->labelStat = CreateWindowW(L"static", L"0", WS_CHILD | WS_VISIBLE, 150, 360, 290, 25, hWnd, (HMENU)6, NULL, NULL);

    CreateWindowW(L"static", L"Mouse position x: ", WS_CHILD | WS_VISIBLE, 10, 390, 125, 25, hWnd, (HMENU)7, NULL, NULL);
    this->labelMouseX = CreateWindowW(L"static", L"0", WS_CHILD | WS_VISIBLE, 150, 390, 290, 25, hWnd, (HMENU)8, NULL, NULL);

    CreateWindowW(L"static", L"Mouse position y: ", WS_CHILD | WS_VISIBLE, 10, 420, 125, 25, hWnd, (HMENU)9, NULL, NULL);
    this->labelMouseY = CreateWindowW(L"static", L"0", WS_CHILD | WS_VISIBLE, 150, 420, 290, 25, hWnd, (HMENU)10, NULL, NULL);
}

void MainWindow::CreateMenus(HWND hWnd)
{
    HMENU hMenuBar;
    HMENU hMenu;

    hMenuBar = CreateMenu();
    hMenu = CreateMenu();

    AppendMenuW(hMenu, MFT_STRING, IDM_FILE_NEW, L"&New");
    AppendMenuW(hMenu, MFT_STRING, IDM_FILE_OPEN, L"&Open");
    AppendMenuW(hMenu, MFT_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MFT_STRING, IDM_FILE_CLOSE, L"&Close");

    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hMenu, L"&File");

    hMenu = CreateMenu();
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hMenu, L"&Port");
    this->hMenuBar = hMenuBar;

    SetMenu(hWnd, this->hMenuBar);
}

void MainWindow::CreateButtons(HWND hWnd)
{
    HWND hwndButton = CreateWindowW(L"BUTTON", L"Test", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 340, 455, 100, 25,
        hWnd, (HMENU)IDB_TEST, NULL, NULL);
};

void MainWindow::SetLabelValue(HWND label, LONG value)
{
    wchar_t buffer[256];
    StringCbPrintfW(buffer, sizeof(buffer) / sizeof(wchar_t), L"%ld", value);
    SetWindowTextW(label, buffer);
}

void MainWindow::CheckPortMenuItems()
{
    std::map<int, int>::iterator port = this->serialPorts.begin();
    while (port != this->serialPorts.end())
    {
        CheckMenuItem(this->hMenuBar, port->first, MF_UNCHECKED);
        port++;
    }
    CheckMenuItem(this->hMenuBar, this->currentPort, MF_CHECKED);
}

void CALLBACK MainWindow::ComSearch(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime) {
    MainWindow *pThis = (MainWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    HMENU hMenu = CreateMenu();
    DeleteMenu(pThis->hMenuBar, 1, MF_BYPOSITION);
    pThis->serialPorts.clear();

    std::vector<std::wstring> portsCOM = MainWindow::GetCOMPortNames(hWnd);
    int counter = 1001;
    for (int index = 0; index < (portsCOM.size()); index++)
    {
        wchar_t* port = const_cast<wchar_t*>(portsCOM[index].c_str() );
        AppendMenuW(hMenu, MFT_STRING, counter, port);
        counter++;
    }
    AppendMenuW(pThis->hMenuBar, MF_POPUP, (UINT_PTR)hMenu, L"&Port");
    DrawMenuBar(pThis->m_hwnd);
    pThis->CheckPortMenuItems();
}

void MainWindow::InitTimer(HWND hWnd)
{
    //SetTimer(this->m_hwnd, IDT_TIMER, 120000, (TIMERPROC)ComSearch);
    SetTimer(this->m_hwnd, IDT_TIMER, 5000, (TIMERPROC)ComSearch);
}

std::vector<std::wstring> MainWindow::GetCOMPortNames(HWND hWnd)
{
    MainWindow* pThis = (MainWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    std::vector<std::wstring> ports;

    for (UINT i = 1; i < 256; i++)
    {
        //Form the Raw device name
        wchar_t sPort [100];
        swprintf(sPort, 100, L"\\\\.\\COM%u",i);
        
        //Try to open the port
        bool bSuccess = false;
        HANDLE port(CreateFile(sPort, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr));
        if (port == INVALID_HANDLE_VALUE)
        {
            const DWORD dwError = GetLastError();

            //Check to see if the error was because some other app had the port open or a general failure
            if ((dwError == ERROR_ACCESS_DENIED) || (dwError == ERROR_GEN_FAILURE) || (dwError == ERROR_SHARING_VIOLATION) || (dwError == ERROR_SEM_TIMEOUT))
                bSuccess = true;
        }
        else
        {
            //The port was opened successfully
            bSuccess = true;
        }

        //Add the port number to the array which will be returned
        if (bSuccess)
        {
            swprintf(sPort, 100, L"COM%u", i);
            std::wstring portName(sPort);
            ports.push_back(portName);
            pThis->serialPorts[1000 + ports.size()] = i;
            CloseHandle(port);
        }
    }
    return ports;
}

int MainWindow::ConnectPort(int port)
{
    if (this->hComm != NULL)
    {
        CloseHandle(this->hComm);
    }
    DCB dcbserialparams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };   // initializing timeouts structure
    char serialbuffer[64] = { 0 };   // buffer to send and receive data
    wchar_t sPort[100];

    swprintf(sPort, 100, L"\\\\.\\COM%u", this->serialPorts.find(port)->second);

    this->hComm = CreateFile(sPort,    // port friendly name
    	GENERIC_READ | GENERIC_WRITE,  // read/write access
    	0,                             // no sharing, ports cant be shared
    	NULL,                          // no security
    	OPEN_EXISTING,                 // open existing port only
    	0,                             // non overlapped i/o
    	NULL);
    if (this->hComm == INVALID_HANDLE_VALUE)
    {
    	MessageBoxW(NULL, L"Port can't be opened", L"error", MB_OK);
    	return 0;
    }
    dcbserialparams.DCBlength= sizeof(dcbserialparams);
    dcbserialparams.BaudRate = CBR_9600;      //baudrate = 9600
    dcbserialparams.ByteSize = 8;             //bytesize = 8
    dcbserialparams.StopBits = ONESTOPBIT;    //stopbits = 1
    dcbserialparams.Parity = NOPARITY;      //parity = none
    bool status = SetCommState(this->hComm, &dcbserialparams);
    if (status == false)
    {
    	MessageBoxW(NULL, L"Error to setting dcb structure", L"error", MB_OK);
    	return 0;
    }
    //setting timeouts
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (SetCommTimeouts(this->hComm, &timeouts) == false)
    {
    	MessageBoxW(NULL, L"Error to setting time outs", L"error", MB_OK);
    	return 0;
    }
    return 1;
}


/// <summary>
/// Sends a message to serial port
/// </summary>
/// <param name="message"></param>
void MainWindow::SendMessageToCOM(char message[])
{
    DWORD BytesWritten = 0;
    // TODO: define message body
    //char message[] = "#01";
    BOOL Status = WriteFile(this->hComm,   // Handle to the Serialport
        message,                           // Data to be written to the port
        sizeof(message),                   // No of bytes to write into the port
        &BytesWritten,                     // No of bytes written to the port
        NULL);
    if (Status == FALSE)
    {
        MessageBoxW(NULL, L"Fail to Written", L"Error", MB_OK);
        return;
    }
};

void MainWindow::OnLoadPicture(HDC hdc)
{
    Graphics graphics(hdc);

    wchar_t directory[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, directory);
    wchar_t* pDirectory;
    pDirectory = directory;

    wchar_t folder[] = L"images";
    wchar_t* pFolder = folder;

    wchar_t filePath[MAX_PATH];
    wchar_t* pFilePath;
    pFilePath = filePath;

    //wchar_t file[] = L"wood.jpg";
    //wchar_t* pFile;
    //pFile = file;

    //wchar_t* pName = const_cast<wchar_t*>(this->vibroFeedback.MaterialPictures[Material::PAPER].c_str());
    wchar_t* pName = const_cast<wchar_t*>(this->vibroFeedback.MaterialPictures[Material::WOOD].c_str());

    wchar_t fullFilePath[MAX_PATH];
    wchar_t* pFullFilePath;
    pFullFilePath = fullFilePath;
        
    //Loading images size 800x500
    Image image(PathCombineW(pFullFilePath, PathCombineW(pFilePath, pDirectory, pFolder) , pName));
    graphics.DrawImage(&image, 10, 10, 430, 270);
};

void MainWindow::OnTestClick()
{
    libCSV lib = libCSV("test_data.csv");
    std::vector<double> values = lib.readVals();
    unsigned long count = values.size();
    
    

    wchar_t message[100];

    swprintf(message, 100, L"The array has %u  items", count);

    MessageBoxW(NULL, message, L"error", MB_OK);
};

