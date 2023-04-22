// Copyright © 2015 Hansoft AB 
// Distributed under the MIT license, see license text in LICENSE.Malterlib

namespace NMib::NTime
{
#ifndef DCompiler_MSVC
	const int32 CTimeConvert_ProlepticGreogrian::ms_MonthDayOfYear[12];// = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
	const int32 CTimeConvert_ProlepticGreogrian::ms_DaysInMonth[12];// = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
#endif

#ifdef DMibDebuggerHelpers
	assure_used ch8 const* CTime::fsp_DebugStr(void* _pTime)
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
		s_FromatTo = (NStr::CFStr256::CFormat("{}-{sj2,sf0}-{sj2,sf0} {sj2,sf0}:{sj2,sf0}:{sj2,sf0}.{sj3,sf0}") << DateTime.m_Year << DateTime.m_Month << DateTime.m_DayOfMonth << DateTime.m_Hour << DateTime.m_Minute << DateTime.m_Second << Fraction);

		return s_FromatTo;
	}

	assure_used ch8 const* CTimeSpan::fsp_DebugStr(void* _pTimeSpan)
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
		return (NMib::NStr::CStr::CFormat("{}-{sj2,sf0}-{sj2,sf0} {sj2,sf0}:{sj2,sf0}:{sj2,sf0}.{sj3,sf0}") << DateTime.m_Year << DateTime.m_Month << DateTime.m_DayOfMonth << DateTime.m_Hour << DateTime.m_Minute << DateTime.m_Second << Fraction).f_GetStr();
	}

	bool fg_ParseFullTimeStr(CTime &_Time, NMib::NStr::CStr const& _Str)
	{
		NTime::CTimeConvert::CDateTime DateTime;

		int32 Fraction;
		aint nParsed;
		(NMib::NStr::CStr::CParse("{}-{}-{} {}:{}:{}.{}") >> DateTime.m_Year >> DateTime.m_Month >> DateTime.m_DayOfMonth >> DateTime.m_Hour >> DateTime.m_Minute >> DateTime.m_Second >> Fraction )
			.f_Parse(_Str, nParsed) ;
		if (nParsed != 7)
			return false;
		DateTime.m_Fraction = fp64(Fraction) / 1000.0;

		_Time = NTime::CTimeConvert::fs_CreateTime(DateTime);
		return true;
	}

	int32 fg_GetAscMonthNumber(NStr::CStr const &_Month)
	{
		static const char *Months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
		for (mint i = 0; i < 12; ++i)
		{
			if (_Month == Months[i])
				return i + 1;
		}
		return -1;
	}

	// Same format as std. C lib's asctime.
	NMib::NStr::CStr fg_GetAscTimeStr(CTime const &_Time)
	{
		static const char *Days[] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
		static const char *Months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

		NTime::CTimeConvert::CDateTime DateTime;
		NTime::CTimeConvert(_Time).f_ExtractDateTime(DateTime);
		return (NMib::NStr::CStr::CFormat("{} {} {} {sj2,sf0}:{sj2,sf0}:{sj2,sf0} {}") << Days[DateTime.m_DayOfWeek] << Months[DateTime.m_Month-1] << DateTime.m_DayOfMonth << DateTime.m_Hour << DateTime.m_Minute << DateTime.m_Second << DateTime.m_Year).f_GetStr();
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

	CTime CTime::f_ToUTC() const
	{
		CTime Ret = *this;
		if (!Ret.f_IsValid())
			return Ret;
		CTimeSpan UTCOffset;
		CSystem_Time::fs_TimeGetUTCOffset(&UTCOffset);
		Ret -= UTCOffset;
		return Ret;
	}

	CTime CTime::f_ToLocal() const
	{
		CTime Ret = *this;
		if (!Ret.f_IsValid())
			return Ret;
		CTimeSpan UTCOffset;
		CSystem_Time::fs_TimeGetUTCOffset(&UTCOffset);
		Ret += UTCOffset;
		return Ret;
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

	uint64 CTimeConvert_BabylonianCommon::f_UnixMilliseconds() const
	{
		uint64 Return = (m_pTime->f_GetSeconds() - constant_int64(237148622167132800)) * constant_int64(1000);
		Return += (m_pTime->f_GetFraction() * 1000.0).f_ToIntRound();
		return Return;
	}

	CTime CTimeConvert_BabylonianCommon::fs_FromUnixMilliseconds(uint64 _Milliseconds)
	{
		uint64 Seconds = _Milliseconds / constant_int64(1000);
		uint64 Fraction = (_Milliseconds - Seconds * constant_int64(1000));

		CTime Ret;
		Ret.f_SetSeconds(237148622167132800 + Seconds);
		Ret.f_SetFraction(fp64(Fraction) / 1000.0);

		return Ret;
	}

	uint64 CTimeConvert_BabylonianCommon::f_UnixSeconds() const
	{
		return m_pTime->f_GetSeconds() - 237148622167132800;
	}

	uint64 CTimeConvert_BabylonianCommon::f_UnixMinutes() const
	{
		return f_UnixSeconds() / 60;
	}

	CTime CTimeConvert_BabylonianCommon::fs_FromUnixMinutes(uint64 _Minutes)
	{
		CTime Ret;
		Ret.f_SetSeconds(237148622167132800 + _Minutes * 60);
		return Ret;
	}

	CTime CTimeConvert_BabylonianCommon::fs_FromUnixSeconds(uint64 _Seconds)
	{
		CTime Ret;
		Ret.f_SetSeconds(237148622167132800 + _Seconds);
		return Ret;
	}

	fp64 CTimeConvert_BabylonianCommon::f_UnixSecondsFraction() const
	{
		return fp64(m_pTime->f_GetSeconds() - 237148622167132800) + m_pTime->f_GetFraction();
	}

	CTime CTimeConvert_BabylonianCommon::fs_FromUnixSecondsFraction(fp64 _Seconds)
	{
		CTime Ret;
		Ret.f_SetSeconds(237148622167132800 + _Seconds.f_ToUnsignedInt());
		Ret.f_SetFraction(_Seconds.f_Fraction());
		return Ret;
	}

	NStr::CStr fg_SecondsDurationToHumanReadable(fp64 const &_Seconds)
	{
		using namespace NStr;

		constexpr auto c_AverageYear = 365.2425_days;
		constexpr auto c_AverageMonth = 30.436875_days;

		if (_Seconds > c_AverageYear)
		{
			fp64 Years = (_Seconds / c_AverageYear);
			auto Months = (Years * 12.0 - Years.f_Floor() * 12.0).f_ToUnsignedInt();
			return "{}y {}m"_f << Years.f_ToUnsignedInt() << Months;
		}
		else if (_Seconds > c_AverageMonth)
		{
			constexpr auto c_AverageDaysInMonth = 30.436875;
			fp64 Months = (_Seconds / c_AverageMonth);
			auto Days = (Months * c_AverageDaysInMonth - Months.f_Floor() * c_AverageDaysInMonth).f_ToUnsignedInt();
			return "{}m {}d"_f << Months.f_ToUnsignedInt() << Days;
		}
		else if (_Seconds > 1_weeks)
		{
			fp64 Weeks = (_Seconds / 1_weeks);
			auto Days = (Weeks * 7.0 - Weeks.f_Floor() * 7.0).f_ToUnsignedInt();
			return "{}w {}d"_f << Weeks.f_ToUnsignedInt() << Days;
		}
		else if (_Seconds > 1_days)
		{
			fp64 Days = (_Seconds / 1_days);
			auto Hours = (Days * 24.0 - Days.f_Floor() * 24.0).f_ToUnsignedInt();
			return "{}d {}h"_f << Days.f_ToUnsignedInt() << Hours;
		}
		else if (_Seconds > 1_hours)
		{
			fp64 Hours = (_Seconds / 1_hours);
			auto Minutes = (Hours * 60.0 - Hours.f_Floor() * 60.0).f_ToUnsignedInt();
			return "{}h {}m"_f << Hours.f_ToUnsignedInt() << Minutes;
		}
		else if (_Seconds > 1_minutes)
		{
			fp64 Minutes = (_Seconds / 1_minutes);
			auto Seconds = (Minutes * 60.0 - Minutes.f_Floor() * 60.0).f_ToUnsignedInt();
			return "{}m {}s"_f << Minutes.f_ToUnsignedInt() << Seconds;
		}
		else if (_Seconds >= 9.95_seconds)
			return "{fe0} s"_f << _Seconds;
		else if (_Seconds > 1_seconds)
			return "{fe1} s"_f << _Seconds;
		else if (_Seconds >= 9.95e-3_seconds)
			return "{fe0} ms"_f << (_Seconds * 1e3);
		else if (_Seconds > 1e-3_seconds)
			return "{fe1} ms"_f << (_Seconds * 1e3);
		else if (_Seconds >= 9.95e-6_seconds)
			return "{fe0} µs"_f << (_Seconds * 1e6);
		else if (_Seconds > 1e-6_seconds)
			return "{fe1} µs"_f << (_Seconds * 1e6);
		else if (_Seconds >= 9.95e-9_seconds)
			return "{fe0} ns"_f << (_Seconds * 1e9);
		else if (_Seconds > 1e-9_seconds)
			return "{fe1} ns"_f << (_Seconds * 1e9);
		else if (_Seconds >= 9.95e-12_seconds)
			return "{fe0} ps"_f << (_Seconds * 1e12);
		else if (_Seconds > 1e-12_seconds)
			return "{fe1} ps"_f << (_Seconds * 1e12);
		else if (_Seconds >= 9.95e-15_seconds)
			return "{fe0} fs"_f << (_Seconds * 1e15);
		else if (_Seconds > 1e-15_seconds)
			return "{fe1} fs"_f << (_Seconds * 1e15);
		else if (_Seconds >= 9.95e-18_seconds)
			return "{fe0} as"_f << (_Seconds * 1e18);
		else if (_Seconds > 1e-18_seconds)
			return "{fe1} as"_f << (_Seconds * 1e18);
		else if (_Seconds >= 9.95e-21_seconds)
			return "{fe0} zs"_f << (_Seconds * 1e21);
		else if (_Seconds > 1e-21_seconds)
			return "{fe1} zs"_f << (_Seconds * 1e21);
		else if (_Seconds >= 9.95e-24_seconds)
			return "{fe0} ys"_f << (_Seconds * 1e24);
		else if (_Seconds > 1e-24_seconds)
			return "{fe1} ys"_f << (_Seconds * 1e24);
		else if (_Seconds >= 9.95e-27_seconds)
			return "{fe0} rs"_f << (_Seconds * 1e27);
		else if (_Seconds > 1e-27_seconds)
			return "{fe1} rs"_f << (_Seconds * 1e27);
		else if (_Seconds >= 9.95e-30_seconds)
			return "{fe0} qs"_f << (_Seconds * 1e30);
		else if (_Seconds > 1e-30_seconds)
			return "{fe1} qs"_f << (_Seconds * 1e30);
		else
			return "{ffe,fd3,fa0} tP"_f << (_Seconds * 10e44);
	}
}
