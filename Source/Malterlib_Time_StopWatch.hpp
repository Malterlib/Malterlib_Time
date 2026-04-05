// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NTime
{
	inline_small void CStopwatch::f_Start()
	{
		mp_StartTime = CSystem_Time::fs_GetTimerValue();
	}

	inline_small void CStopwatchRaw::f_Start()
	{
		mp_StartTime = NPlatform::fg_TimerRaw_PreciseGet();
	}

	inline_small void CCyclesStopwatch::f_Start()
	{
		mp_StartTime = NPlatform::fg_Timer_CyclesFast();
	}

	inline_small void CCyclesStopwatch::f_Start(umint _Offset)
	{
		mp_StartTime = NPlatform::fg_Timer_CyclesFast() - _Offset;
	}
}
