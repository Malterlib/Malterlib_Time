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

#	if DMibEnableSafeCheck > 0
			DMibTestSuite("CreateTime range checks")
			{
				{
					DMibTestPath("Valid boundaries");
					// Month boundaries (1-12)
					DMibExpectNoException(CTimeConvert::fs_CreateTime(2024, 1));
					DMibExpectNoException(CTimeConvert::fs_CreateTime(2024, 12));

					// Day boundaries
					DMibExpectNoException(CTimeConvert::fs_CreateTime(2024, 1, 1));   // January 1st
					DMibExpectNoException(CTimeConvert::fs_CreateTime(2024, 1, 31));  // January 31st
					DMibExpectNoException(CTimeConvert::fs_CreateTime(2024, 2, 29));  // February 29th (leap year)
					DMibExpectNoException(CTimeConvert::fs_CreateTime(2023, 2, 28));  // February 28th (non-leap year)
					DMibExpectNoException(CTimeConvert::fs_CreateTime(2024, 4, 30));  // April 30th

					// Hour boundaries (0-23)
					DMibExpectNoException(CTimeConvert::fs_CreateTime(2024, 1, 1, 0));
					DMibExpectNoException(CTimeConvert::fs_CreateTime(2024, 1, 1, 23));

					// Minute boundaries (0-59)
					DMibExpectNoException(CTimeConvert::fs_CreateTime(2024, 1, 1, 0, 0));
					DMibExpectNoException(CTimeConvert::fs_CreateTime(2024, 1, 1, 0, 59));

					// Second boundaries (0-59)
					DMibExpectNoException(CTimeConvert::fs_CreateTime(2024, 1, 1, 0, 0, 0));
					DMibExpectNoException(CTimeConvert::fs_CreateTime(2024, 1, 1, 0, 0, 59));

					// Fraction boundaries (0.0 to <1.0)
					DMibExpectNoException(CTimeConvert::fs_CreateTime(2024, 1, 1, 0, 0, 0, 0.0));
					DMibExpectNoException(CTimeConvert::fs_CreateTime(2024, 1, 1, 0, 0, 0, 0.999999));
				}

				{
					DMibTestPath("Invalid month");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTime(2024, 0), "_Month >= 0 && _Month <= 11 'Month range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTime(2024, 13), "_Month >= 0 && _Month <= 11 'Month range check error'");
				}

				{
					DMibTestPath("Invalid day of month");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTime(2024, 1, 0), "_DayOfMonth >= 0 && _DayOfMonth < fs_GetDaysInMonth(_Year, _Month) 'Day of month range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTime(2024, 1, 32), "_DayOfMonth >= 0 && _DayOfMonth < fs_GetDaysInMonth(_Year, _Month) 'Day of month range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTime(2024, 2, 30), "_DayOfMonth >= 0 && _DayOfMonth < fs_GetDaysInMonth(_Year, _Month) 'Day of month range check error'");  // February leap year
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTime(2023, 2, 29), "_DayOfMonth >= 0 && _DayOfMonth < fs_GetDaysInMonth(_Year, _Month) 'Day of month range check error'");  // February non-leap year
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTime(2024, 4, 31), "_DayOfMonth >= 0 && _DayOfMonth < fs_GetDaysInMonth(_Year, _Month) 'Day of month range check error'");  // April has 30 days
				}

				{
					DMibTestPath("Invalid hour");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTime(2024, 1, 1, -1), "_Hour >= 0 && _Hour <= 23 'Hour range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTime(2024, 1, 1, 24), "_Hour >= 0 && _Hour <= 23 'Hour range check error'");
				}

				{
					DMibTestPath("Invalid minute");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTime(2024, 1, 1, 0, -1), "_Minute >= 0 && _Minute <= 59 'Minute range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTime(2024, 1, 1, 0, 60), "_Minute >= 0 && _Minute <= 59 'Minute range check error'");
				}

				{
					DMibTestPath("Invalid second");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTime(2024, 1, 1, 0, 0, -1), "_Second >= 0 && _Second <= 59 'Second range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTime(2024, 1, 1, 0, 0, 60), "_Second >= 0 && _Second <= 59 'Second range check error'");
				}

				{
					DMibTestPath("Invalid fraction");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTime(2024, 1, 1, 0, 0, 0, -0.1), "_Fraction >= 0.0 && _Fraction < 1.0 'Fraction range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTime(2024, 1, 1, 0, 0, 0, 1.0), "_Fraction >= 0.0 && _Fraction < 1.0 'Fraction range check error'");
				}
			};

			DMibTestSuite("CreateTimeIntFrac range checks")
			{
				{
					DMibTestPath("Valid boundaries");
					// Month boundaries (1-12)
					DMibExpectNoException(CTimeConvert::fs_CreateTimeIntFrac(2024, 1));
					DMibExpectNoException(CTimeConvert::fs_CreateTimeIntFrac(2024, 12));

					// Day boundaries
					DMibExpectNoException(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 1));   // January 1st
					DMibExpectNoException(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 31));  // January 31st
					DMibExpectNoException(CTimeConvert::fs_CreateTimeIntFrac(2024, 2, 29));  // February 29th (leap year)
					DMibExpectNoException(CTimeConvert::fs_CreateTimeIntFrac(2023, 2, 28));  // February 28th (non-leap year)
					DMibExpectNoException(CTimeConvert::fs_CreateTimeIntFrac(2024, 4, 30));  // April 30th

					// Hour boundaries (0-23)
					DMibExpectNoException(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 1, 0));
					DMibExpectNoException(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 1, 23));

					// Minute boundaries (0-59)
					DMibExpectNoException(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 1, 0, 0));
					DMibExpectNoException(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 1, 0, 59));

					// Second boundaries (0-59)
					DMibExpectNoException(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 1, 0, 0, 0));
					DMibExpectNoException(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 1, 0, 0, 59));

					// FractionInt boundaries (0 to mc_FractionDividend-1)
					DMibExpectNoException(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 1, 0, 0, 0, 0));
					DMibExpectNoException(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 1, 0, 0, 0, NTime::NPrivate::CConst::mc_FractionDividend - 1));
				}

				{
					DMibTestPath("Invalid month");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTimeIntFrac(2024, 0), "_Month >= 0 && _Month <= 11 'Month range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTimeIntFrac(2024, 13), "_Month >= 0 && _Month <= 11 'Month range check error'");
				}

				{
					DMibTestPath("Invalid day of month");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 0), "_DayOfMonth >= 0 && _DayOfMonth < fs_GetDaysInMonth(_Year, _Month) 'Day of month range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 32), "_DayOfMonth >= 0 && _DayOfMonth < fs_GetDaysInMonth(_Year, _Month) 'Day of month range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTimeIntFrac(2024, 2, 30), "_DayOfMonth >= 0 && _DayOfMonth < fs_GetDaysInMonth(_Year, _Month) 'Day of month range check error'");  // February leap year
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTimeIntFrac(2023, 2, 29), "_DayOfMonth >= 0 && _DayOfMonth < fs_GetDaysInMonth(_Year, _Month) 'Day of month range check error'");  // February non-leap year
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTimeIntFrac(2024, 4, 31), "_DayOfMonth >= 0 && _DayOfMonth < fs_GetDaysInMonth(_Year, _Month) 'Day of month range check error'");  // April has 30 days
				}

				{
					DMibTestPath("Invalid hour");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 1, -1), "_Hour >= 0 && _Hour <= 23 'Hour range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 1, 24), "_Hour >= 0 && _Hour <= 23 'Hour range check error'");
				}

				{
					DMibTestPath("Invalid minute");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 1, 0, -1), "_Minute >= 0 && _Minute <= 59 'Minute range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 1, 0, 60), "_Minute >= 0 && _Minute <= 59 'Minute range check error'");
				}

				{
					DMibTestPath("Invalid second");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 1, 0, 0, -1), "_Second >= 0 && _Second <= 59 'Second range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 1, 0, 0, 60), "_Second >= 0 && _Second <= 59 'Second range check error'");
				}

				{
					DMibTestPath("Invalid FractionInt");
					DMibExpectViolatesSafeCheck(CTimeConvert::fs_CreateTimeIntFrac(2024, 1, 1, 0, 0, 0, NTime::NPrivate::CConst::mc_FractionDividend), "_FractionInt >= 0 && _FractionInt < NPrivate::CConst::mc_FractionDividend 'Fraction range check error'");
				}
			};

			DMibTestSuite("CreateTime_ISOWeek range checks")
			{
				{
					DMibTestPath("Valid boundaries");
					// Week boundaries (1-53)
					DMibExpectNoException(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1));
					DMibExpectNoException(CTimeConvert_ISOWeek::fs_CreateTime(2024, 53));

					// Day of week boundaries (0-6)
					DMibExpectNoException(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0));
					DMibExpectNoException(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 6));

					// Hour boundaries (0-23)
					DMibExpectNoException(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0, 0));
					DMibExpectNoException(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0, 23));

					// Minute boundaries (0-59)
					DMibExpectNoException(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0, 0, 0));
					DMibExpectNoException(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0, 0, 59));

					// Second boundaries (0-59)
					DMibExpectNoException(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0, 0, 0, 0));
					DMibExpectNoException(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0, 0, 0, 59));

					// Fraction boundaries (0.0 to <1.0)
					DMibExpectNoException(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0, 0, 0, 0, 0.0));
					DMibExpectNoException(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0, 0, 0, 0, 0.999999));
				}

				{
					DMibTestPath("Invalid week");
					DMibExpectViolatesSafeCheck(CTimeConvert_ISOWeek::fs_CreateTime(2024, 0), "_Week >= 0 && _Week <= 52 'Week range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert_ISOWeek::fs_CreateTime(2024, 54), "_Week >= 0 && _Week <= 52 'Week range check error'");
				}

				{
					DMibTestPath("Invalid day of week");
					DMibExpectViolatesSafeCheck(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, -1), "_DayOfWeek >= 0 && _DayOfWeek <= 6 'Day of week range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 7), "_DayOfWeek >= 0 && _DayOfWeek <= 6 'Day of week range check error'");
				}

				{
					DMibTestPath("Invalid hour");
					DMibExpectViolatesSafeCheck(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0, -1), "_Hour >= 0 && _Hour <= 23 'Hour range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0, 24), "_Hour >= 0 && _Hour <= 23 'Hour range check error'");
				}

				{
					DMibTestPath("Invalid minute");
					DMibExpectViolatesSafeCheck(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0, 0, -1), "_Minute >= 0 && _Minute <= 59 'Minute range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0, 0, 60), "_Minute >= 0 && _Minute <= 59 'Minute range check error'");
				}

				{
					DMibTestPath("Invalid second");
					DMibExpectViolatesSafeCheck(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0, 0, 0, -1), "_Second >= 0 && _Second <= 59 'Second range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0, 0, 0, 60), "_Second >= 0 && _Second <= 59 'Second range check error'");
				}

				{
					DMibTestPath("Invalid fraction");
					DMibExpectViolatesSafeCheck(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0, 0, 0, 0, -0.1), "_Fraction >= 0.0 && _Fraction < 1.0 'Fraction range check error'");
					DMibExpectViolatesSafeCheck(CTimeConvert_ISOWeek::fs_CreateTime(2024, 1, 0, 0, 0, 0, 1.0), "_Fraction >= 0.0 && _Fraction < 1.0 'Fraction range check error'");
				}
			};
