// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>

#include "Malterlib_Time_PerfTimeMeasure.h"

namespace NMib::NTime
{
	CPrefCyclesTimeMeasureMin::CCorrection CPrefCyclesTimeMeasureMin::msp_Correction2(0);
	CPrefCyclesTimeMeasureMin::CCorrection CPrefCyclesTimeMeasureMin::msp_Correction(1);

	CPrefCyclesTimeMeasureMin::CCorrection::CCorrection(bool _bSetCorrection)
	{
		if (_bSetCorrection)
		{
			CPrefCyclesTimeMeasureMin Timer;
			for (umint i = 0; i < 10; ++i)
			{
				Timer.f_Start();
				Timer.f_StopNonCorrected();
			}
			m_Correction = Timer.mp_MinCycles.f_ToInt();
		}
		else
			m_Correction = 0;
	}

	void CPerfTimeMeasureMin::f_Reset()
	{
		mp_MinTime = TCLimitsInt<int64>::mc_Max;
		mp_MinCycles = TCLimitsInt<int64>::mc_Max;
	}

	// In seconds
	CPerfTimeMeasureMin::operator fp64() const
	{
		return ((fp64)mp_MinTime) / CSystem_Time::fs_TimerFrequencyFp();
	}

	fp64 CPerfTimeMeasureMin::f_GetTime() const
	{
		return ((fp64)mp_MinTime) / CSystem_Time::fs_TimerFrequencyFp();
	}

	int64 CPerfTimeMeasureMin::f_GetTicks() const
	{
		return mp_MinTime;
	}

	int64 CPerfTimeMeasureMin::f_GetCycles() const
	{
		return mp_MinCycles;
	}

	int64 CPerfTimeMeasureMin::f_GetNanos() const
	{
		return ((((fp64)mp_MinTime) / CSystem_Time::fs_TimerFrequencyFp()) * 1000000000.0).f_ToIntRound();
	}

	int64 CPerfTimeMeasureMin::f_GetMicros() const
	{
		return ((((fp64)mp_MinTime) / CSystem_Time::fs_TimerFrequencyFp()) * 1000000.0).f_ToIntRound();
	}

	int64 CPerfTimeMeasureMin::f_GetMillis() const
	{
		return ((((fp64)mp_MinTime) / CSystem_Time::fs_TimerFrequencyFp()) * 1000.0).f_ToIntRound();
	}

	fp64 CPerfTimeMeasureMin::operator / ( const CPerfTimeMeasureMin &_Other) const
	{
		return (fp64)mp_MinCycles / (fp64)_Other.mp_MinCycles;
	}

	CPerfTimeMeasureMin CPerfTimeMeasureMin::operator * ( const fp64 &_Other) const
	{
		CPerfTimeMeasureMin Ret;
		Ret.mp_CurrentCycles = mp_CurrentCycles;
		Ret.mp_CurrentTime = mp_CurrentTime;
		Ret.mp_MinCycles = (fp64(mp_MinCycles) * _Other).f_ToIntRound();
		Ret.mp_MinTime = (fp64(mp_MinTime) * _Other).f_ToIntRound();

		return Ret;
	}

	void CPrefCyclesTimeMeasureMin::f_Reset()
	{
		mp_MinCycles = 1e94;
	}

	only_parameters_aliased inline_never void CPrefCyclesTimeMeasureMin::f_Start()
	{
		int64 Cycles = NPlatform::fg_Timer_Cycles();
		NMib::NAtomic::fg_MemoryFence();
		mp_CurrentCycles = -Cycles;
	}

	only_parameters_aliased inline_never void CPrefCyclesTimeMeasureMin::f_Stop()
	{
		int64 Cycles = NPlatform::fg_Timer_Cycles();
		NMib::NAtomic::fg_MemoryFence();
		int64 Corrected = (mp_CurrentCycles + Cycles) - msp_Correction.m_Correction;
		if (Corrected < mp_MinCycles)
		{
			mp_MinCycles = Corrected;
		}
	}

	only_parameters_aliased void CPrefCyclesTimeMeasureMin::f_FeedCycles(CCyclesTimeMeasure const &_Timer)
	{
		if (_Timer.mp_Time < mp_MinCycles)
		{
			mp_MinCycles = _Timer.mp_Time;
		}
	}

	only_parameters_aliased inline_never void CPrefCyclesTimeMeasureMin::f_StopNonCorrected()
	{
		int64 Cycles = NPlatform::fg_Timer_Cycles();
		NMib::NAtomic::fg_MemoryFence();
		int64 Corrected = (mp_CurrentCycles + Cycles) - msp_Correction2.m_Correction;
		if (Corrected < mp_MinCycles)
		{
			mp_MinCycles = Corrected;
		}
	}

	// In seconds
	only_parameters_aliased fp64 CPrefCyclesTimeMeasureMin::f_GetCycles() const
	{
		return mp_MinCycles;
	}

	only_parameters_aliased fp32 CPrefCyclesTimeMeasureMin::operator / ( const CPrefCyclesTimeMeasureMin &_Other) const
	{
		return mp_MinCycles / _Other.mp_MinCycles;
	}

	only_parameters_aliased CPrefCyclesTimeMeasureMin &CPrefCyclesTimeMeasureMin::operator /= ( const fp64 &_Other)
	{
		mp_MinCycles = mp_MinCycles / _Other;
		return *this;
	}

	only_parameters_aliased CPrefCyclesTimeMeasureMin CPrefCyclesTimeMeasureMin::operator * ( const fp64 &_Other) const
	{
		CPrefCyclesTimeMeasureMin Ret;
		Ret.mp_CurrentCycles = mp_CurrentCycles;
		Ret.mp_MinCycles = mp_MinCycles * _Other;

		return Ret;
	}
}
