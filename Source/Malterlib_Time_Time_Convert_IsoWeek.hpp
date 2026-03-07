// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NTime
{
	constexpr CTimeConvert_ISOWeek::CTimeConvert_ISOWeek(CTime const &_Time) noexcept
		: CTimeConvert_BabylonianCommon(_Time)
	{
	}

	constexpr void CTimeConvert_ISOWeek::f_ExtractDateTime(CDateTime &_Destination) const noexcept
	{
		fp_ExtractDateTime<-1>(_Destination);
	}

	constexpr int64 CTimeConvert_ISOWeek::f_GetYear() noexcept
	{
		CDateTime DateTime;
		fp_ExtractDateTime<1>(DateTime);

		return DateTime.m_Year;
	}

	constexpr int64 CTimeConvert_ISOWeek::f_GetWeek() noexcept
	{
		CDateTime DateTime;
		fp_ExtractDateTime<1>(DateTime);

		return DateTime.m_Week;
	}

	constexpr aint CTimeConvert_ISOWeek::f_GetDayOfWeek() noexcept
	{
		return ((mp_pTime->f_GetSeconds() / NPrivate::CConst::mc_SecondsInDay) + 1) % 7;
	}

	constexpr aint CTimeConvert_ISOWeek::f_GetDayOfYear() noexcept
	{
		CDateTime DateTime;
		fp_ExtractDateTime<1>(DateTime);

		return DateTime.m_DayOfYear;
	}

	constexpr aint CTimeConvert_ISOWeek::f_GetHour() const noexcept
	{
		CDateTime DateTime;
		fp_ExtractDateTime<2>(DateTime);
		return DateTime.m_Hour;
	}

	constexpr aint CTimeConvert_ISOWeek::f_GetMinute() const noexcept
	{
		CDateTime DateTime;
		fp_ExtractDateTime<3>(DateTime);
		return DateTime.m_Minute;
	}

	constexpr aint CTimeConvert_ISOWeek::f_GetSecond() const noexcept
	{
		CDateTime DateTime;
		fp_ExtractDateTime<4>(DateTime);
		return DateTime.m_Second;
	}

	constexpr fp64 CTimeConvert_ISOWeek::f_GetFraction() const noexcept
	{
		return mp_pTime->f_GetFraction();
	}

	constexpr CTime CTimeConvert_ISOWeek::fs_CreateTime(int64 _Year, aint _Week, aint _DayOfWeek, aint _Hour, aint _Minute, aint _Second, fp64 _Fraction)
		noexcept(NContract::gc_bSafeCheckNoExcept)
	{
		--_Week;

		DMibSafeCheck(_Week >= 0 && _Week <= 52, "Week range check error");
		DMibSafeCheck(_DayOfWeek >= 0 && _DayOfWeek <= 6, "Day of week range check error");
		DMibSafeCheck(_Hour >= 0 && _Hour <= 23, "Hour range check error");
		DMibSafeCheck(_Minute >= 0 && _Minute <= 59, "Minute range check error");
		DMibSafeCheck(_Second >= 0 && _Second <= 59, "Second range check error");
		DMibSafeCheck(_Fraction >= 0.0 && _Fraction < 1.0, "Fraction range check error");

		CTime NewTime = CTimeConvert_ProlepticGreogrian::fs_CreateTime(_Year);

		CTimeConvert_ProlepticGreogrian::CDateTime DateTime;
		CTimeConvert_ProlepticGreogrian Convert(NewTime);
		Convert.fp_ExtractDateTime<-1, false>(DateTime);

		aint DayOfWeekAtStartOfYear = (DateTime.m_DayOfWeek - (DateTime.m_DayOfYear % 7)) % 7;
		if (DayOfWeekAtStartOfYear < 0)
			DayOfWeekAtStartOfYear += 7;

		aint DaysToAdd = _Week * 7 + _DayOfWeek;

		if (DayOfWeekAtStartOfYear <= 3)
			DaysToAdd -= DayOfWeekAtStartOfYear;
		else
			DaysToAdd += 7 - DayOfWeekAtStartOfYear;

		NewTime += CTimeSpanConvert_BabylonianCommon::fs_CreateDaySpan(DaysToAdd);
		fsp_AddTime(NewTime, _Hour, _Minute, _Second, _Fraction);

		return NewTime;
	}

	template <aint t_StopAtStage>
	constexpr void CTimeConvert_ISOWeek::fp_ExtractDateTime(CDateTime &_Dest) const noexcept
	{
		CTimeConvert_ProlepticGreogrian::CDateTime DateTime;
		CTimeConvert_ProlepticGreogrian Convert(*mp_pTime);

		if constexpr (t_StopAtStage < 0)
			Convert.fp_ExtractDateTime<-1, false>(DateTime);
		else
		{
			if constexpr (t_StopAtStage > 1)
				Convert.fp_ExtractDateTime<4+t_StopAtStage, false>(DateTime);
			else
				Convert.fp_ExtractDateTime<2, false>(DateTime);
		}

		aint DayOfWeekAtStartOfYear = (DateTime.m_DayOfWeek - (DateTime.m_DayOfYear % 7)) % 7;
		if (DayOfWeekAtStartOfYear < 0)
			DayOfWeekAtStartOfYear += 7;

		_Dest.m_Year = DateTime.m_Year;
		_Dest.m_DayOfYear = DateTime.m_DayOfYear;
		_Dest.m_DayOfWeek = DateTime.m_DayOfWeek;
		_Dest.m_Hour = DateTime.m_Hour;
		_Dest.m_Minute = DateTime.m_Minute;
		_Dest.m_Second = DateTime.m_Second;
		_Dest.m_Fraction = DateTime.m_Fraction;
		_Dest.m_FractionInt = DateTime.m_FractionInt;

		if ((DateTime.m_DayOfYear + 1) <= (7 - DayOfWeekAtStartOfYear) && DayOfWeekAtStartOfYear > 3)
		{
			--_Dest.m_Year;
			if (DayOfWeekAtStartOfYear == 4 || (DayOfWeekAtStartOfYear == 5 && CTimeConvert_ProlepticGreogrian::fsp_IsLeapYear(_Dest.m_Year)))
				_Dest.m_Week = 53;
			else
				_Dest.m_Week = 52;
		}
		else
		{
			int DaysInYear = DateTime.m_bIsLeapYear + NPrivate::CConst::mc_DaysInMedianYear;
			if ((DaysInYear - _Dest.m_DayOfYear) < (4 - DateTime.m_DayOfWeek))
			{
				++_Dest.m_Year;
				_Dest.m_Week = 1;
			}
			else
			{
				_Dest.m_Week = ((_Dest.m_DayOfYear + (8 - _Dest.m_DayOfWeek) + DayOfWeekAtStartOfYear) / 7);
				if (DayOfWeekAtStartOfYear > 3)
					--_Dest.m_Week;
			}
		}
	}
}
