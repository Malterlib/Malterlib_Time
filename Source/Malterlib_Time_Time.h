// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/Contract/Contract>
#include <Mib/Concurrency/AsyncResult>

#include "Malterlib_Time_TimeSpan.h"

#include "Malterlib_Time_TimeDefines.hpp"

namespace NMib::NTime
{
	struct CTime
	{
		struct CFormatOptions
		{
			uint32 m_Components = 0;
			bool m_bDateOnly = false;
			bool m_bFullPrecision = false;
			bool m_bBcSuffix = false;
			ch32 m_DateTimeSeparator = ' ';
			ch32 m_DateSeparator = '-';
			ch32 m_TimeSeparator = ':';
			ch32 m_SecondSeparator = '.';
		};

		constexpr CTime() noexcept;

		constexpr auto operator <=> (CTime const &_Other) const noexcept = default;
		constexpr CTime &operator += (CTimeSpan const &_Other) noexcept;
		constexpr CTime &operator -= (CTimeSpan const &_Other) noexcept;
		constexpr CTimeSpan operator - (CTime const &_Other) const noexcept;
		constexpr CTime operator - (CTimeSpan const &_Other) const noexcept;
		constexpr CTime operator + (CTimeSpan const &_Other) const noexcept;
		constexpr CTime &operator ++ () noexcept;
		constexpr CTime &operator -- () noexcept;

		constexpr bool f_IsValid() const noexcept;
		constexpr uint64 f_GetSeconds() const noexcept;
		constexpr void f_SetSeconds(uint64 _Seconds) noexcept;
		constexpr void f_SetSecondsNoFraction(uint64 _Seconds) noexcept;
		constexpr uint64 f_GetFractionInt() const noexcept;
		constexpr void f_SetFractionInt(uint64 _Fraction) noexcept;
		constexpr fp64 f_GetFraction() const noexcept;
		constexpr void f_SetFraction(fp64 _Fraction) noexcept;
		constexpr fp64 f_GetSecondsFraction() const noexcept;

		constexpr static CTime fs_EndOfTime() noexcept;
		constexpr static CTime fs_StartOfTime() noexcept;
		constexpr static CTime fs_Create(uint64 _Seconds, uint64 _Fraction = 0) noexcept;

		template <typename tf_COption, typename tf_CArgs>
		constexpr bool f_FormatParseOption(CFormatOptions &_Options, tf_COption &_Option, tf_CArgs &_Args) const noexcept;

		template <typename tf_CStr, typename tf_COptions>
		constexpr void f_Format(tf_CStr &_FormatInto, tf_COptions const &_Options) const noexcept;

		CTime f_ToUTC() const;
		CTime f_ToLocal() const;
		CTime f_ToUtcLegacy() const;
		CTime f_ToLocalLegacy() const;

		static CTime fs_NowLocal();
		static CTime fs_NowUTC();
		static int64 fs_GetResolution();

	private:
		friend struct CTimeConvert_BabylonianCommon;
		friend struct CTimeConvert_ProlepticGreogrian;

		uint64 mp_Seconds = NPrivate::CConst::mc_InvalidTimeSeconds; // Number of seconds since -7'514'938'706-11-23 (Gregorian Proleptic).
		uint64 mp_Fraction = 0;

		constexpr CTime(uint64 _Seconds, uint64 _Fraction) noexcept;
#ifdef DMibDebuggerHelpers
		static ch8 const* fsp_DebugStr(void *_pTime);
#endif
	};

	template <typename tf_CType>
	concept cIsTime = NPrivate::TCIsTime<NTraits::TCRemoveReferenceAndQualifiers<tf_CType>>::mc_bValue;

	NMib::NStr::CStr fg_GetFullTimeStr(CTime const &_Time);
	bool fg_ParseFullTimeStr(CTime &_Time, NMib::NStr::CStr const& _Str);

	// Parses a flexible date/time string in format: YYYY-MM-DD [HH:MM[:SS[.FFF]]]
	// Returns CTime on success, or error on failure
	NMib::NConcurrency::TCAsyncResult<CTime> fg_TryParseDateTimeStr(NMib::NStr::CStr const &_Str);

	NMib::NStr::CStr fg_GetAscTimeStr(CTime const &_Time); // Same format as std. C lib's asctime. NOT LOCALISED (deliberate)
	int32 fg_GetAscMonthNumber(NStr::CStr const &_Month); // Same format as std. C lib's asctime. NOT LOCALISED (deliberate)
	NMib::NStr::CStr fg_GetISO8601TimeStr(CTime const &_Time);
}

#include "Malterlib_Time_TimeUnits.h"
#include "Malterlib_Time_Time.hpp"

#include "Malterlib_Time_Time_Convert.h"
#include "Malterlib_Time_Time_Utils.hpp"
#include "Malterlib_Time_Time_Format.hpp"
#include "Malterlib_Time_Time_Stream.hpp"
#include "Malterlib_Time_Time_Integration.hpp"
