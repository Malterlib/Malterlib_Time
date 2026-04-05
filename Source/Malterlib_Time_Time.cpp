// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

using namespace NMib::NStr;

namespace NMib::NTime
{
#ifdef DMibDebuggerHelpers
	assure_used ch8 const* CTime::fsp_DebugStr(void *_pTime)
	{
		CTime& Time = *((CTime*)_pTime);
		static NStr::CFStr256 s_FromatTo;

		if (!Time.f_IsValid())
		{
			s_FromatTo = "Invalid";
			return s_FromatTo;
		}

		if (Time == CTime::fs_StartOfTime())
		{
			s_FromatTo = "Start of time";
			return s_FromatTo;
		}

		if (Time == CTime::fs_EndOfTime())
		{
			s_FromatTo = "End of time";
			return s_FromatTo;
		}

		if (Time < CTimeConvert::fs_CreateTime(0))
		{
			s_FromatTo = "1 BC or earlier is not supported";
			return s_FromatTo;
		}

		NTime::CTimeConvert::CDateTime DateTime;
		NTime::CTimeConvert(Time).f_ExtractDateTime(DateTime);

		int32 Fraction = (DateTime.m_Fraction*1000.0).f_ToIntRound();
		if (Fraction >= 1000)
			Fraction = 999;

		s_FromatTo = NStr::CFStr256::CFormat("{}-{sj2,sf0}-{sj2,sf0} {sj2,sf0}:{sj2,sf0}:{sj2,sf0}.{sj3,sf0}")
			<< DateTime.m_Year
			<< DateTime.m_Month
			<< DateTime.m_DayOfMonth
			<< DateTime.m_Hour
			<< DateTime.m_Minute
			<< DateTime.m_Second
			<< Fraction
		;

		return s_FromatTo;
	}

	assure_used ch8 const* CTimeSpan::fsp_DebugStr(void *_pTimeSpan)
	{
		static NStr::CFStr256 s_FromatTo;

		CTimeSpan& TimeSpan = *((CTimeSpan*)_pTimeSpan);


		NTime::CTimeSpanConvert Convert(TimeSpan);

		s_FromatTo = NStr::fg_Format<NStr::CFStr256>
			(
				"{} d {} h {} m {} s   {fn3,fe0}w  {fn3,fe0}d  {fn3,fe0}h  {fn3,fe0}m  {fn3,fe0}s"
				, Convert.f_GetDays()
				, Convert.f_GetHourOfDay()
				, Convert.f_GetMinuteOfHour()
				, fp64(Convert.f_GetSecondOfMinute()) + Convert.f_GetFraction()
				, Convert.f_GetWeeksFloat()
				, Convert.f_GetDaysFloat()
				, Convert.f_GetHoursFloat()
				, Convert.f_GetMinutesFloat()
				, Convert.f_GetSecondsFloat()
			)
		;

		return s_FromatTo;
	}
#endif

	NMib::NStr::CStr fg_GetFullTimeStr(CTime const &_Time)
	{
		NTime::CTimeConvert::CDateTime DateTime;
		NTime::CTimeConvert(_Time).f_ExtractDateTime(DateTime);

		int32 Fraction = (DateTime.m_Fraction*1000.0).f_ToIntRound();
		if (Fraction >= 1000)
			Fraction = 999;

		return "{}-{sj2,sf0}-{sj2,sf0} {sj2,sf0}:{sj2,sf0}:{sj2,sf0}.{sj3,sf0}"_f
			<< DateTime.m_Year
			<< DateTime.m_Month
			<< DateTime.m_DayOfMonth
			<< DateTime.m_Hour
			<< DateTime.m_Minute
			<< DateTime.m_Second
			<< Fraction
		;
	}

	bool fg_ParseFullTimeStr(CTime &_Time, NMib::NStr::CStr const &_Str)
	{
		NTime::CTimeConvert::CDateTime DateTime;

		int32 Fraction;
		aint nParsed;
		(
			NMib::NStr::CStr::CParse("{}-{}-{} {}:{}:{}.{}")
			>> DateTime.m_Year
			>> DateTime.m_Month
			>> DateTime.m_DayOfMonth
			>> DateTime.m_Hour
			>> DateTime.m_Minute
			>> DateTime.m_Second
			>> Fraction
		).f_Parse(_Str, nParsed);

		if (nParsed != 7)
			return false;

		DateTime.m_Fraction = fp64(Fraction) / 1000.0;

		_Time = NTime::CTimeConvert::fs_CreateTime(DateTime);

		return true;
	}

