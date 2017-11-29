#include <Balancer.hpp>
#include <iostream>

int main()
{
	dof::Balancer balancer;
	balancer.Set(50);

	std::cout << "Balancer test: " << balancer.Get() << std::endl;

	return 0;
}
