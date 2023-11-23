#include "stdafx.h"
#include "Server.h"
#include <conio.h>
int main()
{
	Server::Init();

	DWORD startTime = timeGetTime();
	DWORD idealTime = startTime;
	DWORD overTime = 0;

	while (true)
	{
		if (GetAsyncKeyState(VK_RSHIFT))
		{
			PROFILE_DATA_OUT();
			wcout << L"PROFILE SAVED\n";
		}

		Server::Network();

		Server::Update();

		Sleep(20 - overTime);
		idealTime += 20;
		overTime = timeGetTime() - idealTime;

		if (overTime <= 0)
			overTime = 0;
		else if (overTime >= 20)
			overTime = 20;
	}



	return 0;
}