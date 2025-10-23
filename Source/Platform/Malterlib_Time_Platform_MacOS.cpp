// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include <Mib/Core/PlatformSpecific/PosixErrNo>

#include "../Malterlib_Time_Platform.h"

#include <CoreFoundation/CoreFoundation.h>
#include <sys/time.h>
#include <sys/sysctl.h>
#include <mach/mach_time.h>

void NMib::NTime::NPlatform::fg_TimeRaw_GetUTCOffset(NTime::CTimeSpan *_pTimeOffset)
{
#if 1
	time_t RawTime;
	time (&RawTime);
	tm ResultBuffer;
	tm *pTime = localtime_r(&RawTime, &ResultBuffer);
	if (!pTime)
	{
		int ErrNo = errno;
		DMibError(NMib::NPlatform::fg_FormatErrno("localtime_r (get utf offset)", ErrNo));
	}

	*_pTimeOffset = NTime::CTimeSpanConvert::fs_CreateSecondSpan(pTime->tm_gmtoff);
#else
	CFTimeZoneRef TimeZone = CFTimeZoneCopySystem();
	CFAbsoluteTime Now = CFAbsoluteTimeGetCurrent();
	int64 Diff = int64(CFTimeZoneGetSecondsFromGMT(TimeZone, Now));

	*_pTimeOffset = NTime::CTimeSpanConvert::fs_CreateSecondSpan(Diff);
#endif
}

NMib::NTime::CTime NMib::NTime::NPlatform::fg_TimeRaw_ToLocal(CTime const &_Time)
{
	if (_Time.f_GetSeconds() < NTime::NPrivate::CConst::mc_UnixEpochSeconds)
		return _Time;

	auto UnixSeconds = CTimeConvert(_Time).f_UnixSeconds();
	if (UnixSeconds > uint64(TCLimitsInt<time_t>::mc_Max))
		return _Time;

	time_t RawTime = UnixSeconds;

	tm ResultBuffer;
	tm *pTime = localtime_r(&RawTime, &ResultBuffer);

	if (!pTime)
	{
		int ErrNo = errno;
		DMibError(NMib::NPlatform::fg_FormatErrno("localtime_r (get utf offset)", ErrNo));
	}

	return CTimeConvert::fs_CreateTime(1900 + pTime->tm_year, pTime->tm_mon + 1, pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec, _Time.f_GetFraction());
}

NMib::NTime::CTime NMib::NTime::NPlatform::fg_TimeRaw_ToUtc(CTime const &_Time)
{
	if (_Time.f_GetSeconds() < NTime::NPrivate::CConst::mc_UnixEpochSeconds)
		return _Time;

	auto UnixSeconds = CTimeConvert(_Time).f_UnixSeconds();
	if (UnixSeconds > uint64(TCLimitsInt<time_t>::mc_Max))
		return _Time;

	time_t RawTime = UnixSeconds;

	tm ResultBuffer;
	tm *pTime = localtime_r(&RawTime, &ResultBuffer);

	if (!pTime)
	{
		int ErrNo = errno;
		DMibError(NMib::NPlatform::fg_FormatErrno("localtime_r (get utf offset)", ErrNo));
	}

	CTime Return = _Time - NTime::CTimeSpanConvert::fs_CreateSecondSpan(pTime->tm_gmtoff);
	if (Return.f_GetSeconds() < NTime::NPrivate::CConst::mc_UnixEpochSeconds)
		return _Time;

	UnixSeconds = CTimeConvert(Return).f_UnixSeconds();
	if (UnixSeconds > uint64(TCLimitsInt<time_t>::mc_Max))
		return _Time;

	auto LastOffset = pTime->tm_gmtoff;

	RawTime = UnixSeconds;
	pTime = localtime_r(&RawTime, &ResultBuffer);
	if (!pTime)
	{
		int ErrNo = errno;
		DMibError(NMib::NPlatform::fg_FormatErrno("localtime_r (get utf offset)", ErrNo));
	}

	if (pTime->tm_gmtoff != LastOffset)
		Return += NTime::CTimeSpanConvert::fs_CreateSecondSpan(LastOffset - pTime->tm_gmtoff);

	return Return;
}

void NMib::NTime::NPlatform::fg_TimeRaw_GetNow(NMib::NTime::CTime *_pTime)
{
	timeval Time;
	gettimeofday(&Time, nullptr);
	int64 Seconds = Time.tv_sec;
	fp64 Fraction = fp64(Time.tv_usec) / fp64(1000000);
	*_pTime = CTimeConvert::fs_FromUnixSeconds(Seconds);
	_pTime->f_SetFraction(Fraction);
}

fp64 NMib::NTime::NPlatform::fg_TimeRaw_Resolution()
{
	return fg_Max(fp64(1.0/1000000.0), fg_TimerRaw_PreciseResolution());
}

int64 NMib::NTime::NPlatform::fg_TimerRaw_PreciseGet()
{
	return mach_absolute_time();
}

fp64 NMib::NTime::NPlatform::fg_TimerRaw_PreciseResolution()
{
	return fp64(1.0) / fp64(fg_TimerRaw_SafeFrequency());
}

fp64 NMib::NTime::NPlatform::fg_TimerRaw_SafeResolution()
{
	return fp64(1.0) / fp64(fg_TimerRaw_SafeFrequency());
}

int64 NMib::NTime::NPlatform::fg_TimerRaw_SafeFrequency()
{
	return fg_TimerRaw_PreciseFrequency();
}

int64 NMib::NTime::NPlatform::fg_TimerRaw_SafeGet()
{
	return mach_absolute_time();
}


int64 NMib::NTime::NPlatform::fg_TimerRaw_PreciseFrequency()
{
	mach_timebase_info_data_t TimeBase = { 0 };
	mach_timebase_info(&TimeBase);
	return (1000000000LL * TimeBase.denom) / TimeBase.numer;
}

int64 NMib::NTime::NPlatform::fg_TimerRaw_GetCPUFrequency()
{
	NStr::CFStr1024 Name;
	size_t DataSize = 1023;
	int Ret = sysctlbyname("machdep.cpu.brand_string", Name.f_GetStr(1023), &DataSize, nullptr, 0);

	if (Ret == 0)
	{
		if (Name.f_Find(" M5 ") >= 0 || Name.f_EndsWith("M5"))
			return 4'600'000'000;
		else if (Name.f_Find(" M4 ") >= 0 || Name.f_EndsWith("M4"))
			return 4'510'000'000;
		else if (Name.f_Find(" M3 ") >= 0 || Name.f_EndsWith("M3"))
			return 4'050'000'000;
		else if (Name.f_Find(" M2 ") >= 0 || Name.f_EndsWith("M2"))
			return 3'500'000'000;
		else if (Name.f_Find(" M1 ") >= 0 || Name.f_EndsWith("M1"))
			return 3'200'000'000;
	}

	return 0;
}

int64 NMib::NTime::NPlatform::fg_Timer_CyclesRawFrequency()
{
#if defined(DArchitecture_arm64) || defined(DArchitecture_arm64e)
	mint Frequency;
	asm volatile ("mrs %0, CNTFRQ_EL0" : "=r" (Frequency));
	if (Frequency)
		return Frequency;
	else
		return fg_TimerRaw_PreciseFrequency();
#else
	return fg_TimerRaw_PreciseFrequency();
#endif
}
