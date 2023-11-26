#pragma once
#include <list>
#include "NetworkAddress.h"
#include "RingBuffer.h"

class Packet;
enum class Direction : BYTE;

enum class AttackType : BYTE
{
	NONE,
	ATTACK1,
	ATTACK2,
	ATTACK3,
};

struct Session
{
	SOCKET			socket;
	NetworkAddress	netInfo;
	RingBuffer		recvBuffer;
	RingBuffer		sendBuffer;
	int32			id;
	int16			x;
	int16			y;
	int8			hp;
	Direction		moveDir;
	Direction		attackDir;
	bool			isMove;
	AttackType		attackType;
};

class Server
{
public:
	static constexpr int32 MAX_USER				= 63;
	static constexpr int32 MAX_WIDTH			= 640;
	static constexpr int32 MAX_HEIGHT			= 480;

	static constexpr int32 RANGE_MOVE_TOP		= 50;
	static constexpr int32 RANGE_MOVE_LEFT		= 10;
	static constexpr int32 RANGE_MOVE_RIGHT		= 630;
	static constexpr int32 RANGE_MOVE_BOTTOM	= 470;

	static constexpr int32 ATTACK1_RANGE_X		= 80;
	static constexpr int32 ATTACK2_RANGE_X		= 90;
	static constexpr int32 ATTACK3_RANGE_X		= 100;
	static constexpr int32 ATTACK1_RANGE_Y		= 10;
	static constexpr int32 ATTACK2_RANGE_Y		= 10;
	static constexpr int32 ATTACK3_RANGE_Y		= 20;

public:
	static void Init();
	static void Network();
	static void Update();
	static bool OnAccept();
	static void OnRecv(Session* session);
	static void OnSend(Session* session);
	//static void SendUnicast(Session* session, PacketType type, BYTE* pkt, uint8 len);
	static void SendUnicast(Session* session, Packet& pkt);
	//static void SendBroadcast(Session* exceptSession, PacketType type, BYTE* pkt, uint8 len);
	static void SendBroadcast(Session* exceptSession, Packet& pkt);
	static void Disconnect(Session* session);
	static bool IsAttackRange(Session* session, Session* target, int32 rangeX, int32 rangeY);
	static bool IsAlowableRange(Session* session, int32 x, int32 y);

	inline static list<Session*>	sessions;
	inline static SOCKET			listenSocket = INVALID_SOCKET                                                                                                                                                                                                                                                                                                                                                                                                                                                       ;
	inline static int32				connectClients = 0;
	inline static int32				alignId = 0;
};

