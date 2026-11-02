// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>

namespace NMib::NStr
{
	template struct TCStrFormatType_Inline<TCFormat<CStrTraits_CStr>, NTime::CTime, true>;
	template struct TCStrFormatType_Inline<TCFormat<CStrTraits_CStrNonTracked>, NTime::CTime, true>;
}
