#pragma once
#include <string>
class Layer
{
public:
	explicit Layer(const std::string& name = "Layer") : m_DebugName(name)
	{}
	virtual ~Layer() = default;

	
	virtual void OnAttach() 
	{
		// Abstract class
	}
	virtual void OnDetach() 
	{
		// Abstract class
	} 
	virtual void OnUpdate(double ts) 
	{
		// Abstract class
	}

private:
	std::string m_DebugName;
};

