#pragma once
struct Session;
{%- for type in parser.user_type %}
{{type["decl"]}};
{%- endfor %}

{%- for pkt in parser.recv_pkt %}
bool Handle_{{pkt.name}}(Session* session,{{' '}}{%- for var in pkt.data %}{{var["type"]}}{%- if var["isRefer"] == True %}&{%- endif %} {{var["name"]}}{%- if loop.index != pkt.data|length %},{{' '}}{%- endif %}{%- endfor %});
{%- endfor %}

