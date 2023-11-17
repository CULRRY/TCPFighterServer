#include "stdafx.h"
#include "Server.h"
#include "Packet.h"

bool Handle_C_MOVE_START(Session* session, const protocol::C_MOVE_START* pkt)
{
	session->isMove = true;
	session->moveDir = pkt->dir;

	switch (pkt->dir)
	{
	case Direction::LL:
	case Direction::LU:
	case Direction::LD:
		session->attackDir = Direction::LL;
		break;

	case Direction::RR:
	case Direction::RU:
	case Direction::RD:
		session->attackDir = Direction::RR;
		break;

	default:
			break;
	}

	session->x = pkt->x;
	session->y = pkt->y;

	protocol::S_MOVE_START sendPkt{
		session->id,
		pkt->dir,
		pkt->x,
		pkt->y
	};

	Server::SendBroadcast(
		session, 
		PacketType::S_MOVE_START, 
		reinterpret_cast<BYTE*>(&sendPkt),
		sizeof(protocol::S_MOVE_START)
	);

	return true;
}

bool Handle_C_MOVE_STOP(Session* session, const protocol::C_MOVE_STOP* pkt)
{
	session->isMove = false;
	session->attackDir = pkt->dir;
	session->x = pkt->x;
	session->y = pkt->y;

	protocol::S_MOVE_STOP sendPkt{
		session->id,
		pkt->dir,
		pkt->x,
		pkt->y
	};

	Server::SendBroadcast(
		session,
		PacketType::S_MOVE_STOP,
		reinterpret_cast<BYTE*>(&sendPkt),
		sizeof(protocol::S_MOVE_STOP)
	);

	return true;
}

bool Handle_C_ATTACK1(Session* session, const protocol::C_ATTACK1* pkt)
{
	if (session->attackType == AttackType::NONE)
	{
		session->attackType = AttackType::ATTACK1;
		session->moveDir = pkt->dir;
		session->x = pkt->x;
		session->y = pkt->y;

		protocol::S_ATTACK1 sendPkt{
			session->id,
			pkt->dir,
			pkt->x,
			pkt->y
		};

		Server::SendBroadcast(
			session,
			PacketType::S_ATTACK1,
			reinterpret_cast<BYTE*>(&sendPkt),
			sizeof(protocol::S_ATTACK1)
		);
	}

	return true;
}

bool Handle_C_ATTACK2(Session* session, const protocol::C_ATTACK2* pkt)
{

	if (session->attackType == AttackType::NONE)
	{
		session->attackType = AttackType::ATTACK2;
		session->moveDir = pkt->dir;
		session->x = pkt->x;
		session->y = pkt->y;

		protocol::S_ATTACK2 sendPkt{
			session->id,
				pkt->dir,
				pkt->x,
				pkt->y
		};

		Server::SendBroadcast(
			session,
			PacketType::S_ATTACK2,
			reinterpret_cast<BYTE*>(&sendPkt),
			sizeof(protocol::S_ATTACK2)
		);
	}

	return true;
}

bool Handle_C_ATTACK3(Session* session, const protocol::C_ATTACK3* pkt)
{
	if (session->attackType == AttackType::NONE)
	{
		session->attackType = AttackType::ATTACK3;
		session->moveDir = pkt->dir;
		session->x = pkt->x;
		session->y = pkt->y;

		protocol::S_ATTACK2 sendPkt{
			session->id,
				pkt->dir,
				pkt->x,
				pkt->y
		};

		Server::SendBroadcast(
			session,
			PacketType::S_ATTACK3,
			reinterpret_cast<BYTE*>(&sendPkt),
			sizeof(protocol::S_ATTACK3)
		);
	}

	return true;
}
