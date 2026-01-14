// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include <Mib/Core/PlatformSpecific/WindowsError>
#include <Mib/Core/PlatformSpecific/WindowsFile>
#include <Mib/Core/PlatformSpecific/WindowsOptional>
#include <Mib/Core/PlatformSpecific/WindowsRegistry>

#include "../Malterlib_Time_Platform.h"

#include <Windows.h>

#ifndef DCompiler_clang
inline_never int64 NMib::NTime::NPlatform::fg_Timer_Cycles()
{
#ifndef DArchitecture_arm64
	int64 Ret;          // Time
	int Dummy[4];           // For unused returns
	volatile int DontSkip;  // Volatile to prevent optimizing
	__cpuid(Dummy, 0);      // Serialize
	DontSkip = Dummy[0];    // Prevent optimizing away cpuid
#if defined(DArchitecture_x86) || defined(DArchitecture_x64)
	Ret = __rdtsc();     // Read time
#else
	#error "Implement this";
#endif
	__cpuid(Dummy, 0);      // Serialize again
	DontSkip = Dummy[0];    // Prevent optimizing away cpuid
	return Ret;
#else
	NMib::NAtomic::fg_MemoryFence();
#if defined(DArchitecture_x86) || defined(DArchitecture_x64)
	int64 Ret = __rdtsc(); 
#elif defined(DArchitecture_arm64)
	int64 Ret = _ReadStatusReg(DMibArm64_CNTVCT_EL0) * g_CyclesScale;
#else
	#error "Implement this";
#endif
	NMib::NAtomic::fg_MemoryFence();
	return Ret;
#endif
}
#endif

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
	case NMib::NTime::NPlatform::EUnstableTimerMode_Off:
		break;
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


fp64 NMib::NTime::NPlatform::fg_TimerRaw_PreciseResolution()
{
	return fp64(1.0) / fp64(fg_TimerRaw_SafeFrequency());
}

fp64 NMib::NTime::NPlatform::fg_TimerRaw_SafeResolution()
{
	return fp64(1.0) / fp64(fg_TimerRaw_SafeFrequency());
}

extern bool g_bAggregatesDestroyed;

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

	constinit NMib::NStorage::TCAggregate<CIncreaseTimerPrecision> g_IncreaseTimerPrecision = {DAggregateInit};
	constinit NMib::NAtomic::TCAtomicAggregate<uint64> g_SafeTimerFullPrecision = {DAggregateInit};
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

int64 NMib::NTime::NPlatform::fg_MakeSafeTimerWrap(fp32 _InSeconds, uint32 _Where)
{
	int64 LastValue = fg_TimerRaw_SafeGet();
	if (_Where == 0)
		g_SafeTimerOffset = (0x40000000 - timeGetTime()) - (_InSeconds * 1000.0).f_ToInt();
	else if (_Where == 1)
		g_SafeTimerOffset = (0x80000000 - timeGetTime()) - (_InSeconds * 1000.0).f_ToInt();
	else if (_Where == 2)
		g_SafeTimerOffset = (0xC0000000 - timeGetTime()) - (_InSeconds * 1000.0).f_ToInt();
	else
		g_SafeTimerOffset = (0xffffffff - timeGetTime()) - (_InSeconds * 1000.0).f_ToInt();
	g_SafeTimerFullPrecision = (g_SafeTimerFullPrecision.f_Load() & constant_uint64(0xFFFFFFFF00000000)) | (g_SafeTimerOffset + timeGetTime());
	return fg_TimerRaw_SafeGet() - LastValue;
}

int64 NMib::NTime::NPlatform::fg_TimerRaw_SafeOffset(fp32 _InSeconds)
{
	int64 LastValue = fg_TimerRaw_SafeGet();
	(int32 &)g_SafeTimerOffset += (int32)(_InSeconds * 1000.0).f_ToInt();
	return fg_TimerRaw_SafeGet() - LastValue;
}

#endif

