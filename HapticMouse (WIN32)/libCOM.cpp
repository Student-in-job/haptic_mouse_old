#include "libCOM.h"

HANDLE hCom;

std::vector<std::wstring> GetPortNames()
{
    std::vector<std::wstring> ports;

    for (UINT i = 1; i < 256; i++)
    {
        //Form the Raw device name
        wchar_t sPort[100];
        swprintf(sPort, 100, L"\\\\.\\COM%u", i);

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
            CloseHandle(port);
        }
    }
    return ports;
}

int ConnectPort(std::string portName)
{
    if (hCom != NULL)
    {
        CloseHandle(hCom);
    }
    DCB dcbserialparams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };   // initializing timeouts structure
    char serialbuffer[64] = { 0 };   // buffer to send and receive data
    
    //swprintf(sPort, 100, L"\\\\.\\COM%u", this->serialPorts.find(port)->second);

    hCom = CreateFile(ToWChar(portName),    // port friendly name
        GENERIC_READ | GENERIC_WRITE,        // read/write access
        0,                                   // no sharing, ports cant be shared
        NULL,                                // no security
        OPEN_EXISTING,                       // open existing port only
        0,                                   // non overlapped i/o
        NULL);
    if (hCom == INVALID_HANDLE_VALUE)
    {
        MessageBoxW(NULL, L"Port can't be opened", L"error", MB_OK);
        return 0;
    }
    dcbserialparams.DCBlength = sizeof(dcbserialparams);
    dcbserialparams.BaudRate = CBR_115200;    //baudrate = 115200
    dcbserialparams.ByteSize = 8;             //bytesize = 8
    dcbserialparams.StopBits = ONESTOPBIT;    //stopbits = 1
    dcbserialparams.Parity = NOPARITY;        //parity = none
    bool status = SetCommState(hCom, &dcbserialparams);
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
    if (SetCommTimeouts(hCom, &timeouts) == false)
    {
        MessageBoxW(NULL, L"Error to setting time outs", L"error", MB_OK);
        return 0;
    }
    return 1;
}

int SendMessageCom(char message[])
{
    DWORD BytesWritten = 0;
    BOOL Status = WriteFile(hCom,   // Handle to the Serialport
        message,                                 // Data to be written to the port
        sizeof(message),                         // No of bytes to write into the port
        &BytesWritten,                           // No of bytes written to the port
        NULL);
    if (Status == FALSE)
    {
        MessageBoxW(NULL, L"Fail to Written", L"Error", MB_OK);
        return 0;
    }
    return 1;
}