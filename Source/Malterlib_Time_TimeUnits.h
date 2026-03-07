// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

constexpr inline pfp64 operator ""_ns(long double _Value) noexcept;
constexpr inline pfp64 operator ""_µs(long double _Value) noexcept;
constexpr inline pfp64 operator ""_ms(long double _Value) noexcept;
constexpr inline pfp64 operator ""_seconds(long double _Value) noexcept;
constexpr inline pfp64 operator ""_minutes(long double _Value) noexcept;
constexpr inline pfp64 operator ""_hours(long double _Value) noexcept;
constexpr inline pfp64 operator ""_days(long double _Value) noexcept;
constexpr inline pfp64 operator ""_weeks(long double _Value) noexcept;
constexpr inline pfp64 operator ""_ns(unsigned long long _Value) noexcept;
constexpr inline pfp64 operator ""_µs(unsigned long long _Value) noexcept;
constexpr inline pfp64 operator ""_ms(unsigned long long _Value) noexcept;
constexpr inline pfp64 operator ""_seconds(unsigned long long _Value) noexcept;
constexpr inline pfp64 operator ""_minutes(unsigned long long _Value) noexcept;
constexpr inline pfp64 operator ""_hours(unsigned long long _Value) noexcept;
constexpr inline pfp64 operator ""_days(unsigned long long _Value) noexcept;
constexpr inline pfp64 operator ""_weeks(unsigned long long _Value) noexcept;

#include "Malterlib_Time_TimeUnits.hpp"
