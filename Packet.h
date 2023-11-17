#pragma once
#include "Protocol.h"

struct Session;
struct PacketHeader
{
	BYTE		code;
	BYTE		size;
	PacketType	type;
};


bool Handle_C_MOVE_START(Session* session, const protocol::C_MOVE_START* pkt);
bool Handle_C_MOVE_STOP(Session* session, const protocol::C_MOVE_STOP* pkt);
bool Handle_C_ATTACK1(Session* session, const protocol::C_ATTACK1* pkt);
bool Handle_C_ATTACK2(Session* session, const protocol::C_ATTACK2* pkt);
bool Handle_C_ATTACK3(Session* session, const protocol::C_ATTACK3* pkt);