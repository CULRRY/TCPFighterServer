#include "stdafx.h"
#include "Server.h"
#include "Packet.h"

bool IsAlowableRange(Session* session, int32 x, int32 y)
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

bool Handle_C_MOVE_START(Session* session, const protocol::C_MOVE_START* pkt)
{
	session->isMove = true;
	session->moveDir = pkt->dir;

	if (IsAlowableRange(session, pkt->x, pkt->y) == false)
	{
		Server::Disconnect(session);
	}

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

	wcout << ::format(L"[Send] -> [{:^21}] {:21} # id={}, dir={}, x={}, y={}\n",
		L"Broadcast",
		L"S_MOVE_START",
		sendPkt.id,
		(int32)sendPkt.dir,
		sendPkt.x,
		sendPkt.y);

	return true;
}

bool Handle_C_MOVE_STOP(Session* session, const protocol::C_MOVE_STOP* pkt)
{
	if (IsAlowableRange(session, pkt->x, pkt->y) == false)
	{
		Server::Disconnect(session);
	}

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

	wcout << ::format(L"[Send] -> [{:^21}] {:21} # id={}, dir={}, x={}, y={}\n",
		L"Broadcast",
		L"S_MOVE_STOP",
		sendPkt.id,
		(int32)sendPkt.dir,
		sendPkt.x,
		sendPkt.y);

	return true;
}

bool Handle_C_ATTACK1(Session* session, const protocol::C_ATTACK1* pkt)
{
	if (IsAlowableRange(session, pkt->x, pkt->y) == false)
	{
		Server::Disconnect(session);
	}


	if (session->attackType == AttackType::NONE)
	{
		//session->attackType = AttackType::ATTACK1;
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

		wcout << ::format(L"[Send] -> [{:^21}] {:21} # id={}, dir={}, x={}, y={}\n",
			L"Broadcast",
			L"S_ATTACK1",
			sendPkt.id,
			(int32)sendPkt.dir,
			sendPkt.x,
			sendPkt.y);

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
			int32 damage = 5;

			if (Server::IsAttackRange(session, target, Server::ATTACK1_RANGE_X, Server::ATTACK1_RANGE_Y))
			{
				target->hp -= damage;
				protocol::S_DAMAGE pkt{
					session->id,
						target->id,
						target->hp
				};

				Server::SendBroadcast(nullptr, PacketType::S_DAMAGE, reinterpret_cast<BYTE*>(&pkt), sizeof(protocol::S_DAMAGE));

				wcout << ::format(L"[Send] -> [{:^21}] {:21} # attackId={}, damageId={}, damageHp={}\n",
					L"Broadcast",
					L"S_DAMAGE",
					pkt.attackId,
					pkt.damageId,
					(int32)pkt.damageHp);
			}
		}
	}

	return true;
}

bool Handle_C_ATTACK2(Session* session, const protocol::C_ATTACK2* pkt)
{
	if (IsAlowableRange(session, pkt->x, pkt->y) == false)
	{
		Server::Disconnect(session);
	}

	if (session->attackType == AttackType::NONE)
	{
		//session->attackType = AttackType::ATTACK2;
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

		wcout << ::format(L"[Send] -> [{:^21}] {:21} # id={}, dir={}, x={}, y={}\n",
			L"Broadcast",
			L"S_ATTACK2",
			sendPkt.id,
			(int32)sendPkt.dir,
			sendPkt.x,
			sendPkt.y);

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
			int32 damage = 10;

			if (Server::IsAttackRange(session, target, Server::ATTACK2_RANGE_X, Server::ATTACK2_RANGE_Y))
			{
				target->hp -= damage;
				protocol::S_DAMAGE pkt{
					session->id,
						target->id,
						target->hp
				};

				Server::SendBroadcast(nullptr, PacketType::S_DAMAGE, reinterpret_cast<BYTE*>(&pkt), sizeof(protocol::S_DAMAGE));
				wcout << ::format(L"[Send] -> [{:^21}] {:21} # attackId={}, damageId={}, damageHp={}\n",
					L"Broadcast",
					L"S_DAMAGE",
					pkt.attackId,
					pkt.damageId,
					(int32)pkt.damageHp);
			}
		}
	}

	return true;
}

bool Handle_C_ATTACK3(Session* session, const protocol::C_ATTACK3* pkt)
{
	if (IsAlowableRange(session, pkt->x, pkt->y) == false)
	{
		Server::Disconnect(session);
	}

	if (session->attackType == AttackType::NONE)
	{
		//session->attackType = AttackType::ATTACK3;
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

		wcout << ::format(L"[Send] -> [{:^21}] {:21} # id={}, dir={}, x={}, y={}\n",
			L"Broadcast",
			L"S_ATTACK3",
			sendPkt.id,
			(int32)sendPkt.dir,
			sendPkt.x,
			sendPkt.y
		);

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
			int32 damage = 15;

			if (Server::IsAttackRange(session, target, Server::ATTACK3_RANGE_X, Server::ATTACK3_RANGE_Y))
			{
				target->hp -= damage;
				protocol::S_DAMAGE pkt{
					session->id,
						target->id,
						target->hp
				};

				Server::SendBroadcast(nullptr, PacketType::S_DAMAGE, reinterpret_cast<BYTE*>(&pkt), sizeof(protocol::S_DAMAGE));
				wcout << ::format(L"[Send] -> [{:^21}] {:21} # attackId={}, damageId={}, damageHp={}\n",
					L"Broadcast",
					L"S_DAMAGE",
					pkt.attackId,
					pkt.damageId,
					(int32)pkt.damageHp
				);
			}
		}
	}

	return true;
}
