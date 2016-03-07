// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#include <Mib/Core/Core>
#include "Malterlib_Time_System.h"

namespace NMib
{
	namespace NTime
	{
		namespace
		{
			struct CSubSystem_Time : public CSubSystem
			{
		#ifdef DMibSafeTimerAvailable
				struct CThreadLocal
				{
					CThreadLocal();
					~CThreadLocal();
					CThreadLocal(CThreadLocal const &_Other);

					int64 m_LastSafeTimer;
					int64 m_LastTimer;

#if DMibConfig_Tests_Enable
					DMibListLinkDS_Link(CThreadLocal, m_Link);
#endif
				};

#if DMibConfig_Tests_Enable
				NThread::CMutual m_ThreadLocalsLock;
				DMibListLinkDS_List(CThreadLocal, m_Link) m_ThreadLocals;
#endif
				mutable NAggregate::TCAggregateSimple<NThread::TCThreadLocal<CThreadLocal, NMem::CAllocator_NonTrackedHeap, NThread::EThreadLocalFlag_Inherit>> m_ThreadLocal;
		#endif
				NAtomic::TCAtomic<bool> m_bThreadLocalAvailable;
				NAtomic::TCAtomic<bool> m_bThreadLocalCreated;

				fp64 m_CyclesFrequencyFp;
				uint64 m_CyclesFrequency;
				fp64 m_CyclesFrequencyReciprocal;

				//

				NTime::CTime m_TimeBase;
				NTime::CTimeSpan m_UTCOffset;
				int64 m_TimerBase;
				int64 m_Timer;
				int64 m_TimerFrequency;
				fp64 m_TimerFrequencyReciprocal;
				fp64 m_TimerFrequencyFp;
				int64 m_NextUpdate;
				int64 m_NextUTCUpdate;
				fp64 m_Drift; // Timer ticks per tick
				fp64 m_DriftFraction;

				int64 m_ErrorMargin;
				int64 m_ErrorMarginReset;
				int64 m_LastDiff;
				int64 m_LastTimer;

				mutable int64 m_LastInternalTimer;
				mutable int64 m_InternalTimer;
				mutable fp64 m_InternalTimerExtra;

				fp64 m_TimeSpeed;
				fp64 m_TimeSpeedReciprocal;

				bint m_bTimeInitDone;
				bint m_bUsedTimeSpeed;
				mutable NThread::CMutual m_TimerValLock;

				mutable NThread::CMutual m_Lock;

		#ifdef DMibSafeTimerAvailable
				mutable NThread::CMutual m_LockSafeTimer;
				mutable bint m_bUseSafeTimer;
				mutable int64 m_SafeTimerVal;
				mutable uint64 m_SafeTimerLast;
				fp64 m_SafeTimerFrequencyReciprocal;
				fp64 m_SafeTimerScale;
		#endif
				
				NThread::CMutual m_TimeChangeNotificationLock;
				NContainer::TCLinkedList<NFunction::TCFunction<void (NTime::CTime const &_OldTime, NTime::CTime const &_NewTime, NStr::CStr const &_Reason)>> m_TimeChangeNotifications;

				int64 fp_GetTimerFreq() const;
				int64 fp_GetTimerValInternal() const;
				void fp_TimeInit();
				void fp_TimerUpdate(int64 _CurrentTimer, NTime::CTime &_Time, bint _bRecursive);

				void f_TimeGetUTCOffset(NTime::CTimeSpan *_pUTCOffset) const;
				void f_TimeGetNow(NTime::CTime *_pTime, bint _bRecursive = false) const;
				void f_SetTimeSpeed(fp64 _Multiplier, NTime::CTime const *_pOptionalTime, NTime::CTimeSpan const *_pTimeZone);
				void f_DisableTimeSpeed();
				fp64 f_GetTimeSpeed() const;
				fp64 f_GetTimeSpeedReciprocal() const;
				bool f_GetTimeSimulating() const;
				void f_MeasureCycleFrequency();
				int64 f_GetTimerVal() const;
		#ifdef DMibSafeTimerAvailable
				void f_EnableSafeTimer(NStr::CStr const &_ErrorMessage);
#if DMibConfig_Tests_Enable
				void f_MakeSafeTimerWrap(fp64 _InSeconds, uint32 _Where);
#endif
				bool f_IsSafeTimerEnabled() const;
		#endif
				void f_DestroyThreadLocal() override;

