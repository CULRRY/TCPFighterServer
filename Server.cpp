#include "stdafx.h"
#include "Server.h"

#include "Packet.h"
#include "Protocol.h"
#include "SocketUtil.h"

int16 dx[] = {-3, -3, 0, 3, 3, 3, 0, -3};
int16 dy[] = {0, -2, -2, -2, 0, 2, 2, 2};

void Server::Init()
{
	SocketUtil::Init();
	srand(time(nullptr));

	listenSocket = SocketUtil::Create();

	int32 bindRet = SocketUtil::Bind(listenSocket, NetworkAddress(INADDR_ANY, 5000));

	cout << "bind..." << endl;

	if (bindRet == SOCKET_ERROR)
	{
		cout << WSAGetLastError() << endl;
	}

	int32 listenRet = SocketUtil::Listen(listenSocket);
	cout << "listen..." << endl;
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
			if (session->x < RANGE_MOVE_LEFT || session->x > RANGE_MOVE_RIGHT
				|| session->y < RANGE_MOVE_BOTTOM || session->y > RANGE_MOVE_TOP)
				continue;

			session->x += dx[static_cast<int8>(session->moveDir)];
			session->y += dy[static_cast<int8>(session->moveDir)];
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

	int16 randX = (randVal % RANGE_MOVE_RIGHT) + RANGE_MOVE_LEFT;
	int16 randY = (randVal % RANGE_MOVE_BOTTOM) + RANGE_MOVE_TOP;
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
		RingBuffer(10000),
		RingBuffer(10000),
		alignId++,
		randX,
		randY,
		100,
		dir,
		dir,
		false,
		AttackType::NONE
	};

	sessions.push_back(newSession);

	// 캐릭터 만들기
	protocol::S_CREATE_MY_CHARACTER pktCreateMyCharacter{
		newSession->id,
		newSession->attackDir,
		newSession->x,
		newSession->y,
		newSession->hp
	};

	SendUnicast(
		newSession, 
		PacketType::S_CREATE_MY_CHARACTER, 
		reinterpret_cast<BYTE*>(&pktCreateMyCharacter), 
		sizeof(protocol::S_CREATE_MY_CHARACTER)
	);

	protocol::S_CREATE_OTHER_CHARACTER pktCreateOtherCharacter{
		newSession->id,
		newSession->attackDir,
		newSession->x,
		newSession->y,
		newSession->hp
	};

	SendBroadcast(
		newSession,
		PacketType::S_CREATE_OTHER_CHARACTER,
		reinterpret_cast<BYTE*>(&pktCreateOtherCharacter),
		sizeof(protocol::S_CREATE_OTHER_CHARACTER)
	);

	// 다른 플레이어 정보 받기
	for (Session* session : sessions)
	{
		if (newSession == session)
		{
			continue;
		}

		protocol::S_CREATE_OTHER_CHARACTER pktCreateOtherCharacter{
			session->id,
			session->attackDir,
			session->x,
			session->y,
			session->hp
		};
		SendUnicast(
			newSession,
			PacketType::S_CREATE_OTHER_CHARACTER,
			reinterpret_cast<BYTE*>(&pktCreateOtherCharacter),
			sizeof(protocol::S_CREATE_OTHER_CHARACTER)
		);
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
			Disconnect(session);
			return;
		}
	}

	session->recvBuffer.MoveRear(recvRet);

	while (true)
	{
		if (session->recvBuffer.GetUseSize() < sizeof(PacketHeader))
		{
			return;
		}

		PacketHeader header;
		session->recvBuffer.Peek(reinterpret_cast<BYTE*>(&header), sizeof(PacketHeader));

		if (header.code != 0x89)
		{
			Disconnect(session);
		}

		if (session->recvBuffer.GetUseSize() < header.size + sizeof(PacketHeader))
		{
			return;
		}

		session->recvBuffer.MoveFront(sizeof(PacketHeader));

		switch (header.type)
		{
		case PacketType::C_MOVE_START:
		{
			protocol::C_MOVE_START pkt;
			session->recvBuffer.Dequeue(reinterpret_cast<BYTE*>(&pkt), sizeof(protocol::C_MOVE_START));
			Handle_C_MOVE_START(session, &pkt);
			break;
		}
		case PacketType::C_MOVE_STOP:
		{
			protocol::C_MOVE_STOP pkt;
			session->recvBuffer.Dequeue(reinterpret_cast<BYTE*>(&pkt), sizeof(protocol::C_MOVE_STOP));
			Handle_C_MOVE_STOP(session, &pkt);
			break;
		}

		case PacketType::C_ATTACK1:
		{
			protocol::C_ATTACK1 pkt;
			session->recvBuffer.Dequeue(reinterpret_cast<BYTE*>(&pkt), sizeof(protocol::C_ATTACK1));
			Handle_C_ATTACK1(session, &pkt);
			break;
		}

		case PacketType::C_ATTACK2:
		{
			protocol::C_ATTACK2 pkt;
			session->recvBuffer.Dequeue(reinterpret_cast<BYTE*>(&pkt), sizeof(protocol::C_ATTACK2));
			Handle_C_ATTACK2(session, &pkt);
			break;
		}

		case PacketType::C_ATTACK3:
		{
			protocol::C_ATTACK3 pkt;
			session->recvBuffer.Dequeue(reinterpret_cast<BYTE*>(&pkt), sizeof(protocol::C_ATTACK3));
			Handle_C_ATTACK3(session, &pkt);
			break;
		}

		default:;
		}

		cout << "Recv " << (int32)header.size << " Type: " << (int)header.type << endl;
	}
}

void Server::OnSend(Session* session)
{
	int32 sendRet = ::send(session->socket, (char*)session->sendBuffer.GetFrontBufferPtr(), session->sendBuffer.DirectDequeueSize(), 0);
	session->sendBuffer.MoveFront(sendRet);
}

void Server::SendUnicast(Session* session, PacketType type, BYTE* pkt, uint8 len)
{
	PacketHeader header{ 0x89, len, type };
	session->sendBuffer.Enqueue(reinterpret_cast<BYTE*>(&header), sizeof(PacketHeader));
	session->sendBuffer.Enqueue(pkt, len);
}

void Server::SendBroadcast(Session* exceptSession, PacketType type, BYTE* pkt, uint8 len)
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

		SendUnicast(session, type, pkt, len);
	}
}

void Server::Disconnect(Session* session)
{
	SocketUtil::Close(session->socket);
	session->socket = INVALID_SOCKET;

	cout << "Disconnect" << endl;

	protocol::S_DELETE_CHARACTER pkt{session->id};
	SendBroadcast(session, PacketType::S_DELETE_CHARACTER, reinterpret_cast<BYTE*>(&pkt), sizeof(protocol::S_DELETE_CHARACTER));
}

bool Server::IsAttackRange(Session* session, Session* target, int32 rangeX, int32 rangeY)
{
	if (target->y < session->y || target->y >= session->y + rangeY)
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
