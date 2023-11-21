#include "stdafx.h"
#include "Profiler.h"
#include <iostream>

enum : DWORD64
{
	NULLTIME = UINT64_MAX,
};

enum
{
	SECOND_PER_MICRO_SECOND = 1'000'000,
};

struct ProfileSample
{

	bool			useFlag;	///< 현재 인덱스를 사용중인지 확인하는 플래그

	const WCHAR*	name;		///< 구간 이름

	DWORD64			startTime;	///< 프로파일 샘플 실행 시작 시간
	DWORD64			totalTime;	///< 누적 사용시간
	DWORD64			min[2];		///< 최소 사용시간 [0] 가장 최소 [1] 두번쨰 최소
	DWORD64			max[2];		///< 최대 사용시간 [0] 가장 최대 [1] 두번째 최대
	DWORD64			callCount;	///< 누적 호출 횟수
};

static ProfileSample	g_ProfileSamples[100];
static PROFILE_TYPE		g_ProfileType = PROFILE_TYPE::NULL_TYPE; ///< 두개의 프로파일 타입을 중복해서 사용하는 경우를 막는 변수

/// @brief		초 단위로 변경할 떄, 필요한 QueryPerformanceFrequency를 받아온다.
///	@details	frequency를 함수 호출때마다 얻어올 수 없으므로 맨 처음 Get할때만 얻어온다.
static LARGE_INTEGER GetPerformanceFrequency()
{
	static LARGE_INTEGER frequency{ 0, 0 };

	if (frequency.QuadPart == 0)
	{
		::QueryPerformanceFrequency(&frequency);
	}

	return frequency;
}

/// @brief name을 통해서 샘플을 가져온다.
/// @param name 프로파일 구간을 구분하는 태그
/// @return 찾으면 해당 샘플의 포인터 리턴. 없으면 nullptr 리턴.
static ProfileSample* FindSample(const WCHAR* name)
{
	int idx = 0;

	while (g_ProfileSamples[idx].useFlag != false)
	{
		if (::wcscmp(name, g_ProfileSamples[idx].name) == 0)
		{
			return &g_ProfileSamples[idx];
		}

		idx++;
	}

	return nullptr;
}

/// @brief		샘플을 배열에 추가한다.
///	@details	인자로 넘겨주는 구조체는 useFlag를 제외한 정보를 미리 담고 넘겨줘야함
/// @param		sample 추가하고자 하는 샘플 구조체의 주소
static void InsertSample(const ProfileSample* sample)
{
	ProfileSample* space = g_ProfileSamples;
	while (space->useFlag != false)
	{
		space++;
	}

	::memcpy(space, sample, sizeof(ProfileSample));
	space->useFlag = true;
}

void ProfileBegin(const WCHAR* name, PROFILE_TYPE type)
{
	DWORD64 startTime;

	switch (type)
	{
	// NULL_TYPE은 중복 감지용이므로 해당 데이터가 인자로 들어오는 경우는 잘못 사용한것
	case PROFILE_TYPE::NULL_TYPE:
		__debugbreak();
		break;

	case PROFILE_TYPE::MICRO_SECOND:
		LARGE_INTEGER tmpTime;
		::QueryPerformanceCounter(&tmpTime);
		startTime = tmpTime.QuadPart;
		break;

	case PROFILE_TYPE::CPU_CLOCK:
		startTime = __rdtsc();
		break;
	}

	// 두 개의 프로파일 타입을 혼용해서 사용하는 경우에 예외발생 크래쉬
	if (g_ProfileType == PROFILE_TYPE::NULL_TYPE)
	{
		g_ProfileType = type;
	}

	if (g_ProfileType != type)
	{
		__debugbreak();
	}


	ProfileSample* sample = FindSample(name);


	if (sample == nullptr)
	{
		ProfileSample tmp{};
		tmp.name = name;
		tmp.startTime = startTime;
		tmp.callCount = 1;
		InsertSample(&tmp);
	}
	else
	{
		// Begin이후에 End를 하지않는 경우 크래쉬
		if (sample->startTime != NULLTIME)
		{
			__debugbreak();
		}

		sample->startTime = startTime;
		sample->callCount++;
	}
}

