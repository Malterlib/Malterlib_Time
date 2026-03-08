// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include "Malterlib_Time_StopWatch.h"

namespace NMib::NTime
{
	CStopwatch::CStopwatch(bool _bStart)
	{
		if (_bStart)
			f_Start();
	}

	CStopwatch::CStopwatch(CStopwatch const& _ToMove)
		: mp_StartTime(_ToMove.mp_StartTime)
	{
	}

	fp64 CStopwatch::f_Stop()
	{
		auto Elapsed = f_GetTime();
		mp_StartTime = TCLimitsInt<int64>::mc_Min;
		return Elapsed;
	}

	fp64 CStopwatch::f_GetTime() const
	{
		DMibFastCheck(mp_StartTime != TCLimitsInt<int64>::mc_Min);
		return (fp64(CSystem_Time::fs_GetTimerValue() - mp_StartTime)) / CSystem_Time::fs_TimerFrequencyFp();
	}

	fp64 CStopwatch::f_Restart()
	{
		DMibFastCheck(mp_StartTime != TCLimitsInt<int64>::mc_Min);
		auto OldStartTime = mp_StartTime;
		mp_StartTime = CSystem_Time::fs_GetTimerValue();
		return (fp64(mp_StartTime - OldStartTime)) / CSystem_Time::fs_TimerFrequencyFp();
	}

	void CStopwatch::f_AddOffset(fp64 _Offset)
	{
		DMibFastCheck(mp_StartTime != TCLimitsInt<int64>::mc_Min);
		mp_StartTime += (_Offset * CSystem_Time::fs_TimerFrequencyFp()).f_ToInt();
	}

	int64 CStopwatch::f_GetStartTicks()
	{
		return mp_StartTime;
	}

	CStopwatch CStopwatch::fs_CreateFromStartTicks(int64 _StartTime)
	{
		CStopwatch Return;
		Return.mp_StartTime = _StartTime;

		return Return;
	}

	CStopwatch::operator fp64() const
	{
		return f_GetTime();
	}

	CStopwatchRaw::CStopwatchRaw(bool _bStart)
	{
		if (_bStart)
			f_Start();
	}

	CStopwatchRaw::CStopwatchRaw(CStopwatchRaw const &_ToMove)
		: mp_StartTime(_ToMove.mp_StartTime)
	{
	}

	fp64 CStopwatchRaw::f_Stop()
	{
		auto Elapsed = f_GetTime();
		mp_StartTime = TCLimitsInt<int64>::mc_Min;
		return Elapsed;
	}

	fp64 CStopwatchRaw::f_GetTime() const
	{
		DMibFastCheck(mp_StartTime != TCLimitsInt<int64>::mc_Min);
		return (fp64(NPlatform::fg_TimerRaw_PreciseGet() - mp_StartTime)) / fp64(NPlatform::fg_TimerRaw_PreciseFrequency());
	}

	fp64 CStopwatchRaw::f_Restart()
	{
		DMibFastCheck(mp_StartTime != TCLimitsInt<int64>::mc_Min);
		auto OldStartTime = mp_StartTime;
		mp_StartTime = NPlatform::fg_TimerRaw_PreciseGet();
		return (fp64(mp_StartTime - OldStartTime)) / fp64(NPlatform::fg_TimerRaw_PreciseFrequency());
	}

	void CStopwatchRaw::f_AddOffset(fp64 _Offset)
	{
		DMibFastCheck(mp_StartTime != TCLimitsInt<int64>::mc_Min);
		mp_StartTime += (_Offset * fp64(NPlatform::fg_TimerRaw_PreciseFrequency())).f_ToInt();
	}

	int64 CStopwatchRaw::f_GetStartTicks()
	{
		return mp_StartTime;
	}

	CStopwatchRaw CStopwatchRaw::fs_CreateFromStartTicks(int64 _StartTime)
	{
		CStopwatchRaw Return;
		Return.mp_StartTime = _StartTime;

		return Return;
	}

	CStopwatchRaw::operator fp64() const
	{
		return f_GetTime();
	}

	CCyclesStopwatch::CCyclesStopwatch(bool _bStart)
	{
		if (_bStart)
			f_Start();
	}

	int64 CCyclesStopwatch::f_GetCycles() const
	{
		DMibFastCheck(mp_StartTime != TCLimitsInt<int64>::mc_Min);
		return NPlatform::fg_Timer_CyclesFast() - mp_StartTime;
	}

	fp64 CCyclesStopwatch::f_Stop()
	{
		auto Elapsed = f_GetTime();
		mp_StartTime = TCLimitsInt<int64>::mc_Min;
		return Elapsed;
	}

	fp64 CCyclesStopwatch::f_Restart()
	{
		DMibFastCheck(mp_StartTime != TCLimitsInt<int64>::mc_Min);
		auto OldStartTime = mp_StartTime;
		mp_StartTime = NPlatform::fg_Timer_CyclesFast();
		return (fp64(mp_StartTime - OldStartTime)) * CSystem_Time::fs_CyclesFrequencyReciprocal();
	}

	void CCyclesStopwatch::f_AddOffset(fp64 _Offset)
	{
		DMibFastCheck(mp_StartTime != TCLimitsInt<int64>::mc_Min);
		mp_StartTime += (_Offset * fp64(CSystem_Time::fs_CyclesFrequencyFp())).f_ToInt();
	}

	int64 CCyclesStopwatch::f_GetStartTicks()
	{
		return mp_StartTime;
	}

	CCyclesStopwatch CCyclesStopwatch::fs_CreateFromStartTicks(int64 _StartTime)
	{
		CCyclesStopwatch Return;
		Return.mp_StartTime = _StartTime;

		return Return;
	}

	fp64 CCyclesStopwatch::f_GetTime() const
	{
		DMibFastCheck(mp_StartTime != TCLimitsInt<int64>::mc_Min);
		return (fp64(NPlatform::fg_Timer_CyclesFast() - mp_StartTime)) * CSystem_Time::fs_CyclesFrequencyReciprocal();
	}

	CCyclesStopwatch::CCyclesStopwatch::operator fp64() const
	{
		DMibFastCheck(mp_StartTime != TCLimitsInt<int64>::mc_Min);
		return f_GetTime();
	}
}
