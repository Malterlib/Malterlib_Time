// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include "Malterlib_Time_System.h"
#include "Malterlib_Time_Platform.h"

namespace NMib::NTime
{
	class CTimer
	{
	public:
		int64 m_Time;

		CTimer(int64 _Time)
			: m_Time(_Time)
		{
		}
		CTimer()
			: m_Time(0)
		{
		}

		inline_small void f_Start()
		{
			m_Time = -CSystem_Time::fs_GetTimerValue();

		}

		inline_small CTimer f_Elapsed() const
		{
			return CTimer(m_Time + CSystem_Time::fs_GetTimerValue());
		}

		inline_small void f_Stop()
		{
			m_Time += CSystem_Time::fs_GetTimerValue();
		}

		// In seconds
		operator fp64()
		{
			return fp64(m_Time) / CSystem_Time::fs_TimerFrequencyFp();
		}
		fp64 f_GetTime()
		{
			return (fp64(m_Time)) / CSystem_Time::fs_TimerFrequencyFp();
		}

		bool operator < (CTimer const &_Other) const
		{
			return m_Time < _Other.m_Time;
		}

		bool operator == (CTimer const &_Other) const
		{
			return m_Time == _Other.m_Time;
		}

		CTimer &operator -= (CTimer const &_Other)
		{
			m_Time -= _Other.m_Time;
			return *this;
		}
		CTimer &operator += (CTimer const &_Other)
		{
			m_Time += _Other.m_Time;
			return *this;
		}
		CTimer &operator *= (CTimer const &_Other)
		{
			m_Time *= _Other.m_Time;
			return *this;
		}
		CTimer operator - (CTimer const &_Other)
		{
			CTimer Temp = *this;
			Temp -= _Other;
			return Temp;
		}
		CTimer operator + (CTimer const &_Other)
		{
			CTimer Temp = *this;
			Temp += _Other;
			return Temp;
		}
	};

	template <bool t_bDoCorrection>
	class TCCycles
	{
	public:
		int64 m_Time;

		TCCycles(int64 _Time)
		{
			m_Time = _Time;
		}
		TCCycles()
		{
			m_Time = 0;
		}

		void f_Reset()
		{
			m_Time = 0;
		}

		inline_small void f_Start()
		{
			m_Time = -NPlatform::fg_Timer_Cycles();
		}
		inline_small void f_StartAccumulate()
		{
			m_Time -= NPlatform::fg_Timer_Cycles();
		}

		inline_small void f_Stop()
		{
			if (t_bDoCorrection)
				m_Time += NPlatform::fg_Timer_Cycles() - ms_CorrectionStart.m_Correction;
			else
				m_Time += NPlatform::fg_Timer_Cycles();
		}

		inline_small void f_StopAccumulate()
		{
			if (t_bDoCorrection)
				m_Time += NPlatform::fg_Timer_Cycles() - ms_CorrectionStartAccumulate.m_Correction;
			else
				m_Time += NPlatform::fg_Timer_Cycles();

		}

		inline_small void f_StopNonCorrected()
		{
			if (t_bDoCorrection)
				m_Time += NPlatform::fg_Timer_Cycles() - ms_CorrectionNonCorrected.m_Correction;
			else
				m_Time += NPlatform::fg_Timer_Cycles();
		}

		int64 f_GetCycles()
		{
			return m_Time;
		}


		class CCorrection
		{
		public:
			int64 m_Correction;
			CCorrection(aint _CorrectionType);
		};

		static CCorrection ms_CorrectionStart;
		static CCorrection ms_CorrectionStartAccumulate;
		static CCorrection ms_CorrectionNonCorrected;

		bool operator < (TCCycles const &_Other) const
		{
			return m_Time < _Other.m_Time;
		}

		bool operator == (TCCycles const &_Other) const
		{
			return m_Time == _Other.m_Time;
		}

		TCCycles &operator -= (TCCycles const &_Other)
		{
			m_Time -= _Other.m_Time;
			return *this;
		}
		TCCycles &operator += (TCCycles const &_Other)
		{
			m_Time += _Other.m_Time;
			return *this;
		}
		TCCycles &operator *= (TCCycles const &_Other)
		{
			m_Time *= _Other.m_Time;
			return *this;
		}
		TCCycles operator - (TCCycles const &_Other)
		{
			TCCycles Temp = *this;
			Temp -= _Other;
			return Temp;
		}
		TCCycles operator + (TCCycles const &_Other)
		{
			TCCycles Temp = *this;
			Temp += _Other;
			return Temp;
		}
	};