#endif

			DMibTestSuite("TryParseDateTimeStr")
			{
				{
					DMibTestPath("Valid full format");
					auto Result = fg_TryParseDateTimeStr("2024-03-15 14:30:45.123");
					DMibExpectTrue(Result);
					auto DateTime = CTimeConvert(*Result).f_ExtractDateTime();
					DMibExpect(DateTime.m_Year, ==, 2024);
					DMibExpect(DateTime.m_Month, ==, 3);
					DMibExpect(DateTime.m_DayOfMonth, ==, 15);
					DMibExpect(DateTime.m_Hour, ==, 14);
					DMibExpect(DateTime.m_Minute, ==, 30);
					DMibExpect(DateTime.m_Second, ==, 45);
				}

				{
					DMibTestPath("Valid date only");
					auto Result = fg_TryParseDateTimeStr("2024-03-15");
					DMibExpectTrue(Result);
					auto DateTime = CTimeConvert(*Result).f_ExtractDateTime();
					DMibExpect(DateTime.m_Year, ==, 2024);
					DMibExpect(DateTime.m_Month, ==, 3);
					DMibExpect(DateTime.m_DayOfMonth, ==, 15);
					DMibExpect(DateTime.m_Hour, ==, 0);
					DMibExpect(DateTime.m_Minute, ==, 0);
					DMibExpect(DateTime.m_Second, ==, 0);
				}

				{
					DMibTestPath("Valid date and hour:minute");
					auto Result = fg_TryParseDateTimeStr("2024-03-15 14:30");
					DMibExpectTrue(Result);
					auto DateTime = CTimeConvert(*Result).f_ExtractDateTime();
					DMibExpect(DateTime.m_Year, ==, 2024);
					DMibExpect(DateTime.m_Month, ==, 3);
					DMibExpect(DateTime.m_DayOfMonth, ==, 15);
					DMibExpect(DateTime.m_Hour, ==, 14);
					DMibExpect(DateTime.m_Minute, ==, 30);
					DMibExpect(DateTime.m_Second, ==, 0);
				}

				{
					DMibTestPath("Valid leap year February 29");
					auto Result = fg_TryParseDateTimeStr("2024-02-29");
					DMibExpectTrue(Result);
					auto DateTime = CTimeConvert(*Result).f_ExtractDateTime();
					DMibExpect(DateTime.m_Year, ==, 2024);
					DMibExpect(DateTime.m_Month, ==, 2);
					DMibExpect(DateTime.m_DayOfMonth, ==, 29);
				}

				{
					DMibTestPath("Valid boundary values");
					auto Result1 = fg_TryParseDateTimeStr("2024-01-01 00:00:00.000");
					DMibExpectTrue(Result1);
					auto Result2 = fg_TryParseDateTimeStr("2024-12-31 23:59:59.999");
					DMibExpectTrue(Result2);
				}

				{
					DMibTestPath("Invalid format - too few fields");
					auto Result = fg_TryParseDateTimeStr("2024-03");
					DMibExpectException(Result.f_Access(), DMibErrorInstance("Invalid date format - expected: YYYY-MM-DD [HH:MM[:SS[.FFF]]]"));
				}

				{
					DMibTestPath("Invalid month - zero");
					auto Result = fg_TryParseDateTimeStr("2024-00-15");
					DMibExpectException(Result.f_Access(), DMibErrorInstance("Month 0 is out of range (1-12)"));
				}

				{
					DMibTestPath("Invalid month - 13");
					auto Result = fg_TryParseDateTimeStr("2024-13-15");
					DMibExpectException(Result.f_Access(), DMibErrorInstance("Month 13 is out of range (1-12)"));
				}

				{
					DMibTestPath("Invalid day - zero");
					auto Result = fg_TryParseDateTimeStr("2024-03-00");
					DMibExpectException(Result.f_Access(), DMibErrorInstance("Day 0 is out of range for month 3 (1-31)"));
				}

				{
					DMibTestPath("Invalid day - 32 for January");
					auto Result = fg_TryParseDateTimeStr("2024-01-32");
					DMibExpectException(Result.f_Access(), DMibErrorInstance("Day 32 is out of range for month 1 (1-31)"));
				}

				{
					DMibTestPath("Invalid day - February 29 non-leap year");
					auto Result = fg_TryParseDateTimeStr("2023-02-29");
					DMibExpectException(Result.f_Access(), DMibErrorInstance("Day 29 is out of range for month 2 (1-28)"));
				}

				{
					DMibTestPath("Invalid day - April 31");
					auto Result = fg_TryParseDateTimeStr("2024-04-31");
					DMibExpectException(Result.f_Access(), DMibErrorInstance("Day 31 is out of range for month 4 (1-30)"));
				}

				{
					DMibTestPath("Invalid hour - 24");
					auto Result = fg_TryParseDateTimeStr("2024-03-15 24:00:00");
					DMibExpectException(Result.f_Access(), DMibErrorInstance("Hour 24 is out of range (0-23)"));
				}

				{
					DMibTestPath("Invalid minute - 60");
					auto Result = fg_TryParseDateTimeStr("2024-03-15 12:60:00");
					DMibExpectException(Result.f_Access(), DMibErrorInstance("Minute 60 is out of range (0-59)"));
				}

				{
					DMibTestPath("Invalid second - 60");
					auto Result = fg_TryParseDateTimeStr("2024-03-15 12:30:60");
					DMibExpectException(Result.f_Access(), DMibErrorInstance("Second 60 is out of range (0-59)"));
				}

				{
					DMibTestPath("Invalid fraction - 1000");
					auto Result = fg_TryParseDateTimeStr("2024-03-15 12:30:45.1000");
					DMibExpectException(Result.f_Access(), DMibErrorInstance("Fraction 1000 is out of range (0-999)"));
				}
			};

			DMibTestSuite("Time precision")
			{
				CTime Now = CTime::fs_NowUTC();
				NMib::NSys::fg_Thread_Sleep(NTime::NPlatform::fg_TimeRaw_Resolution());
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
				NTime::NPlatform::fg_TimeRaw_GetNow(&StartRaw);

				NMib::NSys::fg_Thread_Sleep(g_CheckTime);

				CTime End = CTime::fs_NowUTC();
				CTime EndRaw;
				NTime::NPlatform::fg_TimeRaw_GetNow(&EndRaw);

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
				int64 StartRaw = NTime::NPlatform::fg_TimerRaw_PreciseGet();

				NMib::NSys::fg_Thread_Sleep(1.0f);

				int64 End = CSystem_Time::fs_GetTimerValue();
				int64 EndRaw = NTime::NPlatform::fg_TimerRaw_PreciseGet();

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
			};

			DMibTestCategory("Turnaround")
			{
				auto fCheckTurnaround = [](CStr const &_Path, CTime const &_StartTime, CTimeSpan const &_Duration)
					{
						DMibTestSuite(_Path)
						{
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
						};
					}
				;

	#if defined(DMibDebug) || defined(DMibSanitizerEnabled)
				fCheckTurnaround("Around 1BC", CTimeConvert::fs_GetYearZero() - CTimeSpanConvert::fs_CreateDaySpan(365) * 300, CTimeSpanConvert::fs_CreateDaySpan(365) * 600);
				fCheckTurnaround("Around start of time", CTime::fs_StartOfTime() + CTimeSpanConvert::fs_CreateDaySpan(365), CTimeSpanConvert::fs_CreateDaySpan(365) * 600);
				fCheckTurnaround("Around end of time", CTime::fs_EndOfTime() - CTimeSpanConvert::fs_CreateDaySpan(365) * 601, CTimeSpanConvert::fs_CreateDaySpan(365) * 600);
	#else
				fCheckTurnaround("Around 1BC", CTimeConvert::fs_GetYearZero() - CTimeSpanConvert::fs_CreateDaySpan(365) * 3000, CTimeSpanConvert::fs_CreateDaySpan(365) * 6000);
				fCheckTurnaround("Around start of time", CTime::fs_StartOfTime() + CTimeSpanConvert::fs_CreateDaySpan(365), CTimeSpanConvert::fs_CreateDaySpan(365) * 6000);
				fCheckTurnaround("Around end of time", CTime::fs_EndOfTime() - CTimeSpanConvert::fs_CreateDaySpan(365) * 6001, CTimeSpanConvert::fs_CreateDaySpan(365) * 6000);
	#endif
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

			DMibTestSuite("UTCConversionFractions")
			{
				auto fTestFraction = [](int64 _Year, fp64 _Fraction)
					{
						DMibTestPath("Year {} Fraction {}"_f << _Year << _Fraction);

						auto OriginalTime = CTimeConvert::fs_CreateTime(_Year, 6, 15, 12, 30, 45, _Fraction);
						auto LocalTime = OriginalTime.f_ToLocal();
						auto RoundTrip = LocalTime.f_ToUTC();

						DMibExpect(OriginalTime, ==, RoundTrip);
					}
				;

				auto fTestFractionInt = [](int64 _Year, uint64 _FractionInt)
					{
						DMibTestPath("Year {} FractionInt {}"_f << _Year << _FractionInt);

						auto OriginalTime = CTimeConvert::fs_CreateTimeIntFrac(_Year, 6, 15, 12, 30, 45, _FractionInt);
						auto LocalTime = OriginalTime.f_ToLocal();
						auto RoundTrip = LocalTime.f_ToUTC();

						DMibExpect(OriginalTime, ==, RoundTrip);
					}
				;

				// Test various fractions close to 1.0
				for (int64 Year : {2000, 2024})
				{
					fTestFraction(Year, 0.0);
					fTestFraction(Year, 0.5);
					fTestFraction(Year, 0.999);
					fTestFraction(Year, 0.9999);
					fTestFraction(Year, 0.99999);
					fTestFraction(Year, 0.999999);
					fTestFraction(Year, 0.9999999);
				}

				// Test using raw fraction integers near maximum
				constexpr uint64 MaxFrac = NTime::NPrivate::CConst::mc_FractionDividend - 1;
				for (int64 Year : {2000, 2024})
				{
					fTestFractionInt(Year, MaxFrac);
					fTestFractionInt(Year, MaxFrac - 10);
					fTestFractionInt(Year, MaxFrac - 100);
					fTestFractionInt(Year, MaxFrac - 1000);
				}
			};

			DMibTestSuite(CTestCategory("CheckUTCConversion") << CTestGroup("Manual"))
			{
				for (CTime Time = CTimeConvert::fs_CreateTime(1980); Time < CTimeConvert::fs_CreateTime(1981); Time += CTimeSpanConvert::fs_CreateMinuteSpan(15))
				{
					auto LocalTime = Time.f_ToLocal();
					auto UtcTime = LocalTime.f_ToUTC();
					if (UtcTime != Time)
						DMibConOut("{tc5} {tc5} {tc5}\n", Time, LocalTime, UtcTime);
				}

				for (CTime Time = CTimeConvert::fs_CreateTime(2024); Time < CTimeConvert::fs_CreateTime(2025); Time += CTimeSpanConvert::fs_CreateMinuteSpan(15))
				{
					auto LocalTime = Time.f_ToLocal();
					auto UtcTime = LocalTime.f_ToUTC();
					if (UtcTime != Time)
						DMibConOut("{tc5} {tc5} {tc5}\n", Time, LocalTime, UtcTime);
				}

				for (CTime Time = CTimeConvert::fs_CreateTime(2024); Time < CTimeConvert::fs_CreateTime(2025); Time += CTimeSpanConvert::fs_CreateMinuteSpan(15))
				{
					auto UtcTime = Time.f_ToUTC();
					auto LocalTime = UtcTime.f_ToLocal();
					if (LocalTime != Time)
						DMibConOut("{tc5} {tc5} {tc5}\n", Time, LocalTime, UtcTime);
				}
			};

			DMibTestSuite("TimeSpan")
			{
 				auto StartTime = CTimeConvert::fs_CreateTime(2001, 3, 14, 12, 53, 50, 0.5);
				auto EndTime = CTimeConvert::fs_CreateTime(2002, 4, 15, 13, 54, 59, 0.6);
				auto PositiveTimeSpan = EndTime - StartTime;
				auto NegativeTimeSpan = StartTime - EndTime;

				DMibExpect(StartTime + PositiveTimeSpan, ==, EndTime);
				DMibExpect(EndTime + NegativeTimeSpan, ==, StartTime);
				DMibExpect(EndTime - PositiveTimeSpan, ==, StartTime);
				DMibExpect(StartTime - NegativeTimeSpan, ==, EndTime);

				DMibExpect(PositiveTimeSpan, ==, -NegativeTimeSpan);
				DMibExpect(-PositiveTimeSpan, ==, NegativeTimeSpan);

				DMibExpect(CTimeSpanConvert::fs_CreateDaySpan(2).f_GetSeconds(), ==, 60 * 60 * 24 * 2);
				DMibExpect(CTimeSpanConvert::fs_CreateDaySpan(-2).f_GetSeconds(), ==, -60 * 60 * 24 * 2);

				DMibExpect(PositiveTimeSpan.f_GetSeconds(), ==, 34304469);
				DMibExpect(PositiveTimeSpan.f_GetSecondsFraction(), ==, 34304469.1);
				DMibExpect(NegativeTimeSpan.f_GetSeconds(), ==, -34304470);
				DMibExpect(NegativeTimeSpan.f_GetSecondsFraction(), ==, -34304469.1);

				DMibExpect(CTimeSpanConvert(PositiveTimeSpan).f_GetWeeks(), ==, 56);
				DMibExpectTrue(CTimeSpanConvert(PositiveTimeSpan).f_GetWeeksFloat().f_AlmostEqual(56.7203523478836, 2));
				DMibExpect(CTimeSpanConvert(PositiveTimeSpan).f_GetDays(), ==, 397);
				DMibExpectTrue(CTimeSpanConvert(PositiveTimeSpan).f_GetDaysFloat().f_AlmostEqual(397.042466435185, 2));
				DMibExpect(CTimeSpanConvert(PositiveTimeSpan).f_GetHours(), ==, 9529);
				DMibExpectTrue(CTimeSpanConvert(PositiveTimeSpan).f_GetHoursFloat().f_AlmostEqual(9529.01919444445, 2));
				DMibExpect(CTimeSpanConvert(PositiveTimeSpan).f_GetMinutes(), ==, 571741);
				DMibExpectTrue(CTimeSpanConvert(PositiveTimeSpan).f_GetMinutesFloat().f_AlmostEqual(571741.151666667, 2));
				DMibExpect(CTimeSpanConvert(PositiveTimeSpan).f_GetSeconds(), ==, 34304469);
				DMibExpect(CTimeSpanConvert(PositiveTimeSpan).f_GetSecondsFloat(), ==, 34304469.1);
				DMibExpect(CTimeSpanConvert(PositiveTimeSpan).f_GetHourOfDay(), ==, 1);
				DMibExpect(CTimeSpanConvert(PositiveTimeSpan).f_GetMinuteOfHour(), ==, 1);
				DMibExpect(CTimeSpanConvert(PositiveTimeSpan).f_GetSecondOfMinute(), ==, 9);
				DMibExpect(CTimeSpanConvert(PositiveTimeSpan).f_GetFraction(), ==, 0.1);

				DMibExpect(CTimeSpanConvert(NegativeTimeSpan).f_GetWeeks(), ==, -56);
				DMibExpectTrue(CTimeSpanConvert(NegativeTimeSpan).f_GetWeeksFloat().f_AlmostEqual(-56.7203523478836, 2));
				DMibExpect(CTimeSpanConvert(NegativeTimeSpan).f_GetDays(), ==, -397);
				DMibExpectTrue(CTimeSpanConvert(NegativeTimeSpan).f_GetDaysFloat().f_AlmostEqual(-397.042466435185, 2));
				DMibExpect(CTimeSpanConvert(NegativeTimeSpan).f_GetHours(), ==, -9529);
				DMibExpectTrue(CTimeSpanConvert(NegativeTimeSpan).f_GetHoursFloat().f_AlmostEqual(-9529.01919444445, 2));
				DMibExpect(CTimeSpanConvert(NegativeTimeSpan).f_GetMinutes(), ==, -571741);
				DMibExpectTrue(CTimeSpanConvert(NegativeTimeSpan).f_GetMinutesFloat().f_AlmostEqual(-571741.151666667, 2));
				DMibExpect(CTimeSpanConvert(NegativeTimeSpan).f_GetSeconds(), ==, -34304469);
				DMibExpect(CTimeSpanConvert(NegativeTimeSpan).f_GetSecondsFloat(), ==, -34304469.1);
				DMibExpect(CTimeSpanConvert(NegativeTimeSpan).f_GetHourOfDay(), ==, 1);
				DMibExpect(CTimeSpanConvert(NegativeTimeSpan).f_GetMinuteOfHour(), ==, 1);
				DMibExpect(CTimeSpanConvert(NegativeTimeSpan).f_GetSecondOfMinute(), ==, 9);
				DMibExpect(CTimeSpanConvert(NegativeTimeSpan).f_GetFraction(), ==, 0.1);

				DMibExpect(CTimeSpanConvert(-CTimeSpanConvert::fs_CreateSpan(1, 0, 23, 59, 59, 0.99999)).f_GetWeeks(), ==, -1);
				DMibExpect(CTimeSpanConvert(-CTimeSpanConvert::fs_CreateSpan(0, 1, 23, 59, 59, 0.99999)).f_GetDays(), ==, -1);
				DMibExpect(CTimeSpanConvert(-CTimeSpanConvert::fs_CreateSpan(0, 0,  1, 59, 59, 0.99999)).f_GetHours(), ==, -1);
				DMibExpect(CTimeSpanConvert(-CTimeSpanConvert::fs_CreateSpan(0, 0,  0,  1, 59, 0.99999)).f_GetMinutes(), ==, -1);
				DMibExpect(CTimeSpanConvert(-CTimeSpanConvert::fs_CreateSpan(0, 0,  0,  0,  1, 0.99999)).f_GetSeconds(), ==, -1);
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
