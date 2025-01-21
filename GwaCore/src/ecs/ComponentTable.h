#pragma once
#include <cstdint>
#include <cassert>
#include <malloc.h>
#include <utility>
namespace gwa::ntity
{
	class ComponentList
	{
	public:
		ComponentList() = default;
		
		void init(size_t typeSize, uint32_t reservedComponentsCount)		
		{
			typeSize_ = typeSize;
			reservedComponentsCount_ = reservedComponentsCount;
			componentData_ = malloc(typeSize * reservedComponentsCount_);
		}

		template <typename Component>
		Component* getComponent(uint32_t index)
		{
			assert(sizeof(Component) == typeSize_);
			assert(index < reservedComponentsCount_); //TODO make data array bigger
			return static_cast<Component*>(componentData_) + index;
		}

		template<typename Component>
		void addComponent(Component&& component)
		{
			assert(sizeof(Component) == typeSize_);
			Component* currentPointer = static_cast<Component*>(componentData_) + currentComponentsCount_;
			new(currentPointer) Component(std::forward<Component>(component));
			currentComponentsCount_++;
		}

		~ComponentList() {
			if (componentData_) {
				free(componentData_);
			}
		}
	private:
		void* componentData_ = nullptr;
		size_t typeSize_ = 0;
		uint32_t currentComponentsCount_ = 0;
		uint32_t reservedComponentsCount_ = 0;
	};
}
