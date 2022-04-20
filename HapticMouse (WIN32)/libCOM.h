#pragma once
#include <Windows.h>
#include <vector>
#include "helpers.h"

std::vector<std::wstring> GetPortNames();
int ConnectPort(std::string portName);
int SendMessageCom(char message[]);
