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

using namespace dof;

int main()
{
	std::string file =
		"    \n"
		"\n"
		"# test!    \n"
		"  #test  \n"
		"   \n"
		"test_1: 123\n"
		"test_2: 100.0\n"
		"test_3: Test\n"
		"test_4: \"Hello world\"\n"
		"test_5: 'Hello world'\n"
		"test_5: \"Hello\\\"world\"\n"
		"test_5: 'Hello\\'world'\n";


	Yaml::Scalar scalar;

	scalar.GetNode() = "test";


	scalar.GetNode() = 123;

	auto val = scalar.Value<std::string>();

	int b = 5;

	std::cout << "Hellooooo!" << std::endl;



/*  Yaml::Value root;
	Yaml::Reader reader;
	reader.ReadFromMemory(file.c_str(), file.size(), root);
*/

/*
	Yaml::Object root;

	Yaml::Value & val = root["test"]["test"];
	val = Yaml::Object();
	val["test"]

	//val["cool"] = Yaml::Object();

	//std::cout << b.Get() << std::endl;
*/

	return 0;
}
