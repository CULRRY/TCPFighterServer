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

	bool			useFlag;	///< ���� �ε����� ��������� Ȯ���ϴ� �÷���

	const WCHAR*	name;		///< ���� �̸�

	DWORD64			startTime;	///< �������� ���� ���� ���� �ð�
	DWORD64			totalTime;	///< ���� ���ð�
	DWORD64			min[2];		///< �ּ� ���ð� [0] ���� �ּ� [1] �ι��� �ּ�
	DWORD64			max[2];		///< �ִ� ���ð� [0] ���� �ִ� [1] �ι�° �ִ�
	DWORD64			callCount;	///< ���� ȣ�� Ƚ��
};

static ProfileSample	g_ProfileSamples[100];
static PROFILE_TYPE		g_ProfileType = PROFILE_TYPE::NULL_TYPE; ///< �ΰ��� �������� Ÿ���� �ߺ��ؼ� ����ϴ� ��츦 ���� ����

/// @brief		�� ������ ������ ��, �ʿ��� QueryPerformanceFrequency�� �޾ƿ´�.
///	@details	frequency�� �Լ� ȣ�⶧���� ���� �� �����Ƿ� �� ó�� Get�Ҷ��� ���´�.
static LARGE_INTEGER GetPerformanceFrequency()
{
	static LARGE_INTEGER frequency{ 0, 0 };

	if (frequency.QuadPart == 0)
	{
		::QueryPerformanceFrequency(&frequency);
	}

	return frequency;
}

/// @brief name�� ���ؼ� ������ �����´�.
/// @param name �������� ������ �����ϴ� �±�
/// @return ã���� �ش� ������ ������ ����. ������ nullptr ����.
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

/// @brief		������ �迭�� �߰��Ѵ�.
///	@details	���ڷ� �Ѱ��ִ� ����ü�� useFlag�� ������ ������ �̸� ��� �Ѱ������
/// @param		sample �߰��ϰ��� �ϴ� ���� ����ü�� �ּ�
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
	// NULL_TYPE�� �ߺ� �������̹Ƿ� �ش� �����Ͱ� ���ڷ� ������ ���� �߸� ����Ѱ�
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

	// �� ���� �������� Ÿ���� ȥ���ؼ� ����ϴ� ��쿡 ���ܹ߻� ũ����
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
		// Begin���Ŀ� End�� �����ʴ� ��� ũ����
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
		// NULL_TYPE�� �ߺ� �������̹Ƿ� �ش� �����Ͱ� ���ڷ� ������ ���� �߸� ����Ѱ�
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

	// �� ���� �������� Ÿ���� ȥ���ؼ� ����ϴ� ��쿡 ���ܹ߻� ũ����
	if (g_ProfileType == PROFILE_TYPE::NULL_TYPE)
	{
		g_ProfileType = type;
	}

	if (g_ProfileType != type)
	{
		__debugbreak();
	}


	ProfileSample* sample = FindSample(name);


	// �����Ͱ� ���µ�, End�� ȣ���ϴ� ��� ũ����
	if (sample == nullptr)
	{
		__debugbreak();
	}


	// Begin�����ʰ�, End�� �ϴ� ��� ũ����
	if (sample->startTime == NULLTIME)
	{
		__debugbreak();
	}

	const DWORD64 processTime = endTime - sample->startTime;;
	sample->totalTime += processTime;
	sample->startTime = NULLTIME;

	// �ִ�, �ּҰ� ����
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
	// NULL_TYPE�� �ߺ� �������̹Ƿ� �ش� �����Ͱ� ���ڷ� ������ ���� �߸� ����Ѱ�
	if (type == PROFILE_TYPE::NULL_TYPE)
		__debugbreak();

	// �� ���� �������� Ÿ���� ȥ���ؼ� ����ϴ� ��쿡 ���ܹ߻� ũ����
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
	// �����ڵ� ����� ���� ����ũ
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


	::wcscat_s(writeBuffer, L"��------------------��-------------------��-------------------��-------------------��---------------��\n");
	::wcscat_s(writeBuffer, L"��       Name       ��      Average      ��        Min        ��        Max        ��     Call      ��\n");
	::wcscat_s(writeBuffer, L"��------------------��-------------------��-------------------��-------------------��---------------��\n");


	::WriteFile(hFile, writeBuffer, sizeof(WCHAR) * static_cast<DWORD>(wcslen(writeBuffer)), NULL, NULL);

	// ��s������ ǥ���ϱ� ����, ���� �����ֱ⿡ 1000000�� ��������.
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

			// callCount�� 3�̻��̸� ����� ���� ��, �ʹ� ����� PageFault�� CacheMiss�� �����ϱ����� �ִ밪 �ΰ��� ����.
			switch (sample.callCount)
			{
			case 0:
				__debugbreak(); // callCount�� 0�ε� ������ ������ �����Ƿ� ũ����
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

			::swprintf_s(line, sizeof(line), L"�� %-16ls �� %14.1lf ��s �� %14.1lf ��s �� %14.1lf ��s �� %13lld ��\n",
				sample.name,
				timeAverage,
				sample.min[0] / perMicroSecond,
				sample.max[0] / perMicroSecond,
				sample.callCount);
		}
		else if (type == PROFILE_TYPE::CPU_CLOCK)
		{
			DWORD64 timeAverage;

			// callCount�� 3�̻��̸� ����� ���� ��, �ʹ� ����� PageFault�� CacheMiss�� �����ϱ����� �ִ밪 �ΰ��� ����.
			switch (sample.callCount)
			{
			case 0:
				__debugbreak(); // callCount�� 0�ε� ������ ������ �����Ƿ� ũ����
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

			::swprintf_s(line, sizeof(line), L"�� %-16ls �� %13lld clk �� %13lld clk �� %13lld clk �� %13lld ��\n",
				sample.name,
				timeAverage,
				sample.min[0],
				sample.max[0],
				sample.callCount);
		}

		::WriteFile(hFile, line, sizeof(WCHAR) * static_cast<DWORD>(wcslen(line)), NULL, NULL);
	}

	::swprintf_s(writeBuffer, L"��------------------��-------------------��-------------------��-------------------��---------------��\n\0");
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
