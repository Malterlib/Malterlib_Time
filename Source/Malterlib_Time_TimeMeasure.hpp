// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NTime
{
	template <bool t_bDoCorrection>
	typename TCCyclesTimeMeasure<t_bDoCorrection>::CCorrection TCCyclesTimeMeasure<t_bDoCorrection>::msp_CorrectionNonCorrected(0);

	template <bool t_bDoCorrection>
	typename TCCyclesTimeMeasure<t_bDoCorrection>::CCorrection TCCyclesTimeMeasure<t_bDoCorrection>::msp_CorrectionStart(1);

	inline_small void CTimeMeasure::f_Start()
	{
		mp_Time = -CSystem_Time::fs_GetTimerValue();
	}

	inline_small CTimeMeasure CTimeMeasure::f_Elapsed() const
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time < 0);
		return CTimeMeasure(mp_Time + CSystem_Time::fs_GetTimerValue());
	}

	inline_small void CTimeMeasure::f_Stop()
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time < 0);
		mp_Time += CSystem_Time::fs_GetTimerValue();
	}

	template <bool t_bDoCorrection>
	inline_small void TCCyclesTimeMeasure<t_bDoCorrection>::f_Start()
	{
		mp_Time = -NPlatform::fg_Timer_Cycles();
	}

	template <bool t_bDoCorrection>
	inline_small auto TCCyclesTimeMeasure<t_bDoCorrection>::f_Elapsed() const -> TCCyclesTimeMeasure
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time < 0);
		return TCCyclesTimeMeasure(mp_Time + NPlatform::fg_Timer_Cycles());
	}

	template <bool t_bDoCorrection>
	inline_small void TCCyclesTimeMeasure<t_bDoCorrection>::f_Stop()
	{
		DMibFastCheck(mp_Time != TCLimitsInt<int64>::mc_Min && mp_Time < 0);
		if constexpr (t_bDoCorrection)
			mp_Time += NPlatform::fg_Timer_Cycles() - msp_CorrectionStart.m_Correction;
		else
			mp_Time += NPlatform::fg_Timer_Cycles();
	}

	template <bool t_bDoCorrection>
	inline_small void TCCyclesTimeMeasure<t_bDoCorrection>::f_StopNonCorrected()
	{
		if constexpr (t_bDoCorrection)
			mp_Time += NPlatform::fg_Timer_Cycles() - msp_CorrectionNonCorrected.m_Correction;
		else
			mp_Time += NPlatform::fg_Timer_Cycles();
	}

	template <typename t_CTimer>
	inline TCMeasureScope<t_CTimer>::TCMeasureScope(t_CTimer &_Timer)
		: mp_pTimer(&_Timer)
	{
		mp_pTimer->f_Start();
	}

	template <typename t_CTimer>
	inline TCMeasureScope<t_CTimer>::~TCMeasureScope()
	{
		mp_pTimer->f_Stop();
	}
}
