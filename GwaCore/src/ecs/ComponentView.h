#pragma once
#include "SparseSet.h"
#include <array>
#include<span>
#include<vector>

namespace gwa::ntity
{
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
		template<typename... Components>
		struct ComponentIterator
		{
			ComponentIterator(std::array<const SparseSet*, sizeof...(Components)>* viewSets, uint32_t smallestIndex, std::span<const uint32_t>::iterator iterator) :sparseSets(viewSets), smallestSparseSetIndex(smallestIndex), current(iterator)
			{
			}

			ComponentIterator<Components...>& operator++()
			{
				do
				{
					current++;
				} while (current != (*sparseSets)[smallestSparseSetIndex]->getDenseList().end() && valid(*current));
				return *this;
			}

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
			std::array<const SparseSet*, sizeof...(Components)>* sparseSets{nullptr};
		};

		ComponentIterator<Components...> begin() 
		{
			std::span<const uint32_t>::iterator it = sparseSets[smallestSet]->getDenseList().begin();
			while (*it == std::numeric_limits<uint32_t>::max())
			{
				it++;
			}
			return ComponentIterator<Components...>(&sparseSets, smallestSet, it);
		}

		template<typename Component>
		Component* get(uint32_t entityID) const
		{
			const uint32_t typeIndex = TypeIDGenerator::type<Component>(false);
			assert(typeIndex < sparseSets.size());
			return sparseSets[typeIndex].get<Component>(entityID);
		}

		ComponentIterator<Components...> end() 
		{
			return ComponentIterator<Components...>(&sparseSets, smallestSet, sparseSets[smallestSet]->getDenseList().end());
		}
	private:
		uint32_t smallestSet{0};
		std::array<const SparseSet*, sizeof...(Components)> sparseSets;
	};
}