	typedef TCCycles<true> CCycles;


	class CTimerAbsolute
	{
	public:
		int64 m_Time;

		inline_small void f_Sample()
		{
			m_Time = CSystem_Time::fs_GetTimerValue();
		}

		// In seconds
		operator fp64()
		{
			return fp64(m_Time) / CSystem_Time::fs_TimerFrequencyFp();
		}
		fp64 f_GetTime()
		{
			return (fp64(m_Time)) / CSystem_Time::fs_TimerFrequencyFp();
		}

		static fp64 fs_GetTime()
		{
			return (fp64(CSystem_Time::fs_GetTimerValue())) / CSystem_Time::fs_TimerFrequencyFp();
		}
	};

	class CClockAggregate
	{
	public:
		int64 m_StartTime;

		void f_Construct()
		{
		}
		void f_Construct(bool _bStart)
		{
			if (_bStart)
				f_Start();
		}
		void f_Destruct()
		{
		}

		inline_small void f_Start()
		{
			m_StartTime = CSystem_Time::fs_GetTimerValue();
		}

		// In seconds
		operator fp64() const
		{
			return f_GetTime();
		}
		fp64 f_GetTime() const
		{
			return (fp64(CSystem_Time::fs_GetTimerValue() - m_StartTime)) / CSystem_Time::fs_TimerFrequencyFp();
		}
	};

	class CClock : public CClockAggregate
	{
	public:
		//int64 m_StartTime;	// TODO: Should this be here? I'm not sure of it's purpose other than to catch unsuspecting coders out :-) I have removed it for now.

		CClock()
		{
			f_Construct();
		}
		CClock(bool _bStart)
		{
			f_Construct(_bStart);
		}

		CClock(CClock const& _ToMove)
		{
			m_StartTime = _ToMove.m_StartTime;
		}

	};

	class CClockRawAggregate
	{
	public:
		int64 m_StartTime;

		void f_Construct()
		{
		}
		void f_Construct(bool _bStart)
		{
			if (_bStart)
				f_Start();
		}
		void f_Destruct()
		{
		}

		inline_small void f_Start()
		{
			m_StartTime = NPlatform::fg_TimerRaw_PreciseGet();
		}

		// In seconds
		operator fp64() const
		{
			return f_GetTime();
		}
		fp64 f_GetTime() const
		{
			return (fp64(NPlatform::fg_TimerRaw_PreciseGet() - m_StartTime)) / fp64(NPlatform::fg_TimerRaw_PreciseFrequency());
		}
	};

	class CClockRaw : public CClockRawAggregate
	{
	public:
		//int64 m_StartTime;	// TODO: Should this be here? I'm not sure of it's purpose other than to catch unsuspecting coders out :-) I have removed it for now.

		CClockRaw()
		{
			f_Construct();
		}
		CClockRaw(bool _bStart)
		{
			f_Construct(_bStart);
		}

		CClockRaw(CClock const& _ToMove)
		{
			m_StartTime = _ToMove.m_StartTime;
		}

	};

	class CCyclesClockAggregate
	{
	public:
		int64 m_StartTime;

		void f_Construct()
		{
		}
		void f_Construct(bool _bStart)
		{
			if (_bStart)
				f_Start();
		}
		void f_Destruct()
		{
		}

		inline_small void f_Start()
		{
			m_StartTime = NPlatform::fg_Timer_CyclesFast();
		}

		int64 f_GetCycles() const
		{
			return NPlatform::fg_Timer_CyclesFast() - m_StartTime;
		}

		// In seconds
		operator fp64() const
		{
			return f_GetTime();
		}
		fp64 f_GetTime() const
		{
			return (fp64(NPlatform::fg_Timer_CyclesFast() - m_StartTime)) * CSystem_Time::fs_CyclesFrequencyReciprocal();
		}
	};

