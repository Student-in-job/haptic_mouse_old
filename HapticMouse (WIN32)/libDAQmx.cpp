#include "libDAQmx.h"
#include "helpers.h"

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

// Global variables for NI DAQ
extern TaskHandle  taskHandle;
const char* channelVoltage = "Dev3/ao0";

extern double vibrationsData[1000];
extern bool running;
extern bool started;

int32 CVICALLBACK DataAquisitionLoop(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void* callbackData)
{
    int32   error = 0;
    int32   read = 0;
    char    errBuff[2048] = { '\0' };

    float64 acquiredData[2];
    float acquiredDataFloat;

    SetProgress(L"Reading data from DAQ channel");
    DAQmxErrChk(DAQmxCreateTask("Read", &taskHandle));
    DAQmxErrChk(DAQmxCreateAOVoltageChan(taskHandle, channelVoltage, "", -10.0, 10.0, DAQmx_Val_Volts, ""));

    DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, 1, 10.0, DAQmx_Val_GroupByScanNumber, acquiredData, 7, &read, NULL));
    acquiredDataFloat = acquiredData[1];
    if (read > 0) {
        float FT[6];
    }
    Error:
    if (DAQmxFailed(error))
    {
        DAQmxGetExtendedErrorInfo(errBuff, 2048);
        wchar_t* err = ToWChar(errBuff);
        SetError(err);
    }
    if (taskHandle != 0)
    {
        /*********************************************/
        // DAQmx Stop Code
        /*********************************************/
        DAQmxStopTask(taskHandle);
        DAQmxClearTask(taskHandle);
    }
    return 0;
}

int32 CVICALLBACK DataGenerationLoop()
{
    char    errBuff[2048] = { '\0' };
    int32   error = 0;
    float64 vibroVal = 5.62f;

    /*********************************************/
    // DAQmx Configure Code
    /*********************************************/
    DAQmxErrChk(DAQmxCreateTask("output", &taskHandle));
    DAQmxErrChk(DAQmxCreateAOVoltageChan(taskHandle, channelVoltage, "", -10.0, 10.0, DAQmx_Val_Volts, NULL));
    DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle, "", 1000.0, DAQmx_Val_Rising, DAQmx_Val_ContSamps, 1000));
    //DAQmxErrChk(DAQmxSetAnlgEdgeStartTrigHyst(taskHandle, 1.0));

    DAQmxErrChk(DAQmxWriteAnalogF64(taskHandle, 1000, 0, 10.0, DAQmx_Val_GroupByChannel, vibrationsData, NULL, NULL));
    DAQmxErrChk(DAQmxStartTask(taskHandle));
    
    SetProgress(L"Writing data to DAQ channel");
    running = true;
    while (started) {}

    Error:
    if (DAQmxFailed(error))
    {
        DAQmxGetExtendedErrorInfo(errBuff, 2048);
        wchar_t* err = ToWChar(errBuff);
        SetError(err);
    }
    if (taskHandle != 0) {
        /*********************************************/
        // DAQmx Stop Code
        /*********************************************/
        DAQmxStopTask(taskHandle);
        DAQmxClearTask(taskHandle);
    }

    SetProgress(L"Finished writing data");
    running = false;
    return 0;
}

int32 CVICALLBACK DynamicDataGenerationLoop()
{
    int			error = 0;
    char		errBuff[2048] = { '\0' };
    int         i = 1;

    /*********************************************/
    // DAQmx Configure Code
    /*********************************************/
    DAQmxErrChk(DAQmxCreateTask("Loop", &taskHandle));
    DAQmxErrChk(DAQmxCreateAOVoltageChan(taskHandle, channelVoltage, "", -10.0, 10.0, DAQmx_Val_Volts, ""));
    /*********************************************/
    // DAQmx Start Code
    /*********************************************/
    DAQmxErrChk(DAQmxStartTask(taskHandle));

    SetProgress(L"Writing data to DAQ channel");
    running = true;
    while (started)
    {
        if (i == 1000) { i = 0; }
        /*********************************************/
        // DAQmx Write Code
        /*********************************************/
        DAQmxErrChk(DAQmxWriteAnalogScalarF64(taskHandle, 1, -1, vibrationsData[i++], NULL));
    }

    Error:
    if (DAQmxFailed(error)) {
        DAQmxGetExtendedErrorInfo(errBuff, 2048);
        wchar_t* err = ToWChar(errBuff);
        SetError(err);
        running = false;
    }
    if (taskHandle != 0) {
        /*********************************************/
        // DAQmx Stop Code
        /*********************************************/
        DAQmxStopTask(taskHandle);
        DAQmxClearTask(taskHandle);
    }    

    SetProgress(L"Finished writing data");
    running = false;
    return 1;
}

int32 CVICALLBACK DAQLoop()
{
    char    errBuff[2048] = { '\0' };
    int32   error = 0;

    /*********************************************/
    // DAQmx Configure Code
    /*********************************************/
    DAQmxErrChk(DAQmxCreateTask("", &taskHandle));
    DAQmxErrChk(DAQmxCreateAOVoltageChan(taskHandle, channelVoltage, "", -10.0, 10.0, DAQmx_Val_Volts, ""));
    /*********************************************/
    // DAQmx Start Code
    /*********************************************/
    DAQmxErrChk(DAQmxStartTask(taskHandle));

    SetProgress(L"Writing data to DAQ channel");
    running = true;
    while (started) {}
    
    Error:
    if (DAQmxFailed(error))
    {
        DAQmxGetExtendedErrorInfo(errBuff, 2048);
        wchar_t* err = ToWChar(errBuff);
        SetError(err);
    }
    if (taskHandle != 0) {
        /*********************************************/
        // DAQmx Stop Code
        /*********************************************/
        DAQmxStopTask(taskHandle);
        DAQmxClearTask(taskHandle);
    }
    running = false;
    SetProgress(L"Finished writing data");
    return 1;
}

void WriteData(double value)
{
    int			error = 0;
    char		errBuff[2048] = { '\0' };
    
    /*********************************************/
    // DAQmx Write Code
    /*********************************************/
    DAQmxErrChk(DAQmxWriteAnalogScalarF64(taskHandle, 1, -1, value, NULL));
    Error:
    if (DAQmxFailed(error))
    {
        DAQmxGetExtendedErrorInfo(errBuff, 2048);
        wchar_t* err = ToWChar(errBuff);
        SetError(err);
    }
}