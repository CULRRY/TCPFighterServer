#include "stdafx.h"
#include "NetworkAddress.h"

NetworkAddress::NetworkAddress(SOCKADDR_IN sockAddr) : _sockAddr(sockAddr)
{
}

NetworkAddress::NetworkAddress(const wstring_view ip, uint16 port)
{
	_sockAddr = {};
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr = Ip2Address(ip.data());
	_sockAddr.sin_port = ::htons(port);
}

NetworkAddress::NetworkAddress(uint32 ipType, uint16 port)
{
	_sockAddr = {};
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr.s_addr = ::htonl(ipType);
	_sockAddr.sin_port = ::htons(port);
}

wstring NetworkAddress::GetIpAddress()
{
	WCHAR buf[100];
	::InetNtopW(AF_INET, &_sockAddr.sin_addr, buf, 200);
	return buf;
}

IN_ADDR NetworkAddress::Ip2Address(const wstring_view ip)
{
	IN_ADDR address;
	::InetPton(AF_INET, ip.data(), &address);
	return address;
}