	class CCyclesClock : public CCyclesClockAggregate
	{
	public:
		//int64 m_StartTime;	// TODO: Should this be here? I'm not sure of it's purpose other than to catch unsuspecting coders out :-) I have removed it for now.

		CCyclesClock()
		{
			f_Construct();
		}
		CCyclesClock(bool _bStart)
		{
			f_Construct(_bStart);
		}

		CCyclesClock(CCyclesClock const& _ToMove)
		{
			m_StartTime = _ToMove.m_StartTime;
		}

	};


	class CTimer2
	{
	public:
		int64 m_Time;
		int64 m_LastTime;

		inline_small void f_Start(bool _bAccumulate = false)
		{
			if (_bAccumulate)
				m_Time -= CSystem_Time::fs_GetTimerValue();
			else
				m_Time = -CSystem_Time::fs_GetTimerValue();
		}

		inline_small void f_Stop()
		{
			m_Time += CSystem_Time::fs_GetTimerValue();
			m_LastTime = m_Time;
		}

		inline_small void f_Delta(bool _bAccumulate = false)
		{
			if (_bAccumulate)
			{
				int64 Time = CSystem_Time::fs_GetTimerValue();
				m_Time += Time;
				m_LastTime = m_Time;
				m_Time -= Time;
			}
			else
			{
				int64 Time = CSystem_Time::fs_GetTimerValue();
				m_LastTime = m_Time + Time;
				m_Time = -Time;
			}
		}

		// In seconds
		operator fp64()
		{
			return fp64(m_LastTime) / CSystem_Time::fs_TimerFrequencyFp();
		}
		fp64 f_GetTime()
		{
			return (fp64(m_LastTime)) / CSystem_Time::fs_TimerFrequencyFp();
		}
	};

	template <typename t_CTimer>
	class TCMeasureScope
	{
	public:
		t_CTimer *m_pTimer;

		TCMeasureScope(t_CTimer &_Timer)
		{
			m_pTimer = &_Timer;
			m_pTimer->f_Start();
		}

		~TCMeasureScope()
		{
			m_pTimer->f_Stop();
		}

	};

#	define DMibScopeTimer(_Timer) NMib::NTime::TCMeasureScope<decltype(_Timer)> TimerScope(_Timer)
#	define DMibScopeMeasure(_Measure) NMib::NTime::TCMeasureScope<decltype(_Measure)> TimerScope(_Measure)

#	ifndef DMibPNoShortCuts
#		define DScopeTimer(_Timer) DMibScopeTimer(_Timer)
#		define DScopeMeasure(_Timer) DMibScopeMeasure(_Timer)
#	endif

	class CTimerTraceScope
	{
	public:
		CTimer m_Timer;

		const ch8 *m_pName;

		CTimerTraceScope(const ch8 *_pName);

		~CTimerTraceScope();

	};

#if defined(DMibDebug) || defined(DConfig_Profile)
#	define DMibScopeTraceTimer(_Name) NMib::NTime::CTimerTraceScope TimerScope(_Name)
#else
#	define DMibScopeTraceTimer(_Name)
#endif

#	ifndef DMibPNoShortCuts
#		define DScopeTraceTimer(_Name) DMibScopeTraceTimer(_Name)
#	endif

	class CTimerConOutScope
	{
	public:
		CTimer m_Timer;

		const ch8 *m_pName;

		CTimerConOutScope(const ch8 *_pName);

		~CTimerConOutScope();

	};

#if defined(DMibDebug) || defined(DConfig_Profile)
#	define DMibScopeConOutTimer(_Name) NMib::NTime::CTimerConOutScope TimerScope(_Name)
#else
#	define DMibScopeConOutTimer(_Name)
#endif

#	ifndef DMibPNoShortCuts
#		define DScopeConOutTimer(_Name) DMibScopeConOutTimer(_Name)
#	endif

	class CTimerMin
	{
		static int64 const &fsp_GetMinCycles();
	public:
		CTimerMin()
		{
			f_Reset();
		}

		void f_Reset()
		{
			m_MinTime = 0x7fffffffffffffffll;
			m_MinCycles = 0x7fffffffffffffffll;
		}

		int64 m_MinTime;
		int64 m_MinCycles;
		int64 m_CurrentTime;
		int64 m_CurrentCycles;

