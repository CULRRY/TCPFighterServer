#pragma once

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#include <Windows.h>
#include <iostream>
using namespace std;

#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include <ws2tcpip.h>

#include "Types.h"
