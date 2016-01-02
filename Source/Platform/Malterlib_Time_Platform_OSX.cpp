// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>

#include "../Malterlib_Time_Platform.h"

#include <CoreFoundation/CoreFoundation.h>
#include <sys/time.h>
#include <mach/mach_time.h>

void NMib::NTime::NPlatform::fg_TimeRaw_GetUTCOffset(NTime::CTimeSpan *_pTimeOffset)
{
	CFTimeZoneRef TimeZone = CFTimeZoneCopySystem();
	CFAbsoluteTime Now = CFAbsoluteTimeGetCurrent();
	int64 Diff = int64(CFTimeZoneGetSecondsFromGMT(TimeZone, Now));
	
	*_pTimeOffset = NTime::CTimeSpanConvert::fs_CreateSpan(0, 0, 0, 0, Diff);
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

int64 NMib::NTime::NPlatform::fg_TimerRaw_PreciseGet()
{
	return mach_absolute_time();
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
	mach_timebase_info_data_t tb = { 0 };
	mach_timebase_info(&tb);
	return (1000000000LL * tb.numer) / tb.denom;
}

