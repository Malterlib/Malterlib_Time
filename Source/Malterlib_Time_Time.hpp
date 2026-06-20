// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NTime::NPrivate
{
	template <typename tf_CType>
	struct TCIsTime
	{
		constexpr static bool mc_bValue = false;
	};

	template <>
	struct TCIsTime<CTime>
	{
		constexpr static bool mc_bValue = true;
	};
}

namespace NMib::NTime
{
	constexpr CTime::CTime(uint64 _Seconds, uint64 _Fraction) noexcept
		: mp_Seconds(_Seconds)
		, mp_Fraction(_Fraction)
	{
	}

	constexpr CTime::CTime() noexcept = default;

	constexpr bool CTime::f_IsValid() const noexcept
	{
		return mp_Seconds != NPrivate::CConst::mc_InvalidTimeSeconds;
	}

	constexpr CTime &CTime::operator += (CTimeSpan const &_Other) noexcept
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

	constexpr CTime &CTime::operator -= (CTimeSpan const &_Other) noexcept
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

	constexpr CTimeSpan CTime::operator - (CTime const &_Other) const noexcept
	{
		CTimeSpan Ret;
		DMibSafeCheck(this->f_IsValid() && _Other.f_IsValid(), "You should not calculate spans on invalid dates");

		int64 ToSet;
		if (mp_Seconds > _Other.mp_Seconds)
		{
			uint64 SpanSeconds = mp_Seconds - _Other.mp_Seconds;
			if (SpanSeconds >= (constant_uint64(1) << 63) - 1)
			{
				SpanSeconds = (constant_uint64(1) << 63) - 2;
			}
			ToSet = SpanSeconds;
		}
		else
		{
			uint64 SpanSeconds = _Other.mp_Seconds - mp_Seconds;
			if (SpanSeconds >= (constant_uint64(1) << 63) - 1)
			{
				SpanSeconds = (constant_uint64(1) << 63) - 2;
			}
			ToSet = -SpanSeconds;
		}
		Ret.f_SetSecondsNoFraction(ToSet);
		uint64 FracSub = _Other.mp_Fraction;
		uint64 Temp = mp_Fraction;

		if (FracSub > Temp)
		{
			--Ret.mp_Seconds;
			Ret.f_SetFractionInt(NPrivate::CConst::mc_FractionDividend - (FracSub - Temp));
		}
		else
			Ret.f_SetFractionInt(Temp - FracSub);

		return Ret;
	}

	constexpr CTime CTime::operator - (CTimeSpan const &_Other) const noexcept
	{
		DMibSafeCheck(f_IsValid(), "Must be valid");
		CTime Ret;

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

	constexpr CTime CTime::operator + (CTimeSpan const &_Other) const noexcept
	{
		DMibSafeCheck(f_IsValid(), "Must be valid");
		CTime Ret;
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

	constexpr CTime &CTime::operator ++ () noexcept
	{
		DMibSafeCheck(f_IsValid(), "Must be valid");
		if (mp_Fraction == NPrivate::CConst::mc_FractionDividend - 1)
		{
			mp_Fraction = 0;
			++mp_Seconds;
		}
		else
		{
			++mp_Fraction;
		}

		return *this;
	}

	constexpr CTime &CTime::operator -- () noexcept
	{
		DMibSafeCheck(f_IsValid(), "Must be valid");
		if (mp_Fraction == 0)
		{
			mp_Fraction = NPrivate::CConst::mc_FractionDividend - 1;
			--mp_Seconds;
		}
		else
		{
			--mp_Fraction;
		}

		return *this;
	}

	constexpr uint64 CTime::f_GetSeconds() const noexcept
	{
		return mp_Seconds;
	}

	constexpr void CTime::f_SetSeconds(uint64 _Seconds) noexcept
	{
		mp_Seconds = _Seconds;
		mp_Fraction = 0;
	}

	constexpr void CTime::f_SetSecondsNoFraction(uint64 _Seconds) noexcept
	{
		mp_Seconds = _Seconds;
	}

	constexpr uint64 CTime::f_GetFractionInt() const noexcept
	{
		return mp_Fraction;
	}

	constexpr void CTime::f_SetFractionInt(uint64 _Fraction) noexcept
	{
		mp_Fraction = _Fraction;
	}

	constexpr fp64 CTime::f_GetFraction() const noexcept
	{
		return fp64(mp_Fraction) * NPrivate::CConst::mc_FractionDividendFpInv;
	}

	constexpr void CTime::f_SetFraction(fp64 _Fraction) noexcept
	{
		mp_Fraction = (_Fraction * NPrivate::CConst::mc_FractionDividendFp).f_ToUnsignedInt();
	}

	constexpr CTime CTime::fs_EndOfTime() noexcept
	{
		CTime Ret;
		Ret.mp_Seconds = NPrivate::CConst::mc_EndOfTime;
		Ret.mp_Fraction = NPrivate::CConst::mc_FractionDividend - 1;

		return Ret;
	}

	constexpr CTime CTime::fs_StartOfTime() noexcept
	{
		CTime Ret;
		Ret.mp_Seconds = 0;
		Ret.mp_Fraction = 0;

		return Ret;
	}

	constexpr CTime CTime::fs_Create(uint64 _Seconds, uint64 _Fraction) noexcept
	{
		return CTime{_Seconds, _Fraction};
	}

	constexpr fp64 CTime::f_GetSecondsFraction() const noexcept
	{
		return fp64(mp_Seconds) + fp64(mp_Fraction) * NPrivate::CConst::mc_FractionDividendFpInv;
	}
}
