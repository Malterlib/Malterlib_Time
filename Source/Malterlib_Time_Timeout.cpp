// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Mib/Core/Core>
#include <Mib/Time/Timeout>

namespace NMib::NTime
{
	CTimeout::CTimeout(fp64 _nSeconds)
		: mp_Timeout(_nSeconds)
	{
		f_Start();
	}

	CTimeout::CTimeout(fp64 _nSeconds, bool _bStart)
		: mp_Timeout(_nSeconds)
	{
		if (_bStart)
			f_Start();
	}

	void CTimeout::f_Start()
	{
		mp_Stopwatch.f_Start();
	}

	bool CTimeout::f_TimedOut() const
	{
		return mp_Stopwatch.f_GetTime() >= mp_Timeout;
	}

	bool CTimeout::operator ! () const
	{
		return !f_TimedOut();
	}
}
