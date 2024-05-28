#pragma once

#include <cstdint>

struct F4SECoreVersionData
{
	enum
	{
		kVersion = 1,
	};

	std::uint32_t	dataVersion;

	std::uint32_t	runtimeVersion;
};
