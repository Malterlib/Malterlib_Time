// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include <Mib/Core/Core>

#include "Malterlib_Time_System.h"
#include "Malterlib_Time_Platform.h"

namespace NMib::NTime
{
	struct CTimeMeasure
	{
		CTimeMeasure() = default;
		CTimeMeasure(int64 _Time);

		void f_Reset(int64 _Time);
		inline_small void f_Start();
		inline_small CTimeMeasure f_Elapsed() const;
		inline_small void f_Stop();
		int64 f_GetTicks();
		fp64 f_GetTime();

		auto operator <=> (CTimeMeasure const &_Other) const = default;

		operator fp64();

		CTimeMeasure &operator -= (CTimeMeasure const &_Other);
		CTimeMeasure &operator += (CTimeMeasure const &_Other);
		CTimeMeasure &operator *= (CTimeMeasure const &_Other);
		CTimeMeasure operator - (CTimeMeasure const &_Other);
		CTimeMeasure operator + (CTimeMeasure const &_Other);

	private:
		int64 mp_Time = TCLimitsInt<int64>::mc_Min;
	};

	struct CPrefCyclesTimeMeasureMin;

	template <bool t_bDoCorrection>
	struct TCCyclesTimeMeasure
	{
		TCCyclesTimeMeasure(int64 _Time);
		TCCyclesTimeMeasure() = default;

		void f_Reset(int64 _Time);
		inline_small TCCyclesTimeMeasure f_Elapsed() const;
		inline_small void f_Start();
		inline_small void f_Stop();
		inline_small void f_StopNonCorrected();
		int64 f_GetCycles();
		fp64 f_GetTime();

		auto operator <=> (TCCyclesTimeMeasure const &_Other) const = default;

		operator fp64();

		TCCyclesTimeMeasure &operator -= (TCCyclesTimeMeasure const &_Other);
		TCCyclesTimeMeasure &operator += (TCCyclesTimeMeasure const &_Other);
		TCCyclesTimeMeasure &operator *= (TCCyclesTimeMeasure const &_Other);
		TCCyclesTimeMeasure operator - (TCCyclesTimeMeasure const &_Other);
		TCCyclesTimeMeasure operator + (TCCyclesTimeMeasure const &_Other);

	private:
		friend struct CPrefCyclesTimeMeasureMin;

		struct CCorrection
		{
			CCorrection(aint _CorrectionType);

			int64 m_Correction;
		};

		static CCorrection msp_CorrectionStart;
		static CCorrection msp_CorrectionNonCorrected;

		int64 mp_Time = TCLimitsInt<int64>::mc_Min;
	};

	extern template struct TCCyclesTimeMeasure<true>;
	extern template struct TCCyclesTimeMeasure<false>;

	using CCyclesTimeMeasure = TCCyclesTimeMeasure<true>;

	template <typename t_CTimer>
	struct TCMeasureScope
	{
		inline TCMeasureScope(t_CTimer &_Timer);
		inline ~TCMeasureScope();

	private:
		t_CTimer *mp_pTimer;
	};

	struct CTimeMeasureTraceScope
	{
		CTimeMeasureTraceScope(ch8 const *_pName);
		~CTimeMeasureTraceScope();

	private:
		CTimeMeasure mp_Timer;
		ch8 const *mp_pName;
	};

	struct CTimeMeasureConOutScope
	{
		CTimeMeasureConOutScope(ch8 const *_pName);
		~CTimeMeasureConOutScope();

	private:
		CTimeMeasure mp_Timer;
		ch8 const *mp_pName;
	};

#	define DMibScopeMeasure(_Timer) NMib::NTime::TCMeasureScope<decltype(_Timer)> TimerScope(_Timer)

#if defined(DMibDebug) || defined(DConfig_Profile)
#	define DMibScopeTraceTimeMeasure(_Name) NMib::NTime::CTimeMeasureTraceScope TimerScope(_Name)
#	define DMibScopeConOutTimeMeasure(_Name) NMib::NTime::CTimeMeasureConOutScope TimerScope(_Name)
#else
#	define DMibScopeTraceTimeMeasure(_Name)
#	define DMibScopeConOutTimeMeasure(_Name)
#endif

#	ifndef DMibPNoShortCuts
#		define DScopeMeasure(_Timer) DMibScopeMeasure(_Timer)
#		define DScopeTraceTimeMeasure(_Name) DMibScopeTraceTimeMeasure(_Name)
#		define DScopeConOutTimeMeasure(_Name) DMibScopeConOutTimeMeasure(_Name)
#	endif
}

#include "Malterlib_Time_TimeMeasure.hpp"
