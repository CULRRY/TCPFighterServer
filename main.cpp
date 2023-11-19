#include "stdafx.h"
#include "Server.h"

int main()
{
	Server::Init();

	DWORD startTime = timeGetTime();
	DWORD idealTime = startTime;
	DWORD overTime = 0;

	while (true)
	{
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