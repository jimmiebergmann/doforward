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


#include <Yaml.hpp>
#include <iostream>

using namespace dof::Yaml;

int main()
{
	std::string file =
		"root1:  \n"
		"  key1: value1\n";



	Node root;
	Reader reader;
	reader.ReadFromMemory(file, root);


	Node & foo1 = root["root1"];
	Node & foo2 = foo1["key1"];

	std::cout << "TEST: " << &foo1 << std::endl;
	std::cout << "TEST: " << &foo2 << std::endl;

	std::cout << root["root1"]["key1"].Value<std::string>() << std::endl;

	/*root["foo"] = "bar";
	root["hello"] = Scalar("world");
	

	Scalar & a = root["foo"].AsScalar();
	Scalar & b = root["hello"].AsScalar();


	int c = 5;*/

	std::cin.get();
	return 0;
}