				void f_ReportTimeChange(NTime::CTime const &_OldTime, NTime::CTime const &_NewTime, NStr::CStr const &_Reason);
				
				CSubSystem_Time();
				~CSubSystem_Time();
			};
			
			TCSubSystem<CSubSystem_Time, ESubSystemDestruction_Last> g_MalterlibSubSystem_Time = {DAggregateInit};
			
			CSubSystem_Time::CSubSystem_Time()
			{
				m_bTimeInitDone = false;
		#ifdef DMibSafeTimerAvailable
				m_bUseSafeTimer = false;
		#endif
				m_TimeSpeed = 1.0;
				m_TimeSpeedReciprocal = 1.0;
				m_bUsedTimeSpeed = false;
				m_TimerFrequency = 0;
				m_LastInternalTimer = -1;
				m_InternalTimer = 0;
				m_LastTimer = 0;
				m_bThreadLocalAvailable = false;

				f_MeasureCycleFrequency();
				
				fp_TimeInit();
			}

			CSubSystem_Time::~CSubSystem_Time()
			{
		#ifdef DMibSafeTimerAvailable
				if (m_bThreadLocalCreated.f_Exchange(false))
					m_ThreadLocal.f_Destruct();
		#endif
			}

		#ifdef DMibSafeTimerAvailable

			CSubSystem_Time::CThreadLocal::CThreadLocal(CThreadLocal const &_Other)
				: m_LastSafeTimer(_Other.m_LastSafeTimer)
				, m_LastTimer(_Other.m_LastTimer)
			{
			}

			CSubSystem_Time::CThreadLocal::CThreadLocal()
				: m_LastSafeTimer(-1)
				, m_LastTimer(-1)
			{
		#if DMibConfig_Tests_Enable
				auto &Internal = *fg_GetSys()->m_TimeInternal;
				DMibLock(Internal.m_ThreadLocalsLock);
				Internal.m_ThreadLocals.f_Insert(this);
		#endif
			}

			CSubSystem_Time::CThreadLocal::~CThreadLocal()
			{
		#if DMibConfig_Tests_Enable
				auto &Internal = *fg_GetSys()->m_TimeInternal;
				DMibLock(Internal.m_ThreadLocalsLock);
				Internal.m_ThreadLocals.f_Remove(this);
		#endif
			}
		#endif

			void CSubSystem_Time::f_MeasureCycleFrequency()
			{
				// Measure correction and precache instruction cache
				uint64 TimerCorrection = TCLimitsInt<uint64>::mc_Max;
				uint64 CyclesCorrection = TCLimitsInt<uint64>::mc_Max;
				for (int i = 0; i < 10; ++i)
				{
					uint64 TimerCorrectionThis = -NPlatform::fg_TimerRaw_PreciseGet();
					TimerCorrectionThis += NPlatform::fg_TimerRaw_PreciseGet();
					uint64 CyclesCorrectionThis = -NPlatform::fg_Timer_Cycles();
					CyclesCorrectionThis += NPlatform::fg_Timer_Cycles();
					TimerCorrection = fg_Min(TimerCorrectionThis, TimerCorrection);
					CyclesCorrection = fg_Min(CyclesCorrectionThis, CyclesCorrection);
				}
				
				uint64 TimeToSpend = fg_Max(NPlatform::fg_TimerRaw_PreciseFrequency() / 100000, 1); // Measure for 10 micro seconds

				// Do measure
				uint64 EndTimer = NPlatform::fg_TimerRaw_PreciseGet();
				uint64 End = NPlatform::fg_Timer_Cycles();
				uint64 StartTimer = NPlatform::fg_TimerRaw_PreciseGet();
				uint64 Start = NPlatform::fg_Timer_Cycles();
				while (Start <= End || StartTimer <= EndTimer)
				{
					// Make sure we start at a granularity
					StartTimer = NPlatform::fg_TimerRaw_PreciseGet();
					Start = NPlatform::fg_Timer_Cycles();
				}
				
				while (true)
				{
					EndTimer = NPlatform::fg_TimerRaw_PreciseGet();
					End = NPlatform::fg_Timer_Cycles();
					if ((End - Start) >= 100 && (EndTimer - StartTimer) >= TimeToSpend)
						break;
				}
				uint64 Cycles = End - Start;
				uint64 Timer = EndTimer - StartTimer;
				m_CyclesFrequencyFp = fp64(Cycles) / (fp64(Timer) / fp64(NPlatform::fg_TimerRaw_PreciseFrequency()));
				m_CyclesFrequency = m_CyclesFrequencyFp.f_ToIntRound();
				m_CyclesFrequencyReciprocal = fp64(1.0) / m_CyclesFrequencyFp;
			}	

