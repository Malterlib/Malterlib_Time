// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

#include "Malterlib_Time_System.h"
#include "Malterlib_Time_Platform.h"

namespace NMib::NTime
{
	struct CStopwatch
	{
		CStopwatch() = default;

		CStopwatch(bool _bStart);
		CStopwatch(CStopwatch const& _ToMove);

		inline_small void f_Start();
		fp64 f_Stop();
		fp64 f_GetTime() const;
		fp64 f_Restart();
		void f_AddOffset(fp64 _Offset);
		bool f_IsValid() const;

		int64 f_GetStartTicks() const;
		static CStopwatch fs_CreateFromStartTicks(int64 _StartTime);

		operator fp64() const;

	private:
		int64 mp_StartTime = TCLimitsInt<int64>::mc_Min;
	};

	struct CStopwatchRaw
	{
		CStopwatchRaw() = default;
		CStopwatchRaw(bool _bStart);
		CStopwatchRaw(CStopwatchRaw const &_ToMove);

		inline_small void f_Start();
		fp64 f_Stop();
		fp64 f_GetTime() const;
		fp64 f_Restart();
		void f_AddOffset(fp64 _Offset);
		bool f_IsValid() const;

		int64 f_GetStartTicks() const;
		static CStopwatchRaw fs_CreateFromStartTicks(int64 _StartTime);

		operator fp64() const;

	private:
		int64 mp_StartTime = TCLimitsInt<int64>::mc_Min;
	};

	struct CCyclesStopwatch
	{
		CCyclesStopwatch() = default;
		CCyclesStopwatch(bool _bStart);
		CCyclesStopwatch(CCyclesStopwatch const &_ToMove) = default;

		inline_small void f_Start();
		inline_small void f_Start(umint _Offset);
		int64 f_GetCycles() const;
		fp64 f_Stop();
		fp64 f_Restart();
		void f_AddOffset(fp64 _Offset);
		fp64 f_GetTime() const;
		bool f_IsValid() const;

		int64 f_GetStartTicks() const;
		static CCyclesStopwatch fs_CreateFromStartTicks(int64 _StartTime);

		operator fp64() const;

	private:
		int64 mp_StartTime = TCLimitsInt<int64>::mc_Min;
	};
}

#include "Malterlib_Time_StopWatch.hpp"
