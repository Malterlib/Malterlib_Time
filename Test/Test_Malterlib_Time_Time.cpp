// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

namespace 
{
	class CTime_Tests : public NMib::NTest::CTest
	{
	public:

		void f_DoTests()
		{
			DMibTestSuite("Time")
			{
				// Not sure exactly how useful these are since no one is ever looking at the results...
				/*
				for (int i = 0; i < 10; ++i)
				{
					NMib::NTime::CTime Test = NMib::NTime::CTime::fs_NowUTC();
					NMib::NTime::CTime Test2 = NMib::NTime::CTime::fs_NowUTC();

					NMib::NTime::CTimeSpan Span = Test2 - Test;

					DMibTrace("Ticks {fe3} {} {} micro\n", (fp64(Span.f_GetSeconds()) + Span.f_GetFraction() * NMib::NTime::CTime::fs_GetResolution()) << ((fp64(Span.f_GetSeconds()) + Span.f_GetFraction() * NMib::NTime::CTime::fs_GetResolution()) + 0.5).f_ToInt() << (fp64(Span.f_GetSeconds()) + Span.f_GetFraction()) * 1000000.0);
				}

				for (int i = 0; i < 2000; ++i)
				{
					NMib::NTime::CTime Test = NMib::NTime::CTime::fs_NowUTC();
					NMib::NSys::fg_Thread_Sleep(0.25f);

					NMib::NTime::CTimeConvert::CDateTime DateTime;
					NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);

					DMibTrace("{}-{sj2,sf0}-{sj2,sf0} {sj2,sf0}:{sj2,sf0}:{sj2,sf0}.{fr1,fe11}\n", DateTime.m_Year << DateTime.m_Month << DateTime.m_DayOfMonth << DateTime.m_Hour << DateTime.m_Minute << DateTime.m_Second << DateTime.m_Fraction);
				}
				*/
				NMib::NTime::CTime Test;

				NMib::NTime::CTimeConvert::CDateTime DateTime;

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(2001, 3, 14, 12, 53, 50, 0.5);
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(2001));

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(2003, 11, 27, 11, 47, 44, 0.5);
				Test += NMib::NTime::CTimeSpanConvert::fs_CreateSpan(0, 2);
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(2003));

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(2003, 02, 28, 0, 0, 0, 0);
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(2003) && DMibExpr(2));

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(2002);
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(2002));

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(19800000);
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(19800000));

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(constant_int64(19800000000));
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(19800000000));

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(constant_int64(19800000001));
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(19800000001));

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(constant_int64(220000000001));
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(220000000001));

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(0);
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(0));

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(1);
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(1));

	#if 0
				Test = NMib::NTime::CTimeConvert::fs_CreateTime(-1);
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(-1));

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(-2000);
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(-2000));

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(-2002);
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(-2002));

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(-29000000001);
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(-29000000001));
	#endif

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(2004, 01, 01, 0, 0, 0, 0);
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2004));
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(1));

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(2003, 12, 28, 0, 0, 0, 0);
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2003));
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(52));

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(2003, 12, 29, 0, 0, 0, 0);
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2004) && DMibExpr(2));
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(1) && DMibExpr(2));

				Test = NMib::NTime::CTimeConvert::fs_CreateTime(2005, 01, 03, 0, 0, 0, 0);
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2005));
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(1) && DMibExpr(3));

				Test = NMib::NTime::CTimeConvert_ISOWeek::fs_CreateTime(2004, 1);
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2004) && DMibExpr(4));
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(1) && DMibExpr(4));

				Test = NMib::NTime::CTimeConvert_ISOWeek::fs_CreateTime(2003, 1, 0);
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2003) && DMibExpr(5));
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(1) && DMibExpr(5));

				Test = NMib::NTime::CTimeConvert_ISOWeek::fs_CreateTime(2003, 52, 6);
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2003) && DMibExpr(6));
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(52) && DMibExpr(6));
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(2003) && DMibExpr(6));

				Test = NMib::NTime::CTimeConvert_ISOWeek::fs_CreateTime(2004, 01, 0);
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2004) && DMibExpr(7));
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(1) && DMibExpr(7));
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(2003) && DMibExpr(7));

				Test = NMib::NTime::CTimeConvert_ISOWeek::fs_CreateTime(2005, 01, 0);
				NMib::NTime::CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2005) && DMibExpr(8));
				DMibTest(DMibExpr(NMib::NTime::CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(1) && DMibExpr(8));
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(2005) && DMibExpr(8));

				{
					Test = NMib::NTime::CTimeConvert::fs_CreateTime(2011, 1, 13, 18, 02, 0, 0);

					NMib::NStr::CStr StrFullTime = NMib::NTime::fg_GetFullTimeStr(Test);
					DMibTest(DMibExpr(StrFullTime) == DMibExpr("2011-01-13 18:02:00.000"));

					NMib::NTime::CTime ReadTime;
					bint bReadOK = NMib::NTime::fg_ParseFullTimeStr(ReadTime, StrFullTime);
					DMibTest(DMibExpr(bReadOK) == DMibExpr((bint)true));
					DMibTest(DMibExpr(ReadTime) == DMibExpr(Test));
				}
			};
			DMibTestSuite("Time precision")
			{
				NMib::NTime::CTime Now = NMib::NTime::CTime::fs_NowUTC();
				NMib::NTime::CTime Now2 = NMib::NTime::CTime::fs_NowUTC();
				DMibTest(DMibExpr(Now) != DMibExpr(Now2)); // This can fail on OSX until we have ported Windows code inte general system
				
				NMib::NTime::CTime Start = NMib::NTime::CTime::fs_NowUTC();
				auto TimerStart = NMib::NTime::CSystem_Time::fs_GetTimerValue();
				NMib::NSys::fg_Thread_Sleep(0.1f);
				NMib::NTime::CTime End = NMib::NTime::CTime::fs_NowUTC();
				auto TimerEnd = NMib::NTime::CSystem_Time::fs_GetTimerValue();
				
				fp64 TotalTime = (End - Start).f_GetSecondsFraction();
				fp64 TotalTimeTimer = fp64(TimerEnd - TimerStart) / NMib::NTime::CSystem_Time::fs_TimerFrequencyFp();
				
				DMibTest((DMibExpr(TotalTime) / DMibExpr(TotalTimeTimer)) > DMibExpr(0.99) && (DMibExpr(TotalTime) / DMibExpr(TotalTimeTimer)) < DMibExpr(1.01));
				
				
			};
			
			DMibTestSuite("Time speed")
			{
				NMib::NTime::CSystem_Time::fs_SetTimeSpeed(2.0, nullptr);
				NMib::NTime::CTime Start = NMib::NTime::CTime::fs_NowUTC();
				NMib::NTime::CTime StartRaw;
				NMib::NTime::NPlatform::fg_TimeRaw_GetNow(&StartRaw);

				NMib::NSys::fg_Thread_Sleep(1.0f);

				NMib::NTime::CTime End = NMib::NTime::CTime::fs_NowUTC();
				NMib::NTime::CTime EndRaw;
				NMib::NTime::NPlatform::fg_TimeRaw_GetNow(&EndRaw);
				
				fp64 TotalTime = (End - Start).f_GetSecondsFraction();
				fp64 TotalTimeRaw = (EndRaw - StartRaw).f_GetSecondsFraction();
				
				DMibTest((DMibExpr(TotalTime) / DMibExpr(TotalTimeRaw)) > DMibExpr(1.96) && (DMibExpr(TotalTime) / DMibExpr(TotalTimeRaw)) < DMibExpr(2.04));
				
				NMib::NTime::CSystem_Time::fs_DisableTimeSpeed();
				
			};

			DMibTestSuite("Timer speed")
			{
				NMib::NTime::CSystem_Time::fs_SetTimeSpeed(2.0, nullptr);
				int64 Start = NMib::NTime::CSystem_Time::fs_GetTimerValue();
				int64 StartRaw = NMib::NTime::NPlatform::fg_TimerRaw_PreciseGet();

				NMib::NSys::fg_Thread_Sleep(1.0f);

				int64 End = NMib::NTime::CSystem_Time::fs_GetTimerValue();
				int64 EndRaw = NMib::NTime::NPlatform::fg_TimerRaw_PreciseGet();
				
				fp64 TotalTime = fp64(End - Start);
				fp64 TotalTimeRaw = fp64(EndRaw - StartRaw);
				
				DMibTest((DMibExpr(TotalTime) / DMibExpr(TotalTimeRaw)) > DMibExpr(1.96) && (DMibExpr(TotalTime) / DMibExpr(TotalTimeRaw)) < DMibExpr(2.04));
				
				NMib::NTime::CSystem_Time::fs_DisableTimeSpeed();
				
			};

			DMibTestSuite("Time change notifications")
			{
				NMib::NThread::CMutual Lock;
				NMib::NStr::CStr Reason;
				auto Subscription = NMib::NTime::CSystem_Time::fs_RegisterTimeChangeNotification
					(
						[&](NMib::NTime::CTime const &_OldTime, NMib::NTime::CTime const &_NewTime, NMib::NStr::CStr const &_Reason)
						{
							DMibLock(Lock);
							Reason = _Reason;
						}
					)
				;
				NMib::NTime::CTime Time = NMib::NTime::CTime::fs_NowUTC();
				Time += NMib::NTime::CTimeSpanConvert::fs_CreateDaySpan(365);
				NMib::NTime::CSystem_Time::fs_SetTimeSpeed(1.0, &Time);
				{
					DMibLock(Lock);
					DMibExpect(Reason, ==, "Set time speed");
				}
				
				NMib::NTime::CSystem_Time::fs_DisableTimeSpeed();
				{
					DMibLock(Lock);
					DMibExpect(Reason, ==, "Disable time speed");
				}
			};
		}
	};
}

DMibTestRegister(CTime_Tests, Malterlib::Time);