			int64 CSubSystem_Time::fp_GetTimerFreq() const
			{
				return NPlatform::fg_TimerRaw_PreciseFrequency();
			}

			int64 CSubSystem_Time::fp_GetTimerValInternal() const
			{
		#ifndef DMibSafeTimerAvailable
				return NPlatform::fg_TimerRaw_PreciseGet();
		#else
				if (m_bUseSafeTimer)
				{
					DMibLock(m_LockSafeTimer);
					uint64 TimeVal = NPlatform::fg_TimerRaw_SafeGet();
					uint64 Diff = TimeVal - m_SafeTimerLast;
					m_SafeTimerLast = TimeVal;
					m_SafeTimerVal += (fp64(Diff) * m_SafeTimerScale).f_ToIntRound();
					return m_SafeTimerVal;
				}

				if (!m_bThreadLocalAvailable)
					return NPlatform::fg_TimerRaw_PreciseGet();

				auto& ThreadLocal = **m_ThreadLocal;

				int64 NewTimerVal = NPlatform::fg_TimerRaw_PreciseGet();
				int64 NewSafeTimerVal = NPlatform::fg_TimerRaw_SafeGet();

				if (ThreadLocal.m_LastSafeTimer != -1 && ThreadLocal.m_LastTimer != -1)
				{
					int64 DiffTimer = NewTimerVal - ThreadLocal.m_LastTimer;
					int64 DiffSafeTimer = NewSafeTimerVal - ThreadLocal.m_LastSafeTimer;

					fp64 TimerReciprocal = fp64(DiffTimer) * m_TimerFrequencyReciprocal;
					fp64 SafeTimerReciprocal = fp64(DiffSafeTimer) * m_SafeTimerFrequencyReciprocal;
					fp64 MaxAllowedError(1.0);

					if (SafeTimerReciprocal > fp64(100.0))
						MaxAllowedError += SafeTimerReciprocal * fp64(1.0/3600.0);

					if ((TimerReciprocal - SafeTimerReciprocal).f_Abs() > MaxAllowedError)
					{
						fg_RemoveQualifiers(*this).f_EnableSafeTimer
							(
								NStr::fg_Format
								(
									"Precise timer: 0x{nfh,sf0,sj16}   Safe timer: 0x{nfh,sf0,sj16}   Precise timer diff: {}   Safe timer diff: {}   Max allowed error: {}"
									, NewTimerVal
									, NewSafeTimerVal
									, TimerReciprocal
									, SafeTimerReciprocal
									, MaxAllowedError
								)
							)
						;
						return fp_GetTimerValInternal();
					}
				}
					
				ThreadLocal.m_LastSafeTimer = NewSafeTimerVal;
				ThreadLocal.m_LastTimer = NewTimerVal;

				return NewTimerVal;
		#endif
			}

			int64 CSubSystem_Time::f_GetTimerVal() const
			{
				if (m_bUsedTimeSpeed)
				{
					DMibLock(m_TimerValLock);
					if (m_LastInternalTimer == -1)
					{
						m_LastInternalTimer = fp_GetTimerValInternal();
						m_InternalTimer = m_LastInternalTimer;
					}
					int64 ThisTime = fp_GetTimerValInternal();
					int64 Change = ThisTime - m_LastInternalTimer;
					m_LastInternalTimer = ThisTime;
					auto AbsoluteValue = (fp64(Change) * m_TimeSpeed) + m_InternalTimerExtra;
					auto Rounded = (AbsoluteValue).f_ToInt();
					m_InternalTimerExtra = AbsoluteValue - fp64(Rounded);
					m_InternalTimer += Rounded;

					return m_InternalTimer;
				}
				else
					return fp_GetTimerValInternal();
			}