int64 NMib::NTime::NPlatform::fg_TimerRaw_SafeGet()
{
	while (true)
	{
		uint64 LastFullPrecision = g_SafeTimerFullPrecision.f_Load();
#if DMibConfig_Tests_Enable
		uint32 Now = timeGetTime() + g_SafeTimerOffset;
#else
		uint32 Now = timeGetTime();
#endif

		uint32 Last = LastFullPrecision & 0xffffffff;
		uint64 NewFullPresision;

		if (LastFullPrecision == constant_uint64(0)) // First time called
			NewFullPresision = constant_uint64(0x100000000) | Now;
		else if (Last < 0x40000000u && Now > 0xC0000000u) // Went backwards wrap
			return LastFullPrecision - constant_uint64(0x100000000);
		else if (Now < 0x40000000u && Last > 0xC0000000u) // Wrapped
			NewFullPresision = ((LastFullPrecision & constant_uint64(0xFFFFFFFF00000000)) + constant_uint64(0x100000000)) | Now;
		else if (Now == Last) // No change
			return LastFullPrecision - constant_uint64(0x100000000);
		else if (Now < Last) // Went backwards.
			return LastFullPrecision - constant_uint64(0x100000000);
		else // Normal forward tick
		{
			DMibFastCheck(Now > Last);
			NewFullPresision = (LastFullPrecision & constant_uint64(0xFFFFFFFF00000000)) | Now;
		}

		if (g_SafeTimerFullPrecision.f_CompareExchangeStrong(LastFullPrecision, NewFullPresision))
			return NewFullPresision - constant_uint64(0x100000000);
	}
}

int64 NMib::NTime::NPlatform::fg_TimerRaw_SafeFrequency()
{
	return 1000;
}

void NMib::NTime::NPlatform::fg_TimeRaw_GetUTCOffset(NTime::CTimeSpan *_pTimeOffset)
{
	TIME_ZONE_INFORMATION TimeZone;
	NMemory::fg_MemClear(TimeZone);
	uint32 CurrentDaylight = GetTimeZoneInformation(&TimeZone); // If this fails the bias will be 0 hours because we cleaned the TimeZone...
	if (CurrentDaylight == TIME_ZONE_ID_DAYLIGHT)
		*_pTimeOffset = NTime::CTimeSpanConvert::fs_CreateMinuteSpan((-TimeZone.Bias) + (-TimeZone.DaylightBias));
	else
		*_pTimeOffset = NTime::CTimeSpanConvert::fs_CreateMinuteSpan((-TimeZone.Bias));
}

namespace
{
	NMib::NTime::CTime fg_TruncateTimeToMillisecond(NMib::NTime::CTime const &_Time)
	{
		constexpr static uint64 c_FractionPerMillisecond = NMib::NTime::NPrivate::CConst::mc_FractionDividend / 1000;

		NMib::NTime::CTime TruncatedTime = _Time;
		uint64 FractionInt = TruncatedTime.f_GetFractionInt();
		uint64 MillisecondFractionInt = NMib::fg_Clamp(FractionInt / c_FractionPerMillisecond, 0, 999) * c_FractionPerMillisecond;

		TruncatedTime.f_SetFractionInt(MillisecondFractionInt);

		return TruncatedTime;
	}
}

NMib::NTime::CTime NMib::NTime::NPlatform::fg_TimeRaw_ToLocal(CTime const &_Time)
{
	uint64 OriginalFractionInt = _Time.f_GetFractionInt();

	CTime TruncatedTime = fg_TruncateTimeToMillisecond(_Time);
	auto DateTime = NTime::CTimeConvert(TruncatedTime).f_ExtractDateTime();

	if (DateTime.m_Year < 1601 || DateTime.m_Year > 30826)
		return _Time;

	SYSTEMTIME Time;
	NFile::NPlatform::fg_MalterlibTimeToSystemTime(DateTime, Time);

	SYSTEMTIME LocalTime;

	if (NLocal::g_OptionalFunctions.m_fSystemTimeToTzSpecificLocalTimeEx)
	{
		if (!NLocal::g_OptionalFunctions.m_fSystemTimeToTzSpecificLocalTimeEx(nullptr, &Time, &LocalTime))
			DMibErrorSystemImp((NStr::CFStr256::CFormat("Windows returned an error from SystemTimeToTzSpecificLocalTimeEx: {}") << NMib::NPlatform::fg_Win32_GetLastErrorStr()).f_GetStr());

		CTime Result = NFile::NPlatform::fg_SystemTimeToMalterlibTime(LocalTime);
		Result.f_SetFractionInt(OriginalFractionInt);

		return Result;
	}

	TIME_ZONE_INFORMATION *pTimeZoneInfo = nullptr;

	TIME_ZONE_INFORMATION TimeZoneInfo;
	if (NLocal::g_OptionalFunctions.m_fGetTimeZoneInformationForYear)
	{
		if (!NLocal::g_OptionalFunctions.m_fGetTimeZoneInformationForYear(Time.wYear, nullptr, &TimeZoneInfo))
			DMibErrorSystemImp((NStr::CFStr256::CFormat("Windows returned an error from GetTimeZoneInformationForYear: {}") << NMib::NPlatform::fg_Win32_GetLastErrorStr()).f_GetStr());
		
		pTimeZoneInfo = &TimeZoneInfo;
	}

	if (!SystemTimeToTzSpecificLocalTime(pTimeZoneInfo, &Time, &LocalTime))
		DMibErrorSystemImp((NStr::CFStr256::CFormat("Windows returned an error from SystemTimeToTzSpecificLocalTime: {}") << NMib::NPlatform::fg_Win32_GetLastErrorStr()).f_GetStr());

	CTime Result = NFile::NPlatform::fg_SystemTimeToMalterlibTime(LocalTime);
	Result.f_SetFractionInt(OriginalFractionInt);

	return Result;
}

