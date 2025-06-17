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
#include "SparseSet.h"
#include "ComponentView.h"
#include "components/ECSObjects.h"

namespace gwa::ntity
{
	class Registry
	{
	public:
		
		Registry() = default;
		template<typename... Component>
			requires std::conjunction_v<std::is_copy_constructible<std::decay_t<Component>>...>
		void initComponentList(const std::array<uint32_t, sizeof...(Component)>& expectedNumberOfComponents, uint32_t expectedNumberOfEntities)
		{
			constexpr size_t numberOfComponentTypes = sizeof...(Component);
			sparseSets.resize(numberOfComponentTypes);

			(..., sparseSets[TypeIDGenerator::type<Component>()].init<Component>(expectedNumberOfComponents[TypeIDGenerator::type<Component>()], expectedNumberOfEntities));
		}

		/**
		 * @brief Register a new Entity
		 * @return The ID of the created Entity
		 */
		uint32_t registerEntity()
		{
			if (!deletedEntities_.empty())
			{
				uint32_t id = deletedEntities_.back();
				deletedEntities_.pop_back();
				return id;
			}
			uint32_t id = getNewEntityID();

			for (auto& sparseSet : sparseSets)
			{
				sparseSet.addEmptyEntity(id);
			}
			return id;
		}

		/**
		 * @brief Add a new table of Components to the ECS system
		 * @tparam Component type of the Components
		 * @param expectedNumberOfComponents Expected number that will be reserved in the datastructure
		 */
		template<typename Component>
		void initNewComponent(uint32_t expectedNumberOfComponents)
		{
			const uint32_t componentIndex = TypeIDGenerator::type<Component>();
			assert(componentIndex == sparseSets.size());

			sparseSets.emplace_back();
			sparseSets.back().init<Component>(expectedNumberOfComponents, ntityIDCounter+expectedNumberOfComponents, ntityIDCounter);		
		}

		/**
		 * @brief Assign a component to an entity. Only assigns the component if a component of the same type is not already assigned to it.
		 * @tparam Component Type of the component. Adding new component types without previous initialization results in an assertion error.
		 * @param entityID Entity Index
		 * @param component Component value to be assigned
		 * @return Return true on successful emplacement, if the component could not be emplace f.e. because there is already an assigned Component return false
		 */
		template<typename Component> requires std::is_move_constructible_v<Component>
		void emplace(uint32_t entityID, Component&& component)
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			assert(typeID < sparseSets.size());
			return sparseSets[typeID].emplace<Component>(entityID, std::forward<Component>(component));
		}

