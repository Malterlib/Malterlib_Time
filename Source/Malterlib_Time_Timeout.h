// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NTime
{
	struct CTimeout
	{
		CTimeout(fp64 _nSeconds)
			: m_Timeout(_nSeconds)
		{
			f_Start();
		}

		CTimeout(fp64 _nSeconds, bool _bStart)
			: m_Timeout(_nSeconds)
		{
			if (_bStart)
				f_Start();
		}

		void f_Start()
		{
			m_Stopwatch.f_Start();
		}

		bool f_TimedOut() const
		{
			return m_Stopwatch.f_GetTime() >= m_Timeout;
		}

		bool operator!() const
		{
			return !f_TimedOut();
		}

	protected:
		CStopwatch m_Stopwatch;
		fp64 m_Timeout;
	};
}
