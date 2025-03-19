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
		 * @brief Initializes empty component table without a specific type.
		 */
		ComponentTable() : _memoryManager(nullptr), componentData_(nullptr)
		{

		}
		
		/**
		*@brief Initializes the table for a specific component type.
		* @tparam Component The type of the components stored in this table.
		* @tparam Mgr The memory manager responsible for handling component lifecycle.
		* @param reservedComponentsCount The initial number of reserved component slots.
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
			:typeID_(other.typeID_), currentComponentsCount_(other.currentComponentsCount_), reservedComponentsCount_(other.reservedComponentsCount_)
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
		 * @tparam Component The expected component type. Has to be matching to the type used at init().
		 * @param index The index of the component in the table.
		 * @return Pointer to the component, or assertion failure if the type or index is invalid.
		*/
		template <typename Component>
		Component* getComponent(uint32_t index)
		{
			assert(typeID_ == std::type_index(typeid(Component)));
			assert(index < reservedComponentsCount_);
			return static_cast<Component*>(componentData_) + index;
		}

		/**
		 * @brief Adds a new component to the table, expanding memory if necessary.
		* @tparam Component The type of the component being added.
		* @param component The component instance to be added.
		*/
		template<typename Component>
		void addComponent(Component&& component)
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

		template<typename Component, typename... Args>
		void emplace_back(Args&&... args)
		{
			assert(typeID_ == std::type_index(typeid(Component)));
			if (currentComponentsCount_ >= reservedComponentsCount_)
			{
				grow<Component>();
			}
			Component* currentPointer = static_cast<Component*>(componentData_) + currentComponentsCount_;
			new(currentPointer) Component(std::forward<Args>(args)...);
			currentComponentsCount_++;
		}

		uint32_t size() const
		{
			return currentComponentsCount_;
		}

		bool operator==(const ComponentTable& other) const = delete;

		~ComponentTable() {
			reset();
		}

		void reset()
		{
			if (_memoryManager != nullptr)
				_memoryManager(destroy, this, nullptr);

			componentData_ = nullptr;
			typeID_ = std::type_index(typeid(void));
			currentComponentsCount_ = 0;
			reservedComponentsCount_ = 0;
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
				new (newComponentPtr) Component(std::move(*currentPointer));
				currentPointer->~Component();
			}
			free(componentData_);
			componentData_ = newPtr;
		}
		enum _Operation
		{
			destroy,
			transfer,
			clone
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
		}
	}
}