void ProfileEnd(const WCHAR* name, PROFILE_TYPE type)
{
	DWORD64 endTime;

	switch (type)
	{
		// NULL_TYPE은 중복 감지용이므로 해당 데이터가 인자로 들어오는 경우는 잘못 사용한것
	case PROFILE_TYPE::NULL_TYPE:
		__debugbreak();
		break;

	case PROFILE_TYPE::MICRO_SECOND:
		LARGE_INTEGER tmpTime;
		::QueryPerformanceCounter(&tmpTime);
		endTime = tmpTime.QuadPart;
		break;

	case PROFILE_TYPE::CPU_CLOCK:
		endTime = __rdtsc();
		break;
	}

	// 두 개의 프로파일 타입을 혼용해서 사용하는 경우에 예외발생 크래쉬
	if (g_ProfileType == PROFILE_TYPE::NULL_TYPE)
	{
		g_ProfileType = type;
	}

	if (g_ProfileType != type)
	{
		__debugbreak();
	}


	ProfileSample* sample = FindSample(name);


	// 데이터가 없는데, End를 호출하는 경우 크래쉬
	if (sample == nullptr)
	{
		__debugbreak();
	}


	// Begin하지않고, End를 하는 경우 크래쉬
	if (sample->startTime == NULLTIME)
	{
		__debugbreak();
	}

	const DWORD64 processTime = endTime - sample->startTime;;
	sample->totalTime += processTime;
	sample->startTime = NULLTIME;

	// 최대, 최소값 갱신
	for (DWORD64& i : sample->max)
	{
		if (processTime > i)
		{
			i = processTime;
			break;
		}
	}

	for (DWORD64& i : sample->min)
	{
		if (processTime < i)
		{
			i = processTime;
			break;
		}

		if (i == 0)
		{
			i = processTime;
			break;
		}
	}
}

