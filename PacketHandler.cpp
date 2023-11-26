#include "stdafx.h"
#include "PacketHandler.h"
#include "HandleRecvPacket.h"
#include "Packet.h"
#include "Server.h"
#include "Direction.h"


bool PacketHandler::HandlePacket(Session* session)
{
	if (session->recvBuffer.GetUseSize() < sizeof(PacketHeader))
	{
		return false;
	}

	PacketHeader header;
	session->recvBuffer.Peek(reinterpret_cast<BYTE*>(&header), sizeof(PacketHeader));

	if (header.code != 0x89)
	{
		Server::Disconnect(session);
	}

	if (session->recvBuffer.GetUseSize() < header.size + sizeof(PacketHeader))
	{
		return false;
	}

	session->recvBuffer.MoveFront(sizeof(PacketHeader));
	Packet pkt(header.size);
	session->recvBuffer.Dequeue(pkt.GetBufferPtr(), header.size);
	pkt.MoveWritePos(header.size);

	switch (header.type)
	{
	case PacketType::C_MOVE_START:
	{
		Direction dir;
		int16 x;
		int16 y;

		pkt >> dir >> x >> y;
		Handle_C_MOVE_START(session, dir, x, y);
		break;
	}
	case PacketType::C_MOVE_STOP:
	{
		Direction dir;
		int16 x;
		int16 y;

		pkt >> dir >> x >> y;
		Handle_C_MOVE_STOP(session, dir, x, y);
		break;
	}
	case PacketType::C_ATTACK1:
	{
		Direction dir;
		int16 x;
		int16 y;

		pkt >> dir >> x >> y;
		Handle_C_ATTACK1(session, dir, x, y);
		break;
	}
	case PacketType::C_ATTACK2:
	{
		Direction dir;
		int16 x;
		int16 y;

		pkt >> dir >> x >> y;
		Handle_C_ATTACK2(session, dir, x, y);
		break;
	}
	case PacketType::C_ATTACK3:
	{
		Direction dir;
		int16 x;
		int16 y;

		pkt >> dir >> x >> y;
		Handle_C_ATTACK3(session, dir, x, y);
		break;
	}

	default:
		return false;
		break;
	}

	return true;
}
bool PacketHandler::Make_S_CREATE_MY_CHARACTER(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y, int8 hp)
{
	pkt << id << dir << x << y << hp;
	pkt.MakeHeader(PacketType::S_CREATE_MY_CHARACTER);
	return true;
}
bool PacketHandler::Make_S_CREATE_OTHER_CHARACTER(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y, int8 hp)
{
	pkt << id << dir << x << y << hp;
	pkt.MakeHeader(PacketType::S_CREATE_OTHER_CHARACTER);
	return true;
}
bool PacketHandler::Make_S_DELETE_CHARACTER(Packet& pkt, int32 id)
{
	pkt << id;
	pkt.MakeHeader(PacketType::S_DELETE_CHARACTER);
	return true;
}
bool PacketHandler::Make_S_MOVE_START(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y)
{
	pkt << id << dir << x << y;
	pkt.MakeHeader(PacketType::S_MOVE_START);
	return true;
}
bool PacketHandler::Make_S_MOVE_STOP(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y)
{
	pkt << id << dir << x << y;
	pkt.MakeHeader(PacketType::S_MOVE_STOP);
	return true;
}
bool PacketHandler::Make_S_ATTACK1(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y)
{
	pkt << id << dir << x << y;
	pkt.MakeHeader(PacketType::S_ATTACK1);
	return true;
}
bool PacketHandler::Make_S_ATTACK2(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y)
{
	pkt << id << dir << x << y;
	pkt.MakeHeader(PacketType::S_ATTACK2);
	return true;
}
bool PacketHandler::Make_S_ATTACK3(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y)
{
	pkt << id << dir << x << y;
	pkt.MakeHeader(PacketType::S_ATTACK3);
	return true;
}
bool PacketHandler::Make_S_DAMAGE(Packet& pkt, int32 attackId, int32 damageId, int8 damageHp)
{
	pkt << attackId << damageId << damageHp;
	pkt.MakeHeader(PacketType::S_DAMAGE);
	return true;
}