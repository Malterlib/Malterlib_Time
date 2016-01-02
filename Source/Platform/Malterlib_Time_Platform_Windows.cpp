// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>

#include "../Malterlib_Time_Platform.h"

#include <CoreFoundation/CoreFoundation.h>
#include <sys/time.h>
#include <mach/mach_time.h>

inline_never int64 NMib::NTime::NPlatform::fg_Timer_Cycles()
{
#if 1
   int dummy[4];           // For unused returns 
   volatile int DontSkip;  // Volatile to prevent optimizing 
   int64 clock;          // Time 
   __cpuid(dummy, 0);      // Serialize 
   DontSkip = dummy[0];    // Prevent optimizing away cpuid 
   clock = __rdtsc();      // Read time 
   __cpuid(dummy, 0);      // Serialize again 
   DontSkip = dummy[0];    // Prevent optimizing away cpuid 
   return clock; 
#else
	NMib::NAtomic::fg_MemoryFence();
	int64 Ret = __rdtsc();
	NMib::NAtomic::fg_MemoryFence();
	return Ret;
#endif
}


#if DMibConfig_Tests_Enable

static NMib::NTime::NPlatform::EUnstableTimerMode gs_UnstableTimerMode = NMib::NTime::NPlatform::EUnstableTimerMode_Off;

void NMib::NTime::NPlatform::fg_MakeTimerUnstable(EUnstableTimerMode _Mode)
{
	gs_UnstableTimerMode = _Mode;
}

#endif

int64 NMib::NTime::NPlatform::fg_TimerRaw_PreciseGet()
{
#if DMibConfig_Tests_Enable

	switch (gs_UnstableTimerMode)
	{
	case NMib::NTime::NPlatform::EUnstableTimerMode_AlwaysZero:
		return 0;
	case NMib::NTime::NPlatform::EUnstableTimerMode_AlwaysMaxInt:
		return TCLimitsInt<int64>::mc_Max;
	case NMib::NTime::NPlatform::EUnstableTimerMode_AlwaysNegative:
		return TCLimitsInt<int64>::mc_Min;
	case NMib::NTime::NPlatform::EUnstableTimerMode_OnceZero:
		gs_UnstableTimerMode = NMib::NTime::NPlatform::EUnstableTimerMode_Off;
		return 0;
	case NMib::NTime::NPlatform::EUnstableTimerMode_OnceMaxInt:
		gs_UnstableTimerMode = NMib::NTime::NPlatform::EUnstableTimerMode_Off;
		return TCLimitsInt<int64>::mc_Max;
	case NMib::NTime::NPlatform::EUnstableTimerMode_OnceNegative:
		gs_UnstableTimerMode = NMib::NTime::NPlatform::EUnstableTimerMode_Off;
		return TCLimitsInt<int64>::mc_Min;
	}

#endif

	LARGE_INTEGER Temp;
	QueryPerformanceCounter(&Temp); // On systems that run Windows XP or later, the function will always succeed and will thus never return zero.
	return Temp.QuadPart;
}

int64 NMib::NTime::NPlatform::fg_TimerRaw_PreciseFrequency()
{
	LARGE_INTEGER Temp;
	QueryPerformanceFrequency(&Temp); // On systems that run Windows XP or later, the function will always succeed and will thus never return zero.
	return Temp.QuadPart;
}

namespace
{
	struct CIncreaseTimerPrecision
	{
		CIncreaseTimerPrecision()
		{
			timeBeginPeriod(1);
		}
		~CIncreaseTimerPrecision()
		{
			timeEndPeriod(1);
		}
	};

	TCAggregate<CIncreaseTimerPrecision> g_IncreaseTimerPrecision;
	bool g_bAggregatesDestroyed = false;

	TCAtomicAggregate<uint64> g_SafeTimerFullPrecision = {DAggregateInit};
}


void NMib::NTime::NPlatform::fg_TimerRaw_SafeIncreasePrecision()
{
	if (!g_bAggregatesDestroyed && !g_IncreaseTimerPrecision.f_WasDestructed())
		*g_IncreaseTimerPrecision;
}

#if DMibConfig_Tests_Enable
namespace
{
	uint32 g_SafeTimerOffset = 0;
}

void NMib::NTime::NPlatform::fg_MakeSafeTimerWrap(fp32 _InSeconds)
{
	g_SafeTimerOffset = (0xffffffff - timeGetTime()) - (_InSeconds * 1000.0).f_ToInt();
	g_SafeTimerFullPrecision = (g_SafeTimerFullPrecision.f_Load() & constant_uint64(0xFFFFFFFF00000000)) | g_SafeTimerOffset;
}

#endif

int64 NMib::NTime::NPlatform::fg_TimerRaw_SafeGet()
{
	while (true)
	{
#if DMibConfig_Tests_Enable
		uint32 Now = timeGetTime() + g_SafeTimerOffset;
#else
		uint32 Now = timeGetTime();
#endif
		uint64 CurrentFullPrecision = g_SafeTimerFullPrecision.f_Load();

		uint32 Current = CurrentFullPrecision & 0xffffffff;
		uint64 NewFullPresision;
		if (Now == Current)
			return CurrentFullPrecision;
		else if (Now > Current)
			NewFullPresision = (CurrentFullPrecision & constant_uint64(0xFFFFFFFF00000000)) | Now;
		else
			NewFullPresision = ((CurrentFullPrecision & constant_uint64(0xFFFFFFFF00000000)) + constant_uint64(0x100000000)) | Now;

		if (g_SafeTimerFullPrecision.f_CompareExchangeStrong(CurrentFullPrecision, NewFullPresision))
			return NewFullPresision;
	}
}

int64 NMib::NTime::NPlatform::fg_TimerRaw_SafeFrequency()
{
	return 1000;
}

void NMib::NTime::NPlatform::fg_TimeRaw_GetUTCOffset(NTime::CTimeSpan *_pTimeOffset)
{
	TIME_ZONE_INFORMATION TimeZone;
	fg_MemClear(TimeZone);
	uint32 CurrentDaylight = GetTimeZoneInformation(&TimeZone); // If this fails the bias will be 0 hours because we cleaned the TimeZone...
	if (CurrentDaylight == TIME_ZONE_ID_DAYLIGHT)
		*_pTimeOffset = NTime::CTimeSpanConvert::fs_CreateMinuteSpan((-TimeZone.Bias) + (-TimeZone.DaylightBias));
	else
		*_pTimeOffset = NTime::CTimeSpanConvert::fs_CreateMinuteSpan((-TimeZone.Bias));
}

void NMib::NTime::NPlatform::fg_TimeRaw_GetNow(NMib::NTime::CTime *_pTime)
{
	FILETIME Time;
	GetSystemTimeAsFileTime(&Time);
	NTime::CTime CurrentTime = fg_GetLocalSys()->m_FileTimeBase;

	LARGE_INTEGER Temp;
	Temp.LowPart = Time.dwLowDateTime;
	Temp.HighPart = Time.dwHighDateTime;

	NTime::CTimeSpan Add;
	uint64 Nano100 = Temp.QuadPart;
	int64 nSeconds = Nano100 / 10000000;
	Add.f_SetSeconds(nSeconds);
	Add.f_SetFraction(fp64(Nano100 % 10000000) / fp64(10000000));
	CurrentTime += Add;

	*_pTime = CurrentTime;
}

