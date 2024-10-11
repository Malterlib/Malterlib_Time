// Copyright © 2015 Hansoft AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#define DMalteribTimeTestWithPrecision

#ifdef DMalteribTimeTestWithPrecision
static fp64 const g_CheckTime = 0.2;
static fp64 const g_AllowedDiff = 1.1;
#else
static fp64 const g_CheckTime = 1.0;
static fp64 const g_AllowedDiff = 1.02;
#endif

#include <Mib/Test/Performance>
#include <Mib/Test/Exception>

namespace
{
	using namespace NMib;
	using namespace NMib::NTime;
	using namespace NMib::NStr;

	class CTime_Tests : public NMib::NTest::CTest
	{
	public:

		void f_DoTests()
		{
			DMibTestSuite("Time")
			{
				CTime Test;

				CTimeConvert::CDateTime DateTime;

				Test = CTimeConvert::fs_CreateTime(2001, 3, 14, 12, 53, 50, 0.5);
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(2001));

				Test = CTimeConvert::fs_CreateTime(2003, 11, 27, 11, 47, 44, 0.5);
				Test += CTimeSpanConvert::fs_CreateSpan(0, 2);
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(2003));

				Test = CTimeConvert::fs_CreateTime(2003, 02, 28, 0, 0, 0, 0);
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(2003) && DMibExpr(2));

