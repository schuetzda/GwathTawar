#pragma once
#include <stdint.h>
#include <vector>
#include "TypeIDGenerator.h"
#include <memory>
#include <array>
#include <cassert>
#include <limits>
#include "ComponentTable.h"
#include <utility>
namespace gwa::ntity
{
	class Registry
	{
	public:
		Registry() = default;
		template<typename... Component>
		void initComponentList(const std::array<uint32_t, sizeof...(Component)>& expectedNumberOfComponents, uint32_t expectedNumberOfEntities)
		{
			constexpr size_t numberOfComponentTypes = sizeof...(Component);
			sparseComponentList.resize(numberOfComponentTypes);
			componentTable.resize(numberOfComponentTypes);
			denseComponentList.resize(numberOfComponentTypes);

			constexpr std::array<uint32_t, numberOfComponentTypes> componentSizes = { sizeof(Component)... };
			for (uint32_t i = 0; i < numberOfComponentTypes; ++i)
			{
				sparseComponentList[i].reserve(expectedNumberOfEntities);
				denseComponentList[i].reserve(expectedNumberOfComponents[i]);
				componentTable[i].init(componentSizes[i], expectedNumberOfComponents[i]);
			}

			(..., assert(TypeIDGenerator::type<Component>() < numberOfComponentTypes));
		}

		uint32_t registerEntity()
		{
			const uint32_t id = getNewEntityID();
			for (size_t i = 0; i < sparseComponentList.size(); ++i)
			{
				sparseComponentList[i].push_back(std::numeric_limits<uint32_t>::max());
				assert(sparseComponentList[i].size() == id + 1);
			}
			return id;
		}
		template<typename Component>
		void addComponent(uint32_t entityID, Component& component)
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();

			assert(typeID < denseComponentList.size());
			
			denseComponentList[typeID].push_back(sparseComponentList[typeID].size() - 1);
			sparseComponentList[typeID][entityID] = denseComponentList[typeID].size() - 1;
			componentTable[typeID].addComponent<Component>(std::move(component));
		}


		void deleteEntity(uint32_t enitityID) const
		{
		}

		template<typename Component>
		std::vector<uint32_t> getEntities()
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			std::vector<uint32_t> entities;
			entities.resize(denseComponentList[typeID].size());

			uint32_t entity = 0;
			for (uint32_t i = 0; i < denseComponentList[typeID].size(); ++i)
			{
				entity = denseComponentList[typeID][i];
				entities[i] = entity;
			}
			return entities;
		}

		template<typename Component>
		uint32_t getComponentCount()
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			return denseComponentList[typeID].size();
		}

		template<typename Component>
		Component * getComponent(uint32_t index)
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			return componentTable[typeID].getComponent<Component>(index);
		}
	private:
		uint32_t getNewEntityID()
		{
			return ntityIDCounter++;
		}
		//NOTE: Since it won't be a big engine we only support up to 32bit number of entites
		uint32_t ntityIDCounter = 0;
		//NOTE: Out of convinience we use a vector, custom memory allocator would be better
		std::vector<std::vector<uint32_t>> sparseComponentList;
		std::vector<std::vector<uint32_t>> denseComponentList;
		std::vector<ComponentList> componentTable;
	};
}
