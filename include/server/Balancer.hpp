#pragma once

namespace dof
{

	class Balancer
	{

	public:

		Balancer();

		void Set(const int value);

		int Get() const;

	private:

		int m_Value;

	};

}