				Test = CTimeConvert::fs_CreateTime(2002);
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(2002));

				Test = CTimeConvert::fs_CreateTime(19800000);
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(19800000));

				Test = CTimeConvert::fs_CreateTime(constant_int64(19800000000));
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(19800000000));

				Test = CTimeConvert::fs_CreateTime(constant_int64(19800000001));
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(19800000001));

				Test = CTimeConvert::fs_CreateTime(constant_int64(220000000001));
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(220000000001));

				Test = CTimeConvert::fs_CreateTime(constant_int64(577'039'110'548));
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(577'039'110'548));

				Test = CTimeConvert::fs_CreateTime(0);
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(0));

				Test = CTimeConvert::fs_CreateTime(1);
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(1));

				Test = CTimeConvert::fs_CreateTime(-1);
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(-1));

				Test = CTimeConvert::fs_CreateTime(-2000);
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(-2000));

				Test = CTimeConvert::fs_CreateTime(-2002);
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(-2002));

				Test = CTimeConvert::fs_CreateTime(constant_int64(-7'514'938'705));
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(constant_int64(-7'514'938'705)));

				Test = CTimeConvert::fs_CreateTime(2004, 01, 01, 0, 0, 0, 0);
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2004));
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(1));

				Test = CTimeConvert::fs_CreateTime(2003, 12, 28, 0, 0, 0, 0);
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2003));
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(52));

				Test = CTimeConvert::fs_CreateTime(2003, 12, 29, 0, 0, 0, 0);
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2004) && DMibExpr(2));
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(1) && DMibExpr(2));

				Test = CTimeConvert::fs_CreateTime(2005, 01, 03, 0, 0, 0, 0);
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2005));
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(1) && DMibExpr(3));

				Test = CTimeConvert_ISOWeek::fs_CreateTime(2004, 1);
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2004) && DMibExpr(4));
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(1) && DMibExpr(4));

				Test = CTimeConvert_ISOWeek::fs_CreateTime(2003, 1, 0);
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2003) && DMibExpr(5));
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(1) && DMibExpr(5));

				Test = CTimeConvert_ISOWeek::fs_CreateTime(2003, 52, 6);
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2003) && DMibExpr(6));
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(52) && DMibExpr(6));
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(2003) && DMibExpr(6));

				Test = CTimeConvert_ISOWeek::fs_CreateTime(2004, 01, 0);
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2004) && DMibExpr(7));
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(1) && DMibExpr(7));
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(2003) && DMibExpr(7));

				Test = CTimeConvert_ISOWeek::fs_CreateTime(2005, 01, 0);
				CTimeConvert(Test).f_ExtractDateTime(DateTime);
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetYear()) == DMibExpr(2005) && DMibExpr(8));
				DMibTest(DMibExpr(CTimeConvert_ISOWeek(Test).f_GetWeek()) == DMibExpr(1) && DMibExpr(8));
				DMibTest(DMibExpr(DateTime.m_Year) == DMibExpr(2005) && DMibExpr(8));

				{
					Test = CTimeConvert::fs_CreateTime(2011, 1, 13, 18, 02, 0, 0);

					NMib::NStr::CStr StrFullTime = fg_GetFullTimeStr(Test);
					DMibTest(DMibExpr(StrFullTime) == DMibExpr("2011-01-13 18:02:00.000"));

					CTime ReadTime;
					bool bReadOK = fg_ParseFullTimeStr(ReadTime, StrFullTime);
					DMibTest(DMibExpr(bReadOK) == DMibExpr(true));
					DMibTest(DMibExpr(ReadTime) == DMibExpr(Test));
				}
			};
			DMibTestSuite("Time precision")
			{
				CTime Now = CTime::fs_NowUTC();
				NMib::NSys::fg_Thread_Sleep(NPlatform::fg_TimeRaw_Resolution());
				CTime Now2 = CTime::fs_NowUTC();
				DMibTest(DMibExpr(Now) != DMibExpr(Now2)); // This can fail on macOS until we have ported Windows code inte general system

				CTime Start = CTime::fs_NowUTC();
				auto TimerStart = CSystem_Time::fs_GetTimerValue();
				NMib::NSys::fg_Thread_Sleep(0.1f);
				CTime End = CTime::fs_NowUTC();
				auto TimerEnd = CSystem_Time::fs_GetTimerValue();

				fp64 TotalTime = (End - Start).f_GetSecondsFraction();
				fp64 TotalTimeTimer = fp64(TimerEnd - TimerStart) / CSystem_Time::fs_TimerFrequencyFp();

				DMibTest((DMibExpr(TotalTime) / DMibExpr(TotalTimeTimer)) > DMibExpr(fp64(1.0)/g_AllowedDiff) && (DMibExpr(TotalTime) / DMibExpr(TotalTimeTimer)) < DMibExpr(g_AllowedDiff));
			};

			DMibTestSuite("Time speed")
			{
				fp64 TimeSpeed = 2.0;
				CSystem_Time::fs_SetTimeSpeed(TimeSpeed, nullptr, nullptr);
				CTime Start = CTime::fs_NowUTC();
				CTime StartRaw;
				NPlatform::fg_TimeRaw_GetNow(&StartRaw);

				NMib::NSys::fg_Thread_Sleep(g_CheckTime);

				CTime End = CTime::fs_NowUTC();
				CTime EndRaw;
				NPlatform::fg_TimeRaw_GetNow(&EndRaw);

				fp64 TotalTime = (End - Start).f_GetSecondsFraction();
				fp64 TotalTimeRaw = (EndRaw - StartRaw).f_GetSecondsFraction();



				DMibTest((DMibExpr(TotalTime) / DMibExpr(TotalTimeRaw)) > DMibExpr(TimeSpeed / g_AllowedDiff) && (DMibExpr(TotalTime) / DMibExpr(TotalTimeRaw)) < DMibExpr(TimeSpeed * g_AllowedDiff));

				CSystem_Time::fs_DisableTimeSpeed();

			};

			DMibTestSuite("Timer speed")
			{
				fp64 TimeSpeed = 2.0;
				CSystem_Time::fs_SetTimeSpeed(TimeSpeed, nullptr, nullptr);
				int64 Start = CSystem_Time::fs_GetTimerValue();
				int64 StartRaw = NPlatform::fg_TimerRaw_PreciseGet();

				NMib::NSys::fg_Thread_Sleep(1.0f);

				int64 End = CSystem_Time::fs_GetTimerValue();
				int64 EndRaw = NPlatform::fg_TimerRaw_PreciseGet();

				fp64 TotalTime = fp64(End - Start);
				fp64 TotalTimeRaw = fp64(EndRaw - StartRaw);

				DMibTest((DMibExpr(TotalTime) / DMibExpr(TotalTimeRaw)) > DMibExpr(TimeSpeed / g_AllowedDiff) && (DMibExpr(TotalTime) / DMibExpr(TotalTimeRaw)) < DMibExpr(TimeSpeed * g_AllowedDiff));

				CSystem_Time::fs_DisableTimeSpeed();

			};

			DMibTestSuite("Time change notifications")
			{
				NMib::NThread::CMutual Lock;
				NMib::NStr::CStr Reason;
				auto Subscription = CSystem_Time::fs_RegisterTimeChangeNotification
					(
						[&](CTime const &_OldTime, CTime const &_NewTime, NMib::NStr::CStr const &_Reason)
						{
							DMibLock(Lock);
							Reason = _Reason;
						}
					)
				;
				CTime Time = CTime::fs_NowUTC();
				Time += CTimeSpanConvert::fs_CreateDaySpan(365);
				CSystem_Time::fs_SetTimeSpeed(1.0, &Time, nullptr);
				{
					DMibLock(Lock);
					DMibExpect(Reason, ==, "Set time speed");
				}

				CSystem_Time::fs_DisableTimeSpeed();
				{
					DMibLock(Lock);
					DMibExpect(Reason, ==, "Disable time speed");
				}
			};

			DMibTestSuite("Comparison")
			{
				using namespace NMib;
				using namespace NMib::NTime;
				CTime Time1 = CTimeConvert::fs_CreateTime(2023, 4, 5);
				CTime Time2 = CTimeConvert::fs_CreateTime(2023, 5, 5);
				CTime TimeInvalid;
				CTime StartOfTime = CTime::fs_StartOfTime();
				CTime EndOfTime = CTime::fs_EndOfTime();

				DMibExpect(Time1, <, Time2);
				DMibExpect(Time2, >, Time1);
				DMibExpect(Time1, ==, Time1);

				DMibExpect(Time1, <, TimeInvalid);
				DMibExpect(TimeInvalid, >, Time1);
				DMibExpect(TimeInvalid, ==, TimeInvalid);

				DMibExpect(fg_Max(Time1, Time2), ==, Time2);
				DMibExpect(fg_Min(Time1, Time2), ==, Time1);
				DMibExpect(fg_Max(Time2, Time1), ==, Time2);
				DMibExpect(fg_Min(Time2, Time1), ==, Time1);

				DMibExpect(fg_Max(Time1, TimeInvalid), ==, Time1);
				DMibExpect(fg_Min(Time1, TimeInvalid), ==, Time1);
				DMibExpect(fg_Max(TimeInvalid, Time1), ==, Time1);
				DMibExpect(fg_Min(TimeInvalid, Time1), ==, Time1);

				DMibExpect(fg_Max(StartOfTime, TimeInvalid), ==, StartOfTime);
				DMibExpect(fg_Min(StartOfTime, TimeInvalid), ==, StartOfTime);
				DMibExpect(fg_Max(TimeInvalid, StartOfTime), ==, StartOfTime);
				DMibExpect(fg_Min(TimeInvalid, StartOfTime), ==, StartOfTime);

				DMibExpect(fg_Max(EndOfTime, TimeInvalid), ==, EndOfTime);
				DMibExpect(fg_Min(EndOfTime, TimeInvalid), ==, EndOfTime);
				DMibExpect(fg_Max(TimeInvalid, EndOfTime), ==, EndOfTime);
				DMibExpect(fg_Min(TimeInvalid, EndOfTime), ==, EndOfTime);

				DMibExpect(fg_Max(fg_Const(Time1), TimeInvalid), ==, Time1);
				DMibExpect(fg_Min(fg_Const(Time1), TimeInvalid), ==, Time1);

				DMibExpect(fg_Max(Time1, fg_Const(TimeInvalid)), ==, Time1);
				DMibExpect(fg_Min(Time1, fg_Const(TimeInvalid)), ==, Time1);

				DMibExpect(fg_Max(fg_Const(Time1), fg_Const(TimeInvalid)), ==, Time1);
				DMibExpect(fg_Min(fg_Const(Time1), fg_Const(TimeInvalid)), ==, Time1);

				DMibExpect(fg_Max(fg_TempCopy(Time1), TimeInvalid), ==, Time1);
				DMibExpect(fg_Min(fg_TempCopy(Time1), TimeInvalid), ==, Time1);

				DMibExpect(fg_Max(Time1, fg_TempCopy(TimeInvalid)), ==, Time1);
				DMibExpect(fg_Min(Time1, fg_TempCopy(TimeInvalid)), ==, Time1);

				DMibExpect(fg_Max(fg_TempCopy(Time1), fg_TempCopy(TimeInvalid)), ==, Time1);
				DMibExpect(fg_Min(fg_TempCopy(Time1), fg_TempCopy(TimeInvalid)), ==, Time1);
			};

			DMibTestSuite("DateTime")
			{
				DMibExpect(CTimeConvert::fs_FromUnixSeconds(0), ==, CTimeConvert::fs_CreateTime(1970));
				DMibExpect(CTimeConvert::fs_GetYearZero(), ==, CTimeConvert::fs_CreateTime(0));
				DMibExpect(CTimeConvert::fs_GetYearZero() - CTimeSpanConvert::fs_CreateDaySpan(365), ==, CTimeConvert::fs_CreateTime(-1));

				DMibExpect((CTimeConvert::fs_CreateTime(1970) - CTimeConvert::fs_GetYearZero()).f_GetSeconds(), ==, 62167219200);

				DMibExpect(CStr("{tc3,tb}"_f << CTimeConvert::fs_CreateTime(-2000, 1, 1)), ==, "2001_BC-01-01");
				DMibExpect(CStr("{tc3}"_f << CTimeConvert::fs_CreateTime(-2000, 1, 1)), ==, "-2000-01-01");
				DMibExpect(CStr("{tc3}"_f << CTimeConvert::fs_CreateTime(-1, 1, 1)), ==, "-1-01-01");
				DMibExpect(CStr("{tc3}"_f << CTimeConvert::fs_CreateTime(0, 1, 1)), ==, "0-01-01");
				DMibExpect(CStr("{tc3}"_f << CTimeConvert::fs_CreateTime(1, 1, 1)), ==, "1-01-01");
				DMibExpect(CStr("{tc3}"_f << CTimeConvert::fs_CreateTime(2000, 1, 1)), ==, "2000-01-01");

 				DMibExpect(CStr("{tc5}"_f << CTimeConvert::fs_FromUnixSeconds(0)), ==, "1970-01-01 00:00");
				DMibExpect(CStr("{tc5}"_f << CTimeConvert::fs_GetYearZero()), ==, "0-01-01 00:00");
				DMibExpect(CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateMinuteSpan(1))), ==, "0-01-01 00:01");
				DMibExpect(CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() - CTimeSpanConvert::fs_CreateMinuteSpan(1))), ==, "-1-12-31 23:59");
				DMibExpect(CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(365))), ==, "0-12-31 00:00");
				DMibExpect(CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(366))), ==, "1-01-01 00:00");
				DMibExpect(CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(366) - CTimeSpanConvert::fs_CreateMinuteSpan(1))), ==, "0-12-31 23:59");
				DMibExpect(CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(366) + CTimeSpanConvert::fs_CreateMinuteSpan(1))), ==, "1-01-01 00:01");

				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 31 + 28) - CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "0-02-27 23:59"
					)
				;
				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 31 + 29) - CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "0-02-28 23:59"
					)
				;
				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 31 + 29 + 1) - CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "0-02-29 23:59"
					)
				;
				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 31 + 29 + 2) - CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "0-03-01 23:59"
					)
				;

				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 31 + 27) + CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "0-02-27 00:01"
					)
				;
				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 31 + 28) + CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "0-02-28 00:01"
					)
				;
				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 31 + 29) + CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "0-02-29 00:01"
					)
				;
				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 31 + 29 + 1) + CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "0-03-01 00:01"
					)
				;
				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 31 + 31) + CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "0-03-02 00:01"
					)
				;

				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 366 + 31 + 28) - CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "1-02-27 23:59"
					)
				;
				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 366 + 31 + 29) - CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "1-02-28 23:59"
					)
				;
				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 366 + 31 + 30) - CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "1-03-01 23:59"
					)
				;
				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 366 + 31 + 31) - CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "1-03-02 23:59"
					)
				;

				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 366 + 31 + 27) + CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "1-02-27 00:01"
					)
				;
				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 366 + 31 + 28) + CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "1-02-28 00:01"
					)
				;
				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 366 + 31 + 29) + CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "1-03-01 00:01"
					)
				;
				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 366 + 31 + 30) + CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "1-03-02 00:01"
					)
				;
				DMibExpect
					(
						CStr("{tc5}"_f << (CTimeConvert::fs_GetYearZero() + CTimeSpanConvert::fs_CreateDaySpan(-1 + 366 + 31 + 31) + CTimeSpanConvert::fs_CreateMinuteSpan(1)))
						, ==
						, "1-03-03 00:01"
					)
				;

				auto fCheckTurnaround = [](CStr const &_Path, CTime const &_StartTime, CTimeSpan const &_Duration)
					{
						DMibTestPath(_Path);
						CTime EndTime = _StartTime + _Duration;
#ifdef DMibDebug
						auto IterationSpan = CTimeSpanConvert::fs_CreateDaySpan(19);
#else
						auto IterationSpan = CTimeSpanConvert::fs_CreateDaySpan(1);
#endif
						{
							DMibTestPath("Normal");
							for (CTime Time = _StartTime; Time < EndTime; Time += IterationSpan)
							{
								auto DateTime = CTimeConvert(Time).f_ExtractDateTime();
								auto GeneratedTime = CTimeConvert::fs_CreateTime(DateTime);
								DMibExpect(GeneratedTime, ==, Time)(ETestFlag_Aggregated);
							}
						}
						{
							DMibTestPath("Minus1Second");
							for (CTime Time = _StartTime - CTimeSpanConvert::fs_CreateSecondSpan(1); Time < EndTime; Time += IterationSpan)
							{
								auto DateTime = CTimeConvert(Time).f_ExtractDateTime();
								auto GeneratedTime = CTimeConvert::fs_CreateTime(DateTime);
								DMibExpect(GeneratedTime, ==, Time)(ETestFlag_Aggregated);
							}
						}
						{
							DMibTestPath("Plus1Second");
							for (CTime Time = _StartTime + CTimeSpanConvert::fs_CreateSecondSpan(1); Time < EndTime; Time += IterationSpan)
							{
								auto DateTime = CTimeConvert(Time).f_ExtractDateTime();
								auto GeneratedTime = CTimeConvert::fs_CreateTime(DateTime);
								DMibExpect(GeneratedTime, ==, Time)(ETestFlag_Aggregated);
							}
						}
					}
				;

				fCheckTurnaround("Around 1BC", CTimeConvert::fs_GetYearZero() - CTimeSpanConvert::fs_CreateDaySpan(365) * 3000, CTimeSpanConvert::fs_CreateDaySpan(365) * 6000);
				fCheckTurnaround("Around start of time", CTime::fs_StartOfTime() + CTimeSpanConvert::fs_CreateDaySpan(365), CTimeSpanConvert::fs_CreateDaySpan(365) * 6000);
				fCheckTurnaround("Around end of time", CTime::fs_EndOfTime() - CTimeSpanConvert::fs_CreateDaySpan(365) * 6001, CTimeSpanConvert::fs_CreateDaySpan(365) * 6000);
			};

			DMibTestSuite("UTCConversion")
			{
				auto fTestYear = [](int64 _Year)
					{
						DMibTestPath("Year {}"_f << _Year);

						auto OriginalTime = CTimeConvert::fs_CreateTime(_Year, 1, 10);

						DMibExpect(OriginalTime, ==, OriginalTime.f_ToLocal().f_ToUTC());
					}
				;

				DMibExpectViolatesRequire(fTestYear(constant_uint64(-7'514'938'706)));
				fTestYear(constant_uint64(-7'514'938'705));
				fTestYear(0);
				fTestYear(1599);
				fTestYear(1600);
				fTestYear(1601);
				fTestYear(1602);
				fTestYear(1800);
				fTestYear(1900);
				fTestYear(2000);
				fTestYear(6000);
				fTestYear(10000);
				fTestYear(20000);
				fTestYear(30000);
				fTestYear(30826);
				fTestYear(30827);
				fTestYear(30828);
				fTestYear(6000000);
				fTestYear(constant_uint64(577'039'110'548));
				DMibExpectViolatesRequire(fTestYear(constant_uint64(577'039'110'549)));
			};

			DMibTestSuite(CTestCategory("CheckUTCConversion") << CTestGroup("Manual"))
			{
				for (CTime Time = CTimeConvert::fs_CreateTime(1980); Time < CTimeConvert::fs_CreateTime(1981); Time += CTimeSpanConvert::fs_CreateMinuteSpan(15))
				{
					auto LocalTime = Time.f_ToLocal();
					auto UtcTime = LocalTime.f_ToUTC();
					if (UtcTime != Time)
						DMibConOut2("{tc5} {tc5} {tc5}\n", Time, LocalTime, UtcTime);
				}
				
				for (CTime Time = CTimeConvert::fs_CreateTime(2024); Time < CTimeConvert::fs_CreateTime(2025); Time += CTimeSpanConvert::fs_CreateMinuteSpan(15))
				{
					auto LocalTime = Time.f_ToLocal();
					auto UtcTime = LocalTime.f_ToUTC();
					if (UtcTime != Time)
						DMibConOut2("{tc5} {tc5} {tc5}\n", Time, LocalTime, UtcTime);
				}

				for (CTime Time = CTimeConvert::fs_CreateTime(2024); Time < CTimeConvert::fs_CreateTime(2025); Time += CTimeSpanConvert::fs_CreateMinuteSpan(15))
				{
					auto UtcTime = Time.f_ToUTC();
					auto LocalTime = UtcTime.f_ToLocal();
					if (LocalTime != Time)
						DMibConOut2("{tc5} {tc5} {tc5}\n", Time, LocalTime, UtcTime);
				}
			};
			
			constexpr static mint c_nLoops = 256 * 1024;

			DMibTestSuite(CTestCategory("UTCConversionPerformance") << CTestGroup("Performance"))
			{
				mint nTests = 9;

				CTestPerformanceMeasure NormalTime("Normal");
				CTestPerformanceMeasure LegacyTime("Legacy");
				{
					for(mint j = 0; j < nTests; ++j)
					{
						NormalTime.f_Start();
						[]() inline_never
							{
								CTime Now = CTime::fs_NowUTC();
								for (mint i = 0; i < c_nLoops / 2; ++i)
									Now = Now.f_ToUTC().f_ToLocal();
								return Now;
							}
							()
						;
						NormalTime.f_Stop(c_nLoops);
					}
				}
				{
					for(mint j = 0; j < nTests; ++j)
					{
						LegacyTime.f_Start();
						[]() inline_never
							{
								CTime Now = CTime::fs_NowLocal();
								for (mint i = 0; i < c_nLoops / 2; ++i)
									Now = Now.f_ToUtcLegacy().f_ToLocalLegacy();

								return Now;
							}
							()
						;
						LegacyTime.f_Stop(c_nLoops);
					}
				}

				CTestPerformance PerfTest(0.005);
				PerfTest.f_AddReference(LegacyTime);
				PerfTest.f_Add(NormalTime);
				DMibExpectTrue(PerfTest);
			};
		}
	};
}

DMibTestRegister(CTime_Tests, Malterlib::Time);
