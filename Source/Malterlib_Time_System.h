// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include <Mib/Core/Core>

namespace NMib::NTime
{
	struct CSystem_Time
	{
		struct CTimeChangeNotificationSubscription
		{
			CTimeChangeNotificationSubscription(CTimeChangeNotificationSubscription const &_Other) = delete;
			CTimeChangeNotificationSubscription &operator =(CTimeChangeNotificationSubscription const &_Other) = delete;

			CTimeChangeNotificationSubscription();
			CTimeChangeNotificationSubscription(CTimeChangeNotificationSubscription &&_Other);
			CTimeChangeNotificationSubscription &operator =(CTimeChangeNotificationSubscription &&_Other);
			~CTimeChangeNotificationSubscription();

		private:
			friend struct CSystem_Time;
			CTimeChangeNotificationSubscription(NFunction::TCFunction<void (NTime::CTime const &_OldTime, NTime::CTime const &_NewTime, NStr::CStr const &_Reason)> *_pNotification);
			void fp_Remove();

			NFunction::TCFunction<void (NTime::CTime const &_OldTime, NTime::CTime const &_NewTime, NStr::CStr const &_Reason)> *mp_pNotification;
		};

		static CTimeChangeNotificationSubscription fs_RegisterTimeChangeNotification
			(
				NFunction::TCFunction<void (NTime::CTime const &_OldTime, NTime::CTime const &_NewTime, NStr::CStr const &_Reason)> &&_fNotification
			)
		;

		static bool fs_TimeInitDone();
		static fp64 fs_CyclesFrequencyFp();
		static uint64 fs_CyclesFrequency();
		static fp64 fs_CyclesFrequencyReciprocal();
		static int64 fs_TimerFrequency();
		static fp64 fs_TimerFrequencyFp();
		static int64 fs_GetTimerValue();
		static void fs_SetTimeSpeed(fp64 _Multiplier, NTime::CTime const *_pOptionalTime, NTime::CTimeSpan const *_pTimeZone);
		static fp64 fs_GetTimeSpeed();
		static fp64 fs_GetTimeSpeedReciprocal();
		static void fs_DisableTimeSpeed();
		static bool fs_GetTimeSimulating();
		static void fs_TimeGetUTCOffset(NTime::CTimeSpan *_pUTCOffset);
		static void fs_TimeGetNowUTC(NTime::CTime *_pTime);
		static NTime::CTime fs_TimeToLocal(NTime::CTime const &_Time);
		static NTime::CTime fs_TimeToUtc(NTime::CTime const &_Time);
		static int64 fs_TimeResolution();
#ifdef DMibSafeTimerAvailable
	#if DMibConfig_Tests_Enable
		static void fs_MakeSafeTimerWrap(fp64 _InSeconds, uint32 _Where);
	#endif
		static void fs_EnableSafeTimer();
		static bool fs_IsSafeTimerEnabled();
#endif
	};
}