			void CSubSystem_Time::fp_TimeInit()
			{
				NTime::TCCycles<true> Cycles;
		#ifdef DMibSafeTimerAvailable
				if (!m_bThreadLocalCreated.f_Exchange(true))
				{
					m_ThreadLocal.f_Construct();
					m_bThreadLocalAvailable = true;
				}
		#endif

				NPlatform::fg_TimeRaw_GetNow(&m_TimeBase);

				m_TimerBase = f_GetTimerVal();
				m_TimerFrequency = fp_GetTimerFreq();
		#ifdef DMibSafeTimerAvailable
				m_SafeTimerFrequencyReciprocal = fp64(1.0) / fp64(NPlatform::fg_TimerRaw_SafeFrequency());
		#endif
				m_TimerFrequencyReciprocal = fp64(1.0) / fp64(m_TimerFrequency);
				m_TimerFrequencyFp = m_TimerFrequency;
				m_Drift = 0.0;
				m_DriftFraction = 0.0;

				m_ErrorMargin = (m_TimerFrequencyFp * 0.1).f_ToInt(); // 100 ms allowed (timer is about 10 ms resolution)
				m_ErrorMarginReset = (m_TimerFrequencyFp * 60.0).f_ToInt(); // If we are more than 60 seconds off lets just reset

				m_LastDiff = m_TimerBase;

				NPlatform::fg_TimeRaw_GetUTCOffset(&m_UTCOffset);

				m_NextUpdate = m_TimerBase; // Update once now
				m_NextUTCUpdate = m_TimerBase + m_TimerFrequency * 60;

				NTime::CTime TempVal;
				f_TimeGetNow(&TempVal, false);

				m_bTimeInitDone = true;
			}

			void CSubSystem_Time::fp_TimerUpdate(int64 _CurrentTimer, NTime::CTime &_Time, bint _bRecursive)
			{
				if (m_bUsedTimeSpeed || _bRecursive)
					return;

				NTime::CTime SystemTime;
				NSys::fg_TimeRaw_GetNow(&SystemTime);
				
				int64 UpdateDiff = m_NextUpdate - _CurrentTimer;
				if 
					(
						(UpdateDiff < 0) 
						|| (UpdateDiff > m_TimerFrequency * 2) 
						|| (fg_Abs((_Time - SystemTime).f_GetSeconds()) > 60)
					)
				{
					NTime::CTime OldTime;
					{
						DMibLock(m_Lock);
						m_LastTimer = _CurrentTimer;
						UpdateDiff = m_NextUpdate - _CurrentTimer;
						NTime::CTime SystemTime;
						NSys::fg_TimeRaw_GetNow(&SystemTime);
						if 
							(
								(UpdateDiff < 0)
								|| (UpdateDiff > m_TimerFrequency * 2)
								|| (fg_Abs((_Time - SystemTime).f_GetSeconds()) > 60)
							)
						{
							m_NextUpdate = _CurrentTimer + m_TimerFrequency;
							

							NTime::CTimeSpan TimeDiff = SystemTime - m_TimeBase;

							int64 Diff = (_CurrentTimer - m_TimerBase);
							int64 CurrentTimeFrequency = (TimeDiff.f_GetSeconds() * m_TimerFrequency + (TimeDiff.f_GetFraction() * m_TimerFrequencyFp).f_ToInt());
							int64 Error = CurrentTimeFrequency - Diff;
							if (fg_Abs(Error) > m_ErrorMarginReset)
							{
								m_TimerBase -= Error;
								OldTime = _Time;
								_Time = SystemTime;
							}
							else if (fg_Abs(Error) > m_ErrorMargin)
							{
								m_Drift = fp64(-Error) / (m_TimerFrequencyFp * 10); // Fix any error over 10 seconds
								m_Drift = fg_Max(fg_Min(m_Drift, fp64(0.5)), fp64(-0.5)); // Only go max 50% or 150% of normal time speed
								m_LastDiff = _CurrentTimer;
							}
							else
								m_Drift = 0.0;

							int64 UTCUpdateDiff = m_NextUTCUpdate - _CurrentTimer;
							if (UTCUpdateDiff < 0 || UTCUpdateDiff > m_TimerFrequency * 120)
							{
								NSys::fg_TimeRaw_GetUTCOffset(&m_UTCOffset);
								m_NextUTCUpdate = _CurrentTimer + m_TimerFrequency * 60; // Update once every minute
							}
						}
					}
					if (OldTime.f_IsValid())
						f_ReportTimeChange(OldTime, _Time, "System time more than 60 seconds off");
				}
				else
				{
					if (m_Drift != fp64(0.0))
					{
						DMibLock(m_Lock);
						m_LastTimer = _CurrentTimer;
						int64 Diff = (_CurrentTimer - m_LastDiff);
						fp64 ToDrift = fp64(Diff) * m_Drift + m_DriftFraction;
						int64 IntDrift = ToDrift.f_ToInt();
						m_TimerBase += IntDrift;
						m_DriftFraction = ToDrift - IntDrift;
						m_LastDiff = _CurrentTimer;
					}
				}
			}

