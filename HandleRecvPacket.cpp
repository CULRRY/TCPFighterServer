#include "stdafx.h"
#include "HandleRecvPacket.h"

#include "Direction.h"
#include "PacketHandler.h"
#include "Server.h"

bool Handle_C_MOVE_START(Session* session, Direction& dir, int16 x, int16 y)
{
	session->isMove = true;
	session->moveDir = dir;

	if (Server::IsAlowableRange(session, x, y) == false)
	{
		Server::Disconnect(session);
	}

	switch (dir)
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

	session->x = x;
	session->y = y;

	Packet sendPkt;
	PacketHandler::Make_S_MOVE_START(sendPkt, session->id, dir, x, y);

	Server::SendBroadcast(session, sendPkt);

	return true;
}

bool Handle_C_MOVE_STOP(Session* session, Direction& dir, int16 x, int16 y)
{
	if (Server::IsAlowableRange(session, x, y) == false)
	{
		Server::Disconnect(session);
	}

	session->isMove = false;
	session->moveDir = dir;
	session->x = x;
	session->y = y;


	Packet sendPkt;
	PacketHandler::Make_S_MOVE_STOP(sendPkt, session->id, dir, x, y);

	Server::SendBroadcast(session, sendPkt);

	return true;
}

bool Handle_C_ATTACK1(Session* session, Direction& dir, int16 x, int16 y)
{
	if (Server::IsAlowableRange(session, x, y) == false)
	{
		Server::Disconnect(session);
	}


	if (session->attackType == AttackType::NONE)
	{
		session->attackDir = dir;
		session->x = x;
		session->y = y;

		Packet sendPkt;
		PacketHandler::Make_S_ATTACK1(sendPkt, session->id, dir, x, y);
		Server::SendBroadcast(session, sendPkt);

		for (Session* target : Server::sessions)
		{
			if (target->socket == INVALID_SOCKET)
			{
				continue;
			}

			if (session == target)
			{
				continue;
			}

			bool isAttack = false;
			int8 damage = 5;

			if (Server::IsAttackRange(session, target, Server::ATTACK1_RANGE_X, Server::ATTACK1_RANGE_Y))
			{
				target->hp -= damage;

				Packet damagePkt;
				PacketHandler::Make_S_DAMAGE(damagePkt, session->id, target->id, target->hp);
				Server::SendBroadcast(nullptr, damagePkt);
			}
		}
	}

	return true;
}

bool Handle_C_ATTACK2(Session* session, Direction& dir, int16 x, int16 y)
{
	if (Server::IsAlowableRange(session, x, y) == false)
	{
		Server::Disconnect(session);
	}


	if (session->attackType == AttackType::NONE)
	{
		session->attackDir = dir;
		session->x = x;
		session->y = y;

		Packet sendPkt;
		PacketHandler::Make_S_ATTACK2(sendPkt, session->id, dir, x, y);
		Server::SendBroadcast(session, sendPkt);

		for (Session* target : Server::sessions)
		{
			if (target->socket == INVALID_SOCKET)
			{
				continue;
			}

			if (session == target)
			{
				continue;
			}

			bool isAttack = false;
			int8 damage = 5;

			if (Server::IsAttackRange(session, target, Server::ATTACK2_RANGE_X, Server::ATTACK2_RANGE_Y))
			{
				target->hp -= damage;

				Packet damagePkt;
				PacketHandler::Make_S_DAMAGE(damagePkt, session->id, target->id, target->hp);
				Server::SendBroadcast(nullptr, damagePkt);
			}
		}
	}

	return true;
}

bool Handle_C_ATTACK3(Session* session, Direction& dir, int16 x, int16 y)
{
	if (Server::IsAlowableRange(session, x, y) == false)
	{
		Server::Disconnect(session);
	}


	if (session->attackType == AttackType::NONE)
	{
		session->attackDir = dir;
		session->x = x;
		session->y = y;

		Packet sendPkt;
		PacketHandler::Make_S_ATTACK2(sendPkt, session->id, dir, x, y);
		Server::SendBroadcast(session, sendPkt);

		for (Session* target : Server::sessions)
		{
			if (target->socket == INVALID_SOCKET)
			{
				continue;
			}

			if (session == target)
			{
				continue;
			}

			bool isAttack = false;
			int8 damage = 5;

			if (Server::IsAttackRange(session, target, Server::ATTACK2_RANGE_X, Server::ATTACK2_RANGE_Y))
			{
				target->hp -= damage;

				Packet damagePkt;
				PacketHandler::Make_S_DAMAGE(damagePkt, session->id, target->id, target->hp);
				Server::SendBroadcast(nullptr, damagePkt);
			}
		}
	}

	return true;
}
