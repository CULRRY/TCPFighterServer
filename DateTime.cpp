#include "stdafx.h"
#include "DateTime.h"

DateTime::DateTime()
{
	const time_t timer = ::time(nullptr);
	errno_t err = ::localtime_s(&_time, &timer);
}

wstring DateTime::to_string(wstring_view fmt)
{
	WCHAR str[100];

	int32 size = wcsftime(str, 200, fmt.data(), &_time);

	wstring ret(str);
	return str;
}
