// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NTime
{
	struct CTimeConvert_BabylonianCommon
	{
		constexpr CTimeConvert_BabylonianCommon(NTime::CTime const &_Time) noexcept;

		constexpr CTimeConvert_BabylonianCommon &operator = (NTime::CTime const &_Time) noexcept;

		constexpr uint64 f_UnixMilliseconds() const noexcept;
		constexpr uint64 f_UnixSeconds() const noexcept;
		constexpr uint64 f_UnixMinutes() const noexcept;
		constexpr fp64 f_UnixSecondsFraction() const noexcept;

		constexpr static CTime fs_FromUnixMilliseconds(uint64 _Milliseconds) noexcept;
		constexpr static CTime fs_FromUnixSeconds(uint64 _Seconds) noexcept;
		constexpr static CTime fs_FromUnixMinutes(uint64 _Minutes) noexcept;
		constexpr static CTime fs_FromUnixSecondsFraction(fp64 _Seconds) noexcept;

	protected:
		constexpr static void fsp_AddTime(CTime &_AddResult, aint _Hour = 0, aint _Minute = 0, aint _Second = 0, fp64 _Fraction = 0) noexcept;
		constexpr static void fsp_AddTimeIntFrac(CTime &_AddResult, aint _Hour = 0, aint _Minute = 0, aint _Second = 0, uint64 _Fraction = 0) noexcept;
		constexpr static CTime fsp_AddTimeIntFracConstexr(CTime const &_Old, aint _Hour = 0, aint _Minute = 0, aint _Second = 0, uint64 _Fraction = 0) noexcept;

		NTime::CTime const *mp_pTime;
	};

	struct CTimeConvert_ProlepticGreogrian : public CTimeConvert_BabylonianCommon
	{
		class CDateTime
		{
		public:
			int64 m_Year; // Note: 1 BC is year 0.
			aint m_Month;
			aint m_DayOfYear;
			aint m_DayOfMonth;
			aint m_DayOfWeek;
			aint m_Hour;
			aint m_Minute;
			aint m_Second;
			fp64 m_Fraction;
			uint64 m_FractionInt = NPrivate::CConst::mc_InvalidFractionInt;
			bool m_bIsLeapYear;
		};

	public:
		constexpr CTimeConvert_ProlepticGreogrian(NTime::CTime const &_Time) noexcept;
		constexpr CTimeConvert_ProlepticGreogrian &operator = (NTime::CTime const &_Time) noexcept;

		constexpr static inline_small aint fs_GetDaysInMonth(aint _Month) noexcept;
		constexpr static inline_small aint fs_GetDaysInMonth(int64 _Year, aint _Month) noexcept;

		constexpr void f_ExtractDateTime(CDateTime &_Destination) const noexcept;

		constexpr CDateTime f_ExtractDateTime() const noexcept;

		template <aint t_StopAtStage>
		constexpr void f_ExtractDateTime(CDateTime &_Destination) const noexcept;

		template <aint t_StopAtStage>
		constexpr CDateTime f_ExtractDateTime() const noexcept;

		constexpr int64 f_GetYear() const noexcept;
		constexpr aint f_GetMonth() const noexcept;
		constexpr aint f_GetDayOfYear() const noexcept;
		constexpr aint f_GetDayOfWeek() const noexcept;
		constexpr aint f_GetDayOfMonth() const noexcept;
		constexpr aint f_GetHour() const noexcept;
		constexpr aint f_GetMinute() const noexcept;
		constexpr aint f_GetSecond() const noexcept;
		constexpr fp64 f_GetFraction() const noexcept;

		constexpr static CTime fs_GetYearZero() noexcept;
		constexpr static CTime fs_CreateTime(int64 _Year, aint _Month = 1, aint _DayOfMonth = 1, aint _Hour = 0, aint _Minute = 0, aint _Second = 0, fp64 _Fraction = 0)
			noexcept(NContract::gc_bSafeCheckNoExcept)
		;
		constexpr static CTime fs_CreateTimeIntFrac(int64 _Year, aint _Month = 1, aint _DayOfMonth = 1, aint _Hour = 0, aint _Minute = 0, aint _Second = 0, uint64 _FractionInt = 0)
			noexcept(NContract::gc_bSafeCheckNoExcept)
		;
		constexpr static CTime fs_CreateTimeConstExpr(int64 _Year, aint _Month = 1, aint _DayOfMonth = 1, aint _Hour = 0, aint _Minute = 0, aint _Second = 0, uint64 _Fraction = 0) noexcept;
		constexpr static CTime fs_CreateTime(CDateTime const &_DateTime) noexcept;

		constexpr static void fs_RoundTimeToSecondDown(NTime::CTime &_Time) noexcept;
		constexpr static void fs_RoundTimeToMinuteDown(NTime::CTime &_Time) noexcept;
		constexpr static void fs_RoundTimeToHourDown(NTime::CTime &_Time) noexcept;
		constexpr static void fs_RoundTimeToDayDown(NTime::CTime &_Time) noexcept;
		constexpr static void fs_RoundTimeToSecondUp(NTime::CTime &_Time) noexcept;
		constexpr static void fs_RoundTimeToMinuteUp(NTime::CTime &_Time) noexcept;
		constexpr static void fs_RoundTimeToHourUp(NTime::CTime &_Time) noexcept;
		constexpr static void fs_RoundTimeToDayUp(NTime::CTime &_Time) noexcept;
		constexpr static void fs_RoundTimeToSecondClosest(NTime::CTime &_Time) noexcept;
		constexpr static void fs_RoundTimeToMinuteClosest(NTime::CTime &_Time) noexcept;
		constexpr static void fs_RoundTimeToHourClosest(NTime::CTime &_Time) noexcept;
		constexpr static void fs_RoundTimeToDayClosest(NTime::CTime &_Time) noexcept;

	private:
		friend struct CTimeConvert_ISOWeek;

		constexpr static inline_small aint fsp_GetDayOfYearFromMonth(aint _Month) noexcept;
		constexpr static inline_small aint fsp_IsLeapYear(int64 _Year) noexcept;
		constexpr static inline_small aint fsp_GetDayOfYearFromMonth(int64 _Year, aint _Month) noexcept;

		constexpr inline_always static int64 fsp_GetSecondsFromYearConstexpr(int64 _Year) noexcept;

		constexpr static int64 fsp_GetSecondsFromYear(int64 _Year) noexcept(NContract::gc_bSafeCheckNoExcept);

		constexpr static inline_medium aint fsp_GetWeekDay(int64 _Year, aint _DayOfYear) noexcept;

		template <aint t_StopAtStage, bool t_bExtractMonth>
		constexpr void fp_ExtractDateTimeAD(CDateTime &_Dest, uint64 _Seconds) const noexcept;

		template <aint t_StopAtStage, bool t_bExtractMonth>
		constexpr void fp_ExtractDateTimeBC(CDateTime &_Dest, uint64 _Seconds) const noexcept;

		template <aint t_StopAtStage, bool t_bExtractMonth>
		constexpr void fp_ExtractDateTime(CDateTime &_Dest) const noexcept;

		constexpr static int32 mcp_MonthDayOfYear[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
		constexpr static int32 mcp_DaysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	};

	struct CTimeConvert_ISOWeek : public CTimeConvert_BabylonianCommon
	{
		class CDateTime
		{
		public:
			int64 m_Year;
			aint m_Week;
			aint m_DayOfYear;
			aint m_DayOfWeek;
			aint m_Hour;
			aint m_Minute;
			aint m_Second;
			uint64 m_FractionInt = NPrivate::CConst::mc_InvalidFractionInt;
			fp64 m_Fraction;
		};

		constexpr CTimeConvert_ISOWeek(CTime const &_Time) noexcept;

		constexpr void f_ExtractDateTime(CDateTime &_Destination) const noexcept;
		constexpr int64 f_GetYear() noexcept;
		constexpr int64 f_GetWeek() noexcept;
		constexpr aint f_GetDayOfWeek() noexcept;
		constexpr aint f_GetDayOfYear() noexcept;
		constexpr aint f_GetHour() const noexcept;
		constexpr aint f_GetMinute() const noexcept;
		constexpr aint f_GetSecond() const noexcept;
		constexpr fp64 f_GetFraction() const noexcept;

		constexpr static CTime fs_CreateTime(int64 _Year, aint _Week = 1, aint _DayOfWeek = 0, aint _Hour = 0, aint _Minute = 0, aint _Second = 0, fp64 _Fraction = 0)
			noexcept(NContract::gc_bSafeCheckNoExcept)
		;

	private:
		template <aint t_StopAtStage>
		constexpr void fp_ExtractDateTime(CDateTime &_Dest) const noexcept;
	};

	// Default fo Proleptic Gregorian time
	using CTimeConvert = CTimeConvert_ProlepticGreogrian;
	using CTimeSpanConvert = CTimeSpanConvert_BabylonianCommon;
}

#include "Malterlib_Time_Time_Convert_Common.hpp"
#include "Malterlib_Time_Time_Convert_IsoWeek.hpp"
#include "Malterlib_Time_Time_Convert_ProlepticGregorian.hpp"
