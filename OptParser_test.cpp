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

void test_parser(int argc, char *argv[]) {
	using namespace std;

	OP::OptParser parser("OptParser_test.cpp");

	parser.addOpt<float>("length", 20, 'l', true);
	parser.addOpt<int>("height", 20, 'h', true);
	parser.addOpt<string>("name", "Kinsman", 'n', true);

	parser.parse("./go -l  -6001.45e-2 -h 45 -n Joe");

	assert(parser.getOpt<string>("name") == "Joe");
	assert(parser.getOpt<float>("length") - -6001.45e-2 < .0001);
	assert(parser.getOpt<int>("height") == 45);

	try {
		bool b = parser.getOpt<bool>("lol");
	} catch (OP::OptParseExcept& e) {
		cout << "Exception successfully thrown\n";
	}

	parser.clear();

	parser.addOpt<bool>("lol", false, 'l', true, "I am here to test you ");
	parser.addOpt<int>("rofl", 25, 'r', true, "I am here to test you as well");
	parser.addOpt<bool>("mao", true, 'm', false,
			longstring + " This string tests my wrapping");
	parser.addOptList("hare", 'h', true, "I test vector input");

	parser.parse("./go-again -lm --rofl 334 -h 1 2 3 4 5");

	assert(parser.getOpt<int>("rofl") == 334);
	assert(parser.getOpt<bool>("lol") == true);
	assert(parser.getOpt<bool>("mao") == false);

	cout << "Expect: \"1 2 3 4 5\"\n";
	OP::printVec(parser.getOptList("hare"), ' ');

	parser.help();

	cout << "Parser testing successful!\n";
}

int main(int argc, char* argv[]) {
	using namespace std;

	test_parser(argc, argv);

	return 0;
}
