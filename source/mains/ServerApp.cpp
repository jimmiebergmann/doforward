/*
* MIT License
*
* Copyright(c) 2017 Jimmie Bergmann
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/


#include <Balancer.hpp>
#include <iostream>
#include <algorithm>

int main(int argc, char ** argv)
{
	// Get configuration path
	std::string configPath = "";
	if (argc > 1)
	{
		configPath = argv[1];
	}
	else
	{
		configPath = argv[0];
		size_t pathEnd1 = configPath.find_last_of('/');
		size_t pathEnd2 = configPath.find_last_of('\\');
		size_t pathEnd = std::min(pathEnd1, pathEnd2);
		if (pathEnd != std::string::npos)
		{
			configPath = configPath.substr(0, pathEnd);
		}

		configPath += "/doforward.conf";
	}

	// Load balancer configurations.
	dof::Balancer::Config config;
	try
	{
		config.LoadFromFile(configPath);
	}
	catch (const dof::Exception & e)
	{
		std::cerr << "Excption: " << e.GetCode() << " - " << e.GetMessage() << std::endl;
		return 0;
	}

	// Run balancer with loaded configurations.
	dof::Balancer balancer;
	balancer.Run(config);
	balancer.Finish();
	return 0;
}
