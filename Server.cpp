#include "stdafx.h"
#include "Server.h"

#include "Direction.h"
#include "PacketHandler.h"
#include "Protocol.h"
#include "SocketUtil.h"

int16 dx[] = {-3, -3, 0, 3, 3, 3, 0, -3};
int16 dy[] = {0, -2, -2, -2, 0, 2, 2, 2};

void Server::Init()
{
	SocketUtil::Init();
	srand(static_cast<unsigned>(time(nullptr)));

	listenSocket = SocketUtil::Create();

	int32 bindRet = SocketUtil::Bind(listenSocket, NetworkAddress(INADDR_ANY, 5000));

	wcout << ::format(L"Bind Port : {}\n", 5000);

	if (bindRet == SOCKET_ERROR)
	{
		cout << WSAGetLastError() << endl;
	}

	int32 listenRet = SocketUtil::Listen(listenSocket);
	wcout << "listen...\n";
	if (bindRet == SOCKET_ERROR)
	{
		cout << WSAGetLastError() << endl;
	}
}

void Server::Network()
{
	FD_SET rset;
	FD_SET wset;
	FD_ZERO(&rset);
	FD_ZERO(&wset);

	if (connectClients < MAX_USER)
	{
		FD_SET(listenSocket, &rset);
	}

	for (Session* session : sessions)
	{
		if (session -> socket == INVALID_SOCKET)
		{
			continue;
		}

		FD_SET(session->socket, &rset);
		FD_SET(session->socket, &wset);
	}

	timeval tv{ 0, 0 };
	int32 selectResult = ::select(0, &rset, &wset, nullptr, &tv);

	if (FD_ISSET(listenSocket, &rset))
	{
		OnAccept();
	}

	
	for (Session* session : sessions)
	{
		if (session->socket == INVALID_SOCKET)
		{
			continue;
		}
		if (FD_ISSET(session->socket, &rset))
		{
			OnRecv(session);
		}
	}

	for (Session* session : sessions)
	{
		if (session->socket == INVALID_SOCKET)
		{
			continue;
		}

		if (FD_ISSET(session->socket, &wset))
		{
			OnSend(session);
		}
	}

	auto it = sessions.begin();
	while (it != sessions.end())
	{
		Session* session = *it;
		if (session->socket == INVALID_SOCKET)
		{
			delete session;
			it = sessions.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void Server::Update()
{
	for (Session* session : sessions)
	{
		if (session->hp <= 0)
		{
			Disconnect(session);
		}

		if (session->socket == INVALID_SOCKET)
		{
			continue;
		}

		if (session->isMove == true)
		{
			int16 nx = session->x + dx[static_cast<int8>(session->moveDir)];
			int16 ny = session->y + dy[static_cast<int8>(session->moveDir)];

			if (nx < RANGE_MOVE_LEFT || nx > RANGE_MOVE_RIGHT
				|| ny < RANGE_MOVE_TOP || ny > RANGE_MOVE_BOTTOM)
				continue;

			session->x = nx;
			session->y = ny;
			//cout << session->x << " " << session->y << endl;
		}
	}
}

bool Server::OnAccept()
{
	SOCKADDR_IN clientAddr;
	int32 addrLen = sizeof(clientAddr);


	SOCKET clientSocket = ::accept(listenSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &addrLen);

	if (clientSocket == INVALID_SOCKET)
	{
		return false;
	}

	SocketUtil::SetLinger(clientSocket, 1, 0);


	int16 randVal = rand();

	int16 randX = randVal % RANGE_MOVE_RIGHT;
	int16 randY = randVal % RANGE_MOVE_BOTTOM;

	if (randX < RANGE_MOVE_LEFT)
		randX += RANGE_MOVE_LEFT;
	if (randY < RANGE_MOVE_TOP)
		randY += RANGE_MOVE_TOP;
	Direction dir;
	if (randX < RANGE_MOVE_RIGHT / 2)
	{
		dir = Direction::RR;
	}
	else
	{
		dir = Direction::LL;
	}

	Session* newSession = new Session{
		clientSocket,
		NetworkAddress(clientAddr),
		RingBuffer(5000),
		RingBuffer(5000),
		alignId++,
		randX,
		randY,
		100,
		dir,
		dir,
		false,
		AttackType::NONE
	};

	//wcout << ::format(L"[Conn] {}:{}\n", newSession->netInfo.GetIpAddress(), newSession->netInfo.GetPort());
	sessions.push_back(newSession);
	// 캐릭터 만들기
	//protocol::S_CREATE_MY_CHARACTER pktCreateMyCharacter{
	//	newSession->id,
	//	newSession->attackDir,
	//	newSession->x,
	//	newSession->y,
	//	newSession->hp
	//};
	{
		Packet pkt;
		PacketHandler::Make_S_CREATE_MY_CHARACTER(
			pkt,
			newSession->id,
			newSession->attackDir,
			newSession->x,
			newSession->y,
			newSession->hp
		);
		SendUnicast(newSession, pkt);
		//SendUnicast(
		//	newSession,
		//	PacketType::S_CREATE_MY_CHARACTER,
		//	reinterpret_cast<BYTE*>(&pktCreateMyCharacter),
		//	sizeof(protocol::S_CREATE_MY_CHARACTER)
		//);
		
	}



	//wprintf(L"[Send] -> [%s:%d] %s # id=%d, dir=%d, x=%d, y=%d, hp=%d", 
	//	newSession->netInfo.GetIpAddress().c_str(), 
	//	newSession->netInfo.GetPort(), 
	//	L"S_CREATE_MY_CHARACTER",
	//	pktCreateMyCharacter.id,
	//	(int32)pktCreateMyCharacter.dir,
	//	pktCreateMyCharacter.x,
	//	pktCreateMyCharacter.y,
	//	pktCreateMyCharacter.hp
	//);

	{
		Packet pkt;
		PacketHandler::Make_S_CREATE_OTHER_CHARACTER(
			pkt,
			newSession->id,
			newSession->attackDir,
			newSession->x,
			newSession->y,
			newSession->hp
		);
	//	protocol::S_CREATE_OTHER_CHARACTER pktCreateOtherCharacter{
	//newSession->id,
	//newSession->attackDir,
	//newSession->x,
	//newSession->y,
	//newSession->hp
	//	};

		SendBroadcast(newSession, pkt);
		//SendBroadcast(
		//	newSession,
		//	PacketType::S_CREATE_OTHER_CHARACTER,
		//	reinterpret_cast<BYTE*>(&pktCreateOtherCharacter),
		//	sizeof(protocol::S_CREATE_OTHER_CHARACTER)
		//);
	}




	//wprintf(L"[Send] -> [%s] %s # id=%d, dir=%d, x=%d, y=%d, hp=%d",
	//	L"Broadcast",
	//	L"S_CREATE_OTHER_CHARACTER",
	//	pktCreateOtherCharacter.id,
	//	(int32)pktCreateOtherCharacter.dir,
	//	pktCreateOtherCharacter.x,
	//	pktCreateOtherCharacter.y,
	//	pktCreateOtherCharacter.hp
	//);



	// 다른 플레이어 정보 받기
	for (Session* session : sessions)
	{
		if (newSession == session)
		{
			continue;
		}

		//protocol::S_CREATE_OTHER_CHARACTER pktCreateOtherCharacter{
		//	session->id,
		//	session->attackDir,
		//	session->x,
		//	session->y,
		//	session->hp
		//};

		Packet pktCreateOtherCharacter;
		PacketHandler::Make_S_CREATE_OTHER_CHARACTER(
			pktCreateOtherCharacter,
			session->id,
			session->attackDir,
			session->x,
			session->y,
			session->hp
		);
		SendUnicast(newSession, pktCreateOtherCharacter);
		//SendUnicast(
		//	newSession,
		//	PacketType::S_CREATE_OTHER_CHARACTER,
		//	reinterpret_cast<BYTE*>(&pktCreateOtherCharacter),
		//	sizeof(protocol::S_CREATE_OTHER_CHARACTER)
		//);

		//wprintf(L"[Send] -> [%s:%d] %s # id=%d, dir=%d, x=%d, y=%d, hp=%d",
		//	newSession->netInfo.GetIpAddress().c_str(),
		//	newSession->netInfo.GetPort(),
		//	L"S_CREATE_OTHER_CHARACTER",
		//	pktCreateMyCharacter.id,
		//	(int32)pktCreateMyCharacter.dir,
		//	pktCreateMyCharacter.x,
		//	pktCreateMyCharacter.y,
		//	pktCreateMyCharacter.hp
		//);
	}

	return true;
	
}

void Server::OnRecv(Session* session)
{
	int32 recvRet = ::recv(
		session->socket,
		reinterpret_cast<char*>(session->recvBuffer.GetRearBufferPtr()),
		session->recvBuffer.DirectEnqueueSize(),
		0
	);

	if (recvRet == 0)
	{
		Disconnect(session);
		return;
	}

	if (recvRet == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			cout << WSAGetLastError() << endl;
			Disconnect(session);
			return;
		}
	}

	session->recvBuffer.MoveRear(recvRet);

	while (true)
	{
		if (PacketHandler::HandlePacket(session) == false)
		{
			return;
		}
	}
}

void Server::OnSend(Session* session)
{
	int32 sendRet = ::send(session->socket, (char*)session->sendBuffer.GetFrontBufferPtr(), session->sendBuffer.DirectDequeueSize(), 0);

	if (sendRet == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			cout << WSAGetLastError() << endl;
			Disconnect(session);
		}
	}

	session->sendBuffer.MoveFront(sendRet);
}

void Server::SendUnicast(Session* session, Packet& pkt)
{
	if (session->sendBuffer.GetFreeSize() < pkt.GetDataSize())
	{
		Disconnect(session);
		return;
	}

	session->sendBuffer.Enqueue(pkt.GetBufferPtr(), pkt.GetDataSize());
}
//void Server::SendUnicast(Session* session, PacketType type, BYTE* pkt, uint8 len)
//{
//	if (session->sendBuffer.GetFreeSize() < sizeof(PacketHeader) + len)
//	{
//		Disconnect(session);
//		return;
//	}
//
//	PacketHeader header{ 0x89, len, type };
//	session->sendBuffer.Enqueue(reinterpret_cast<BYTE*>(&header), sizeof(PacketHeader));
//	session->sendBuffer.Enqueue(pkt, len);
//}

//void Server::SendBroadcast(Session* exceptSession, PacketType type, BYTE* pkt, uint8 len)
//{
//	int32 sendRet = SOCKET_ERROR;
//
//	for (Session* session : sessions)
//	{
//		if (session == exceptSession)
//		{
//			continue;
//		}
//
//		if (session->socket == INVALID_SOCKET)
//		{
//			continue;
//		}
//
//		if (session->sendBuffer.GetFreeSize() < sizeof(PacketHeader) + len)
//		{
//			Disconnect(session);
//			return;
//		}
//
//		SendUnicast(session, type, pkt, len);
//	}
//}

void Server::SendBroadcast(Session* exceptSession, Packet& pkt)
{
	int32 sendRet = SOCKET_ERROR;

	for (Session* session : sessions)
	{
		if (session == exceptSession)
		{
			continue;
		}

		if (session->socket == INVALID_SOCKET)
		{
			continue;
		}

		SendUnicast(session, pkt);
	}
}

void Server::Disconnect(Session* session)
{
	wcout << ::format(L"[Disc] -> [{:^21}]\n", ::format(L"{}:{}",session->netInfo.GetIpAddress(), session->netInfo.GetPort()));
	SocketUtil::Close(session->socket);
	session->socket = INVALID_SOCKET;

	Packet pkt;
	PacketHandler::Make_S_DELETE_CHARACTER(pkt, session->id);
	SendBroadcast(session, pkt);
}

bool Server::IsAttackRange(Session* session, Session* target, int32 rangeX, int32 rangeY)
{
	if (target->y < session->y - rangeY || target->y >= session->y + rangeY)
	{
		return false;
	}

	if (session->attackDir == Direction::LL)
	{
		if (target->x > session->x || target->x <= session->x - rangeX)
		{
			return false;
		}
	}
	else
	{
		if (target->x < session->x || target->x >= session->x + rangeX)
		{
			return false;
		}
	}

	return true;
}

bool Server::IsAlowableRange(Session* session, int32 x, int32 y)
{
	if (abs(session->x - x) > 50)
	{
		return false;
	}

	if (abs(session->y - y) > 50)
	{
		return false;
	}

	return true;
}
