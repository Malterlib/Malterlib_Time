// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NTime
{
	constexpr CTimeSpanConvert_BabylonianCommon::CTimeSpanConvert_BabylonianCommon(const CTimeSpan &_Time) noexcept
		: mp_pTime(&_Time)
	{
	}

	constexpr CTimeSpanConvert_BabylonianCommon &CTimeSpanConvert_BabylonianCommon::operator = (const CTimeSpan &_Time) noexcept
	{
		mp_pTime = &_Time;

		return *this;
	}

	constexpr CTimeSpan CTimeSpanConvert_BabylonianCommon::fs_CreateSpan(int64 _Weeks, int64 _Days, int64 _Hours, int64 _Minutes, int64 _Seconds, fp64 _Fraction) noexcept
	{
		CTimeSpan NewSpan;
		int64 Seconds = _Seconds;
		Seconds += _Minutes * NPrivate::CConst::mc_SecondsInMinuteSigned;
		Seconds += _Hours * NPrivate::CConst::mc_SecondsInHourSigned;
		Seconds += _Days * NPrivate::CConst::mc_SecondsInDaySigned;
		Seconds += _Weeks * NPrivate::CConst::mc_SecondsInWeekSigned;

		NewSpan.f_SetSecondsNoFraction(Seconds);
		NewSpan.f_SetFraction(_Fraction);

		return NewSpan;
	}

	constexpr CTimeSpan CTimeSpanConvert_BabylonianCommon::fs_CreateWeekSpan(int64 _Weeks) noexcept
	{
		CTimeSpan NewSpan;
		NewSpan.f_SetSeconds(_Weeks * 604800);
		return NewSpan;
	}

	constexpr CTimeSpan CTimeSpanConvert_BabylonianCommon::fs_CreateDaySpan(int64 _Days) noexcept
	{
		CTimeSpan NewSpan;
		NewSpan.f_SetSeconds(_Days * NPrivate::CConst::mc_SecondsInDaySigned);
		return NewSpan;
	}

	constexpr CTimeSpan CTimeSpanConvert_BabylonianCommon::fs_CreateHourSpan(int64 _Hours) noexcept
	{
		CTimeSpan NewSpan;
		NewSpan.f_SetSeconds(_Hours * NPrivate::CConst::mc_SecondsInHourSigned);
		return NewSpan;
	}


	constexpr CTimeSpan CTimeSpanConvert_BabylonianCommon::fs_CreateMinuteSpan(int64 _Minutes) noexcept
	{
		CTimeSpan NewSpan;
		NewSpan.f_SetSeconds(_Minutes * NPrivate::CConst::mc_SecondsInMinuteSigned);
		return NewSpan;
	}


	constexpr CTimeSpan CTimeSpanConvert_BabylonianCommon::fs_CreateSecondSpan(int64 _Seconds) noexcept
	{
		CTimeSpan NewSpan;
		NewSpan.f_SetSeconds(_Seconds);
		return NewSpan;
	}

	constexpr CTimeSpan CTimeSpanConvert_BabylonianCommon::fs_CreateSpanFromWeeks(fp64 _Weeks) noexcept
	{
		return fs_CreateSpanFromSeconds(_Weeks * 604800.0);
	}

	constexpr CTimeSpan CTimeSpanConvert_BabylonianCommon::fs_CreateSpanFromDays(fp64 _Days) noexcept
	{
		return fs_CreateSpanFromSeconds(_Days * NPrivate::CConst::mc_SecondsInDayFp);
	}

	constexpr CTimeSpan CTimeSpanConvert_BabylonianCommon::fs_CreateSpanFromHours(fp64 _Hours) noexcept
	{
		return fs_CreateSpanFromSeconds(_Hours * NPrivate::CConst::mc_SecondsInHourFp);
	}

	constexpr CTimeSpan CTimeSpanConvert_BabylonianCommon::fs_CreateSpanFromMinutes(fp64 _Minutes) noexcept
	{
		return fs_CreateSpanFromSeconds(_Minutes * NPrivate::CConst::mc_SecondsInMinuteFp);
	}

	constexpr CTimeSpan CTimeSpanConvert_BabylonianCommon::fs_CreateSpanFromSeconds(fp64 _Seconds) noexcept
	{
		CTimeSpan NewSpan;

		int64 Seconds = _Seconds.f_ToInt();
		NewSpan.f_SetSecondsNoFraction(Seconds);
		NewSpan.f_SetFraction(_Seconds - Seconds);

		return NewSpan;
	}

	constexpr CTimeSpan CTimeSpanConvert_BabylonianCommon::fs_CreateSpanFromSeconds(int64 _Seconds, fp64 _Fraction) noexcept
	{
		CTimeSpan NewSpan;

		NewSpan.f_SetSecondsNoFraction(_Seconds);
		NewSpan.f_SetFraction(_Fraction);

		return NewSpan;
	}

	constexpr int64 CTimeSpanConvert_BabylonianCommon::f_GetWeeks() const noexcept
	{
		return f_GetSeconds() / NPrivate::CConst::mc_SecondsInWeekSigned;
	}

	constexpr fp64 CTimeSpanConvert_BabylonianCommon::f_GetWeeksFloat() const noexcept
	{
		return mp_pTime->f_GetSecondsFraction() / NPrivate::CConst::mc_SecondsInWeekFp;
	}

	constexpr int64 CTimeSpanConvert_BabylonianCommon::f_GetDays() const noexcept
	{
		return f_GetSeconds() / NPrivate::CConst::mc_SecondsInDaySigned;
	}

	constexpr fp64 CTimeSpanConvert_BabylonianCommon::f_GetDaysFloat() const noexcept
	{
		return mp_pTime->f_GetSecondsFraction() / NPrivate::CConst::mc_SecondsInDayFp;
	}

	constexpr int64 CTimeSpanConvert_BabylonianCommon::f_GetHours() const noexcept
	{
		return f_GetSeconds() / NPrivate::CConst::mc_SecondsInHourSigned;
	}

	constexpr fp64 CTimeSpanConvert_BabylonianCommon::f_GetHoursFloat() const noexcept
	{
		return mp_pTime->f_GetSecondsFraction() / NPrivate::CConst::mc_SecondsInHourFp;
	}

	constexpr int64 CTimeSpanConvert_BabylonianCommon::f_GetMinutes() const noexcept
	{
		return f_GetSeconds() / NPrivate::CConst::mc_SecondsInMinuteSigned;
	}

	constexpr fp64 CTimeSpanConvert_BabylonianCommon::f_GetMinutesFloat() const noexcept
	{
		return mp_pTime->f_GetSecondsFraction() / NPrivate::CConst::mc_SecondsInMinuteFp;
	}

	constexpr int64 CTimeSpanConvert_BabylonianCommon::f_GetSeconds() const noexcept
	{
		auto Seconds = mp_pTime->f_GetSeconds();
		if (Seconds < 0 && mp_pTime->f_GetFractionInt() != 0)
			return Seconds + 1;
		else
			return Seconds;
	}

	constexpr fp64 CTimeSpanConvert_BabylonianCommon::f_GetSecondsFloat() const noexcept
	{
		return mp_pTime->f_GetSecondsFraction();
	}

	constexpr fp64 CTimeSpanConvert_BabylonianCommon::f_GetFraction() const noexcept
	{
		auto Seconds = mp_pTime->f_GetSeconds();
		if (Seconds < 0)
			return fp64(NPrivate::CConst::mc_FractionDividend - mp_pTime->f_GetFractionInt()) * NPrivate::CConst::mc_FractionDividendFpInv;
		else
			return mp_pTime->f_GetFraction();
	}

	constexpr aint CTimeSpanConvert_BabylonianCommon::f_GetHourOfDay() const noexcept
	{
		int64 Seconds = fg_Abs(f_GetSeconds());
		Seconds -= (Seconds / NPrivate::CConst::mc_SecondsInDaySigned) * NPrivate::CConst::mc_SecondsInDaySigned;
		return Seconds / NPrivate::CConst::mc_SecondsInHourSigned;
	}

	constexpr aint CTimeSpanConvert_BabylonianCommon::f_GetMinuteOfHour() const noexcept
	{
		int64 Seconds = fg_Abs(f_GetSeconds());
		Seconds -= (Seconds / NPrivate::CConst::mc_SecondsInDaySigned) * NPrivate::CConst::mc_SecondsInDaySigned;
		Seconds -= (Seconds / NPrivate::CConst::mc_SecondsInHourSigned) * NPrivate::CConst::mc_SecondsInHourSigned;
		return Seconds / NPrivate::CConst::mc_SecondsInMinuteSigned;
	}

	constexpr aint CTimeSpanConvert_BabylonianCommon::f_GetSecondOfMinute() const noexcept
	{
		int64 Seconds = fg_Abs(f_GetSeconds());
		Seconds -= (Seconds / NPrivate::CConst::mc_SecondsInDaySigned) * NPrivate::CConst::mc_SecondsInDaySigned;
		Seconds -= (Seconds / NPrivate::CConst::mc_SecondsInHourSigned) * NPrivate::CConst::mc_SecondsInHourSigned;
		Seconds -= (Seconds / NPrivate::CConst::mc_SecondsInMinuteSigned) * NPrivate::CConst::mc_SecondsInMinuteSigned;
		return Seconds;
	}
}
