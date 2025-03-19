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
#include <span>
#include <ranges>

namespace gwa::ntity
{
	constexpr uint32_t INVALID_ENTITY_ID = std::numeric_limits<uint32_t>::max();
	class Registry
	{
	public:
		
		Registry() = default;
		template<typename... Component>
			requires std::conjunction_v<std::is_copy_constructible<std::decay_t<Component>>...>
		void initComponentList(const std::array<uint32_t, sizeof...(Component)>& expectedNumberOfComponents, uint32_t expectedNumberOfEntities)
		{
			constexpr size_t numberOfComponentTypes = sizeof...(Component);
			sparseComponentList.resize(numberOfComponentTypes);
			componentTables.resize(numberOfComponentTypes);
			denseComponentList.resize(numberOfComponentTypes);

			for (uint32_t i = 0; i < numberOfComponentTypes; ++i)
			{
				sparseComponentList[i].reserve(expectedNumberOfEntities);
				denseComponentList[i].reserve(expectedNumberOfComponents[i]);
			}

			(..., componentTables[TypeIDGenerator::type<Component>()].init<Component>(expectedNumberOfComponents[TypeIDGenerator::type<Component>()]));
		}

		uint32_t registerEntity()
		{
			if (!deletedEntities_.empty())
			{
				uint32_t id = deletedEntities_.back();
				deletedEntities_.pop_back();
				return id;
			}
			uint32_t id = getNewEntityID();

			for (auto& sparseList : sparseComponentList)
			{
				sparseList.push_back(INVALID_ENTITY_ID);
				assert(sparseList.size() == id + 1);
			}
			return id;
		}

		template<typename Component>
		void addComponentTable(uint32_t expectedNumberOfComponents)
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			assert(typeID == componentTables.size());
			
			sparseComponentList.emplace_back();
			sparseComponentList.back().assign(ntityIDCounter, INVALID_ENTITY_ID);

			denseComponentList.emplace_back();
			denseComponentList.back().reserve(expectedNumberOfComponents);

			componentTables.emplace_back();
			componentTables.back().init<Component>(expectedNumberOfComponents);
		}


		template<typename Component>
		void addComponent(uint32_t entityID, Component&& component)
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();

			assert(typeID < denseComponentList.size());
			denseComponentList[typeID].push_back(static_cast<uint32_t>(sparseComponentList[typeID].size() - 1));
			sparseComponentList[typeID][entityID] = static_cast<uint32_t>(denseComponentList[typeID].size() - 1);
			componentTables[typeID].addComponent<Component>(std::forward<Component>(component));
		}

		template<typename Component, typename ...Args>
		void emplace_back(uint32_t entityID, Args&&... args)
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();

			assert(typeID < denseComponentList.size());
			denseComponentList[typeID].push_back(static_cast<uint32_t>(sparseComponentList[typeID].size() - 1));
			sparseComponentList[typeID][entityID] = static_cast<uint32_t>(denseComponentList[typeID].size() - 1);
			componentTables[typeID].emplace_back<Component>(std::forward<Args>(args)...);
		}

		template<typename Component>
		void flushComponents()
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();

			for (const uint32_t entity : denseComponentList[typeID])
			{
				sparseComponentList[typeID][entity] = INVALID_ENTITY_ID;
			}
			denseComponentList[typeID].clear();
			componentTables[typeID].reset();
		}

		void deleteEntity(uint32_t enitityID)
		{
			deletedEntities_.push_back(enitityID);
		}

		template<typename Component>
		std::span<const uint32_t> getEntities()
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			return std::span<const uint32_t>(denseComponentList[typeID]);
		}

		
		template<typename Component>
		size_t getComponentCount()
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			return denseComponentList[typeID].size();
		}

		template<typename Component>
		Component* getComponent(uint32_t index)
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			return componentTables[typeID].getComponent<Component>(index);
		}

		template<typename Component>
		std::span<Component> getComponents()
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			Component* startingComponent = componentTables[typeID].getComponent<Component>(0);
			return std::span<Component>(startingComponent, componentTables[typeID].size());
		}

		template<typename... Component>
		std::vector<uint32_t> getEntities()
		{
			constexpr size_t numberOfComponentTypes = sizeof...(Component);
			std::array<uint32_t, numberOfComponentTypes> typeIDs = { TypeIDGenerator::type<Component>()... };

			uint32_t minSizeType = 0;
			uint32_t minSize = std::numeric_limits<uint32_t>::max();
			for (uint32_t typeID : typeIDs)
			{
				uint32_t typeSize = static_cast<uint32_t>(denseComponentList[typeID].size());
				if (minSize > typeSize)
				{
					minSizeType = typeID;
				}
			}

			std::vector<uint32_t> validEntities;
			for (uint32_t entity : denseComponentList[minSizeType])
			{
				bool invalidID = false;
				for (uint32_t typeID : typeIDs)
				{
					if (sparseComponentList[typeID][entity] == INVALID_ENTITY_ID)
					{
						invalidID = true;
						continue;
					}
				}
				if (!invalidID)
				{
					validEntities.push_back(entity);
				}
			}
			return validEntities;
		}

		template <typename Component, typename Func>
		void each(Func&& func)
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			for (uint32_t i = 0; i < componentTables[typeID].size(); ++i)
			{
				Component * const component = componentTables[typeID].getComponent(i);
				func(component);
			}
		}

		template<typename Component>
		std::pair<Component*, uint32_t> getComponentWithEntity(uint32_t index)
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			uint32_t entityID = denseComponentList[typeID][index];
			Component* component = componentTables[typeID].getComponent<Component>(index);
			return std::pair<Component*, uint32_t>(component, entityID);
		}

		private:
		uint32_t getNewEntityID()
		{
			return ntityIDCounter++;
		}
		//NOTE: Since it won't be a big engine we only support up to 32bit number of entites
		uint32_t ntityIDCounter = 0;
		std::vector<std::vector<uint32_t>> sparseComponentList;
		std::vector<std::vector<uint32_t>> denseComponentList;
		std::vector<ComponentTable> componentTables;

		std::vector<uint32_t> deletedEntities_;
	};
}
