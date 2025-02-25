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
		ComponentTable() : _memoryManager(nullptr), componentData_(nullptr)
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

		/*ComponentTable(const ComponentTable& other)
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
				other._memoryManager(clone, &other, &arg);
			}
		}*/
		// NOTE For simplicity we don't allow a copy constructor. Otherwise all Components need to implement one.
		// Disallowing use of f.e. unique pointer as Component class members.
		ComponentTable(const ComponentTable& other) = delete;


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

		void flushTable()
		{
			reset();
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
		enum _Operation
		{
			destroy,
			transfer
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
		}
	}
}
