// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NTime
{
	inline_small void CPerfTimeMeasureMin::f_Start()
	{
		mp_CurrentTime = -CSystem_Time::fs_GetTimerValue();
		mp_CurrentCycles = -NPlatform::fg_Timer_Cycles();
	}

	inline_medium void CPerfTimeMeasureMin::f_Stop()
	{
		mp_CurrentTime += CSystem_Time::fs_GetTimerValue();
		mp_CurrentCycles += NPlatform::fg_Timer_Cycles();

		if (mp_CurrentTime < mp_MinTime)
			mp_MinTime = mp_CurrentTime;

		if (mp_CurrentCycles < mp_MinCycles)
			mp_MinCycles = mp_CurrentCycles;
	}

	template <typename t_CFormatter>
	auto CPerfTimeMeasureMin::f_CreateStringFormatter(t_CFormatter &_Formatter) const -> decltype(NStr::fg_CreateStringFormatter(_Formatter, fg_ByValue(f_GetTime())))
	{
		return NStr::fg_CreateStringFormatter(_Formatter, fg_ByValue(f_GetTime()));
	}

	template <typename t_CFormatter>
	auto CPrefCyclesTimeMeasureMin::f_CreateStringFormatter(t_CFormatter &_Formatter) const
	{
		return NMib::NStr::fg_CreateStringFormatter(_Formatter, mp_MinCycles);
	}

	inline CPerfTimeMeasureMinScope::CPerfTimeMeasureMinScope(CPerfTimeMeasureMin &_Timer)
	{
		mp_pTimeMeausureMin = &_Timer;
		mp_pTimeMeausureMin->f_Start();
	}

	inline CPerfTimeMeasureMinScope::~CPerfTimeMeasureMinScope()
	{
		mp_pTimeMeausureMin->f_Stop();
	}
}
