#pragma once
#include <cstdint>

namespace gwa::ntity
{
	struct ComponentHandle
	{
		uint32_t typeID;
		uint32_t entity;
		uint32_t componentByteSize;
		uint32_t version;
	};
}