		inline_small void f_Start()
		{
			m_CurrentTime = -CSystem_Time::fs_GetTimerValue();
			m_CurrentCycles = -NPlatform::fg_Timer_Cycles();
		}

		inline_medium void f_Stop()
		{
			m_CurrentTime += CSystem_Time::fs_GetTimerValue();
			m_CurrentCycles += NPlatform::fg_Timer_Cycles();
			if (m_CurrentTime < m_MinTime)
			{
				m_MinTime = m_CurrentTime;
			}
			if (m_CurrentCycles < m_MinCycles)
			{
				m_MinCycles = m_CurrentCycles;
			}
		}

		void f_FeedTimer(const CTimer &_Timer)
		{
			if (_Timer.m_Time < m_MinTime)
			{
				m_MinTime = _Timer.m_Time;
			}
		}
		// In seconds
		operator fp64() const
		{
			return ((fp64)m_MinTime) / CSystem_Time::fs_TimerFrequencyFp();
		}
		fp64 f_GetTime() const
		{
			return ((fp64)m_MinTime) / CSystem_Time::fs_TimerFrequencyFp();
		}
		int64 f_GetTicks() const
		{
			return m_MinTime;
		}
		int64 f_GetCycles() const
		{
			return m_MinCycles;
		}
		int64 f_GetNanos() const
		{
			return ((((fp64)m_MinTime) / CSystem_Time::fs_TimerFrequencyFp()) * 1000000000.0).f_ToIntRound();
		}
		int64 f_GetMicros() const
		{
			return ((((fp64)m_MinTime) / CSystem_Time::fs_TimerFrequencyFp()) * 1000000.0).f_ToIntRound();
		}
		int64 f_GetMillis() const
		{
			return ((((fp64)m_MinTime) / CSystem_Time::fs_TimerFrequencyFp()) * 1000.0).f_ToIntRound();
		}
		fp64 operator / ( const CTimerMin &_Other) const
		{
			return (fp64)m_MinCycles / (fp64)_Other.m_MinCycles;
		}

		CTimerMin operator * ( const fp64 &_Other) const
		{
			CTimerMin Ret;
			Ret.m_CurrentCycles = m_CurrentCycles;
			Ret.m_CurrentTime = m_CurrentTime;
			Ret.m_MinCycles = (fp64(m_MinCycles) * _Other).f_ToIntRound();
			Ret.m_MinTime = (fp64(m_MinTime) * _Other).f_ToIntRound();

			return Ret;
		}

		template <typename t_CFormatter>
		int f_GetStringFormatType(t_CFormatter &_Formatter);

		template <typename t_CFormatter>
		auto f_CreateStringFormatter(t_CFormatter &_Formatter) const -> decltype(NStr::fg_CreateStringFormatter(_Formatter, fsp_GetMinCycles()))
		{
			return NStr::fg_CreateStringFormatter(_Formatter, m_MinCycles);
		}

	};

	class CCyclesMin
	{
		static fp64 const &fsp_GetMinCycles();
	public:
		CCyclesMin()
		{
			f_Reset();
		}

		void f_Reset()
		{
			m_MinCycles = 1e94;
		}

		fp64 m_MinCycles;
		int64 m_CurrentCycles;


		only_parameters_aliased inline_never void f_Start()
		{
			int64 Cycles = NPlatform::fg_Timer_Cycles();
			NMib::NAtomic::fg_MemoryFence();
			m_CurrentCycles = -Cycles;
		}

		only_parameters_aliased inline_never void f_Stop()
		{
			int64 Cycles = NPlatform::fg_Timer_Cycles();
			NMib::NAtomic::fg_MemoryFence();
			int64 Corrected = (m_CurrentCycles + Cycles) - ms_Correction.m_Correction;
			if (Corrected < m_MinCycles)
			{
				m_MinCycles = Corrected;
			}
		}

		only_parameters_aliased void f_FeedCycles(const CCycles &_Timer)
		{
			if (_Timer.m_Time < m_MinCycles)
			{
				m_MinCycles = _Timer.m_Time;
			}
		}

