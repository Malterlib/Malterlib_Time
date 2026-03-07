// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NTime
{
	template <typename tf_CStr>
	constexpr void CTimeSpan::f_Format(tf_CStr &o_FormatInto) const noexcept
	{
		if (!f_IsValid())
		{
			o_FormatInto += "INVALID";
			return;
		}

		fg_FormatSecondsDurationToHumanReadable(o_FormatInto, f_GetSecondsFraction());
	}
}
