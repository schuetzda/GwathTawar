#pragma once
#include <stdint.h>
#include <vector>
#include "TypeIDGenerator.h"
#include <memory>
#include <array>
#include <cassert>
namespace gwa::ntity
{
	class Registry
	{
	public:
		Registry()
		{
		}

		template<typename... Component>
		void initComponentList(const std::array<uint32_t, sizeof...(Component)>& expectedNumberOfComponents, uint32_t expectedNumberOfEntities)
		{
			const uint32_t numberOfComponentTypes = expectedNumberOfComponents.size();
			sparseComponentPool.resize(numberOfComponentTypes);
			componentMemoryPool.resize(numberOfComponentTypes);

			for (uint32_t i = 0; i < numberOfComponentTypes; ++i)
			{
				sparseComponentPool[i].reserve(expectedNumberOfEntities);
				componentMemoryPool[i].reserve(expectedNumberOfComponents[i]);
			}

			(..., assert(TypeIDGenerator::type<Component>() < numberOfComponentTypes));
		}

		template<typename Component>
		bool hasComponent() const
		{
			return TypeIDGenerator::type<Component> >= componentMemoryPool.size();
		}

		template<typename...Component>
		uint32_t registerEntity(const Component&...) const
		{
			return 2;
		}

	private:
		static bool instantiated;
		//NOTE: Since it won't be a big engine we only support up to 32bit number of entites
		uint32_t ntityIDCounter = 1; // 0 is reserved for nullptr entities

		//NOTE: Out of convinience we use a vector, custom memory allocator would be better
		std::vector<std::vector<uint32_t>> sparseComponentPool;
		std::vector<std::vector<void*>> componentMemoryPool;
	};
}
