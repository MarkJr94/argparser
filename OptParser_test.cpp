/*
 * ArgumentParser_test.cpp
 *
 *  Created on: Aug 10, 2012
 *      Author: markjr
 */

#include "OptParser.hpp"
#include <iostream>
#include <cassert>

std::string longstring =
		"Unlike traditional c-strings, which are mere sequences of characters in a memory array,"
				" C++ string objects belong to a class with many built-in features to operate with"
				" strings in a more intuitive way and with some additional useful features common "
				"to C++ containers.";

void test_parser(int argc, char *argv[])
{
	using namespace std;

	OP::OptParser parser("OptParser_test.cpp");

	parser.add_opt("length", "20", 'l', true, " Length of arms in centimeters");
	parser.add_opt("height", "20", 'h', true, "Height of individual");
	parser.add_opt("name", "Kinsman", 'n', true, "Name of individual");
	parser.add_opt("mao", "false", 'm', false,
			"Is the individual Mao Zedong? " + longstring
					+ " This string tests my wrapping", OP::FLAG);
	parser.add_opt("numbers", "20 52 52 63 58", 'a', true,
			"Individual's favorite numbers", OP::LIST);

	string test_1 =
			"./go -l  -6001.45e-2 -h 45 -a 25 48 62  48 62 98 16 4 84 -n Joe -mmmm";
	parser.parse(test_1);

	auto always_zero = [](const std::string&) -> float {return 0;};
	auto str_to_bool =
			[](const string& s) -> bool {return s == "true" ? true : false;};

	assert(parser.get_as<int>("height") == 45);
	assert(parser.get_as<float>("length") - -6001.45e-2 < .00001);
	assert(parser.get_as<bool>("mao") == true);
	assert(parser.get_as<string>("name") == "Joe");
	assert(parser.get_as<bool>("mao", str_to_bool) == true);
	assert(parser.get_as<float>("length", always_zero) == 0);

	/* Testing series parsing */
	auto numbers = parser.get_as<vector<string>>("numbers");

	cout << "Expect: 25, 48, 62, 48, 62, 98, 16, 4, 84\n";
	cout << "Result: ";
	OP::print_series(begin(numbers),end(numbers),", ");
	cout << endl;

	parser.help();
	cout << "Parser testing successful!\n";
}

int main(int argc, char* argv[])
{
	using namespace std;

	test_parser(argc, argv);

	return 0;
}
