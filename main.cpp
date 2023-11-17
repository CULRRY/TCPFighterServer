#include "stdafx.h"
#include "Server.h"

int main()
{
	Server::Init();

	while (true)
	{
		Server::Network();
		Server::Update();
		Sleep(20);
	}



	return 0;
}