			void CSubSystem_Time::f_TimeGetUTCOffset(NTime::CTimeSpan *_pUTCOffset) const
			{
				*_pUTCOffset = m_UTCOffset;
			}

		#ifdef DMibSafeTimerAvailable
		#if DMibConfig_Tests_Enable
			void CSubSystem_Time::f_MakeSafeTimerWrap(fp64 _InSeconds, uint32 _Where)
			{
				int64 Offset = NMib::NSys::fg_MakeSafeTimerWrap(_InSeconds, _Where);

				{
					DMibLock(m_Lock);
					if (m_bUseSafeTimer)
						m_SafeTimerLast += Offset;
				}

				{
					DMibLock(m_ThreadLocalsLock);
					for (auto &Local : m_ThreadLocals)
						Local.m_LastSafeTimer += Offset;
				}
			}
		#endif
			void CSubSystem_Time::f_EnableSafeTimer(NStr::CStr const &_Error)
			{
				{
					DMibLock(m_Lock);
					if (m_bUseSafeTimer)
						return;
					//DMibDTrace("Switching to safe timer because of broken precise timer\n", 0);
					m_bUseSafeTimer = true;
					NPlatform::fg_TimerRaw_SafeIncreasePrecision();
					m_LastInternalTimer = -1;
					int64 LastTimer = m_LastTimer;
				
					m_SafeTimerLast = NPlatform::fg_TimerRaw_SafeGet();
					m_SafeTimerVal = LastTimer;
					m_SafeTimerScale = fp64(NPlatform::fg_TimerRaw_PreciseFrequency()) / fp64(NPlatform::fg_TimerRaw_SafeFrequency());
				}
				
				NTime::CTime NewTime;
				f_TimeGetNow(&NewTime);
				f_ReportTimeChange(NTime::CTime(), NewTime, NStr::fg_Format("Safe timer enabled: {}", _Error));
			}
		#endif

			void CSubSystem_Time::f_DestroyThreadLocal()
			{
				m_bThreadLocalAvailable = false;
		#ifdef DMibSafeTimerAvailable
				if (m_bThreadLocalCreated.f_Exchange(false))
					m_ThreadLocal.f_Destruct();
		#endif
			}

			void CSubSystem_Time::f_TimeGetNow(NTime::CTime *_pTime, bint _bRecursive) const
			{
				if (!m_TimerFrequency)
					fg_RemoveQualifiers(*this).fp_TimeInit();
				NTime::CTime Ret = m_TimeBase;	

				int64 CurrentTimer = f_GetTimerVal();

				NTime::CTimeSpan Add;
				int64 Diff = (CurrentTimer - m_TimerBase);
				if (Diff < 0)
				{
					int64 nSeconds = fg_Abs(Diff) / m_TimerFrequency;
					Add.f_SetSeconds(nSeconds);
					Add.f_SetFraction(fp64(fg_Abs(Diff) % m_TimerFrequency) / m_TimerFrequencyFp);
					Ret -= Add;
				}
				else
				{
					int64 nSeconds = Diff / m_TimerFrequency;
					Add.f_SetSeconds(nSeconds);
					Add.f_SetFraction(fp64(Diff % m_TimerFrequency) / m_TimerFrequencyFp);
					Ret += Add;
				}

				fg_RemoveQualifiers(*this).fp_TimerUpdate(CurrentTimer, Ret, _bRecursive);

				*_pTime = Ret;
			}

