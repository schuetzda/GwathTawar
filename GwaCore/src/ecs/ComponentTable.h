#pragma once
#include <cstdint>
#include <cassert>
#include <malloc.h>
#include <utility>
#include <typeindex>
namespace gwa::ntity
{
	/**
	 * @brief 
	 */
	class ComponentTable
	{
	public:
		ComponentTable() = default;
		
		template<typename Component>
		void init(uint32_t reservedComponentsCount)		
		{
			typeID_ = std::type_index(typeid(Component));
			reservedComponentsCount_ = reservedComponentsCount;
			componentData_ = malloc(sizeof(Component) * reservedComponentsCount_);
		}

		template <typename Component>
		Component* getComponent(uint32_t index)
		{
			assert(typeID_ == std::type_index(typeid(Component)));
			assert(index < reservedComponentsCount_); //TODO make data array bigger
			return static_cast<Component*>(componentData_) + index;
		}

		template<typename Component>
		void addComponent(Component&& component)
		{
			assert(typeID_ == std::type_index(typeid(Component)));
			Component* currentPointer = static_cast<Component*>(componentData_) + currentComponentsCount_;
			new(currentPointer) Component(std::forward<Component>(component));
			currentComponentsCount_++;
		}

		template<typename Component>
		void deleteComponent(uint32_t index)
		{
			assert(typeID_ == std::type_index(typeid(Component)));
			Component* currentPointer = static_cast<Component*>(componentData_) + index;
			delete currentPointer;
		}

		template<typename Component>
		void flushTable()
		{
			assert(typeID_ == std::type_index(typeid(Component)));
			if constexpr (std::negation_v<std::is_fundamental<Component>>)
			{
				for (uint32_t i = 0; i < currentComponentsCount_; i++)
				{
					Component* currentPointer = static_cast<Component*>(componentData_) + i;
					currentPointer->~Component();
				}
			}
			currentComponentsCount_ = 0;
		}
				
		template<typename Component>
		requires std::negation_v<std::is_fundamental<Component>>
		void freeTable()
		{
			assert(typeID_ == std::type_index(typeid(Component)));
			if (componentData_)
			{
				for (uint32_t i = 0; i < currentComponentsCount_; i++)
				{
					Component* currentPointer = static_cast<Component*>(componentData_) + i;
					currentPointer->~Component();
				}
				free(componentData_);
			}
			componentData_ = nullptr;
			typeID_ = std::type_index(typeid(void));
			currentComponentsCount_ = 0;
			reservedComponentsCount_ = 0;
		}

		template<typename Component>
		requires std::is_fundamental_v<Component>
		void freeTable()
		{
			assert(typeID_ == std::type_index(typeid(Component)));
			if (componentData_)
			{
				free(componentData_);
			}
			componentData_ = nullptr;
			typeID_ = std::type_index(typeid(void));
			currentComponentsCount_ = 0;
			reservedComponentsCount_ = 0;
		}
		
		uint32_t size() const
		{
			return currentComponentsCount_;
		}

		~ComponentTable() {
			if (componentData_) {
				free(componentData_);
			}
		}
	private:
		void* componentData_ = nullptr;
		std::type_index typeID_{std::type_index(typeid(void))};
		uint32_t currentComponentsCount_ = 0;
		uint32_t reservedComponentsCount_ = 0;
	};
}
