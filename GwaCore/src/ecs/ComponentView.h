#pragma once
#include "SparseSet.h"
#include <array>
#include<span>
#include<vector>

namespace gwa::ntity
{
	/**
	 * @brief The component table serves as view allowing to iterate over entities with a specific set of components.
	 * @tparam ...Components The type of components all iterated entities need to have.
	 */
	template<typename... Components>
	class ComponentView
	{
	public:
		explicit ComponentView(std::array<const SparseSet*, sizeof...(Components)> sets) :sparseSets(sets) {
			uint32_t size = sparseSets[0]->size();
			for (uint32_t i = 1; i < sparseSets.size(); i++)
			{
				if (size < sparseSets[i]->size())
				{
					size = sparseSets[i]->size();
					smallestSet = i;
				}
			}
		}

		/**
		 * @brief Iterator that iterates through the smallest denseList of all components. Entities with no component of the specified type will be skipped. 
		 * @tparam ...Components 
		 */
		template<uint32_t numberOfComponents>
		struct ComponentIterator
		{
			ComponentIterator(std::array<const SparseSet*, numberOfComponents>* viewSets, uint32_t smallestIndex, std::span<const uint32_t>::iterator iterator) :sparseSets(viewSets), smallestSparseSetIndex(smallestIndex), current(iterator)
			{
			}

			ComponentIterator<numberOfComponents>& operator++()
			{
				do
				{
					current++;
				} while (current != (*sparseSets)[smallestSparseSetIndex]->getDenseList().end() && valid(*current));
				return *this;
			}
			
			/**
			 * @brief Checks wether an entity has a valid component for all component types of the view.
			 * @param entityID Id of the entity
			 * @return true if the entity has valid components
			 */
			bool valid(uint32_t entityID) {
				for (uint32_t i = 0; i < sparseSets->size(); i++)
				{
					if (!(*sparseSets)[i]->hasEntity(entityID))
					{
						return false;
					}
				}
				return true;
			}

			uint32_t operator*() const { return *current; }
			bool operator!=(const ComponentIterator& other) const { return current != other.current; }

			const uint32_t smallestSparseSetIndex;
			std::span<const uint32_t>::iterator current;
			std::array<const SparseSet*, numberOfComponents>* sparseSets{nullptr};
		};

		/**
		 * @brief Returns an iterator to the first valid entity with all specified components.
		 * @return A ComponentIterator pointing to the first valid matching entity.
		 */
		ComponentIterator<sizeof...(Components)> begin()
		{
			std::span<const uint32_t>::iterator it = sparseSets[smallestSet]->getDenseList().begin();
			while (*it == std::numeric_limits<uint32_t>::max())
			{
				it++;
			}
			return ComponentIterator<sizeof...(Components)>(&sparseSets, smallestSet, it);
		}

		template<typename Component>
		Component* get(uint32_t entityID) const
		{
			const uint32_t typeIndex = TypeIDGenerator::type<Component>(false);
			assert(typeIndex < sparseSets.size());
			return sparseSets[typeIndex].get<Component>(entityID);
		}

		ComponentIterator<sizeof...(Components)> end()
		{
			return ComponentIterator<sizeof...(Components)>(&sparseSets, smallestSet, sparseSets[smallestSet]->getDenseList().end());
		}
	private:
		uint32_t smallestSet{0};
		std::array<const SparseSet*, sizeof...(Components)> sparseSets;
	};
}
