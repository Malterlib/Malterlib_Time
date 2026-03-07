// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NTime
{
	constexpr CTime CTimeConvert_BabylonianCommon::fs_FromUnixSeconds(uint64 _Seconds) noexcept
	{
		CTime Ret;
		Ret.f_SetSeconds(NPrivate::CConst::mc_UnixEpochSeconds + _Seconds);
		return Ret;
	}

	constexpr CTimeConvert_BabylonianCommon::CTimeConvert_BabylonianCommon(NTime::CTime const &_Time) noexcept
		: mp_pTime(&_Time)
	{
	}

	constexpr CTimeConvert_BabylonianCommon &CTimeConvert_BabylonianCommon::operator = (NTime::CTime const &_Time) noexcept
	{
		mp_pTime = &_Time;

		return *this;
	}

	constexpr void CTimeConvert_BabylonianCommon::fsp_AddTime(CTime &_AddResult, aint _Hour, aint _Minute, aint _Second, fp64 _Fraction) noexcept
	{
		_AddResult.mp_Seconds += _Hour * NPrivate::CConst::mc_SecondsInHour + _Minute * NPrivate::CConst::mc_SecondsInMinute + _Second;
		uint64 FracAdd = (_Fraction * NPrivate::CConst::mc_FractionDividendFp).f_ToUnsignedInt();
		if (FracAdd >= NPrivate::CConst::mc_FractionDividend)
			FracAdd = NPrivate::CConst::mc_FractionDividend - 1;
		uint64 Temp = NPrivate::CConst::mc_FractionDividend - _AddResult.mp_Fraction;
		if (FracAdd >= Temp)
		{
			++_AddResult.mp_Seconds;
			_AddResult.mp_Fraction = FracAdd - Temp;
		}
		else
			_AddResult.mp_Fraction += FracAdd;
	}

	constexpr void CTimeConvert_BabylonianCommon::fsp_AddTimeIntFrac(CTime &_AddResult, aint _Hour, aint _Minute, aint _Second, uint64 _Fraction) noexcept
	{
		_AddResult.mp_Seconds += _Hour * NPrivate::CConst::mc_SecondsInHour + _Minute * NPrivate::CConst::mc_SecondsInMinute + _Second;
		uint64 FracAdd = _Fraction;
		uint64 Temp = NPrivate::CConst::mc_FractionDividend - _AddResult.mp_Fraction;
		if (FracAdd >= Temp)
		{
			++_AddResult.mp_Seconds;
			_AddResult.mp_Fraction = FracAdd - Temp;
		}
		else
			_AddResult.mp_Fraction += FracAdd;
	}

	constexpr CTime CTimeConvert_BabylonianCommon::fsp_AddTimeIntFracConstexr(CTime const &_Old, aint _Hour, aint _Minute, aint _Second, uint64 _Fraction) noexcept
	{
		return (_Fraction >= (NPrivate::CConst::mc_FractionDividend - _Old.mp_Fraction))
			? CTime
			{
				_Old.mp_Seconds + _Hour * NPrivate::CConst::mc_SecondsInHour + _Minute * NPrivate::CConst::mc_SecondsInMinute + _Second + 1
				, _Fraction - (NPrivate::CConst::mc_FractionDividend - _Old.mp_Fraction)
			}
			: CTime{_Old.mp_Seconds + _Hour * NPrivate::CConst::mc_SecondsInHour + _Minute * NPrivate::CConst::mc_SecondsInMinute + _Second, _Old.mp_Fraction + _Fraction}
		;
	}

	constexpr uint64 CTimeConvert_BabylonianCommon::f_UnixMilliseconds() const noexcept
	{
		uint64 Return = (mp_pTime->f_GetSeconds() - NPrivate::CConst::mc_UnixEpochSeconds) * constant_int64(1000);
		Return += (mp_pTime->f_GetFraction() * 1000.0).f_ToIntRound();
		return Return;
	}

	constexpr CTime CTimeConvert_BabylonianCommon::fs_FromUnixMilliseconds(uint64 _Milliseconds) noexcept
	{
		uint64 Seconds = _Milliseconds / constant_int64(1000);
		uint64 Fraction = (_Milliseconds - Seconds * constant_int64(1000));

		CTime Ret;
		Ret.f_SetSeconds(NPrivate::CConst::mc_UnixEpochSeconds + Seconds);
		Ret.f_SetFraction(fp64(Fraction) / 1000.0);

		return Ret;
	}

	constexpr uint64 CTimeConvert_BabylonianCommon::f_UnixSeconds() const noexcept
	{
		return mp_pTime->f_GetSeconds() - NPrivate::CConst::mc_UnixEpochSeconds;
	}

	constexpr uint64 CTimeConvert_BabylonianCommon::f_UnixMinutes() const noexcept
	{
		return f_UnixSeconds() / NPrivate::CConst::mc_SecondsInMinute;
	}

	constexpr CTime CTimeConvert_BabylonianCommon::fs_FromUnixMinutes(uint64 _Minutes) noexcept
	{
		CTime Ret;
		Ret.f_SetSeconds(NPrivate::CConst::mc_UnixEpochSeconds + _Minutes * NPrivate::CConst::mc_SecondsInMinute);
		return Ret;
	}

	constexpr fp64 CTimeConvert_BabylonianCommon::f_UnixSecondsFraction() const noexcept
	{
		return fp64(mp_pTime->f_GetSeconds() - NPrivate::CConst::mc_UnixEpochSeconds) + mp_pTime->f_GetFraction();
	}

	constexpr CTime CTimeConvert_BabylonianCommon::fs_FromUnixSecondsFraction(fp64 _Seconds) noexcept
	{
		CTime Ret;
		Ret.f_SetSeconds(NPrivate::CConst::mc_UnixEpochSeconds + _Seconds.f_ToUnsignedInt());
		Ret.f_SetFraction(_Seconds.f_Fraction());
		return Ret;
	}
}
