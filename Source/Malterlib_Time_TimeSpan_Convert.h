// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NTime
{
	struct CTimeSpanConvert_BabylonianCommon
	{
		constexpr CTimeSpanConvert_BabylonianCommon(const CTimeSpan &_Time) noexcept;

		constexpr CTimeSpanConvert_BabylonianCommon &operator = (const CTimeSpan &_Time) noexcept;

		constexpr static CTimeSpan fs_CreateSpan(int64 _Weeks, int64 _Days, int64 _Hours = 0, int64 _Minutes = 0, int64 _Seconds = 0, fp64 _Fraction = 0.0) noexcept;
		constexpr static CTimeSpan fs_CreateWeekSpan(int64 _Weeks) noexcept;
		constexpr static CTimeSpan fs_CreateDaySpan(int64 _Days) noexcept;
		constexpr static CTimeSpan fs_CreateHourSpan(int64 _Hours) noexcept;
		constexpr static CTimeSpan fs_CreateMinuteSpan(int64 _Minutes) noexcept;
		constexpr static CTimeSpan fs_CreateSecondSpan(int64 _Seconds) noexcept;
		constexpr static CTimeSpan fs_CreateSpanFromWeeks(fp64 _Weeks) noexcept;
		constexpr static CTimeSpan fs_CreateSpanFromDays(fp64 _Days) noexcept;
		constexpr static CTimeSpan fs_CreateSpanFromHours(fp64 _Hours) noexcept;
		constexpr static CTimeSpan fs_CreateSpanFromMinutes(fp64 _Minutes) noexcept;
		constexpr static CTimeSpan fs_CreateSpanFromSeconds(fp64 _Seconds) noexcept;
		constexpr static CTimeSpan fs_CreateSpanFromSeconds(int64 _Seconds, fp64 _Fraction) noexcept;

		constexpr int64 f_GetWeeks() const noexcept;
		constexpr fp64 f_GetWeeksFloat() const noexcept;
		constexpr int64 f_GetDays() const noexcept;
		constexpr fp64 f_GetDaysFloat() const noexcept;
		constexpr int64 f_GetHours() const noexcept;
		constexpr fp64 f_GetHoursFloat() const noexcept;
		constexpr int64 f_GetMinutes() const noexcept;
		constexpr fp64 f_GetMinutesFloat() const noexcept;
		constexpr int64 f_GetSeconds() const noexcept;
		constexpr fp64 f_GetSecondsFloat() const noexcept;
		constexpr fp64 f_GetFraction() const noexcept;
		constexpr aint f_GetHourOfDay() const noexcept;
		constexpr aint f_GetMinuteOfHour() const noexcept;
		constexpr aint f_GetSecondOfMinute() const noexcept;

	protected:
		CTimeSpan const *mp_pTime;
	};
}

#include "Malterlib_Time_TimeSpan_Convert.hpp"