			void CSubSystem_Time::f_DisableTimeSpeed()
			{
				NTime::CTime OldTime;
				f_TimeGetNow(&OldTime, false);
		
				m_TimeSpeed = 1.0;
				m_TimeSpeedReciprocal = 1.0;
				m_bUsedTimeSpeed = false;
				m_LastInternalTimer = -1;
				m_InternalTimer = 0;

				fp_TimeInit();
		
				NTime::CTime NewTime;
				f_TimeGetNow(&NewTime, false);
				f_ReportTimeChange(OldTime, NewTime, "Disable time speed");
			}

			void CSubSystem_Time::f_SetTimeSpeed(fp64 _Multiplier, NTime::CTime const *_pOptionalTime, NTime::CTimeSpan const *_pTimeZone)
			{
				NTime::CTime OldTime;
				f_TimeGetNow(&OldTime, false);
				
				DMibLock(m_TimerValLock);
				m_TimeSpeed = _Multiplier;
				m_TimeSpeedReciprocal = fp64(1.0) / _Multiplier;
				
				if (_Multiplier != 1.0)
					m_bUsedTimeSpeed = true;
				
				if (_pOptionalTime)
				{
					int64 TimerVal = f_GetTimerVal();
					NTime::CTimeSpan Diff = *_pOptionalTime - m_TimeBase;
					m_TimerBase = TimerVal - (Diff.f_GetSecondsFraction() * m_TimerFrequencyFp).f_ToInt();
					m_bUsedTimeSpeed = true;
				}
				
				if (_pTimeZone)
				{
					m_UTCOffset = *_pTimeZone;
					m_bUsedTimeSpeed = true;
				}
				
				NTime::CTime NewTime;
				f_TimeGetNow(&NewTime, false);
				
				if (_pOptionalTime)
					f_ReportTimeChange(OldTime, NewTime, "Set time speed");
			}

			fp64 CSubSystem_Time::f_GetTimeSpeed() const
			{
				return m_TimeSpeed;
			}

			fp64 CSubSystem_Time::f_GetTimeSpeedReciprocal() const
			{
				return m_TimeSpeedReciprocal;
			}

			bool CSubSystem_Time::f_GetTimeSimulating() const
			{
				return m_bUsedTimeSpeed;
			}
			
			void CSubSystem_Time::f_ReportTimeChange(NTime::CTime const &_OldTime, NTime::CTime const &_NewTime, NStr::CStr const &_Reason)
			{
				DMibLock(m_TimeChangeNotificationLock);
				
				for (auto &fNotification : m_TimeChangeNotifications)
					fNotification(_OldTime, _NewTime, _Reason);
			}
		}
		
		CSystem_Time::CTimeChangeNotificationSubscription::CTimeChangeNotificationSubscription()
			: mp_pNotification(nullptr)
		{
		}
		
		CSystem_Time::CTimeChangeNotificationSubscription::CTimeChangeNotificationSubscription(CTimeChangeNotificationSubscription &&_Other)
		{
			mp_pNotification = _Other.mp_pNotification;
			_Other.mp_pNotification = nullptr;
		}

		CSystem_Time::CTimeChangeNotificationSubscription &CSystem_Time::CTimeChangeNotificationSubscription::operator =(CTimeChangeNotificationSubscription &&_Other)
		{
			fp_Remove();
			mp_pNotification = _Other.mp_pNotification;
			_Other.mp_pNotification = nullptr;
			return *this;
		}
		
		void CSystem_Time::CTimeChangeNotificationSubscription::fp_Remove()
		{
			if (mp_pNotification)
			{
				auto &SubSystem = *g_MalterlibSubSystem_Time;
				DMibLock(SubSystem.m_TimeChangeNotificationLock);
				SubSystem.m_TimeChangeNotifications.f_Remove(*mp_pNotification);
				mp_pNotification = nullptr;
			}
		}

		CSystem_Time::CTimeChangeNotificationSubscription::~CTimeChangeNotificationSubscription()
		{
			fp_Remove();
		}

