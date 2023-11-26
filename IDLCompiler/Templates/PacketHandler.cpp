#include "stdafx.h"
#include "PacketHandler.h"
#include "HandleRecvPacket.h"
#include "Packet.h"
#include "Server.h"
{%- for type in parser.user_type %}
#include "{{type["header"]}}"
{%- endfor %}


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
{%- for pkt in parser.recv_pkt %}
	case PacketType::{{pkt.name}}:
	{
		{%- for var in pkt.data %}
		{{var["type"]}} {{var["name"]}};
		{%- endfor %}

		pkt{%- for var in pkt.data %} >> {{var["name"]}}{%- endfor %};
		Handle_{{pkt.name}}(session,{{" "}}{%- for var in pkt.data %}{{var["name"]}}{%- if loop.index != pkt.data | length %},{{' '}}{%- endif %}{%- endfor %});
		break;
	}
{%- endfor %}

	default:
		return false;
		break;
	}

	return true;
}


{%- for pkt in parser.send_pkt %}
bool PacketHandler::Make_{{pkt.name}}(Packet& pkt,{{' '}}{%- for var in pkt.data %}{{var["type"]}}{%- if var["isRefer"] == True %}&{%- endif %} {{var["name"]}}{%- if loop.index != pkt.data|length %},{{' '}}{%- endif %}{%- endfor %})
{
	pkt{%- for var in pkt.data %} << {{var["name"]}}{%- endfor %};
	pkt.MakeHeader(PacketType::{{pkt.name}});
	return true;
}
{%- endfor %}