// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NStr
{
	extern template struct TCStrFormatType_Inline<TCFormat<CStrTraits_CStr>, NTime::CTime, true>;
	extern template struct TCStrFormatType_Inline<TCFormat<CStrTraits_CStrNonTracked>, NTime::CTime, true>;
}
