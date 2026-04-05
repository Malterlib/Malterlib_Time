// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

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
