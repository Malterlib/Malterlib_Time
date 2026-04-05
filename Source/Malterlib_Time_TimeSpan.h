// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

#include "Malterlib_Time_TimeDefines.hpp"

namespace NMib::NTime
{
	struct CTimeSpan
	{
		constexpr CTimeSpan() noexcept;
		constexpr explicit CTimeSpan(int64 _Value) noexcept;
		constexpr explicit CTimeSpan(int64 _Value, uint64 _Fraction) noexcept;

		constexpr CTimeSpan operator + (const CTimeSpan &_Other) const noexcept;
		constexpr CTimeSpan operator - (const CTimeSpan &_Other) const noexcept;
		constexpr CTimeSpan operator - () const noexcept;
		constexpr CTimeSpan operator * (int64 _nTimes) const noexcept;
		constexpr CTimeSpan operator / (int64 _nTimes) const noexcept;
		constexpr CTimeSpan &operator += (const CTimeSpan &_Other) noexcept;
		constexpr CTimeSpan &operator -= (const CTimeSpan &_Other) noexcept;
		constexpr CTimeSpan &operator *= (int64 _nTimes) noexcept;
		constexpr CTimeSpan &operator /= (int64 _nTimes) noexcept;
		constexpr auto operator <=> (const CTimeSpan &_Other) const noexcept = default;

		constexpr bool f_IsValid() const noexcept;
		constexpr int64 f_GetSeconds() const noexcept;
		constexpr void f_SetSeconds(int64 _Seconds) noexcept;
		constexpr void f_SetSecondsNoFraction(int64 _Seconds) noexcept;
		constexpr uint64 f_GetFractionInt() const noexcept;
		constexpr void f_SetFractionInt(uint64 _Fraction) noexcept;
		constexpr fp64 f_GetFraction() const noexcept;
		constexpr void f_SetFraction(fp64 _Fraction) noexcept;
		constexpr fp64 f_GetSecondsFraction() const noexcept;
		constexpr void f_SetSecondsFraction(fp64 _Seconds) noexcept;

		constexpr static CTimeSpan fs_SmallestTime() noexcept;
		constexpr static CTimeSpan fs_Zero() noexcept;
		static CTimeSpan fs_UTCOffset();

		template <typename tf_CStr>
		constexpr void f_Format(tf_CStr &_FormatInto) const noexcept;

	private:
		friend struct CTimeSpanConvert_BabylonianCommon;
		friend struct CTime;

#ifdef DMibDebuggerHelpers
		static ch8 const* fsp_DebugStr(void *_pTimeSpan);
#endif
		int64 mp_Seconds = NPrivate::CConst::mc_InvalidSpanSeconds;
		uint64 mp_Fraction = 0;
	};

	NMib::NStr::CStr fg_SecondsDurationToHumanReadable(fp64 const &_Seconds);
}

#include "Malterlib_Time_TimeSpan_Convert.h"

#include "Malterlib_Time_TimeSpan.hpp"
#include "Malterlib_Time_TimeSpan_Stream.hpp"
#include "Malterlib_Time_TimeSpan_Format.hpp"