NMib::NTime::CTime NMib::NTime::NPlatform::fg_TimeRaw_ToUtc(CTime const &_Time)
{
	uint64 OriginalFractionInt = _Time.f_GetFractionInt();

	CTime TruncatedTime = fg_TruncateTimeToMillisecond(_Time);
	auto DateTime = NTime::CTimeConvert(TruncatedTime).f_ExtractDateTime();

	if (DateTime.m_Year < 1601 || DateTime.m_Year > 30826)
		return _Time;

	SYSTEMTIME Time;
	NFile::NPlatform::fg_MalterlibTimeToSystemTime(DateTime, Time);

	SYSTEMTIME LocalTime;

	if (NLocal::g_OptionalFunctions.m_fTzSpecificLocalTimeToSystemTimeEx)
	{
		if (!NLocal::g_OptionalFunctions.m_fTzSpecificLocalTimeToSystemTimeEx(nullptr, &Time, &LocalTime))
			DMibErrorSystemImp((NStr::CFStr256::CFormat("Windows returned an error from SystemTimeToTzSpecificLocalTimeEx: {}") << NMib::NPlatform::fg_Win32_GetLastErrorStr()).f_GetStr());

		CTime Result = NFile::NPlatform::fg_SystemTimeToMalterlibTime(LocalTime);
		Result.f_SetFractionInt(OriginalFractionInt);

		return Result;
	}

	TIME_ZONE_INFORMATION *pTimeZoneInfo = nullptr;

	TIME_ZONE_INFORMATION TimeZoneInfo;
	if (NLocal::g_OptionalFunctions.m_fGetTimeZoneInformationForYear)
	{
		if (!NLocal::g_OptionalFunctions.m_fGetTimeZoneInformationForYear(Time.wYear, nullptr, &TimeZoneInfo))
			DMibErrorSystemImp((NStr::CFStr256::CFormat("Windows returned an error from GetTimeZoneInformationForYear: {}") << NMib::NPlatform::fg_Win32_GetLastErrorStr()).f_GetStr());
		
		pTimeZoneInfo = &TimeZoneInfo;
	}

	if (!TzSpecificLocalTimeToSystemTime(pTimeZoneInfo, &Time, &LocalTime))
		DMibErrorSystemImp((NStr::CFStr256::CFormat("Windows returned an error from SystemTimeToTzSpecificLocalTime: {}") << NMib::NPlatform::fg_Win32_GetLastErrorStr()).f_GetStr());

	CTime Result = NFile::NPlatform::fg_SystemTimeToMalterlibTime(LocalTime);
	Result.f_SetFractionInt(OriginalFractionInt);

	return Result;
}

namespace
{
	// NMib::NTime::CTimeConvert::fs_CreateTimeConstExpr(1601, 1, 1)
	constexpr static NMib::NTime::CTime const g_FileTimeBase = NMib::NTime::CTime::fs_Create(237148610522659200, 0);
}

void NMib::NTime::NPlatform::fg_TimeRaw_GetNow(NMib::NTime::CTime *_pTime)
{
	FILETIME Time;
	GetSystemTimeAsFileTime(&Time);
	NTime::CTime CurrentTime = g_FileTimeBase;

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

fp64 NMib::NTime::NPlatform::fg_TimeRaw_Resolution()
{
	return fg_Min(fp64(1.0/10000000.0), fg_TimerRaw_PreciseResolution());
}

int64 NMib::NTime::NPlatform::fg_TimerRaw_GetCPUFrequency()
{
	if (NMib::NPlatform::CWin32_Registry Registry; auto Frequency = Registry.f_Read_uint32("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "~MHz", 0))
		return Frequency * 1'000'000;

	return 0;
}

int64 NMib::NTime::NPlatform::fg_Timer_CyclesRawFrequency()
{
#if defined(DArchitecture_arm64) || defined(DArchitecture_arm64e)
	int64 Return = _ReadStatusReg(DMibArm64_CNTFRQ_EL0);
	if (Return)
		return Return;
	else
		return fg_TimerRaw_PreciseFrequency();
#else
	return fg_TimerRaw_PreciseFrequency();
#endif
}

