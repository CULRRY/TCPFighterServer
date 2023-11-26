#pragma once

struct Session;
class Packet;
{%- for type in parser.user_type %}
{{type["decl"]}};
{%- endfor %}

enum class PacketType : BYTE
{
{%- for pkt in  parser.total_pkt %}
	{{pkt.name}} = {{pkt.id}},
{%- endfor %}
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
	{%- for pkt in parser.send_pkt %}
	static bool Make_{{pkt.name}}(Packet& pkt,{{' '}}{%- for var in pkt.data %}{{var["type"]}}{%- if var["isRefer"] == True %}&{%- endif %} {{var["name"]}}{%- if loop.index != pkt.data|length %},{{' '}}{%- endif %}{%- endfor %});
	{%- endfor %}
};

