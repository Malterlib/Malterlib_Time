// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NTime
{
	struct CTimeSpanConvert_BabylonianCommon;
	struct CTime;
	struct CTimeConvert_BabylonianCommon;
	struct CTimeConvert_ProlepticGreogrian;
	struct CTimeConvert_ISOWeek;

	template <typename tf_CStr>
	constexpr void fg_FormatSecondsDurationToHumanReadable(tf_CStr &o_FormatInto, fp64 const &_Seconds);
}

namespace NMib::NTime::NPrivate
{
	struct CConst
	{
		constexpr static uint64 mc_SecondsInDay = 86'400;
		constexpr static uint64 mc_SecondsInDayHalf = mc_SecondsInDay / 2;
		constexpr static pfp64 mc_SecondsInDayFp = 86'400.0;
		constexpr static uint64 mc_SecondsInHour = 3'600;
		constexpr static uint64 mc_SecondsInHourHalf = mc_SecondsInHour / 2;
		constexpr static pfp64 mc_SecondsInHourFp = 3'600.0;
		constexpr static uint64 mc_SecondsInMinute = 60;
		constexpr static uint64 mc_SecondsInMinuteHalf = mc_SecondsInMinute / 2;
		constexpr static pfp64 mc_SecondsInMinuteFp = 60.0;
		constexpr static pfp64 mc_SecondsInWeekFp = 604'800.0;

		constexpr static uint64 mc_DaysInMedianYear = 365;
		constexpr static uint64 mc_DaysInLeapYear = 366;

		constexpr static int64 mc_SecondsInDaySigned = mc_SecondsInDay;
		constexpr static int64 mc_SecondsInHourSigned = mc_SecondsInHour;
		constexpr static int64 mc_SecondsInMinuteSigned = mc_SecondsInMinute;
		constexpr static int64 mc_SecondsInWeekSigned = 604'800;

		constexpr static uint64 mc_InvalidTimeSeconds = constant_uint64(0xffffffffffffffff);
		constexpr static int64 mc_InvalidSpanSeconds = constant_int64(0x7fffffffffffffff);
		constexpr static uint64 mc_InvalidFractionInt = constant_uint64(0xffffffffffffffff);

		constexpr static uint64 mc_EndOfTime = mc_InvalidTimeSeconds - 1;

		constexpr static uint64 mc_FractionDividend = 9'223'372'031'757'829'470; // Fraction of second one second is 9223372031757829470 (2006688023 * Cs-133)
		constexpr static uint64 mc_FractionDividendHalf = mc_FractionDividend / 2;
		constexpr static pfp64 mc_FractionDividendFp = 9'223'372'031'757'829'470.0;
		constexpr static pfp64 mc_FractionDividendFpInv = 1.0 / mc_FractionDividendFp;

		constexpr static uint64 mc_UnixEpochSeconds = constant_uint64(237'148'622'167'132'800);
		constexpr static uint64 mc_YearZeroPlus1DaySeconds = constant_uint64(237'148'560'000'000'000);
		constexpr static uint64 mc_YearOneBcSeconds = mc_YearZeroPlus1DaySeconds - mc_SecondsInDay;
		constexpr static uint64 mc_YearOneAdSeconds = mc_YearOneBcSeconds + mc_SecondsInDay * mc_DaysInLeapYear;
		constexpr static uint64 mc_YearTwoAdSeconds = mc_YearOneAdSeconds + mc_SecondsInDay * mc_DaysInMedianYear;

		constexpr static uint64 mc_AverageSecondsInYear = constant_uint64(31'556'952);
		constexpr static uint64 mc_SecondsInLeapYear = mc_SecondsInDay * mc_DaysInLeapYear;
		constexpr static uint64 mc_YearOffset = constant_uint64(7'514'938'800);
		constexpr static int64 mc_MaxYear = ((mc_InvalidTimeSeconds - 1) - mc_YearOneBcSeconds) / mc_AverageSecondsInYear;
		constexpr static int64 mc_MinYear = -int64(mc_YearOneBcSeconds / mc_AverageSecondsInYear);
		constexpr static uint64 mc_YearOffsetSeconds = (mc_YearOffset * mc_AverageSecondsInYear) - mc_YearZeroPlus1DaySeconds;

		static_assert(mc_MaxYear == constant_int64(577'039'110'548));
		static_assert(mc_MinYear == constant_int64(-7'514'938'705));
	};

	template <typename tf_CType>
	struct TCIsTime;
}
