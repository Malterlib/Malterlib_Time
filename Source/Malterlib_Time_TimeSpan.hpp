// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NTime
{
	constexpr CTimeSpan::CTimeSpan(int64 _Value) noexcept
		: mp_Seconds(_Value)
	{
#ifdef DMibDebuggerHelpers
		static_assert(TCInstantiateValue<&CTimeSpan::fsp_DebugStr>::mc_Value);
#endif
	}

	constexpr CTimeSpan::CTimeSpan(int64 _Value, uint64 _Fraction) noexcept
		: mp_Seconds (_Value)
		, mp_Fraction(_Fraction)
	{
#ifdef DMibDebuggerHelpers
		static_assert(TCInstantiateValue<&CTimeSpan::fsp_DebugStr>::mc_Value);
#endif
	}

	constexpr CTimeSpan::CTimeSpan() noexcept
	{
#ifdef DMibDebuggerHelpers
		static_assert(TCInstantiateValue<&CTimeSpan::fsp_DebugStr>::mc_Value);
#endif
	}

	constexpr bool CTimeSpan::f_IsValid() const noexcept
	{
		return mp_Seconds != NPrivate::CConst::mc_InvalidSpanSeconds;
	}

	constexpr CTimeSpan CTimeSpan::operator + (const CTimeSpan &_Other) const noexcept
	{
		CTimeSpan Ret;
		Ret.mp_Seconds = mp_Seconds + _Other.mp_Seconds;
		uint64 FracAdd = _Other.mp_Fraction;
		uint64 Temp = NPrivate::CConst::mc_FractionDividend - mp_Fraction;

		if (FracAdd >= Temp)
		{
			++Ret.mp_Seconds;
			Ret.mp_Fraction = FracAdd - Temp;
		}
		else
			Ret.mp_Fraction = mp_Fraction + FracAdd;
		return Ret;
	}

	constexpr CTimeSpan CTimeSpan::operator - (const CTimeSpan &_Other) const noexcept
	{
		CTimeSpan Ret;

		Ret.mp_Seconds = mp_Seconds - _Other.mp_Seconds;
		uint64 FracSub = _Other.mp_Fraction;
		uint64 Temp = mp_Fraction;

		if (FracSub > Temp)
		{
			--Ret.mp_Seconds;
			Ret.mp_Fraction = NPrivate::CConst::mc_FractionDividend - (FracSub - Temp);
		}
		else
			Ret.mp_Fraction = Temp - FracSub;

		return Ret;
	}

	constexpr CTimeSpan CTimeSpan::operator - () const noexcept
	{
		CTimeSpan Temp;
		Temp.f_SetSeconds(0);
		return Temp - *this;
	}

	constexpr CTimeSpan CTimeSpan::operator * (int64 _nTimes) const noexcept
	{
		CTimeSpan Ret;
		Ret.mp_Seconds = mp_Seconds * _nTimes;
		Ret.mp_Fraction = mp_Fraction;

		if (mp_Fraction)
		{
			int128 TempFrac = mp_Fraction;
			TempFrac *= _nTimes;
			int64 nSeconds = int64(TempFrac / int128(NPrivate::CConst::mc_FractionDividend));
			Ret.mp_Seconds += nSeconds;
			Ret.mp_Fraction = uint64(TempFrac - int128(NPrivate::CConst::mc_FractionDividend) * (int128)nSeconds);
		}

		return Ret;
	}

	constexpr CTimeSpan CTimeSpan::operator / (int64 _nTimes) const noexcept
	{
		CTimeSpan Ret;
		int128 Temp = (int128(mp_Seconds) * int128(NPrivate::CConst::mc_FractionDividend) + int128(mp_Fraction)) / int128(_nTimes);
		int64 nSeconds = int64(Temp / int128(NPrivate::CConst::mc_FractionDividend));
		Ret.mp_Seconds = nSeconds;
		Ret.mp_Fraction = uint64(Temp - int128(NPrivate::CConst::mc_FractionDividend) * int128(nSeconds));

		return Ret;
	}

	constexpr CTimeSpan &CTimeSpan::operator += (const CTimeSpan &_Other) noexcept
	{
		mp_Seconds += _Other.mp_Seconds;
		uint64 FracAdd = _Other.mp_Fraction;
		uint64 Temp = NPrivate::CConst::mc_FractionDividend - mp_Fraction;
		if (FracAdd >= Temp)
		{
			++mp_Seconds;
			mp_Fraction = FracAdd - Temp;
		}
		else
			mp_Fraction += FracAdd;

		return *this;
	}

	constexpr CTimeSpan &CTimeSpan::operator -= (const CTimeSpan &_Other) noexcept
	{
		mp_Seconds -= _Other.mp_Seconds;
		uint64 FracSub = _Other.mp_Fraction;
		uint64 Temp = mp_Fraction;

		if (FracSub > Temp)
		{
			--mp_Seconds;
			mp_Fraction = NPrivate::CConst::mc_FractionDividend - (FracSub - Temp);
		}
		else
			mp_Fraction -= FracSub;

		return *this;
	}

	constexpr CTimeSpan &CTimeSpan::operator *= (int64 _nTimes) noexcept
	{
		mp_Seconds *= _nTimes;
		if (mp_Fraction)
		{
			int128 TempFrac = mp_Fraction;
			TempFrac *= _nTimes;
			int64 nSeconds = fg_Convert<int64>(TempFrac / int128(NPrivate::CConst::mc_FractionDividend));
			mp_Seconds += nSeconds;
			mp_Fraction = fg_Convert<int64>(TempFrac - int128(NPrivate::CConst::mc_FractionDividend) * int128(nSeconds));
		}

		return *this;
	}

	constexpr CTimeSpan &CTimeSpan::operator /= (int64 _nTimes) noexcept
	{
		int128 Temp = (int128(mp_Seconds) * int128(NPrivate::CConst::mc_FractionDividend) + int128(mp_Fraction)) / int128(_nTimes);
		int64 nSeconds = fg_Convert<int64>(Temp / int128(NPrivate::CConst::mc_FractionDividend));
		mp_Seconds = nSeconds;
		mp_Fraction = fg_Convert<int64>(Temp - int128(NPrivate::CConst::mc_FractionDividend) * int128(nSeconds));

		return *this;
	}

	constexpr int64 CTimeSpan::f_GetSeconds() const noexcept
	{
		return mp_Seconds;
	}

	constexpr void CTimeSpan::f_SetSeconds(int64 _Seconds) noexcept
	{
		mp_Seconds = _Seconds;
		mp_Fraction = 0;
	}

	constexpr void CTimeSpan::f_SetSecondsNoFraction(int64 _Seconds) noexcept
	{
		mp_Seconds = _Seconds;
	}

	constexpr uint64 CTimeSpan::f_GetFractionInt() const noexcept
	{
		return mp_Fraction;
	}

	constexpr void CTimeSpan::f_SetFractionInt(uint64 _Fraction) noexcept
	{
		mp_Fraction = _Fraction;
	}

	constexpr fp64 CTimeSpan::f_GetFraction() const noexcept
	{
		return fp64(mp_Fraction) * NPrivate::CConst::mc_FractionDividendFpInv;
	}

	constexpr void CTimeSpan::f_SetFraction(fp64 _Fraction) noexcept
	{
		mp_Fraction = (_Fraction * NPrivate::CConst::mc_FractionDividendFp).f_ToUnsignedInt();
	}

	constexpr fp64 CTimeSpan::f_GetSecondsFraction() const noexcept
	{
		return fp64(mp_Seconds) + fp64(mp_Fraction) * NPrivate::CConst::mc_FractionDividendFpInv;
	}

	constexpr void CTimeSpan::f_SetSecondsFraction(fp64 _Seconds) noexcept
	{
		f_SetSeconds(_Seconds.f_ToInt());
		f_SetFraction(_Seconds.f_Fraction());
	}

	constexpr CTimeSpan CTimeSpan::fs_SmallestTime() noexcept
	{
		CTimeSpan Ret;
		Ret.mp_Seconds = 0;
		Ret.mp_Fraction = 1;
		return Ret;
	}

	constexpr CTimeSpan CTimeSpan::fs_Zero() noexcept
	{
		CTimeSpan Ret;
		Ret.mp_Seconds = 0;
		Ret.mp_Fraction = 0;
		return Ret;
	}
}
