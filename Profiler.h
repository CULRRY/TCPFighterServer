///@file	Profiler.h
///@brief	�ش� ������ ��Ŭ����ϰ� �� ��Ŭ��幮 ��ܿ� PROFILE�� define���־�� �ϸ�, �� ������ ������ ���ϸ� PROFILE_CLOCK��
///			define ���ָ�ȴ�. �ΰ��� ���ÿ� define�ϰԵǸ� �⺻ ����ũ���� ������� �۵��ȴ�.
///@author	��ȣ��
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


/// @brief ������ ���� �������ϸ�(�ð�����)�� �����Ѵ�.
/// @param name �������� ������ �����ϴ� �±�
///	@param type �������ϸ� Ÿ���� ����.\n
///				(����ũ�� �� ��� = PROFILE_TYPE::MICRO_SECOND, CPU Ŭ�� ��� = PROFILE_TYPE::CPU_CLOCK) 
extern void ProfileBegin(const WCHAR* name, PROFILE_TYPE type = PROFILE_TYPE::MICRO_SECOND);

/// @brief ������ ���� �������ϸ�(�ð�����)�� �����Ѵ�.
/// @param name �������� ������ �����ϴ� �±�
///	@param type �������ϸ� Ÿ���� ����.\n
///				(����ũ�� �� ��� = PROFILE_TYPE::MICRO_SECOND, CPU Ŭ�� ��� = PROFILE_TYPE::CPU_CLOCK)
extern void ProfileEnd(const WCHAR* name, PROFILE_TYPE type = PROFILE_TYPE::MICRO_SECOND);


/// @brief �������� ����� file�� �����Ѵ�.
/// @param fileName ������ ���� �̸�(Default: "ProfileResult.txt")
///	@param type �������ϸ� Ÿ���� ����.\n
///				(����ũ�� �� ��� = PROFILE_TYPE::MICRO_SECOND, CPU Ŭ�� ��� = PROFILE_TYPE::CPU_CLOCK)
extern void ProfileDataOutText(const WCHAR* fileName = L"", PROFILE_TYPE type = PROFILE_TYPE::MICRO_SECOND);


/// @brief �������� �����͸� �ʱ�ȭ�Ѵ�.
///
extern void ProfileReset();