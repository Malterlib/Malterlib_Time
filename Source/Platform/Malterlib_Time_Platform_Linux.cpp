// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include "../Malterlib_Time_Platform.h"
#include <errno.h>

#include <Mib/Core/PlatformSpecific/PosixErrNo>

void NMib::NTime::NPlatform::fg_TimeRaw_GetUTCOffset(NTime::CTimeSpan *_pTimeOffset)
{
	time_t RawTime;
	time (&RawTime);
	tm ResultBuffer;
	tm *pTime = localtime_r(&RawTime, &ResultBuffer);
	if (!pTime)
	{
		int ErrNo = errno;
		DMibError(::NMib::NPlatform::fg_FormatErrno("localtime_r (get utf offset)", ErrNo));
	}
	
	*_pTimeOffset = NTime::CTimeSpanConvert::fs_CreateSecondSpan(pTime->tm_gmtoff);
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

	return CTimeConvert::fs_CreateTimeIntFrac(1900 + pTime->tm_year, pTime->tm_mon + 1, pTime->tm_mday, pTime->tm_hour, pTime->tm_min, pTime->tm_sec, _Time.f_GetFractionInt());
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
	timespec Time;
	clock_gettime(CLOCK_REALTIME, &Time);
	int64 Seconds = Time.tv_sec;
	fp64 Fraction = fp64(Time.tv_nsec) / fp64(1000000000);
	*_pTime = CTimeConvert::fs_FromUnixSeconds(Seconds);
	_pTime->f_SetFraction(Fraction);
}

fp64 NMib::NTime::NPlatform::fg_TimeRaw_Resolution()
{
	timespec Time;
	clock_getres(CLOCK_REALTIME, &Time);
	return (fp64)Time.tv_sec + fp64(Time.tv_nsec) / fp64(1000000000);
}

int64 NMib::NTime::NPlatform::fg_TimerRaw_PreciseGet()
{
	timespec Time;
	clock_gettime(CLOCK_MONOTONIC, &Time);
	return (int64)Time.tv_sec * 1000000000 + int64(Time.tv_nsec);
}

fp64 NMib::NTime::NPlatform::fg_TimerRaw_PreciseResolution()
{
	timespec Time;
	clock_getres(CLOCK_MONOTONIC, &Time);
	return (fp64)Time.tv_sec + fp64(Time.tv_nsec) / fp64(1000000000);
}

fp64 NMib::NTime::NPlatform::fg_TimerRaw_SafeResolution()
{
	timespec Time;
	clock_getres(CLOCK_MONOTONIC, &Time);
	return (fp64)Time.tv_sec + fp64(Time.tv_nsec) / fp64(1000000000);
}

int64 NMib::NTime::NPlatform::fg_TimerRaw_PreciseFrequency()
{
	return 1000000000LL;
}

int64 NMib::NTime::NPlatform::fg_TimerRaw_SafeFrequency()
{
	return fg_TimerRaw_PreciseFrequency();
}

int64 NMib::NTime::NPlatform::fg_TimerRaw_SafeGet()
{
	timespec Time;
	clock_gettime(CLOCK_MONOTONIC, &Time);
	return (int64) Time.tv_sec * 1000000000 + int64(Time.tv_nsec);
}

int64 NMib::NTime::NPlatform::fg_TimerRaw_GetCPUFrequency()
{
	// TODO: Find a way to implement this
	return 0;
}

int64 NMib::NTime::NPlatform::fg_Timer_CyclesRawFrequency()
{
#if defined(DArchitecture_arm64) || defined(DArchitecture_arm64e)
	mint Counter;
	asm volatile ("mrs %0, CNTFRQ_EL0" : "=r" (Counter));
	if (Counter)
		return Counter;
	else
		return fg_TimerRaw_PreciseFrequency();
#else
	return fg_TimerRaw_PreciseFrequency();;
#endif
}
