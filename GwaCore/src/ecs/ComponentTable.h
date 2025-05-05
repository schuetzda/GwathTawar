#pragma once
#include <cstdint>
#include <cassert>
#include <malloc.h>
#include <utility>
#include <typeindex>
namespace gwa::ntity
{
	/**
	 * @brief A Table that contains Objects of one arbitrary type.
	 * The type has not to be known at compile but only at compile time similar to std::any.
	 */
	class ComponentTable
	{
		template<typename Component>
		struct Manager;
	public:
		/**
		 * @brief Initializes an empty component table without a specific type.
		 */
		ComponentTable() : _memoryManager(nullptr), componentData_(nullptr)
		{

		}

		/**
		*@brief Initializes the table for a specific component type.
		* @tparam Component The type of the components stored in this table.
		* @tparam Mgr The memory manager responsible for handling component lifecycle.
		* @param reservedComponentsCount The expected number of component that can be hold without a memory reallocation
		*/
		template<typename Component, typename Mgr = Manager<Component>>
			requires std::is_copy_constructible_v<std::decay_t<Component>>
		void init(uint32_t reservedComponentsCount)
		{
			_memoryManager = Mgr::manage;
			typeID_ = std::type_index(typeid(Component));
			reservedComponentsCount_ = reservedComponentsCount;
			componentData_ = malloc(sizeof(Component) * reservedComponentsCount_);
		}

		/**
		 * @brief Move constructor.
		 * @param other The table being moved.
		*/
		ComponentTable(ComponentTable&& other) noexcept
			:typeID_(other.typeID_), currentComponentsCount_(other.currentComponentsCount_), reservedComponentsCount_(other.reservedComponentsCount_), componentData_(nullptr)
		{
			if (other._memoryManager == nullptr)
			{
				_memoryManager = nullptr;
			}
			else
			{
				_Arguments arg;
				arg.otherTable = this;
				other._memoryManager(transfer, &other, &arg);
			}
		}

		/**
		 * @brief Retrieves a pointer to a component at the specified index.
		 * @param index The index of the component in the table.
		 * @return Pointer to the component
		 */
		template <typename Component>
		Component* getComponent(uint32_t index) const
		{
			assert(typeID_ == std::type_index(typeid(Component)));
			assert(index < reservedComponentsCount_);
			return static_cast<Component*>(componentData_) + index;
		}

				
		template<typename Component, typename... Args>
		bool try_emplace(uint32_t entity, Args&&... args)
		{
			if (typeID_ == std::type_index(typeid(Component)) && entity < size())
			{
				return false;
			}

			Component const* currentPointer = static_cast<Component*>(componentData_) + entity;
			new(currentPointer) Component(std::forward<Component, Args>(args)...);
			return currentPointer != nullptr;
		}
		
		/**
		 * @brief Adds a new component to the end of the table, reallocating memory if necessary.
		* @param component The component to be added.
		*/
		template<typename Component> requires std::is_copy_assignable_v<Component>
		void emplace_back(Component& component)
		{
			assert(typeID_ == std::type_index(typeid(Component)));
			if (currentComponentsCount_ >= reservedComponentsCount_)
			{
				grow<Component>();
			}
			Component* currentPointer = static_cast<Component*>(componentData_) + currentComponentsCount_;
			*currentPointer = component;			
			currentComponentsCount_++;
		}

		template<typename Component> requires std::is_move_constructible_v<Component>
		void emplace_back(Component&& component)
		{
			assert(typeID_ == std::type_index(typeid(Component)));
			if (currentComponentsCount_ >= reservedComponentsCount_)
			{
				grow<Component>();
			}
			Component* currentPointer = static_cast<Component*>(componentData_) + currentComponentsCount_;
			new(currentPointer) Component(std::forward<Component>(component));
			currentComponentsCount_++;
		}

		
		template<typename Component, typename... Args> requires std::is_constructible_v<Component, Args...>
		void emplace_back(Args&&... args)
		{
			assert(typeID_ == std::type_index(typeid(Component)));
			if (currentComponentsCount_ >= reservedComponentsCount_)
			{
				grow<Component>();
			}
			Component const* currentPointer = static_cast<Component*>(componentData_) + currentComponentsCount_;
			new(currentPointer) Component(std::forward<Args>(args)...);
			currentComponentsCount_++;
		}

		uint32_t size() const
		{
			return currentComponentsCount_;
		}

		bool operator==(const ComponentTable& other) const = delete;

		~ComponentTable() {
			clear();
		}

		void clear()
		{
			if (_memoryManager != nullptr)
				_memoryManager(destroy, this, nullptr);

			componentData_ = nullptr;
			typeID_ = std::type_index(typeid(void));
			currentComponentsCount_ = 0;
			reservedComponentsCount_ = 0;
		}

		void* begin()
		{
			return componentData_;
		}

		void* end()
		{
			_Arguments arg;
			_memoryManager(ending, this, &arg);
			return arg._obj;
		}

	private:
		template<typename Component>
		void grow()
		{
			reservedComponentsCount_ = (reservedComponentsCount_ + 1) * 2;
			void* newPtr = malloc(sizeof(Component) * reservedComponentsCount_);
			for (uint32_t i = 0; i < currentComponentsCount_; i++)
			{
				Component* currentPointer = static_cast<Component*>(componentData_) + i;
				Component* newComponentPtr = static_cast<Component*>(newPtr) + i;
				new (newComponentPtr) Component(*currentPointer);
				currentPointer->~Component();
			}
			free(componentData_);
			componentData_ = newPtr;
		}
		enum _Operation
		{
			destroy,
			transfer,
			clone,
			ending
		};
		union _Arguments
		{
			void* _obj;
			ComponentTable* otherTable;
		};

		void (*_memoryManager)(_Operation, const ComponentTable*, _Arguments*);

		template <typename Component>
		struct Manager
		{
			static void manage(_Operation, const ComponentTable*, _Arguments*);
		};

		std::type_index typeID_{ std::type_index(typeid(void)) };
		uint32_t currentComponentsCount_ = 0;
		uint32_t reservedComponentsCount_ = 0;
		void* componentData_;
	};

	template <typename Component>
	void ComponentTable::Manager<Component>::manage(_Operation operation, const ComponentTable* componentTable, _Arguments* args)
	{
		const Component* ptr = static_cast<const Component*>(componentTable->componentData_);

		switch (operation)
		{
		case destroy:
			if (componentTable->componentData_&& componentTable->currentComponentsCount_ > 0)
			{
				for (uint32_t i = 0; i < componentTable->currentComponentsCount_; i++)
				{
					const Component* currentPointer = ptr + i;
					currentPointer->~Component();
				}
				free(componentTable->componentData_);
			}
			break;
		case transfer:
			args->otherTable->componentData_ = componentTable->componentData_;
			args->otherTable->_memoryManager = componentTable->_memoryManager;
			const_cast<ComponentTable*>(componentTable)->_memoryManager = nullptr;
			break;
		case clone:
			break;
		case ending:
			args->_obj = const_cast<Component*>(ptr + componentTable->size());
			break;
		}
	}
}