		only_parameters_aliased inline_never void f_StopNonCorrected()
		{
			int64 Cycles = NPlatform::fg_Timer_Cycles();
			NMib::NAtomic::fg_MemoryFence();
			int64 Corrected = (m_CurrentCycles + Cycles) - ms_Correction2.m_Correction;
			if (Corrected < m_MinCycles)
			{
				m_MinCycles = Corrected;
			}
		}

		class CCorrection
		{
		public:
			int64 m_Correction;
			CCorrection(bool _bSetCorrection);
		};

		static CCorrection ms_Correction;
		static CCorrection ms_Correction2;

		// In seconds
		only_parameters_aliased fp64 f_GetCycles() const
		{
			return m_MinCycles;
		}

		only_parameters_aliased fp32 operator / ( const CCyclesMin &_Other) const
		{
			return fp64(m_MinCycles) / fp64(_Other.m_MinCycles);
		}

		only_parameters_aliased CCyclesMin &operator /= ( const fp64 &_Other)
		{
			m_MinCycles = fp64(((fp64(m_MinCycles) / _Other) * 100.0).f_ToIntRound()) / 100.0;
			return *this;
		}

		only_parameters_aliased CCyclesMin operator * ( const fp64 &_Other) const
		{
			CCyclesMin Ret;
			Ret.m_CurrentCycles = m_CurrentCycles;
			Ret.m_MinCycles = fp64(((fp64(m_MinCycles) * _Other) * 100.0).f_ToIntRound()) / 100.0;

			return Ret;
		}

		template <typename t_CFormatter>
		int f_GetStringFormatType(t_CFormatter &_Formatter);

		template <typename t_CFormatter>
		auto f_CreateStringFormatter(t_CFormatter &_Formatter) const -> decltype(NMib::NStr::fg_CreateStringFormatter(_Formatter, fsp_GetMinCycles()))
		{
			return NMib::NStr::fg_CreateStringFormatter(_Formatter, m_MinCycles);
		}

	};

	class CTimerMinScope
	{
	public:
		CTimerMin *m_pTimer;

		CTimerMinScope(CTimerMin &_Timer)
		{
			m_pTimer = &_Timer;

			m_pTimer->f_Start();
		}

		~CTimerMinScope()
		{
			m_pTimer->f_Stop();
		}

	};
#	define DMibScopeTimerMin(_Timer) NMib::NTime::CTimerMinScope CTimerMinScope(_Timer)

#	ifndef DMibPNoShortCuts
#		define DScopeTimerMin(_Timer) DMibScopeTimerMin(_Timer)
#	endif

	template <bool t_bDoCorrection>
	TCCycles<t_bDoCorrection>::CCorrection::CCorrection(aint _CorrectionType)
	{
		if (_CorrectionType)
		{
			if (_CorrectionType == 1)
			{
				CCycles Timer;
				CCyclesMin TimerMin;
				for (mint i = 0; i < 10; ++i)
				{
					for (mint i = 0; i < 2; ++i)
					{
						Timer.f_Start();
						Timer.f_StopNonCorrected();
					}
					TimerMin.f_FeedCycles(Timer);
				}
				m_Correction = TimerMin.f_GetCycles().f_ToIntRound();
			}
			else
			{
				CCyclesMin TimerMin;
				for (mint i = 0; i < 10; ++i)
				{
					CCycles Timer;
					for (mint i = 0; i < 10; ++i)
					{
						Timer.f_StartAccumulate();
						Timer.f_StopNonCorrected();
					}
					TimerMin.f_FeedCycles(Timer);
				}
				TimerMin /= 10;
				m_Correction = TimerMin.f_GetCycles().f_ToIntRound();
			}
		}
		else
			m_Correction = 0;
	}

	template <bool t_bDoCorrection>
	typename TCCycles<t_bDoCorrection>::CCorrection TCCycles<t_bDoCorrection>::ms_CorrectionNonCorrected(0);
	template <bool t_bDoCorrection>
	typename TCCycles<t_bDoCorrection>::CCorrection TCCycles<t_bDoCorrection>::ms_CorrectionStart(1);
	template <bool t_bDoCorrection>
	typename TCCycles<t_bDoCorrection>::CCorrection TCCycles<t_bDoCorrection>::ms_CorrectionStartAccumulate(2);
}
