// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>

namespace NMib::NTime
{
	CCyclesMin::CCorrection::CCorrection(bool _bSetCorrection)
	{
		if (_bSetCorrection)
		{
			CCyclesMin Timer;
			for (mint i = 0; i < 10; ++i)
			{
				Timer.f_Start();
				Timer.f_StopNonCorrected();
			}
			m_Correction = Timer.m_MinCycles.f_ToInt();
		}
		else
			m_Correction = 0;
	}

	CCyclesMin::CCorrection CCyclesMin::ms_Correction2(0);
	CCyclesMin::CCorrection CCyclesMin::ms_Correction(1);


	CTimerTraceScope::CTimerTraceScope(const ch8 *_pName)
	{
		m_pName = _pName;

		m_Timer.f_Start();
	}

	CTimerTraceScope::~CTimerTraceScope()
	{
		m_Timer.f_Stop();

		DMibTraceSafe("Timer {}: {} s" DMibNewLine, m_pName, m_Timer.f_GetTime());
	}

	CTimerConOutScope::CTimerConOutScope(const ch8 *_pName)
	{
		m_pName = _pName;

		m_Timer.f_Start();
	}

	CTimerConOutScope::~CTimerConOutScope()
	{
		m_Timer.f_Stop();

		DMibConErrOut("Timer {}: {} s" DMibNewLine, m_pName, m_Timer.f_GetTime());
	}
}
