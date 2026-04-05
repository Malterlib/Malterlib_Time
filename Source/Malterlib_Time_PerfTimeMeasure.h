// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "Malterlib_Time_TimeMeasure.h"

namespace NMib::NTime
{
	struct CPerfTimeMeasureMin
	{
		CPerfTimeMeasureMin() = default;

		void f_Reset();

		inline_small void f_Start();
		inline_medium void f_Stop();

		fp64 f_GetTime() const;
		int64 f_GetTicks() const;
		int64 f_GetCycles() const;
		int64 f_GetNanos() const;
		int64 f_GetMicros() const;
		int64 f_GetMillis() const;

		operator fp64() const;
		fp64 operator / ( const CPerfTimeMeasureMin &_Other) const;
		CPerfTimeMeasureMin operator * ( const fp64 &_Other) const;

		template <typename t_CFormatter>
		auto f_GetStringFormatType(t_CFormatter &_Formatter) -> decltype(NStr::fg_GetStringFormatType(_Formatter, fg_ByValue(f_GetTime())));

		template <typename t_CFormatter>
		auto f_CreateStringFormatter(t_CFormatter &_Formatter) const -> decltype(NStr::fg_CreateStringFormatter(_Formatter, fg_ByValue(f_GetTime())));

	private:
		int64 mp_MinTime = TCLimitsInt<int64>::mc_Max;
		int64 mp_MinCycles = TCLimitsInt<int64>::mc_Max;
		int64 mp_CurrentTime = 0;
		int64 mp_CurrentCycles = 0;
	};

	struct CPrefCyclesTimeMeasureMin
	{
		CPrefCyclesTimeMeasureMin() = default;

		void f_Reset();
		only_parameters_aliased inline_never void f_Start();
		only_parameters_aliased inline_never void f_Stop();
		only_parameters_aliased void f_FeedCycles(CCyclesTimeMeasure const &_Timer);
		only_parameters_aliased inline_never void f_StopNonCorrected();

		only_parameters_aliased fp64 f_GetCycles() const;
		only_parameters_aliased fp32 operator / ( const CPrefCyclesTimeMeasureMin &_Other) const;
		only_parameters_aliased CPrefCyclesTimeMeasureMin &operator /= ( const fp64 &_Other);
		only_parameters_aliased CPrefCyclesTimeMeasureMin operator * ( const fp64 &_Other) const;

		template <typename t_CFormatter>
		int f_GetStringFormatType(t_CFormatter &_Formatter);

		template <typename t_CFormatter>
		auto f_CreateStringFormatter(t_CFormatter &_Formatter) const;

	private:
		struct CCorrection
		{
			CCorrection(bool _bSetCorrection);

			int64 m_Correction;
		};

		static CCorrection msp_Correction;
		static CCorrection msp_Correction2;

		fp64 mp_MinCycles = 1e94;
		int64 mp_CurrentCycles;
	};

	struct CPerfTimeMeasureMinScope
	{
		inline CPerfTimeMeasureMinScope(CPerfTimeMeasureMin &_Timer);
		inline ~CPerfTimeMeasureMinScope();

	private:
		CPerfTimeMeasureMin *mp_pTimeMeausureMin;
	};

#	define DMibScopePerfTimeMeasureMin(_Timer) NMib::NTime::CPerfTimeMeasureMinScope TimerMinScope(_Timer)

#	ifndef DMibPNoShortCuts
#		define DScopePerfTimeMeasureMin(_Timer) DMibScopePerfTimeMeasureMin(_Timer)
#	endif
}

#include "Malterlib_Time_PerfTimeMeasure.hpp"
