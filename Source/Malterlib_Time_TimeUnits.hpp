// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

constexpr inline pfp64 operator ""_ns(long double _Value) noexcept
{
	return _Value * 0.000'000'001;
}

constexpr inline pfp64 operator ""_µs(long double _Value) noexcept
{
	return _Value * 0.000'001;
}

constexpr inline pfp64 operator ""_ms(long double _Value) noexcept
{
	return _Value * 0.001;
}

constexpr inline pfp64 operator ""_seconds(long double _Value) noexcept
{
	return _Value;
}

constexpr inline pfp64 operator ""_minutes(long double _Value) noexcept
{
	return _Value * NMib::NTime::NPrivate::CConst::mc_SecondsInMinuteFp;
}

constexpr inline pfp64 operator ""_hours(long double _Value) noexcept
{
	return _Value * NMib::NTime::NPrivate::CConst::mc_SecondsInHourFp;
}

constexpr inline pfp64 operator ""_days(long double _Value) noexcept
{
	return _Value * NMib::NTime::NPrivate::CConst::mc_SecondsInDayFp;
}

constexpr inline pfp64 operator ""_weeks(long double _Value) noexcept
{
	return _Value * NMib::NTime::NPrivate::CConst::mc_SecondsInWeekFp;
}

constexpr inline pfp64 operator ""_ns(unsigned long long _Value) noexcept
{
	return pfp64(_Value) * 0.000'000'001;
}

constexpr inline pfp64 operator ""_µs(unsigned long long _Value) noexcept
{
	return pfp64(_Value) * 0.000'001;
}

constexpr inline pfp64 operator ""_ms(unsigned long long _Value) noexcept
{
	return pfp64(_Value) * 0.001;
}

constexpr inline pfp64 operator ""_seconds(unsigned long long _Value) noexcept
{
	return pfp64(_Value);
}

constexpr inline pfp64 operator ""_minutes(unsigned long long _Value) noexcept
{
	return pfp64(_Value) * NMib::NTime::NPrivate::CConst::mc_SecondsInMinuteFp;
}

constexpr inline pfp64 operator ""_hours(unsigned long long _Value) noexcept
{
	return pfp64(_Value) * NMib::NTime::NPrivate::CConst::mc_SecondsInHourFp;
}

constexpr inline pfp64 operator ""_days(unsigned long long _Value) noexcept
{
	return pfp64(_Value) * NMib::NTime::NPrivate::CConst::mc_SecondsInDayFp;
}

constexpr inline pfp64 operator ""_weeks(unsigned long long _Value) noexcept
{
	return pfp64(_Value) * NMib::NTime::NPrivate::CConst::mc_SecondsInWeekFp;
}
