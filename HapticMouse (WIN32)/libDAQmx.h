#pragma once
#include <NIDAQmx.h>

int32 CVICALLBACK DataAquisitionLoop(TaskHandle taskHandle, int32 everyNsamplesEventType, uInt32 nSamples, void* callbackData);
//int32 CVICALLBACK EndCallback(TaskHandle taskHandle, int32 status, void* callbackData);
int32 CVICALLBACK DynamicDataGenerationLoop();
int32 CVICALLBACK DAQLoop();
void WriteData(double value);
extern void SetProgress(const wchar_t* value);
extern void SetError(const wchar_t* value);