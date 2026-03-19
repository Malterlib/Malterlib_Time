// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include <Mib/Time/PerfTimeMeasure>

namespace NMib::NTime
{
	CTimeMeasureTraceScope::CTimeMeasureTraceScope(ch8 const *_pName)
	{
		mp_pName = _pName;
		mp_Timer.f_Start();
	}

	CTimeMeasureTraceScope::~CTimeMeasureTraceScope()
	{
		mp_Timer.f_Stop();

		DMibTraceSafe("Time measure {}: {} s" DMibNewLine, mp_pName, mp_Timer.f_GetTime());
	}

	CTimeMeasureConOutScope::CTimeMeasureConOutScope(ch8 const *_pName)
	{
		mp_pName = _pName;
		mp_Timer.f_Start();
	}

	CTimeMeasureConOutScope::~CTimeMeasureConOutScope()
	{
		mp_Timer.f_Stop();

		DMibConErrOut("Time measure {}: {} s" DMibNewLine, mp_pName, mp_Timer.f_GetTime());
	}

	template <bool t_bDoCorrection>
	TCCyclesTimeMeasure<t_bDoCorrection>::CCorrection::CCorrection(aint _CorrectionType)
	{
		if (_CorrectionType)
		{
			CCyclesTimeMeasure Timer;
			CPrefCyclesTimeMeasureMin TimerMin;
			for (umint i = 0; i < 10; ++i)
			{
				for (umint i = 0; i < 2; ++i)
				{
					Timer.f_Start();
					Timer.f_StopNonCorrected();
				}
				TimerMin.f_FeedCycles(Timer);
			}
			m_Correction = TimerMin.f_GetCycles().f_ToIntRound();
		}
		else
			m_Correction = 0;
	}

	template <bool t_bDoCorrection>
	TCCyclesTimeMeasure<t_bDoCorrection>::TCCyclesTimeMeasure(int64 _Time)
	{
		mp_Time = _Time;
	}

	template <bool t_bDoCorrection>
	void TCCyclesTimeMeasure<t_bDoCorrection>::f_Reset(int64 _Time)
	{
		mp_Time = _Time;
	}

	template <bool t_bDoCorrection>
	int64 TCCyclesTimeMeasure<t_bDoCorrection>::f_GetCycles() const
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0);
		return mp_Time;
	}

	template <bool t_bDoCorrection>
	fp64 TCCyclesTimeMeasure<t_bDoCorrection>::f_GetTime() const
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0);
		return fp64(mp_Time) * CSystem_Time::fs_CyclesFrequencyReciprocal();
	}

	template <bool t_bDoCorrection>
	bool TCCyclesTimeMeasure<t_bDoCorrection>::f_IsValid() const
	{
		return mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0;
	}

	template <bool t_bDoCorrection>
	TCCyclesTimeMeasure<t_bDoCorrection>::operator fp64() const
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0);
		return fp64(mp_Time) * CSystem_Time::fs_CyclesFrequencyReciprocal();
	}

	template <bool t_bDoCorrection>
	auto TCCyclesTimeMeasure<t_bDoCorrection>::operator -= (TCCyclesTimeMeasure const &_Other) -> TCCyclesTimeMeasure &
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0);
		DMibFastCheck(_Other.mp_Time != TCLimitsInt<int64>::mc_Min && _Other.mp_Time >= 0);
		mp_Time -= _Other.mp_Time;
		return *this;
	}

	template <bool t_bDoCorrection>
	auto TCCyclesTimeMeasure<t_bDoCorrection>::operator += (TCCyclesTimeMeasure const &_Other) -> TCCyclesTimeMeasure &
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0);
		DMibFastCheck(_Other.mp_Time != TCLimitsInt<int64>::mc_Min && _Other.mp_Time >= 0);
		mp_Time += _Other.mp_Time;
		return *this;
	}

	template <bool t_bDoCorrection>
	auto TCCyclesTimeMeasure<t_bDoCorrection>::operator *= (TCCyclesTimeMeasure const &_Other) -> TCCyclesTimeMeasure &
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0);
		DMibFastCheck(_Other.mp_Time != TCLimitsInt<int64>::mc_Min && _Other.mp_Time >= 0);
		mp_Time *= _Other.mp_Time;
		return *this;
	}

	template <bool t_bDoCorrection>
	auto TCCyclesTimeMeasure<t_bDoCorrection>::operator - (TCCyclesTimeMeasure const &_Other) const -> TCCyclesTimeMeasure
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0);
		DMibFastCheck(_Other.mp_Time != TCLimitsInt<int64>::mc_Min && _Other.mp_Time >= 0);
		TCCyclesTimeMeasure Temp = *this;
		Temp -= _Other;
		return Temp;
	}

	template <bool t_bDoCorrection>
	auto TCCyclesTimeMeasure<t_bDoCorrection>::operator + (TCCyclesTimeMeasure const &_Other) const -> TCCyclesTimeMeasure
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0);
		DMibFastCheck(_Other.mp_Time != TCLimitsInt<int64>::mc_Min && _Other.mp_Time >= 0);
		TCCyclesTimeMeasure Temp = *this;
		Temp += _Other;
		return Temp;
	}

	template struct TCCyclesTimeMeasure<true>;
	template struct TCCyclesTimeMeasure<false>;

	CTimeMeasure::CTimeMeasure(int64 _Time)
		: mp_Time(_Time)
	{
	}

	void CTimeMeasure::f_Reset(int64 _Time)
	{
		mp_Time = _Time;
	}

	bool CTimeMeasure::f_IsValid() const
	{
		return mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0;
	}

	CTimeMeasure::operator fp64() const
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0);
		return fp64(mp_Time) / CSystem_Time::fs_TimerFrequencyFp();
	}

	int64 CTimeMeasure::f_GetTicks() const
	{
		return mp_Time;
	}

	fp64 CTimeMeasure::f_GetTime() const
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0);
		return (fp64(mp_Time)) / CSystem_Time::fs_TimerFrequencyFp();
	}

	CTimeMeasure &CTimeMeasure::operator -= (CTimeMeasure const &_Other)
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0);
		DMibFastCheck(_Other.mp_Time != TCLimitsInt<int64>::mc_Min && _Other.mp_Time >= 0);
		mp_Time -= _Other.mp_Time;
		return *this;
	}

	CTimeMeasure &CTimeMeasure::operator += (CTimeMeasure const &_Other)
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0);
		DMibFastCheck(_Other.mp_Time != TCLimitsInt<int64>::mc_Min && _Other.mp_Time >= 0);
		mp_Time += _Other.mp_Time;
		return *this;
	}

	CTimeMeasure &CTimeMeasure::operator *= (CTimeMeasure const &_Other)
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0);
		DMibFastCheck(_Other.mp_Time != TCLimitsInt<int64>::mc_Min && _Other.mp_Time >= 0);
		mp_Time *= _Other.mp_Time;
		return *this;
	}

	CTimeMeasure CTimeMeasure::operator - (CTimeMeasure const &_Other) const
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0);
		DMibFastCheck(_Other.mp_Time != TCLimitsInt<int64>::mc_Min && _Other.mp_Time >= 0);
		CTimeMeasure Temp = *this;
		Temp -= _Other;
		return Temp;
	}

	CTimeMeasure CTimeMeasure::operator + (CTimeMeasure const &_Other) const
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time >= 0);
		DMibFastCheck(_Other.mp_Time != TCLimitsInt<int64>::mc_Min && _Other.mp_Time >= 0);
		CTimeMeasure Temp = *this;
		Temp += _Other;
		return Temp;
	}
}