		CSystem_Time::CTimeChangeNotificationSubscription::CTimeChangeNotificationSubscription
			(
				NFunction::TCFunction<void (NTime::CTime const &_OldTime, NTime::CTime const &_NewTime, NStr::CStr const &_Reason)> *_pNotification
			)
			: mp_pNotification(_pNotification)
		{
			
		}
		
		CSystem_Time::CTimeChangeNotificationSubscription CSystem_Time::fs_RegisterTimeChangeNotification
			(
				NFunction::TCFunction<void (NTime::CTime const &_OldTime, NTime::CTime const &_NewTime, NStr::CStr const &_Reason)> &&_fNotification
			)
		{
			auto &SubSystem = *g_MalterlibSubSystem_Time;
			DMibLock(SubSystem.m_TimeChangeNotificationLock);
			
			auto &NewNotification = SubSystem.m_TimeChangeNotifications.f_Insert(fg_Move(_fNotification));
			return &NewNotification;		
		}
		
		
		bool CSystem_Time::fs_TimeInitDone()
		{
			return g_MalterlibSubSystem_Time->m_bTimeInitDone;
		}
		fp64 CSystem_Time::fs_CyclesFrequencyFp()
		{
			return g_MalterlibSubSystem_Time->m_CyclesFrequencyFp;
		}
		uint64 CSystem_Time::fs_CyclesFrequency()
		{
			return g_MalterlibSubSystem_Time->m_CyclesFrequency;
		}
		
		fp64 CSystem_Time::fs_CyclesFrequencyReciprocal()
		{
			return g_MalterlibSubSystem_Time->m_CyclesFrequencyReciprocal;
		}	

		int64 CSystem_Time::fs_TimerFrequency()
		{
			return g_MalterlibSubSystem_Time->m_TimerFrequency;
		}
		fp64 CSystem_Time::fs_TimerFrequencyFp()
		{
			return g_MalterlibSubSystem_Time->m_TimerFrequencyFp;
		}
		int64 CSystem_Time::fs_GetTimerValue()
		{
			return g_MalterlibSubSystem_Time->f_GetTimerVal();
		}

		void CSystem_Time::fs_SetTimeSpeed(fp64 _Multiplier, NTime::CTime const *_pOptionalTime, NTime::CTimeSpan const *_pTimeZone)
		{
			return g_MalterlibSubSystem_Time->f_SetTimeSpeed(_Multiplier, _pOptionalTime, _pTimeZone);
		}

		fp64 CSystem_Time::fs_GetTimeSpeed()
		{
			return g_MalterlibSubSystem_Time->f_GetTimeSpeed();
		}
		fp64 CSystem_Time::fs_GetTimeSpeedReciprocal()
		{
			return g_MalterlibSubSystem_Time->f_GetTimeSpeedReciprocal();
		}
		void CSystem_Time::fs_DisableTimeSpeed()
		{
			return g_MalterlibSubSystem_Time->f_DisableTimeSpeed();
		}
		bool CSystem_Time::fs_GetTimeSimulating()
		{
			return g_MalterlibSubSystem_Time->f_GetTimeSimulating();
		}

		void CSystem_Time::fs_TimeGetUTCOffset(NTime::CTimeSpan *_pUTCOffset)
		{
			return g_MalterlibSubSystem_Time->f_TimeGetUTCOffset(_pUTCOffset);
		}

		void CSystem_Time::fs_TimeGetNowUTC(NTime::CTime *_pTime)
		{
			return g_MalterlibSubSystem_Time->f_TimeGetNow(_pTime);
		}

		int64 CSystem_Time::fs_TimeResolution()
		{
			return g_MalterlibSubSystem_Time->m_TimerFrequency;
		}
		
	#ifdef DMibSafeTimerAvailable
#if DMibConfig_Tests_Enable
		void CSystem_Time::fs_MakeSafeTimerWrap(fp64 _InSeconds, uint32 _Where)
		{
			g_MalterlibSubSystem_Time->f_MakeSafeTimerWrap(_InSeconds, _Where);
		}
#endif
		void CSystem_Time::fs_EnableSafeTimer()
		{
			g_MalterlibSubSystem_Time->f_EnableSafeTimer("Manually");
		}
		bool CSystem_Time::fs_IsSafeTimerEnabled() const
		{
			return g_MalterlibSubSystem_Time->m_bUseSafeTimer;
		}
	#endif
	}
} 
