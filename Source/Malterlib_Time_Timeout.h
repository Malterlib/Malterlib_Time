// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NTime
{
	struct CTimeout
	{
		CTimeout(fp64 _nSeconds);
		CTimeout(fp64 _nSeconds, bool _bStart);
		void f_Start();
		bool f_TimedOut() const;
		bool operator ! () const;

	protected:
		CStopwatch mp_Stopwatch;
		fp64 mp_Timeout;
	};
}
