// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Core/Core>

namespace NMib
{
	namespace NTime
	{
		struct CSystem_Time
		{
			static bool fs_TimeInitDone();
			static fp64 fs_CyclesFrequencyFp();
			static uint64 fs_CyclesFrequency();
			static fp64 fs_CyclesFrequencyReciprocal();
			static int64 fs_TimerFrequency();
			static fp64 fs_TimerFrequencyFp();
			static int64 fs_GetTimerValue();
			static void fs_SetTimeSpeed(fp64 _Multiplier, NTime::CTime const *_pOptionalTime);
			static fp64 fs_GetTimeSpeed();
			static fp64 fs_GetTimeSpeedReciprocal();
			static void fs_DisableTimeSpeed();
			static bool fs_GetTimeSimulating();
			static void fs_TimeGetUTCOffset(NTime::CTimeSpan *_pUTCOffset);
			static void fs_TimeGetNowUTC(NTime::CTime *_pTime);
			static int64 fs_TimeResolution();
		#ifdef DMibSafeTimerAvailable
			static void fs_EnableSafeTimer();
		#endif
		};
	}
}

