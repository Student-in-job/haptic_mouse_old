#include "MainWindow.h"
#include "Shlwapi.h"
#pragma comment(lib, "Shlwapi.lib")
#include <atlstr.h>
#include <CommCtrl.h>

#define IDM_FILE_NEW   10001
#define IDM_FILE_OPEN  10002
#define IDM_FILE_CLOSE 10003
#define IDT_TIMER  11001
#define IDB_START  21001
#define IDB_STOP   21002

std::string resourceRoot;
#define RESOURCE_PATH(p)    (char*)((resourceRoot+std::string(p)).c_str())
#define OUTPUT_PATH(p)    (char*)((resourceRoot+std::string(p)).c_str())

HANDLE MainWindow::hComm;
std::thread* MainWindow::comThread;
bool MainWindow::comThreadInitialized;
bool MainWindow::comThreadRunning;

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
            case WM_CREATE: // Initialization of interface and application
            {
                pThis->CreateMenus(pThis->m_hwnd);
                pThis->CreateLabels(pThis->m_hwnd);
                pThis->CreateCombobox(pThis->m_hwnd);
                pThis->CreateButtons(pThis->m_hwnd);
                pThis->CreateStatusbar(pThis->m_hwnd);
                pThis->InitWindow();
                pThis->InitTimer(pThis->m_hwnd);
                break;
            }
            case WM_MOUSEWHEEL: //Aproximation of speed of scrolling the mouse wheel
            {
                short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
                int rotations = abs((int)(zDelta / WHEEL_DELTA));
                /*wchar_t buffer[256];
                pThis->SetLabelValue(pThis->labelSpeed, rotations);*/
                
                // Detect number of scrolled sections of mouse wheel
                pThis->timepoint = std::chrono::high_resolution_clock::now();
                int randomInc = rand() % 100 + 1;
                Sleep(50 + randomInc);
                std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now(); 
                std::chrono::duration<double, std::milli> elapsed = now - pThis->timepoint;
                
                //Change the magniture Velocity
                magVel = LinearVelosity(rotations, elapsed.count());
                pThis->SetLabelValue(pThis->labelSpeed, magVel);

                break;
            }
            case WM_COMMAND:
            {
                // Check the chosen port from menu
                if ((UINT(wParam) > 1000) && (UINT(wParam) < 2000) && (UINT(wParam) != pThis->currentPort))
                {
                    int result = pThis->ConnectPort(UINT(wParam));
                    if (result == 1)
                    {
                        pThis->currentPort = UINT(wParam);
                        pThis->CheckPortMenuItems();
                        pThis->COMactivated = true;

                        int port = pThis->serialPorts.find(UINT(wParam))->second;
                        wchar_t format[100];
                        wchar_t* message = format;
                        swprintf(format, sizeof(format) / sizeof(wchar_t), L"COM%d port is connected", port);
                        pThis->SetLabelValue(pThis->labelPort, message);
                    }
                }

                if (HIWORD(wParam) == CBN_SELCHANGE)
                {
                    int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL,
                        (WPARAM)0, (LPARAM)0);
                    mdl = ItemIndex + 1;

                    InvalidateRect(pThis->m_hwnd, NULL, TRUE);

                    /*TCHAR  ListItem[256];
                    (TCHAR)SendMessage((HWND)lParam, (UINT)CB_GETLBTEXT,
                        (WPARAM)ItemIndex, (LPARAM)ListItem);
                    MessageBox(NULL, (LPCWSTR)ListItem, TEXT("Item Selected"), MB_OK);*/
                }

                // Events of clicking menu and buttons
                switch (LOWORD(wParam))
                {
                    case IDM_FILE_NEW:
                        break;
                    case IDM_FILE_OPEN:
                        MessageBeep(MB_ICONINFORMATION);
                        break;
                    case IDM_FILE_CLOSE:
                        atexit(WaitForLibClose);
                        SendMessage(hWnd, WM_CLOSE, 0, 0);
                        break;
                    case IDB_START:
                        pThis->OnStartClick();
                        break;
                    case IDB_STOP:
                        pThis->OnStopClick();
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
        // Other events
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
    materialPictures = InitPictures();
    this->InitPaths();
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

PCWSTR MainWindow::ClassName() const
{
    return L"DesktopApp";
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT  ps;
    
    switch (uMsg)
    {
        case WM_DESTROY: // Event: Closing app
        {
            PostQuitMessage(0);
            GdiplusShutdown(this->gdiplusToken);
            simulationRunning = false;
            return 0;
        }
        case WM_PAINT: // Event: Repainting window of app
        {
            hdc = BeginPaint(this->m_hwnd, &ps);
            this->OnLoadPicture(hdc);
            EndPaint(this->m_hwnd, &ps);
            return 0;
        }
        //case WM_MOVE:  // Event: Changing window position
        //{
        //    RECT rect;
        //    wchar_t buf[10];
        //    GetWindowRect(this->m_hwnd, &rect);

        //    StringCbPrintfW(buf, sizeof(buf) / sizeof(wchar_t), L"%ld", rect.left);
        //    SetWindowTextW(this->label1, buf);

        //    StringCbPrintfW(buf, sizeof(buf) / sizeof(wchar_t), L"%ld", rect.top);
        //    SetWindowTextW(this->label2, buf);
        //    break;
        //}
        case WM_SIZE: // Event: Resizing window
        {
            SendMessage(this->statusBar, WM_SIZE, 0, 0);
            break;
        }
        default:
            return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
}

void MainWindow::CreateLabels(HWND hWnd)
{
    CreateWindowW(L"static", L"Port: ", WS_CHILD | WS_VISIBLE, 10, 300, 125, 25, hWnd, (HMENU) 1, NULL, NULL);
    this->labelPort = CreateWindowW(L"static", L"None", WS_CHILD | WS_VISIBLE, 150, 300, 290, 25, hWnd, (HMENU)2, NULL, NULL);
    
    CreateWindowW(L"static", L"Model: ", WS_CHILD | WS_VISIBLE, 10, 330, 125, 25, hWnd, (HMENU) 3, NULL, NULL);
    this->labelModel = CreateWindowW(L"static", L"Not chosen", WS_CHILD | WS_VISIBLE, 150, 330, 290, 25, hWnd, (HMENU)4, NULL, NULL);

    CreateWindowW(L"static", L"Velocity: ", WS_CHILD | WS_VISIBLE, 10, 360, 125, 25, hWnd, (HMENU)5, NULL, NULL);
    this->labelSpeed = CreateWindowW(L"static", L"0", WS_CHILD | WS_VISIBLE, 150, 360, 290, 25, hWnd, (HMENU)6, NULL, NULL);

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

void MainWindow::CreateCombobox(HWND hWnd)
{
    HWND hwndCombo = CreateWindowW(L"Combobox", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, 10, 455, 325, 50,
        hWnd, NULL, NULL, NULL);
     this->comboBox = hwndCombo;
}

void MainWindow::CreateStatusbar(HWND hWnd)
{
    HWND hwndSatusbar = CreateWindowEx(0, STATUSCLASSNAME, L"Ready", WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
        hWnd, NULL, NULL, NULL);
    //int iStatusWidths[] = { 80, 200, -1 };
    //SendMessage(hwndSatusbar, SB_SETPARTS, 3, (LPARAM)iStatusWidths);
    this->statusBar = hwndSatusbar;
}

void MainWindow::CreateButtons(HWND hWnd)
{
    HWND hwndButtonStart = CreateWindowW(L"BUTTON", L"Start", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 340, 455, 100, 25,
        hWnd, (HMENU)IDB_START, NULL, NULL);
    HWND hwndButtonStop = CreateWindowW(L"BUTTON", L"Stop", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 340, 485, 100, 25,
        hWnd, (HMENU)IDB_STOP, NULL, NULL);
};

void MainWindow::SetLabelValue(HWND label, LONG value)
{
    wchar_t buffer[256];
    StringCbPrintfW(buffer, sizeof(buffer) / sizeof(wchar_t), L"%d", value);
    SetWindowTextW(label, buffer);
}

void MainWindow::SetLabelValue(HWND label, double value)
{
    wchar_t buffer[256];
    StringCbPrintfW(buffer, sizeof(buffer) / sizeof(wchar_t), L"%f", value);
    SetWindowTextW(label, buffer);
}

void MainWindow::SetLabelValue(HWND label, wchar_t* value)
{
    wchar_t buffer[256];
    StringCbPrintfW(buffer, sizeof(buffer) / sizeof(wchar_t), L"%s", value);
    SetWindowTextW(label, buffer);
}

void MainWindow::SetComboValue(HWND hWnd, std::wstring value)
{
    wchar_t* pValue = const_cast<wchar_t*>(value.c_str());
    SendMessageW(hWnd, CB_ADDSTRING, 0, (LPARAM)pValue);
}

void MainWindow::SetStatusValue(std::wstring value, int part)
{
    wchar_t* pValue = const_cast<wchar_t*>(value.c_str());
    SendMessage(this->statusBar, SB_SETTEXT, part, (LPARAM)pValue);
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
    if (MainWindow::hComm != NULL)
    {
        CloseHandle(MainWindow::hComm);
    }
    DCB dcbserialparams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };   // initializing timeouts structure
    char serialbuffer[64] = { 0 };   // buffer to send and receive data
    wchar_t sPort[100];

    swprintf(sPort, 100, L"\\\\.\\COM%u", this->serialPorts.find(port)->second);

    MainWindow::hComm = CreateFile(sPort,    // port friendly name
    	GENERIC_READ | GENERIC_WRITE,        // read/write access
    	0,                                   // no sharing, ports cant be shared
    	NULL,                                // no security
    	OPEN_EXISTING,                       // open existing port only
    	0,                                   // non overlapped i/o
    	NULL);
    if (MainWindow::hComm == INVALID_HANDLE_VALUE)
    {
    	MessageBoxW(NULL, L"Port can't be opened", L"error", MB_OK);
    	return 0;
    }
    dcbserialparams.DCBlength= sizeof(dcbserialparams);
    dcbserialparams.BaudRate = CBR_115200;    //baudrate = 115200
    dcbserialparams.ByteSize = 8;             //bytesize = 8
    dcbserialparams.StopBits = ONESTOPBIT;    //stopbits = 1
    dcbserialparams.Parity = NOPARITY;        //parity = none
    bool status = SetCommState(MainWindow::hComm, &dcbserialparams);
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
    if (SetCommTimeouts(MainWindow::hComm, &timeouts) == false)
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
    BOOL Status = WriteFile(MainWindow::hComm,   // Handle to the Serialport
        message,                                 // Data to be written to the port
        sizeof(message),                         // No of bytes to write into the port
        &BytesWritten,                           // No of bytes written to the port
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
    
    wchar_t pDefault[] = L"default.jpg";
    wchar_t* pImage;

    wchar_t* pName;
    if (mdl == -1)
    {
        pName = pDefault;
    }
    else
    {
        pImage = const_cast<wchar_t*>(this->materialPictures[mdl].c_str());
        pName = pImage;
    }
    //wchar_t* pName = const_cast<wchar_t*>(this->materialPictures[Material::WOOD].c_str());

    wchar_t fullFilePath[MAX_PATH];
    wchar_t* pFullFilePath;
    pFullFilePath = fullFilePath;
    
    Image image(PathCombineW(pFullFilePath, this->imageDirectory.c_str(), pName));
    graphics.DrawImage(&image, 10, 10, 430, 270);
};

void MainWindow::InitWindow()
{
    // Load types of models
    std::map<int, std::wstring>::iterator it = this->materialPictures.begin();
    while (it != this->materialPictures.end())
    {
        // Accessing KEY from element pointed by it.
        std::wstring value = it->second;
        // Accessing VALUE from element pointed by it.
        int key = it->first;
        this->SetComboValue(this->comboBox, value.substr(0, value.size()-4).c_str()); //Remove dot with file extension
        it++;
    }

    size_t hight = 50 * (this->materialPictures.size() + 1);
    SetWindowPos(this->comboBox, NULL, 10, 455, 325, hight, NULL);
    SendMessage(this->comboBox, CB_SETCURSEL, 0, 0);
    MainWindow::comThreadInitialized = false;
    MainWindow::comThreadRunning = false;
}

void MainWindow::OnStartClick()
{
    /*
    libCSV lib = libCSV("test_data.csv");
    std::vector<double> values = lib.readVals();
    unsigned long count = values.size();
    
    wchar_t message[100];
    swprintf(message, 100, L"The array has %u  items", count);

    MessageBoxW(NULL, message, L"error", MB_OK);
    */
    LoadModel(this->hapticMdlPath, mdl);

    MainWindow::comThreadRunning = true;
    MainWindow::comThread = new std::thread(MainWindow::ComThreadRun);
    //MainWindow::comThread = comThread;
    MainWindow::comThread->detach();

    // TODO: start two threads with model results

    simulationRunning = true;
    simulationFinished = false;

    std::thread hapticsThread(updateHaptics);
    hapticsThread.detach();
    std::thread vibroGenThread(updateVibrationPattern);
    vibroGenThread.detach();
    atexit(WaitForLibClose);
};

void MainWindow::OnStopClick()
{
    /*MainWindow::comThreadRunning = false;
    simulationRunning = false;
    simulationFinished = true;*/
    std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(currentTime);
    //std::string timeString = std::format("%F %T", std::chrono::system_clock::now());
    char timeString[100];
    std::strftime(timeString, 100, "%F_%H-%M-%S", localtime(&time));

    //char* timeString = ctime(&time);

    char* fileName = new char[this->outputDirectory.length() + 1];
    strcpy(fileName,  this->outputDirectory.c_str());

    strcat(fileName, timeString);
    strcat(fileName, ".txt");

    libCSV lib = libCSV("test_data.csv");
    std::vector<double> values = lib.readVals();

    libCSV outlib = libCSV(fileName);
    outlib.WriteVals(values);
};

void MainWindow::InitPaths()
{
    wchar_t directory[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, directory);
    wchar_t* pDirectory;
    pDirectory = directory;

    wchar_t filePath[MAX_PATH];
    wchar_t* pFilePath;
    pFilePath = filePath;

    wchar_t folder[] = L"images";
    wchar_t* pFolder = folder;
    PathCombineW(pFilePath, pDirectory, pFolder);
    std::wstring wsImageDirectory(pFilePath);
    this->imageDirectory = wsImageDirectory;

    std::wstring wst(pDirectory);
    std::string st = std::string(wst.begin(), wst.end());
    resourceRoot = st;

    this->hapticMdlPath = RESOURCE_PATH("\\resources\\haptic_models\\");
    this->outputDirectory = OUTPUT_PATH("\\output\\");
}

void MainWindow::ComThreadRun(void)
{
    using namespace std::chrono_literals;
    char message[10];
    while (MainWindow::comThreadRunning)
    {
        std::this_thread::sleep_for(2s);
        sprintf(message, "$1, %u#", 25);
        MainWindow::SendMessageToCOM(message);
        std::this_thread::sleep_for(2s);
        
        sprintf(message, "$1, %u#", 50);
        MainWindow::SendMessageToCOM(message);
        
        sprintf(message, "$1, %u#", 100);
        MainWindow::SendMessageToCOM(message);
    }
    std::this_thread::sleep_for(2s);
    sprintf(message, "$1, %u#", 0);
}