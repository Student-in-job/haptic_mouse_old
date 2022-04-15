#pragma once
#include <chrono>
#include <thread>

#define LIB_COUNT				84

void updateHaptics(void);
void updateVibrationPattern(void);
int startLib(char* directory, int mdl);
int StopLib();
/// <summary>
/// This function runs until all the threads finished and then only closes the main thread (Need to be called before exiting from main application)
/// </summary>
void WaitForLibClose(void);