#pragma once

using namespace std;
#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "winmm.lib")

#include <Windows.h>
#include <iostream>
#include <format>
#include <timeapi.h>

#include <WinSock2.h>
#include <ws2tcpip.h>

#include "Types.h"


#define PROFILE
#include "Profiler.h"