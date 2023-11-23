#include "stdafx.h"
#include "Profiler.h"
#include <fstream>
#include <format>

void Profiler::Begin(const wstring_view name)
{
	Sample* sample = findSample(name);

	if (sample == nullptr)
	{
		sample = getEmptySample();
		sample->name = name;
		sample->callCount = 0;
	}
	else
	{
		if (sample->startTime.QuadPart != NULLTIME)
		{
			__debugbreak();
		}

		//sample->callCount++;
	}

	::QueryPerformanceCounter(&sample->startTime);
}

void Profiler::End(wstring_view name)
{
	LARGE_INTEGER endTime;
	::QueryPerformanceCounter(&endTime);

	Sample* sample = findSample(name);

	if (sample == nullptr)
	{
		__debugbreak();
	}

	if (sample->startTime.QuadPart == NULLTIME)
	{
		__debugbreak();
	}

	const int64 processTime = endTime.QuadPart - sample->startTime.QuadPart;

	sample->callCount += 1;
	sample->totalTime += processTime;
	sample->startTime.QuadPart = NULLTIME;

	// 최대, 최소값 갱신
	for (uint64& i : sample->max)
	{
		if (processTime > i)
		{
			i = processTime;
			break;
		}
	}

	for (uint64& i : sample->min)
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

void Profiler::DataOut(wstring_view fileName)
{
	wofstream fout(fileName.data());
	fout.imbue(std::locale("kor"));
	fout << L"┌----------------------------------┬-------------------┬-------------------┬-------------------┬---------------┐\n";
	fout << L"│               Name               │      Average      │        Min        │        Max        │     Call      │\n";
	fout << L"├----------------------------------┼-------------------┼-------------------┼-------------------┼---------------┤\n";

	int64 freq = getPerformanceFrequency().QuadPart;
	double perMicroSecond = static_cast<double>(freq) / SECOND_PER_MICRO_SECOND;

	for (const Sample& sample : profileSamples)
	{
		double timeAverage;
		if (sample.callCount == 0)
		{
			continue;
		}

		if (sample.callCount > 4)
		{
			timeAverage =
				static_cast<double>(sample.totalTime - (sample.max[0] + sample.max[1] + sample.min[0] + sample.min[1]))
				/ (sample.callCount - 4) / perMicroSecond;
		}
		else
		{
			timeAverage =
				static_cast<double>(sample.totalTime) / sample.callCount / perMicroSecond;
		}

		fout << ::format(
			L"│ {:<32} │ {:>14.1f} μs │ {:>14.1f} μs │ {:>14.1f} μs │ {:>13} │\n",
			sample.name,
			timeAverage,
			sample.min[0] / perMicroSecond,
			sample.max[0] / perMicroSecond,
			sample.callCount
			);
	}

	fout << L"└----------------------------------┴-------------------┴-------------------┴-------------------┴---------------┘\n";

	const time_t timer = ::time(NULL);
	tm localTime;
	::localtime_s(&localTime, &timer);

	fout << ::format(
		L"Updated at {}-{:0>2d}-{:0>2d} {:0>2d}:{:0>2d}:{:0>2d}\n",
		localTime.tm_year + 1900,
		localTime.tm_mon + 1,
		localTime.tm_mday,
		localTime.tm_hour,
		localTime.tm_min,
		localTime.tm_sec
	);

	fout.close();
}

void Profiler::DataOut()
{
	const time_t timer = ::time(NULL);
	tm localTime;
	::localtime_s(&localTime, &timer);

	wstring fileName = ::format(L"profile/Profile_{}{:0>2d}{:0>2d}_{:0>2d}{:0>2d}{:0>2d}.txt",
		localTime.tm_year + 1900,
		localTime.tm_mon + 1,
		localTime.tm_mday,
		localTime.tm_hour,
		localTime.tm_min,
		localTime.tm_sec
	);

	DataOut(fileName);
}

void Profiler::Reset()
{
	::memset(profileSamples, 0, sizeof(profileSamples));
}

LARGE_INTEGER Profiler::getPerformanceFrequency()
{
	static LARGE_INTEGER frequency{ 0, 0 };

	if (frequency.QuadPart == 0)
	{
		::QueryPerformanceFrequency(&frequency);
	}

	return frequency;
}

Profiler::Sample* Profiler::findSample(wstring_view name)
{
	int idx = 0;

	while (profileSamples[idx].useFlag != false)
	{
		if (profileSamples[idx].name.compare(name) == 0)
		{
			return &profileSamples[idx];
		}

		idx++;
	}

	return nullptr;
}

Profiler::Sample* Profiler::getEmptySample()
{
	Sample* space = profileSamples;
	while (space->useFlag != false)
	{
		space++;
	}

	space->useFlag = true;

	return space;
}
