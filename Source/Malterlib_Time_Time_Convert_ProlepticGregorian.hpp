// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NTime
{
	constexpr inline_small aint CTimeConvert_ProlepticGreogrian::fsp_GetDayOfYearFromMonth(aint _Month) noexcept
	{
		return mcp_MonthDayOfYear[_Month];
	}

	constexpr inline_small aint CTimeConvert_ProlepticGreogrian::fsp_IsLeapYear(int64 _Year) noexcept
	{
		return !(_Year & 0x3) && ((_Year % 100) || (!(_Year % 400)));
	}

	constexpr inline_small aint CTimeConvert_ProlepticGreogrian::fsp_GetDayOfYearFromMonth(int64 _Year, aint _Month) noexcept
	{
		return fsp_GetDayOfYearFromMonth(_Month) + (fsp_IsLeapYear(_Year) && _Month > 1 ? 1 : 0);
	}

	constexpr inline_always int64 CTimeConvert_ProlepticGreogrian::fsp_GetSecondsFromYearConstexpr(int64 _Year) noexcept
	{
		DMibFastCheck(_Year >= NPrivate::CConst::mc_MinYear);
		DMibFastCheck(_Year <= NPrivate::CConst::mc_MaxYear);

		auto BasedYear = NPrivate::CConst::mc_YearOffset + _Year;

		return
			(BasedYear) * (NPrivate::CConst::mc_DaysInMedianYear * NPrivate::CConst::mc_SecondsInDay)	// Days in normal year
			- NPrivate::CConst::mc_YearOffsetSeconds
			+
			(
				((BasedYear-1) / 4) * NPrivate::CConst::mc_SecondsInDay		// Leap year every 4 years
				- ((BasedYear-1) / 100) * NPrivate::CConst::mc_SecondsInDay	// Not a leap year every 100 years
				+ ((BasedYear-1) / 400) * NPrivate::CConst::mc_SecondsInDay	// A leap year every 400 years despite 100 year rule
			)
		;
	}

	constexpr int64 CTimeConvert_ProlepticGreogrian::fsp_GetSecondsFromYear(int64 _Year) noexcept(NContract::gc_bSafeCheckNoExcept)
	{
		DMibRequire(_Year >= NPrivate::CConst::mc_MinYear);
		DMibRequire(_Year <= NPrivate::CConst::mc_MaxYear);

		return fsp_GetSecondsFromYearConstexpr(_Year);
	}

	constexpr inline_medium aint CTimeConvert_ProlepticGreogrian::fsp_GetWeekDay(int64 _Year, aint _DayOfYear) noexcept
	{
		int64 Days = _Year * NPrivate::CConst::mc_DaysInMedianYear;
		Days += (_Year-1) / 4;
		Days -= (_Year-1) / 100;
		Days += (_Year-1) / 400;
		Days += _DayOfYear;

		Days = ((Days-1) % 7);
		if (Days < 0)
			Days += 7;
		return Days;
	}

	template <aint t_StopAtStage, bool t_bExtractMonth>
	constexpr void CTimeConvert_ProlepticGreogrian::fp_ExtractDateTimeAD(CDateTime &_Dest, uint64 _Seconds) const noexcept
	{
		uint64 Seconds = _Seconds;

		_Dest.m_Year = (Seconds / NPrivate::CConst::mc_AverageSecondsInYear);

		for (umint i = 0; i < 2; ++i)
		{
			uint64 YearSub = _Dest.m_Year;

			if (YearSub > 0)
				--YearSub;

			Seconds -= uint64(_Dest.m_Year) * (NPrivate::CConst::mc_DaysInMedianYear * NPrivate::CConst::mc_SecondsInDay);	// Days in normal year
			Seconds -= (YearSub / 4) * NPrivate::CConst::mc_SecondsInDay;	// Leap year every 4 years
			Seconds += (YearSub / 100) * NPrivate::CConst::mc_SecondsInDay;	// Not a leap year every 100 years
			Seconds -= (YearSub / 400) * NPrivate::CConst::mc_SecondsInDay;	// A leap year every 400 years despite 100 year rule
			if (Seconds <= NPrivate::CConst::mc_SecondsInLeapYear)
				break;
			else if (Seconds <= NPrivate::CConst::mc_SecondsInLeapYear * 2)
			{
				Seconds = _Seconds;
				++_Dest.m_Year;
			}
			else
			{
				Seconds = _Seconds;
				--_Dest.m_Year;
			}
		}

		DMibFastCheck(Seconds <= NPrivate::CConst::mc_SecondsInLeapYear);

		bool bIsLeapYear = fsp_IsLeapYear(_Dest.m_Year);
		if (bIsLeapYear)
		{
			if (Seconds >= NPrivate::CConst::mc_DaysInLeapYear * NPrivate::CConst::mc_SecondsInDay)
			{
				_Dest.m_Year += 1;
				Seconds -= NPrivate::CConst::mc_DaysInLeapYear * NPrivate::CConst::mc_SecondsInDay;
				bIsLeapYear = false;
			}
		}
		else
		{
			if (Seconds >= NPrivate::CConst::mc_DaysInMedianYear * NPrivate::CConst::mc_SecondsInDay)
			{
				_Dest.m_Year += 1;
				Seconds -= NPrivate::CConst::mc_DaysInMedianYear * NPrivate::CConst::mc_SecondsInDay;
				if (!(_Dest.m_Year & 3))
					bIsLeapYear = fsp_IsLeapYear(_Dest.m_Year);
				else
					bIsLeapYear = false;
			}
		}

		_Dest.m_bIsLeapYear = bIsLeapYear;

		if constexpr (t_StopAtStage == 1)
			return;

		aint DayOfYear = Seconds / NPrivate::CConst::mc_SecondsInDay;

		_Dest.m_DayOfWeek = fsp_GetWeekDay(_Dest.m_Year, DayOfYear);
		_Dest.m_DayOfYear = DayOfYear;

		if constexpr (t_StopAtStage == 2)
			return;

		Seconds -= DayOfYear * NPrivate::CConst::mc_SecondsInDay;

		if constexpr (t_bExtractMonth)
		{
			aint PassedLeapDay = 0;
			aint DayOfYearMonth = DayOfYear;

			if (bIsLeapYear && DayOfYear >= 59)
			{
				--DayOfYearMonth;
				PassedLeapDay = 1;
			}

			_Dest.m_Month = 11;
			for (aint i = 1; i < 12; ++i)
			{
				if (DayOfYearMonth < fsp_GetDayOfYearFromMonth(i))
				{
					_Dest.m_Month = i - 1;
					break;
				}
			}

			++_Dest.m_Month;

			if constexpr (t_StopAtStage == 3)
				return;

			if (PassedLeapDay && _Dest.m_Month == 2)
				_Dest.m_DayOfMonth = DayOfYearMonth - fsp_GetDayOfYearFromMonth(_Dest.m_Month-1) + 2;
			else
				_Dest.m_DayOfMonth = DayOfYearMonth - fsp_GetDayOfYearFromMonth(_Dest.m_Month-1) + 1;

			if constexpr (t_StopAtStage == 4)
				return;
		}

		_Dest.m_Hour = Seconds / NPrivate::CConst::mc_SecondsInHour;

		if constexpr (t_StopAtStage == 5)
			return;
		Seconds -= _Dest.m_Hour * NPrivate::CConst::mc_SecondsInHour;

		_Dest.m_Minute = Seconds / NPrivate::CConst::mc_SecondsInMinute;
		if constexpr (t_StopAtStage == 6)
			return;
		Seconds -= _Dest.m_Minute * NPrivate::CConst::mc_SecondsInMinute;

		_Dest.m_Second = Seconds;
		if constexpr (t_StopAtStage == 7)
			return;

		_Dest.m_Fraction = mp_pTime->f_GetFraction();
		_Dest.m_FractionInt = mp_pTime->f_GetFractionInt();
	}

	template <aint t_StopAtStage, bool t_bExtractMonth>
	constexpr void CTimeConvert_ProlepticGreogrian::fp_ExtractDateTimeBC(CDateTime &_Dest, uint64 _Seconds) const noexcept
	{
		auto FixupYear = g_OnScopeExit / [&]()
			{
				_Dest.m_Year -= NPrivate::CConst::mc_YearOffset;
			}
		;

		fp_ExtractDateTimeAD<t_StopAtStage, t_bExtractMonth>(_Dest, _Seconds + NPrivate::CConst::mc_YearOffsetSeconds);
	}

	template <aint t_StopAtStage, bool t_bExtractMonth>
	constexpr void CTimeConvert_ProlepticGreogrian::fp_ExtractDateTime(CDateTime &_Dest) const noexcept
	{
		uint64 Seconds = mp_pTime->f_GetSeconds();

		if (mp_pTime->f_IsValid())
		{
			if (Seconds <= NPrivate::CConst::mc_YearTwoAdSeconds)
				fp_ExtractDateTimeBC<t_StopAtStage, t_bExtractMonth>(_Dest, Seconds);
			else
			{
				Seconds -= NPrivate::CConst::mc_YearZeroPlus1DaySeconds;
				fp_ExtractDateTimeAD<t_StopAtStage, t_bExtractMonth>(_Dest, Seconds);
			}
		}
		else
		{
			NMemory::fg_ObjectSet((int64 *)&_Dest, 0, sizeof(_Dest)/sizeof(int64));
			_Dest.m_Month = 1;
			_Dest.m_DayOfMonth = 1;
		}
	}

	constexpr CTimeConvert_ProlepticGreogrian::CTimeConvert_ProlepticGreogrian(NTime::CTime const &_Time) noexcept
		: CTimeConvert_BabylonianCommon(_Time)
	{
	}

	constexpr CTimeConvert_ProlepticGreogrian &CTimeConvert_ProlepticGreogrian::operator = (NTime::CTime const &_Time) noexcept
	{
			mp_pTime = &_Time;

			return *this;
	}

	constexpr inline_small aint CTimeConvert_ProlepticGreogrian::fs_GetDaysInMonth(aint _Month) noexcept
	{
		return mcp_DaysInMonth[_Month];
	}

	constexpr inline_small aint CTimeConvert_ProlepticGreogrian::fs_GetDaysInMonth(int64 _Year, aint _Month) noexcept
	{
		aint Day;
		if (fsp_IsLeapYear(_Year) && _Month == 1)
			Day = 1;
		else
			Day = 0;
		Day += fs_GetDaysInMonth(_Month);
		return Day;
	}

	constexpr void CTimeConvert_ProlepticGreogrian::f_ExtractDateTime(CDateTime &_Destination) const noexcept
	{
		fp_ExtractDateTime<-1, true>(_Destination);
	}

	constexpr auto CTimeConvert_ProlepticGreogrian::f_ExtractDateTime() const noexcept -> CDateTime
	{
		CDateTime Destination;
		fp_ExtractDateTime<-1, true>(Destination);
		return Destination;
	}

	template <aint t_StopAtStage>
	constexpr void CTimeConvert_ProlepticGreogrian::f_ExtractDateTime(CDateTime &_Destination) const noexcept
	{
		fp_ExtractDateTime<t_StopAtStage, true>(_Destination);
	}

	template <aint t_StopAtStage>
	constexpr auto CTimeConvert_ProlepticGreogrian::f_ExtractDateTime() const noexcept -> CDateTime
	{
		CDateTime Destination;
		fp_ExtractDateTime<t_StopAtStage, true>(Destination);
		return Destination;
	}

	constexpr int64 CTimeConvert_ProlepticGreogrian::f_GetYear() const noexcept
	{
		CDateTime DateTime;
		fp_ExtractDateTime<1, true>(DateTime);
		return DateTime.m_Year;
	}

	constexpr aint CTimeConvert_ProlepticGreogrian::f_GetMonth() const noexcept
	{
		CDateTime DateTime;
		fp_ExtractDateTime<3, true>(DateTime);
		return DateTime.m_Month;
	}

	constexpr aint CTimeConvert_ProlepticGreogrian::f_GetDayOfYear() const noexcept
	{
		CDateTime DateTime;
		fp_ExtractDateTime<3, true>(DateTime);
		return DateTime.m_DayOfYear;
	}

	constexpr aint CTimeConvert_ProlepticGreogrian::f_GetDayOfWeek() const noexcept
	{
		return ((mp_pTime->f_GetSeconds() / NPrivate::CConst::mc_SecondsInDay) + 1) % 7;
	}

	constexpr aint CTimeConvert_ProlepticGreogrian::f_GetDayOfMonth() const noexcept
	{
		CDateTime DateTime;
		fp_ExtractDateTime<4, true>(DateTime);
		return DateTime.m_DayOfMonth;
	}

	constexpr aint CTimeConvert_ProlepticGreogrian::f_GetHour() const noexcept
	{
		CDateTime DateTime;
		fp_ExtractDateTime<5, true>(DateTime);
		return DateTime.m_Hour;
	}

	constexpr aint CTimeConvert_ProlepticGreogrian::f_GetMinute() const noexcept
	{
		CDateTime DateTime;
		fp_ExtractDateTime<6, true>(DateTime);
		return DateTime.m_Minute;
	}

	constexpr aint CTimeConvert_ProlepticGreogrian::f_GetSecond() const noexcept
	{
		CDateTime DateTime;
		fp_ExtractDateTime<7, true>(DateTime);
		return DateTime.m_Second;
	}

	constexpr fp64 CTimeConvert_ProlepticGreogrian::f_GetFraction() const noexcept
	{
		return mp_pTime->f_GetFraction();
	}

	constexpr CTime CTimeConvert_ProlepticGreogrian::fs_GetYearZero() noexcept
	{
		return CTime::fs_Create(NPrivate::CConst::mc_YearOneBcSeconds);
	}

	constexpr CTime CTimeConvert_ProlepticGreogrian::fs_CreateTime(int64 _Year, aint _Month, aint _DayOfMonth, aint _Hour, aint _Minute, aint _Second, fp64 _Fraction)
		noexcept(NContract::gc_bSafeCheckNoExcept)
	{
		--_Month;
		--_DayOfMonth;

		DMibSafeCheck(_Month >= 0 && _Month <= 11, "Month range check error");
		DMibSafeCheck(_DayOfMonth >= 0 && _DayOfMonth < fs_GetDaysInMonth(_Year, _Month), "Day of month range check error");
		DMibSafeCheck(_Hour >= 0 && _Hour <= 23, "Hour range check error");
		DMibSafeCheck(_Minute >= 0 && _Minute <= 59, "Minute range check error");
		DMibSafeCheck(_Second >= 0 && _Second <= 59, "Second range check error");
		DMibSafeCheck(_Fraction >= 0.0 && _Fraction < 1.0, "Fraction range check error");

		uint64 Seconds = fsp_GetSecondsFromYear(_Year);
		Seconds += fsp_GetDayOfYearFromMonth(_Year, _Month) * NPrivate::CConst::mc_SecondsInDay;
		Seconds += _DayOfMonth * NPrivate::CConst::mc_SecondsInDay;
		CTime NewTime{Seconds, 0};
		fsp_AddTime(NewTime, _Hour, _Minute, _Second, _Fraction);
		return NewTime;
	}

	constexpr CTime CTimeConvert_ProlepticGreogrian::fs_CreateTimeIntFrac(int64 _Year, aint _Month, aint _DayOfMonth, aint _Hour, aint _Minute, aint _Second, uint64 _FractionInt)
		noexcept(NContract::gc_bSafeCheckNoExcept)
	{
		--_Month;
		--_DayOfMonth;

		DMibSafeCheck(_Month >= 0 && _Month <= 11, "Month range check error");
		DMibSafeCheck(_DayOfMonth >= 0 && _DayOfMonth < fs_GetDaysInMonth(_Year, _Month), "Day of month range check error");
		DMibSafeCheck(_Hour >= 0 && _Hour <= 23, "Hour range check error");
		DMibSafeCheck(_Minute >= 0 && _Minute <= 59, "Minute range check error");
		DMibSafeCheck(_Second >= 0 && _Second <= 59, "Second range check error");
		DMibSafeCheck(_FractionInt >= 0 && _FractionInt < NPrivate::CConst::mc_FractionDividend, "Fraction range check error");

		uint64 Seconds = fsp_GetSecondsFromYear(_Year);
		Seconds += fsp_GetDayOfYearFromMonth(_Year, _Month) * NPrivate::CConst::mc_SecondsInDay;
		Seconds += _DayOfMonth * NPrivate::CConst::mc_SecondsInDay;
		CTime NewTime{Seconds, 0};
		fsp_AddTimeIntFrac(NewTime, _Hour, _Minute, _Second, _FractionInt);
		return NewTime;
	}

	constexpr CTime CTimeConvert_ProlepticGreogrian::fs_CreateTimeConstExpr(int64 _Year, aint _Month, aint _DayOfMonth, aint _Hour, aint _Minute, aint _Second, uint64 _Fraction) noexcept
	{
		return fsp_AddTimeIntFracConstexr
			(
				CTime
				{
					uint64
					(
						fsp_GetSecondsFromYearConstexpr(_Year)
						+ fsp_GetDayOfYearFromMonth(_Year, _Month - 1) * NPrivate::CConst::mc_SecondsInDay
						+ (_DayOfMonth - 1) * NPrivate::CConst::mc_SecondsInDay
						)
					, 0
				}
				, _Hour
				, _Minute
				, _Second
				, _Fraction
			)
		;
	}

	constexpr CTime CTimeConvert_ProlepticGreogrian::fs_CreateTime(const CDateTime &_DateTime) noexcept
	{
		if (_DateTime.m_FractionInt != NPrivate::CConst::mc_InvalidFractionInt)
			return fs_CreateTimeIntFrac(_DateTime.m_Year, _DateTime.m_Month, _DateTime.m_DayOfMonth, _DateTime.m_Hour, _DateTime.m_Minute, _DateTime.m_Second, _DateTime.m_FractionInt);
		else
			return fs_CreateTime(_DateTime.m_Year, _DateTime.m_Month, _DateTime.m_DayOfMonth, _DateTime.m_Hour, _DateTime.m_Minute, _DateTime.m_Second, _DateTime.m_Fraction);
	}

	constexpr void CTimeConvert_ProlepticGreogrian::fs_RoundTimeToSecondDown(NTime::CTime &_Time) noexcept
	{
		_Time.f_SetFractionInt(0);
	}

	constexpr void CTimeConvert_ProlepticGreogrian::fs_RoundTimeToMinuteDown(NTime::CTime &_Time) noexcept
	{
		uint64 Seconds = _Time.f_GetSeconds();
		_Time.f_SetSeconds(Seconds - (Seconds % NPrivate::CConst::mc_SecondsInMinute));
	}

	constexpr void CTimeConvert_ProlepticGreogrian::fs_RoundTimeToHourDown(NTime::CTime &_Time) noexcept
	{
		uint64 Seconds = _Time.f_GetSeconds();
		_Time.f_SetSeconds(Seconds - (Seconds % NPrivate::CConst::mc_SecondsInHour));
	}

	constexpr void CTimeConvert_ProlepticGreogrian::fs_RoundTimeToDayDown(NTime::CTime &_Time) noexcept
	{
		uint64 Seconds = _Time.f_GetSeconds();
		_Time.f_SetSeconds(Seconds - (Seconds % NPrivate::CConst::mc_SecondsInDay));
	}

	constexpr void CTimeConvert_ProlepticGreogrian::fs_RoundTimeToSecondUp(NTime::CTime &_Time) noexcept
	{
		if (_Time.f_GetFractionInt())
			_Time.f_SetSeconds(_Time.f_GetSeconds() + 1);
	}

	constexpr void CTimeConvert_ProlepticGreogrian::fs_RoundTimeToMinuteUp(NTime::CTime &_Time) noexcept
	{
		uint64 Seconds = _Time.f_GetSeconds();
		uint64 ModSeconds = Seconds % NPrivate::CConst::mc_SecondsInMinute;
		Seconds = Seconds - ModSeconds;
		if (ModSeconds || _Time.f_GetFractionInt())
			Seconds += NPrivate::CConst::mc_SecondsInMinute;
		_Time.f_SetSeconds(Seconds);
	}

	constexpr void CTimeConvert_ProlepticGreogrian::fs_RoundTimeToHourUp(NTime::CTime &_Time) noexcept
	{
		uint64 Seconds = _Time.f_GetSeconds();
		uint64 ModSeconds = Seconds % NPrivate::CConst::mc_SecondsInHour;
		Seconds = Seconds - ModSeconds;
		if (ModSeconds || _Time.f_GetFractionInt())
			Seconds += NPrivate::CConst::mc_SecondsInHour;
		_Time.f_SetSeconds(Seconds);
	}

	constexpr void CTimeConvert_ProlepticGreogrian::fs_RoundTimeToDayUp(NTime::CTime &_Time) noexcept
	{
		uint64 Seconds = _Time.f_GetSeconds();
		uint64 ModSeconds = Seconds % NPrivate::CConst::mc_SecondsInDay;
		Seconds = Seconds - ModSeconds;
		if (ModSeconds || _Time.f_GetFractionInt())
			Seconds += NPrivate::CConst::mc_SecondsInDay;
		_Time.f_SetSeconds(Seconds);
	}

	constexpr void CTimeConvert_ProlepticGreogrian::fs_RoundTimeToSecondClosest(NTime::CTime &_Time) noexcept
	{
		uint64 Seconds = _Time.f_GetSeconds();
		if (_Time.f_GetFractionInt() >= NPrivate::CConst::mc_FractionDividendHalf)
		{
			++Seconds;
		}
		_Time.f_SetSeconds(Seconds);
	}

	constexpr void CTimeConvert_ProlepticGreogrian::fs_RoundTimeToMinuteClosest(NTime::CTime &_Time) noexcept
	{
		uint64 Seconds = _Time.f_GetSeconds();
		uint64 ModSeconds = Seconds % NPrivate::CConst::mc_SecondsInMinute;
		Seconds = Seconds - ModSeconds;
		if (ModSeconds >= NPrivate::CConst::mc_SecondsInMinuteHalf)
			Seconds += NPrivate::CConst::mc_SecondsInMinute;
		_Time.f_SetSeconds(Seconds);
	}

	constexpr void CTimeConvert_ProlepticGreogrian::fs_RoundTimeToHourClosest(NTime::CTime &_Time) noexcept
	{
		uint64 Seconds = _Time.f_GetSeconds();
		uint64 ModSeconds = Seconds % NPrivate::CConst::mc_SecondsInHour;
		Seconds = Seconds - ModSeconds;
		if (ModSeconds >= NPrivate::CConst::mc_SecondsInHourHalf)
			Seconds += NPrivate::CConst::mc_SecondsInHour;
		_Time.f_SetSeconds(Seconds);
	}

	constexpr void CTimeConvert_ProlepticGreogrian::fs_RoundTimeToDayClosest(NTime::CTime &_Time) noexcept
	{
		uint64 Seconds = _Time.f_GetSeconds();
		uint64 ModSeconds = Seconds % NPrivate::CConst::mc_SecondsInDay;
		Seconds = Seconds - ModSeconds;
		if (ModSeconds >= NPrivate::CConst::mc_SecondsInDayHalf)
			Seconds += NPrivate::CConst::mc_SecondsInDay;
		_Time.f_SetSeconds(Seconds);
	}
}
