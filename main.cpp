#include "stdafx.h"
#include "Server.h"
#include <conio.h>

#include "DateTime.h"
#include "Frame.h"

void test()
{
	int a = 1;
}

int main()
{
	Server::Init();

	Frame::Init(20);

	while (true)
	{ 
		PROFILE_SAVE(VK_SHIFT);

		PROFILE_BEGIN(L"Network");
		Server::Network();
		PROFILE_END(L"Network");

		PROFILE_BEGIN(L"Logic");
		Server::Update();
		PROFILE_END(L"Logic");

		Frame::Sleep();
	}

	return 0;
}
