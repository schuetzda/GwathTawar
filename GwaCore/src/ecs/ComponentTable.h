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
	 * The type has not to be known at compile but isn't a class template, similar to std::any.
	 * That way the user of the ECS system can use it's own classes without having to modify the internal ecs systems components.
	 */
	class ComponentTable
	{
		template<typename Component>
		struct Manager;
	public:
		/**
		 * @brief Initializes an empty component table without a specific type.
		 */
		ComponentTable() : _memoryManager(nullptr) {

		}

		/**
		*@brief Initializes the table for a specific component type.
		* @tparam Component The type of the components stored in this table. The component has to be copy constructible.
		* @tparam Mgr The memory manager responsible for handling component lifecycle.
		* @param reservedComponentsCount The expected number of component that can be hold without a memory reallocation.
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
			:_memoryManager(nullptr), typeID_(other.typeID_), currentComponentsCount_(other.currentComponentsCount_), reservedComponentsCount_(other.reservedComponentsCount_), componentData_(nullptr)
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
		 * @return Pointer to the component.
		 */
		template <typename Component>
		Component* getComponent(uint32_t index) const
		{
			assert(typeID_ == std::type_index(typeid(Component)));
			assert(index < reservedComponentsCount_);
			return static_cast<Component*>(componentData_) + index;
		}

		/**
		 * @brief Returns a pointer to the byte at the specified offset within the component data.
		 * @param offset The offset in bytes from the beginning of the component data.
		 * @return A pointer to the byte at the given offset.
		 * @note It is the caller's responsibility to ensure that the index is within bounds.
		 */
		const void* getVoidPtr(size_t offset) const
		{
			return static_cast<const void*>(static_cast<const std::byte*>(componentData_) + offset);
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

		/**
		 * @brief Adds a new component to the end of the table, reallocating memory if necessary.
		* @param component The component to be added.
		*/
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

		/**
		 * @brief Constructs and adds a new component to the storage
		 * @tparam Component Type of the component
		 * @tparam ...Args 
		 * @param ...args Arguments to be forwarded to the Component constructor
		 */
		template<typename Component, typename... Args> requires std::is_constructible_v<Component, Args...>
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

		/**
		 * @brief Removes a component at the given index by swapping it with the last component.
		 * @param index The index of the component to remove.
		 */
		void swapAndDelete(uint32_t index)
		{
			assert(index < size() && _memoryManager != nullptr);
			_Arguments arg;
			arg.index = index;
			if (index == currentComponentsCount_ -1)
			{
				_memoryManager(pop, this, &arg);
			}
			else
			{
				_memoryManager(delElement, this, &arg);
			}
			currentComponentsCount_--;
		}

		/**
		 * @brief Returns the number of currently stored components.
		 * @return The current number of active components in the container.
		 */
		uint32_t size() const
		{
			return currentComponentsCount_;
		}

		bool operator==(const ComponentTable& other) const = delete;

		~ComponentTable() {
			clear();
		}

		/**
		 * @brief Clears the storage of the component table and resets the member variables.
		 */
		void clear()
		{
			if (_memoryManager != nullptr)
				_memoryManager(destroy, this, nullptr);

			componentData_ = nullptr;
			typeID_ = std::type_index(typeid(void));
			currentComponentsCount_ = 0;
			reservedComponentsCount_ = 0;
		}

	private:
		/**
		 * @brief Increases the capacity of the component storage and reallocates memory.
		 * @tparam Component The type of the component being stored
		 */
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
			delElement,
			pop
		};
		union _Arguments
		{
			void* _obj;
			uint32_t index;
			ComponentTable* otherTable;
		};

		void (*_memoryManager)(_Operation, const ComponentTable*, _Arguments*) {};

		template <typename Component>
		struct Manager
		{
			static void manage(_Operation, const ComponentTable*, _Arguments*);
		};

		std::type_index typeID_{ std::type_index(typeid(void)) };
		uint32_t currentComponentsCount_ = 0;
		uint32_t reservedComponentsCount_ = 0;
		void* componentData_{};
	};

	/**
	 * @brief @brief Performs a memory management operation on a component table. It serves as a type-erased manager for component memory operations.
	 * @tparam Component The type of component being managed.
	 * @param operation The operation to perform (e.g., destroy, transfer, delElement, pop).
	 * @param componentTable Pointer to the component table on which the operation will be performed.
	 * @param args Optional arguments required for some operations (e.g., index for deletion).
	 * 
	**/
	template <typename Component>
	void ComponentTable::Manager<Component>::manage(_Operation operation, const ComponentTable* componentTable, _Arguments* args)
	{
		const Component* ptr = static_cast<const Component*>(componentTable->componentData_);

		switch (operation)
		{
		case destroy:
		{
			if (componentTable->componentData_ && componentTable->currentComponentsCount_ > 0)
			{
				for (uint32_t i = 0; i < componentTable->currentComponentsCount_; i++)
				{
					const Component* currentPointer = ptr + i;
					currentPointer->~Component();
				}
				free(componentTable->componentData_);
			}
		}
			break;
		case transfer:
		{
			args->otherTable->componentData_ = componentTable->componentData_;
			args->otherTable->_memoryManager = componentTable->_memoryManager;
			const_cast<ComponentTable*>(componentTable)->_memoryManager = nullptr;
		}
			break;
		case delElement:
		{
			//NOTE reducing CurrentComponentsCount_ has to be done manually after calling deleteElement
			assert(args->index != componentTable->currentComponentsCount_);
			Component* removeCandidate = static_cast<Component*>(componentTable->componentData_) + args->index;
			Component* lastElement = static_cast<Component*>(componentTable->componentData_) + (componentTable->currentComponentsCount_ - 1);

			removeCandidate->~Component();
			new (removeCandidate) Component(std::move(*lastElement));
			lastElement->~Component();
		}
			break;
		case pop:
		{
			const Component* popElement = ptr + (componentTable->currentComponentsCount_-1);
			popElement->~Component();
		}
			break;
		case clone:
			// not implemented yet
			break;
		}
	}
}