		/**
		 * @brief Assign a trivial component to an entity. Only assigns the component if a component of the same type is not already assigned to it.
		 * @tparam Component Type of the component. Adding new component types without previous initialization results in an assertion error. The type has to be trivially copyable.
		 * @param entityID Entity Index
		 * @param component Component value to be assigned
		 */
		template<typename Component> requires std::is_copy_assignable_v<Component>
		void emplace(uint32_t entityID, Component& component)
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			assert(typeID < sparseSets.size());
			return sparseSets[typeID].emplace<Component>(entityID, component);
		}

		/**whose
		 * @brief  Assign a component to an entity. Only assigns the component if a component of the same type is not already assigned to it.
		 * @tparam Component  Type of the component. Adding new component types without previous initialization results in an assertion error.
		 * @tparam ...Args 
		 * @param entityID Entity Index 
		 * @param ...args Constructor parameters of the given component type
		 */
		template<typename Component, typename ...Args> requires std::is_constructible_v<Component, Args...>
		void emplace(uint32_t entityID, Args&&... args)
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			assert(typeID < sparseSets.size());
			sparseSets[typeID].emplace<Component, Args...>(entityID, std::forward<Args>(args)...);
		}

		/**
		 * @brief Removes all components of a certain type from all entities
		 * @tparam Component to be removed from all entities
		 */
		template<typename Component>
		void flushComponents()
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			assert(typeID < sparseSets.size());
			sparseSets[typeID].clearComponents();
		}

		/**
		 * @brief Delete an entity and add the ID to the pool of reusable entities
		 * @param enitityID 
		 */
		void deleteEntity(uint32_t entityID)
		{
			deletedEntities_.push_back(entityID);
			for (size_t sparseSetIndex = 0; sparseSetIndex < sparseSets.size(); sparseSetIndex++)
			{
				sparseSets[sparseSetIndex].deleteEntity(entityID);
			}
		}

		/**
		 * @brief 
		 * @tparam Component Type of the Component
		 * @return span of all entities that have an assigned Component of the given type
		 */
		template<typename Component>
		std::span<const uint32_t> getEntities()
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			assert(typeID < sparseSets.size());
			return sparseSets[typeID].getDenseList();
		}
		
		/**
		 * @brief 
		 * @tparam Component 
		 * @return Return the number of entities with the given Component type
		 */
		template<typename Component>
		size_t getComponentCount()
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			assert(typeID < sparseSets.size());
			return sparseSets[typeID].size();
		}

		/**
		 * @brief 
		 * @tparam Component
		 * @param index 
		 * @return component of the type Component of the given entity
		 */
		template<typename Component>
		Component* getComponent(uint32_t entity)
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			assert(typeID < sparseSets.size());
			return sparseSets[typeID].get<Component>(entity);
		}

		/**
		 * @brief Retrieves a handle to a component of the specified type for a given entity.
		 * @tparam Component The type of the component to retrieve.
		 * @param entity The ID of the entity that owns the component.
		 * @return A ComponentHandle representing the component's location and version.
		 */
		template<typename Component>
		ComponentHandle getComponentHandle(uint32_t entity)
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			assert(typeID < sparseSets.size());
			return sparseSets[typeID].getComponentHandle<Component>(entity);
		}

		/**
		 * @brief Get the associated Component from a ComponentHandle. Asserts that the Component parameter matches the ComponentHandle type.
		 * @tparam Component
		 * @param handle
		 * @return Return the associated Component or nullptr if version does not match or entity is invalid
		 */
		template<typename Component>
		ComponentHandle getFromComponentHandle(const ComponentHandle& handle)
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			assert(typeID < sparseSets.size() && typeID == handle.typeID);
			return sparseSets[typeID].getFromComponentHandle<Component>(handle);
		}
		
		/**
		* @brief Returns a pointer to the component data at the specified byte offset.
		* @param offset The offset in bytes from the start of the component data.
		* @return A const void pointer to the data at the given offset.
		*
		* @note The caller is responsible for knowing the type and bounds of the data.
		*/
		const void* getRawComponentData(const ComponentHandle& handle)
		{
			const uint32_t typeID = handle.typeID;
			return sparseSets[typeID].getRawComponentData(handle);
		}

		template <typename Component, typename Func>
		void each(Func&& func)
		{
			const uint32_t typeID = TypeIDGenerator::type<Component>();
			assert(typeID < sparseSets.size());
			sparseSets[typeID].each(std::forward(func));
		}

		template<typename... ViewComponents>
		ComponentView<ViewComponents...> view()
		{
			constexpr size_t numberOfComponentTypes = sizeof...(ViewComponents);
			std::array<const SparseSet*, numberOfComponentTypes> viewSets;
			size_t i = 0;
			(..., void(viewSets[i++] = &sparseSets[TypeIDGenerator::type<ViewComponents>(false)]));
			return ComponentView<ViewComponents...>(viewSets);
		}

		private:
		uint32_t getNewEntityID()
		{
			return ntityIDCounter++;
		}
		//NOTE: Since it won't be a big engine we only support up to 32bit number of entities
		uint32_t ntityIDCounter = 0;
		std::vector<SparseSet> sparseSets;
		std::vector<uint32_t> deletedEntities_;

	};
}
