// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include <Mib/Core/PlatformSpecific/PosixErrNo>

#include "../Malterlib_Time_Platform.h"

#include <CoreFoundation/CoreFoundation.h>
#include <sys/time.h>
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
	
	int64 Diff = 0;
	if (pTime)
		Diff = pTime->tm_gmtoff;
	*_pTimeOffset = NTime::CTimeSpanConvert::fs_CreateSpan(0, 0, 0, 0, Diff);
#else
	CFTimeZoneRef TimeZone = CFTimeZoneCopySystem();
	CFAbsoluteTime Now = CFAbsoluteTimeGetCurrent();
	int64 Diff = int64(CFTimeZoneGetSecondsFromGMT(TimeZone, Now));
	
	*_pTimeOffset = NTime::CTimeSpanConvert::fs_CreateSpan(0, 0, 0, 0, Diff);
#endif
}

void NMib::NTime::NPlatform::fg_TimeRaw_GetNow(NMib::NTime::CTime *_pTime)
{
	static CTime EpochStart = NTime::CTimeConvert::fs_CreateTime(1970, 1, 1);
	timeval Time;
	gettimeofday(&Time, nullptr);
	int64 Seconds = Time.tv_sec;
	fp64 Fraction = fp64(Time.tv_usec) / fp64(1000000);
	*_pTime = EpochStart + CTimeSpanConvert_BabylonianCommon::fs_CreateSpan(0, 0, 0, 0, Seconds, Fraction);
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
	// TODO: Find a way to implement this
	return 0;
}
