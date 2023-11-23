#pragma once
#include "Protocol.h"

class Packet;
struct Session;
struct PacketHeader
{
	BYTE		code;
	BYTE		size;
	PacketType	type;
};


bool IsAlowableRange(Session* session, int32 x, int32 y);

//bool Handle_C_MOVE_START(Session* session, const protocol::C_MOVE_START* pkt);
bool Handle_C_MOVE_START(Session* session, Packet& pkt);
bool Handle_C_MOVE_STOP(Session* session, Packet& pkt);
//bool Handle_C_ATTACK1(Session* session, const protocol::C_ATTACK1* pkt);
bool Handle_C_ATTACK1(Session* session, Packet& pkt);
//bool Handle_C_ATTACK2(Session* session, const protocol::C_ATTACK2* pkt);
bool Handle_C_ATTACK2(Session* session, Packet& pkt);
//bool Handle_C_ATTACK3(Session* session, const protocol::C_ATTACK3* pkt);
bool Handle_C_ATTACK3(Session* session, Packet& pkt);


bool Make_S_CREATE_MY_CHARACTER(Packet& pkt, int32 id, Direction dir, int16 x, int16 y, int8 hp);
bool Make_S_CREATE_OTHER_CHARACTER(Packet& pkt, int32 id, Direction dir, int16 x, int16 y, int8 hp);
bool Make_S_DELETE_CHARACTER(Packet& pkt, int32 id);
bool Make_S_MOVE_START(Packet& pkt, int32 id, Direction dir, int16 x, int16 y);
bool Make_S_MOVE_STOP(Packet& pkt, int32 id, Direction dir, int16 x, int16 y);
bool Make_S_ATTACK1(Packet& pkt, int32 id, Direction dir, int16 x, int16 y);
bool Make_S_ATTACK2(Packet& pkt, int32 id, Direction dir, int16 x, int16 y);
bool Make_S_ATTACK3(Packet& pkt, int32 id, Direction dir, int16 x, int16 y);
bool Make_S_DAMAGE(Packet& pkt, int32 attackId, int32 damageId, int8 damageHp);
