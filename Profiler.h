///@file	Profiler.h
///@brief	해당 파일을 인클루드하고 그 인클루드문 상단에 PROFILE을 define해주어야 하며, 더 세밀한 축정을 원하면 PROFILE_CLOCK을
///			define 해주면된다. 두개를 동시에 define하게되면 기본 마이크로초 기반으로 작동된다.
///@author	김호준
///@date	2023-08-20

#pragma once

#ifdef PROFILE
	#define PROFILE_BEGIN(TagName)		ProfileBegin(TagName, PROFILE_TYPE::MICRO_SECOND)
	#define PROFILE_END(TagName)		ProfileEnd(TagName, PROFILE_TYPE::MICRO_SECOND)
	#define PROFILE_DATA_OUT(FileName)	ProfileDataOutText(FileName, PROFILE_TYPE::MICRO_SECOND)
	#define PROFILE_RESET()				ProfileReset()
#else
	#ifndef PROFILE_CLOCK
		#define PROFILE_BEGIN(TagName)
		#define PROFILE_END(TagName)
		#define PROFILE_DATA_OUT(FileName)	
		#define PROFILE_RESET()	
	#endif
#endif


#ifndef PROFILE
	#ifdef PROFILE_CLOCK
		#define PROFILE_BEGIN(TagName)		ProfileBegin(TagName, PROFILE_TYPE::CPU_CLOCK)
		#define PROFILE_END(TagName)		ProfileEnd(TagName, PROFILE_TYPE::CPU_CLOCK)
		#define PROFILE_DATA_OUT(FileName)	ProfileDataOutText(FileName, PROFILE_TYPE::CPU_CLOCK)
		#define PROFILE_RESET()				ProfileReset()
	#else
		#define PROFILE_BEGIN(TagName)
		#define PROFILE_END(TagName)
		#define PROFILE_DATA_OUT(FileName)	
		#define PROFILE_RESET()		
	#endif
#endif

#include <Windows.h>


enum class PROFILE_TYPE
{
	NULL_TYPE,
	MICRO_SECOND,
	CPU_CLOCK,
};


/// @brief 구간에 대해 프로파일링(시간측정)을 시작한다.
/// @param name 프로파일 구간을 구분하는 태그
///	@param type 프로파일링 타입을 결정.\n
///				(마이크로 초 기반 = PROFILE_TYPE::MICRO_SECOND, CPU 클럭 기반 = PROFILE_TYPE::CPU_CLOCK) 
extern void ProfileBegin(const WCHAR* name, PROFILE_TYPE type = PROFILE_TYPE::MICRO_SECOND);

/// @brief 구간에 대해 프로파일링(시간측정)을 종료한다.
/// @param name 프로파일 구간을 구분하는 태그
///	@param type 프로파일링 타입을 결정.\n
///				(마이크로 초 기반 = PROFILE_TYPE::MICRO_SECOND, CPU 클럭 기반 = PROFILE_TYPE::CPU_CLOCK)
extern void ProfileEnd(const WCHAR* name, PROFILE_TYPE type = PROFILE_TYPE::MICRO_SECOND);


/// @brief 프로파일 결과를 file에 저장한다.
/// @param fileName 저장할 파일 이름(Default: "ProfileResult.txt")
///	@param type 프로파일링 타입을 결정.\n
///				(마이크로 초 기반 = PROFILE_TYPE::MICRO_SECOND, CPU 클럭 기반 = PROFILE_TYPE::CPU_CLOCK)
extern void ProfileDataOutText(const WCHAR* fileName = L"", PROFILE_TYPE type = PROFILE_TYPE::MICRO_SECOND);


/// @brief 프로파일 데이터를 초기화한다.
///
extern void ProfileReset();