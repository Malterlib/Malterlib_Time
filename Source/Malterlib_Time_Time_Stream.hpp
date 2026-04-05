// Copyright © Unbroken AB
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#pragma once

namespace NMib::NStream
{
	template <typename t_CStream>
	class TCBinaryStreamTypeReference<t_CStream, NTime::CTime>
	{
	public:
		static constexpr void fs_Feed(t_CStream &_Stream, NTime::CTime const &_Data)
		{
			_Stream << _Data.f_GetSeconds();
			_Stream << _Data.f_GetFractionInt();
		}

		static constexpr void fs_Consume(t_CStream &_Stream, NTime::CTime &_Data)
		{
			uint64 Temp;
			_Stream >> Temp;
			_Data.f_SetSecondsNoFraction(Temp);
			_Stream >> Temp;
			_Data.f_SetFractionInt(Temp);
		}
	};
}
