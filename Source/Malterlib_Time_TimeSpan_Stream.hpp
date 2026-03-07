// Copyright © 2026 Unbroken AB
// Distributed under the MIT license, see license text in LICENSE.Malterlib

#pragma once

namespace NMib::NStream
{
	template <typename t_CStream>
	class TCBinaryStreamTypeReference<t_CStream, NTime::CTimeSpan>
	{
	public:
		static constexpr void fs_Feed(t_CStream &_Stream, NTime::CTimeSpan const &_Data)
		{
			_Stream << _Data.f_GetSeconds();
			_Stream << _Data.f_GetFractionInt();
		}

		static constexpr void fs_Consume(t_CStream &_Stream, NTime::CTimeSpan &_Data)
		{
			uint64 Temp;
			_Stream >> Temp;
			_Data.f_SetSecondsNoFraction(Temp);
			_Stream >> Temp;
			_Data.f_SetFractionInt(Temp);
		}
	};
}
