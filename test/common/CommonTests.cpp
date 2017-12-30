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
		"root1: \n"
		"    - \n"
		"      foo: 5 \n"
		"      bar: 6 \n"
	 	"    - bar: 7 \n"
	    "test: hello world \n"
		"data: \n"
		"    test: hello world 2\n";
	

	Node root;
	Reader reader;
	reader.ReadFromMemory(file, root);


	std::cout << root["root1"][0]["foo"].Value<std::string>() << std::endl;
	std::cout << root["root1"][0]["bar"].Value<std::string>() << std::endl;
	std::cout << root["root1"][1]["bar"].Value<std::string>() << std::endl;
	std::cout << root["test"].Value<std::string>() << std::endl;
	std::cout << root["data"]["test"].Value<std::string>() << std::endl;
	//std::cout << root["root1"][1].Value<std::string>() << std::endl;

	/*Node & root1 = root["root1"];
	Node & root2 = root1["root2"];

	Node & root3 = root["root3"];

	Node & root4 = root["root4"];
	Node & root5 = root4[0];
	Node & root6 = root4[1];
	*/


	/*std::cout << root["root1"]["root2"].Value<std::string>() << std::endl;
	std::cout << root["root1"]["root3"].Value<std::string>() << std::endl;
	std::cout << root["root4"].Value<std::string>() << std::endl;*/
	//std::cout << root["root4"][0].Value<std::string>() << std::endl;
	//std::cout << root["root4"][1].Value<std::string>() << std::endl;


	std::cin.get();
	return 0;
}
