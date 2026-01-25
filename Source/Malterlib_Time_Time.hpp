// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NTime
{
	template <typename tf_CStr>
	void fg_FormatSecondsDurationToHumanReadable(tf_CStr &o_FormatInto, fp64 const &_Seconds)
	{
		using namespace NStr;

		constexpr auto c_AverageYear = 365.2425_days;
		constexpr auto c_AverageMonth = 30.436875_days;

		if (_Seconds.f_IsNan())
			return;
		else if (_Seconds == fp64::fs_Inf())
		{
			o_FormatInto += "Infinity";
			return;
		}
		else if (_Seconds == fp64::fs_NegInf())
		{
			o_FormatInto += "Negative Infinity";
			return;
		}
		else if (_Seconds == fp64::fs_0())
		{
			o_FormatInto += "0 s";
			return;
		}

		if (_Seconds > c_AverageYear)
		{
			fp64 Years = (_Seconds / c_AverageYear);
			auto Months = (Years * 12.0 - Years.f_Floor() * 12.0).f_ToUnsignedInt();
			o_FormatInto += typename tf_CStr::CFormat("{}y {}m") << Years.f_ToUnsignedInt() << Months;
		}
		else if (_Seconds > c_AverageMonth)
		{
			constexpr auto c_AverageDaysInMonth = 30.436875;
			fp64 Months = (_Seconds / c_AverageMonth);
			auto Days = (Months * c_AverageDaysInMonth - Months.f_Floor() * c_AverageDaysInMonth).f_ToUnsignedInt();
			o_FormatInto += typename tf_CStr::CFormat("{}m {}d") << Months.f_ToUnsignedInt() << Days;
		}
		else if (_Seconds > 1_weeks)
		{
			fp64 Weeks = (_Seconds / 1_weeks);
			auto Days = (Weeks * 7.0 - Weeks.f_Floor() * 7.0).f_ToUnsignedInt();
			o_FormatInto += typename tf_CStr::CFormat("{}w {}d") << Weeks.f_ToUnsignedInt() << Days;
		}
		else if (_Seconds > 1_days)
		{
			fp64 Days = (_Seconds / 1_days);
			auto Hours = (Days * 24.0 - Days.f_Floor() * 24.0).f_ToUnsignedInt();
			o_FormatInto += typename tf_CStr::CFormat("{}d {}h") << Days.f_ToUnsignedInt() << Hours;
		}
		else if (_Seconds > 1_hours)
		{
			fp64 Hours = (_Seconds / 1_hours);
			auto Minutes = (Hours * 60.0 - Hours.f_Floor() * 60.0).f_ToUnsignedInt();
			o_FormatInto += typename tf_CStr::CFormat("{}h {}m") << Hours.f_ToUnsignedInt() << Minutes;
		}
		else if (_Seconds > 1_minutes)
		{
			fp64 Minutes = (_Seconds / 1_minutes);
			auto Seconds = (Minutes * 60.0 - Minutes.f_Floor() * 60.0).f_ToUnsignedInt();
			o_FormatInto += typename tf_CStr::CFormat("{}m {}s") << Minutes.f_ToUnsignedInt() << Seconds;
		}
		else if (_Seconds >= 9.95_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe0} s") << _Seconds;
		else if (_Seconds > 1_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe1} s") << _Seconds;
		else if (_Seconds >= 9.95e-3_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe0} ms") << (_Seconds * 1e3);
		else if (_Seconds > 1e-3_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe1} ms") << (_Seconds * 1e3);
		else if (_Seconds >= 9.95e-6_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe0} µs") << (_Seconds * 1e6);
		else if (_Seconds > 1e-6_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe1} µs") << (_Seconds * 1e6);
		else if (_Seconds >= 9.95e-9_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe0} ns") << (_Seconds * 1e9);
		else if (_Seconds > 1e-9_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe1} ns") << (_Seconds * 1e9);
		else if (_Seconds >= 9.95e-12_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe0} ps") << (_Seconds * 1e12);
		else if (_Seconds > 1e-12_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe1} ps") << (_Seconds * 1e12);
		else if (_Seconds >= 9.95e-15_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe0} fs") << (_Seconds * 1e15);
		else if (_Seconds > 1e-15_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe1} fs") << (_Seconds * 1e15);
		else if (_Seconds >= 9.95e-18_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe0} as") << (_Seconds * 1e18);
		else if (_Seconds > 1e-18_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe1} as") << (_Seconds * 1e18);
		else if (_Seconds >= 9.95e-21_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe0} zs") << (_Seconds * 1e21);
		else if (_Seconds > 1e-21_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe1} zs") << (_Seconds * 1e21);
		else if (_Seconds >= 9.95e-24_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe0} ys") << (_Seconds * 1e24);
		else if (_Seconds > 1e-24_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe1} ys") << (_Seconds * 1e24);
		else if (_Seconds >= 9.95e-27_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe0} rs") << (_Seconds * 1e27);
		else if (_Seconds > 1e-27_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe1} rs") << (_Seconds * 1e27);
		else if (_Seconds >= 9.95e-30_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe0} qs") << (_Seconds * 1e30);
		else if (_Seconds > 1e-30_seconds)
			o_FormatInto += typename tf_CStr::CFormat("{fe1} qs") << (_Seconds * 1e30);
		else
			o_FormatInto += typename tf_CStr::CFormat("{ffe,fd3,fa0} tP") << (_Seconds * 10e44);
	}
}
