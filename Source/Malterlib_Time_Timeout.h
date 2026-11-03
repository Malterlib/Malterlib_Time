// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NTime
{
	struct CTimeout
	{
		CTimeout(fp64 _nSeconds)
			: mp_Timeout(_nSeconds)
		{
			f_Start();
		}

		CTimeout(fp64 _nSeconds, bool _bStart)
			: mp_Timeout(_nSeconds)
		{
			if (_bStart)
				f_Start();
		}

		void f_Start()
		{
			mp_Stopwatch.f_Start();
		}

		bool f_TimedOut() const
		{
			return mp_Stopwatch.f_GetTime() >= mp_Timeout;
		}

		bool operator ! () const
		{
			return !f_TimedOut();
		}

	protected:
		CStopwatch mp_Stopwatch;
		fp64 mp_Timeout;
	};
}
