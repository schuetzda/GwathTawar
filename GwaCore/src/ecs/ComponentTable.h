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
		template<typename Component>
		struct Manager;
	public:
		ComponentTable() : _memoryManager(nullptr)
		{

		}

		template<typename Component, typename Mgr = Manager<Component>>
		void init(uint32_t reservedComponentsCount)
		{
			_memoryManager = Mgr::manage;
			typeID_ = std::type_index(typeid(Component));
			reservedComponentsCount_ = reservedComponentsCount;
			componentData_ = malloc(sizeof(Component) * reservedComponentsCount_);
		}

		template <typename Component>
		Component* getComponent(uint32_t index)
		{
			assert(typeID_ == std::type_index(typeid(Component)));
			assert(index < reservedComponentsCount_);
			return static_cast<Component*>(componentData_) + index;
		}

		template<typename Component>
		void addComponent(Component&& component)
		{
			assert(typeID_ == std::type_index(typeid(Component)));
			if (currentComponentsCount_ >= reservedComponentsCount_)
			{
				reservedComponentsCount_ = reservedComponentsCount_ * 2;
				void* newPtr = malloc(sizeof(Component) * reservedComponentsCount_);
				std::memcpy(newPtr, componentData_, sizeof(Component) * currentComponentsCount_);
				free(componentData_);
				componentData_ = newPtr;
			}
			Component* currentPointer = static_cast<Component*>(componentData_) + currentComponentsCount_;
			new(currentPointer) Component(std::forward<Component>(component));
			currentComponentsCount_++;
		}

		void deleteComponent(uint32_t index)
		{
			_Arguments args;
			//args._obj = 
			//_memoryManager(deleteComponent, this, )
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
			if (componentData_ && currentComponentsCount_ > 0)
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

		bool operator==(const ComponentTable& other) const = delete;

		~ComponentTable() {
			if (componentData_ && currentComponentsCount_ > 0) {
				free(componentData_);
			}
		}


		void* componentData_;
	private:
		enum _Operation
		{
			access,
			destroy,
			clone,
			info
		};
		union _Arguments
		{
			void* _obj;
		};

		void (*_memoryManager)(_Operation, ComponentTable*, _Arguments*);

		template <typename Component>
		struct Manager
		{
			static void manage(_Operation, ComponentTable*, _Arguments*);
		};

		std::type_index typeID_{ std::type_index(typeid(void)) };
		uint32_t currentComponentsCount_ = 0;
		uint32_t reservedComponentsCount_ = 0;
	};

	template <typename Component>
	void ComponentTable::Manager<Component>::manage(_Operation operation, ComponentTable* componentTable, _Arguments* args)
	{
		Component* ptr = static_cast<Component*>(componentTable->componentData_);
		switch (operation)
		{
		case destroy:
			if (componentTable->componentData_)
			{
				for (uint32_t i = 0; i < componentTable->currentComponentsCount_; i++)
				{
					Component* currentPointer = ptr + i;
					currentPointer->~Component();
				}
				free(componentTable->componentData_);
			}
			componentTable->componentData_ = nullptr;
			componentTable->typeID_ = std::type_index(typeid(void));
			componentTable->currentComponentsCount_ = 0;
			componentTable->reservedComponentsCount_ = 0;
			break;
		}
	}
}
