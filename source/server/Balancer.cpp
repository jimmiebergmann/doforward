#include <Balancer.hpp>

namespace dof
{

	Balancer::Balancer() :
		m_Value(100)
	{
	}

	void Balancer::Set(const int value)
	{
		m_Value = value;
	}

	int Balancer::Get() const
	{
		return m_Value;
	}

}
