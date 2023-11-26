#pragma once

struct Session;
class Packet;
enum class Direction : BYTE;

enum class PacketType : BYTE
{
	S_CREATE_MY_CHARACTER = 0,
	S_CREATE_OTHER_CHARACTER = 1,
	S_DELETE_CHARACTER = 2,
	C_MOVE_START = 10,
	S_MOVE_START = 11,
	C_MOVE_STOP = 12,
	S_MOVE_STOP = 13,
	C_ATTACK1 = 20,
	S_ATTACK1 = 21,
	C_ATTACK2 = 22,
	S_ATTACK2 = 23,
	C_ATTACK3 = 24,
	S_ATTACK3 = 25,
	S_DAMAGE = 30,
};

struct PacketHeader
{
	BYTE code;
	BYTE size;
	PacketType type;
};

class PacketHandler                                                                   
{
public:
	static bool HandlePacket(Session* session);

	//////////////////
	/// MakePacket ///
	//////////////////
	static bool Make_S_CREATE_MY_CHARACTER(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y, int8 hp);
	static bool Make_S_CREATE_OTHER_CHARACTER(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y, int8 hp);
	static bool Make_S_DELETE_CHARACTER(Packet& pkt, int32 id);
	static bool Make_S_MOVE_START(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y);
	static bool Make_S_MOVE_STOP(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y);
	static bool Make_S_ATTACK1(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y);
	static bool Make_S_ATTACK2(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y);
	static bool Make_S_ATTACK3(Packet& pkt, int32 id, Direction& dir, int16 x, int16 y);
	static bool Make_S_DAMAGE(Packet& pkt, int32 attackId, int32 damageId, int8 damageHp);
};