	NMib::NConcurrency::TCAsyncResult<CTime> fg_TryParseDateTimeStr(NMib::NStr::CStr const &_Str)
	{
		using namespace NMib::NStr;

		aint nParsed = 0;
		int64 Year = 0;
		int32 Month = 0;
		int32 Day = 0;
		int32 Hour = 0;
		int32 Minute = 0;
		int32 Second = 0;
		int32 Fraction = 0;

		(CStr::CParse("{}-{}-{} {}:{}:{}.{}") >> Year >> Month >> Day >> Hour >> Minute >> Second >> Fraction).f_Parse(_Str, nParsed);

		NMib::NConcurrency::TCAsyncResult<CTime> Return;

		if (nParsed < 3)
		{
			Return.f_SetException(DMibErrorInstance("Invalid date format - expected: YYYY-MM-DD [HH:MM[:SS[.FFF]]]"));
			return Return;
		}

		if (Month < 1 || Month > 12)
		{
			Return.f_SetException(DMibErrorInstance("Month {} is out of range (1-12)"_f << Month));
			return Return;
		}

		aint nDaysInMonth = NTime::CTimeConvert::fs_GetDaysInMonth(Year, Month - 1);
		if (Day < 1 || Day > nDaysInMonth)
		{
			Return.f_SetException(DMibErrorInstance("Day {} is out of range for month {} (1-{})"_f << Day << Month << nDaysInMonth));
			return Return;
		}

		if (Hour < 0 || Hour > 23)
		{
			Return.f_SetException(DMibErrorInstance("Hour {} is out of range (0-23)"_f << Hour));
			return Return;
		}

		if (Minute < 0 || Minute > 59)
		{
			Return.f_SetException(DMibErrorInstance("Minute {} is out of range (0-59)"_f << Minute));
			return Return;
		}

		if (Second < 0 || Second > 59)
		{
			Return.f_SetException(DMibErrorInstance("Second {} is out of range (0-59)"_f << Second));
			return Return;
		}

		if (Fraction < 0 || Fraction > 999)
		{
			Return.f_SetException(DMibErrorInstance("Fraction {} is out of range (0-999)"_f << Fraction));
			return Return;
		}

		Return.f_SetResult(NTime::CTimeConvert::fs_CreateTime(Year, Month, Day, Hour, Minute, Second, fp64(Fraction) / 1000.0));
		return Return;
	}

	constexpr static const ch8 *gc_Months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	constexpr static const ch8 *gc_Days[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};

	int32 fg_GetAscMonthNumber(NStr::CStr const &_Month)
	{
		for (umint i = 0; i < 12; ++i)
		{
			if (_Month == gc_Months[i])
				return i + 1;
		}

		return -1;
	}

	// Same format as std. C lib's asctime.
	NMib::NStr::CStr fg_GetAscTimeStr(CTime const &_Time)
	{
		NTime::CTimeConvert::CDateTime DateTime;
		NTime::CTimeConvert(_Time).f_ExtractDateTime(DateTime);

		return "{} {} {} {sj2,sf0}:{sj2,sf0}:{sj2,sf0} {}"_f
			<< gc_Days[DateTime.m_DayOfWeek]
			<< gc_Months[DateTime.m_Month-1]
			<< DateTime.m_DayOfMonth
			<< DateTime.m_Hour
			<< DateTime.m_Minute
			<< DateTime.m_Second
			<< DateTime.m_Year
		;
	}

	NMib::NStr::CStr fg_GetISO8601TimeStr(CTime const &_Time)
	{
		using namespace NMib::NStr;

		auto DateTime = CTimeConvert{_Time}.f_ExtractDateTime();

		return "{}{sj2,sf0}{sj2,sf0}T{sj2,sf0}{sj2,sf0}{sj2,sf0}Z"_f
			<< DateTime.m_Year
			<< DateTime.m_Month
			<< DateTime.m_DayOfMonth
			<< DateTime.m_Hour
			<< DateTime.m_Minute
			<< DateTime.m_Second
		;
	}

	CTimeSpan CTimeSpan::fs_UTCOffset()
	{
		CTimeSpan UTCOffset;
		CSystem_Time::fs_TimeGetUTCOffset(&UTCOffset);
		return UTCOffset;
	}

	CTime CTime::f_ToUtcLegacy() const
	{
		CTime Ret = *this;
		if (!Ret.f_IsValid())
			return Ret;
		CTimeSpan UTCOffset;
		CSystem_Time::fs_TimeGetUTCOffset(&UTCOffset);
		Ret -= UTCOffset;
		return Ret;
	}

	CTime CTime::f_ToLocalLegacy() const
	{
		CTime Ret = *this;
		if (!Ret.f_IsValid())
			return Ret;
		CTimeSpan UTCOffset;
		CSystem_Time::fs_TimeGetUTCOffset(&UTCOffset);
		Ret += UTCOffset;
		return Ret;
	}

	CTime CTime::f_ToUTC() const
	{
		if (!f_IsValid())
			return *this;
		return CSystem_Time::fs_TimeToUtc(*this);
	}

	CTime CTime::f_ToLocal() const
	{
		if (!f_IsValid())
			return *this;
		return CSystem_Time::fs_TimeToLocal(*this);
	}

	CTime CTime::fs_NowLocal()
	{
		CTime Ret;
		CSystem_Time::fs_TimeGetNowUTC(&Ret);

		CTimeSpan UTCOffset;
		CSystem_Time::fs_TimeGetUTCOffset(&UTCOffset);
		Ret += UTCOffset;

		return Ret;
	}

	CTime CTime::fs_NowUTC()
	{
		CTime Ret;
		CSystem_Time::fs_TimeGetNowUTC(&Ret);

		return Ret;
	}

	int64 CTime::fs_GetResolution()
	{
		return CSystem_Time::fs_TimeResolution();
	}

	NStr::CStr fg_SecondsDurationToHumanReadable(fp64 const &_Seconds)
	{
		NStr::CStr Return;
		fg_FormatSecondsDurationToHumanReadable(Return, _Seconds);
		return Return;
	}
}