void ProfileDataOutText(const WCHAR* fileName, PROFILE_TYPE type)
{
	// NULL_TYPE은 중복 감지용이므로 해당 데이터가 인자로 들어오는 경우는 잘못 사용한것
	if (type == PROFILE_TYPE::NULL_TYPE)
		__debugbreak();

	// 두 개의 프로파일 타입을 혼용해서 사용하는 경우에 예외발생 크래쉬
	if (g_ProfileType == PROFILE_TYPE::NULL_TYPE)
	{
		g_ProfileType = type;
	}

	if (g_ProfileType != type)
	{
		__debugbreak();
	}

	const HANDLE hFile = ::CreateFile(fileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		__debugbreak();
	}
	// 유니코드 출력을 위한 마스크
	unsigned short mark = 0xFEFF;
	::WriteFile(hFile, &mark, sizeof(mark), NULL, NULL);

	WCHAR writeBuffer[1024] = { 0, };
	if (type == PROFILE_TYPE::CPU_CLOCK)
	{
		::wcscat_s(writeBuffer, L"*Profile result by CPU clock*\n\n");
	}
	else if (type == PROFILE_TYPE::MICRO_SECOND)
	{
		::wcscat_s(writeBuffer, L"*Profile result by micro second*\n\n");
	}


	::wcscat_s(writeBuffer, L"┌------------------┬-------------------┬-------------------┬-------------------┬---------------┐\n");
	::wcscat_s(writeBuffer, L"│       Name       │      Average      │        Min        │        Max        │     Call      │\n");
	::wcscat_s(writeBuffer, L"├------------------┼-------------------┼-------------------┼-------------------┼---------------┤\n");


	::WriteFile(hFile, writeBuffer, sizeof(WCHAR) * static_cast<DWORD>(wcslen(writeBuffer)), NULL, NULL);

	// μs단위로 표기하기 위해, 현재 진동주기에 1000000을 나누어줌.
	const __int64 freq = GetPerformanceFrequency().QuadPart;
	const double perMicroSecond = static_cast<double>(freq) / SECOND_PER_MICRO_SECOND;

	for (const ProfileSample& sample : g_ProfileSamples)
	{
		WCHAR line[1024] = { 0, };
		if (sample.useFlag == false)
			break;

		if (type == PROFILE_TYPE::MICRO_SECOND)
		{
			double timeAverage;

			// callCount가 3이상이면 평균을 구할 떄, 초반 실행시 PageFault나 CacheMiss를 배제하기위해 최대값 두개를 뺀다.
			switch (sample.callCount)
			{
			case 0:
				__debugbreak(); // callCount가 0인데 들어오면 문제가 있으므로 크래쉬
				break;
			case 1:
			case 2:
			case 3:
			case 4:
				timeAverage = static_cast<double>(sample.totalTime) / sample.callCount / perMicroSecond;
				break;
			default:
				timeAverage = static_cast<double>(sample.totalTime - sample.max[0] - sample.max[1] - sample.min[0] - sample.min[1])
					/ (sample.callCount - 4) / perMicroSecond;
			}

			::swprintf_s(line, sizeof(line), L"│ %-16ls │ %14.1lf μs │ %14.1lf μs │ %14.1lf μs │ %13lld │\n",
				sample.name,
				timeAverage,
				sample.min[0] / perMicroSecond,
				sample.max[0] / perMicroSecond,
				sample.callCount);
		}
		else if (type == PROFILE_TYPE::CPU_CLOCK)
		{
			DWORD64 timeAverage;

			// callCount가 3이상이면 평균을 구할 떄, 초반 실행시 PageFault나 CacheMiss를 배제하기위해 최대값 두개를 뺀다.
			switch (sample.callCount)
			{
			case 0:
				__debugbreak(); // callCount가 0인데 들어오면 문제가 있으므로 크래쉬
				break;
			case 1:
			case 2:
			case 3:
			case 4:
				timeAverage = sample.totalTime / sample.callCount;
				break;
			default:
				timeAverage = (sample.totalTime - sample.max[0] - sample.max[1] - sample.min[0] - sample.min[1])
					/ (sample.callCount - 4);
			}

			::swprintf_s(line, sizeof(line), L"│ %-16ls │ %13lld clk │ %13lld clk │ %13lld clk │ %13lld │\n",
				sample.name,
				timeAverage,
				sample.min[0],
				sample.max[0],
				sample.callCount);
		}

		::WriteFile(hFile, line, sizeof(WCHAR) * static_cast<DWORD>(wcslen(line)), NULL, NULL);
	}

	::swprintf_s(writeBuffer, L"└------------------┴-------------------┴-------------------┴-------------------┴---------------┘\n\0");
	::WriteFile(hFile, writeBuffer, sizeof(WCHAR) * static_cast<DWORD>(wcslen(writeBuffer)), NULL, NULL);

	const time_t timer = ::time(NULL);
	tm localTime;
	::localtime_s(&localTime, &timer);

	::swprintf_s(
		writeBuffer,
		L"Updated at %d-%02d-%02d %02d:%02d:%02d\n\0",
		localTime.tm_year + 1900,
		localTime.tm_mon + 1,
		localTime.tm_mday,
		localTime.tm_hour,
		localTime.tm_min,
		localTime.tm_sec
	);

	::WriteFile(hFile, writeBuffer, sizeof(WCHAR) * static_cast<DWORD>(wcslen(writeBuffer)), NULL, NULL);

	CloseHandle(hFile);
}

void ProfileReset()
{
	::memset(g_ProfileSamples, 0, sizeof(g_ProfileSamples));
}
