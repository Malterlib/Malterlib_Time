// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NTime::NPlatform
{
#if DMibConfig_Tests_Enable && defined(DMibSafeTimerAvailable)

	enum EUnstableTimerMode
	{
		EUnstableTimerMode_Off
		, EUnstableTimerMode_AlwaysZero
		, EUnstableTimerMode_AlwaysMaxInt
		, EUnstableTimerMode_AlwaysNegative
		, EUnstableTimerMode_OnceZero
		, EUnstableTimerMode_OnceMaxInt
		, EUnstableTimerMode_OnceNegative
	};

	void fg_MakeTimerUnstable(EUnstableTimerMode _Mode);
	int64 fg_MakeSafeTimerWrap(fp32 _InSeconds, uint32 _Where);
	int64 fg_TimerRaw_SafeOffset(fp32 _InSeconds);
#endif

	int64 fg_TimerRaw_PreciseGet();
	int64 fg_TimerRaw_PreciseFrequency();
	int64 fg_TimerRaw_GetCPUFrequency();
	void fg_TimerRaw_SafeIncreasePrecision();
	int64 fg_TimerRaw_SafeGet();
	int64 fg_TimerRaw_SafeFrequency();

	fp64 fg_TimerRaw_PreciseResolution();
	fp64 fg_TimerRaw_SafeResolution();

	int64 fg_Timer_Cycles();
	int64 fg_Timer_CyclesFast();
	int64 fg_Timer_CyclesRawFrequency();

	void fg_TimeRaw_GetNow(NTime::CTime *_pTime);
	void fg_TimeRaw_GetUTCOffset(NTime::CTimeSpan *_pTimeOffset);
	fp64 fg_TimeRaw_Resolution();

	NTime::CTime fg_TimeRaw_ToLocal(NTime::CTime const &_Time);
	NTime::CTime fg_TimeRaw_ToUtc(NTime::CTime const &_Time);
}

#if defined(DCompiler_clang)
#	include "Platform/Malterlib_Time_Platform_clang.hpp"
#elif defined(DCompiler_MSVC)
#	include "Platform/Malterlib_Time_Platform_MSVC.hpp"
#endif
