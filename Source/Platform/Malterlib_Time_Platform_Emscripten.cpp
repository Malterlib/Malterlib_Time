// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include "../Malterlib_Time_Platform.h"

void NMib::NTime::NPlatform::fg_TimeRaw_GetUTCOffset(NTime::CTimeSpan *_pTimeOffset)
{
	time_t RawTime;
	time (&RawTime);
	tm ResultBuffer;
	tm *pTime = localtime_r(&RawTime, &ResultBuffer);
	if (!pTime)
	{
		int ErrNo = errno;
		DMibError(fg_FormatErrno("localtime_r (get utf offset)", ErrNo));
	}

	int64 Diff = 0;
	if (pTime)
		Diff = pTime->tm_gmtoff;
	*_pTimeOffset = NTime::CTimeSpanConvert::fs_CreateSpan(0, 0, 0, 0, Diff);
}

void NMib::NTime::NPlatform::fg_TimeRaw_GetNow(NMib::NTime::CTime *_pTime)
{
	static CTime EpochStart = NTime::CTimeConvert::fs_CreateTime(1970, 1, 1);
	timespec Time;
	clock_gettime(CLOCK_REALTIME, &Time);
	int64 Seconds = Time.tv_sec;
	fp64 Fraction = fp64(Time.tv_nsec) / fp64(1000000000);
	*_pTime = EpochStart + CTimeSpanConvert_BabylonianCommon::fs_CreateSpan(0, 0, 0, 0, Seconds, Fraction);
}

int64 NMib::NTime::NPlatform::fg_TimerRaw_PreciseGet()
{
	timespec Time;
	clock_gettime(CLOCK_MONOTONIC, &Time);
	return (int64)Time.tv_sec * 1000000000 + int64(Time.tv_nsec);
}

int64 NSys::fg_TimerRaw_PreciseFrequency()
{
	return 1000000000LL;
}

int64 NSys::fg_TimerRaw_SafeFrequency()
{
	return fg_TimerRaw_PreciseFrequency();
}

int64 NSys::fg_TimerRaw_SafeGet()
{
	return fg_TimerRaw_PreciseGet();
}


