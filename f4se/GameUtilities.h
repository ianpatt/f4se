#pragma once

#include "f4se_common/Relocation.h"

typedef void (* _CalculateCRC32_64)(UInt32 * out, UInt64 data, UInt32 previous);
extern RelocAddr <_CalculateCRC32_64> CalculateCRC32_64;

typedef void (* _CalculateCRC32_32)(UInt32 * out, UInt32 data, UInt32 previous);
extern RelocAddr <_CalculateCRC32_32> CalculateCRC32_32;

typedef void(*_CalculateCRC32_SIZE)(UInt32 * out, const void* data, UInt32 size, UInt32 previous);
extern RelocAddr <_CalculateCRC32_SIZE> CalculateCRC32_SIZE;

namespace BSHash
{
	template <class Key, class Enable = void>
	struct CRC32Hash
	{
	public:
		UInt32 operator()(const Key& a_key) const
		{
			UInt32 crc32;
			CalculateCRC32_SIZE(crc32, &a_key, sizeof(Key), 0);
			return crc32;
		}
	};


	template <>
	struct CRC32Hash<SInt32>
	{
	public:
		UInt32 operator()(const SInt32& a_key) const
		{
			UInt32 crc32;
			CalculateCRC32_32(&crc32, (UInt32)a_key, 0);
			return crc32;
		}
	};


	template <>
	struct CRC32Hash<UInt32>
	{
	public:
		UInt32 operator()(const UInt32& a_key) const
		{
			UInt32 crc32;
			CalculateCRC32_32(&crc32, (UInt32)a_key, 0);
			return crc32;
		}
	};

	template <>
	struct CRC32Hash<SInt64>
	{
	public:
		UInt32 operator()(const SInt64& a_key) const
		{
			UInt32 crc32;
			CalculateCRC32_64(&crc32, (UInt64)a_key, 0);
			return crc32;
		}
	};

	template <>
	struct CRC32Hash<UInt64>
	{
	public:
		UInt32 operator()(const UInt64& a_key) const
		{
			UInt32 crc32;
			CalculateCRC32_64(&crc32, (UInt64)a_key, 0);
			return crc32;
		}
	};
}
