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

	parser.add_opt(OP::FLOAT,"length", "20", 'l', true);
	parser.add_opt(OP::INT,"height", "20", 'h', true);
	parser.add_opt(OP::STRING,"name", "Kinsman", 'n', true);
	parser.add_opt(OP::BOOL,"mao", "false", 'm', false,
			longstring + " This string tests my wrapping");

	string test_1 = "./go -l  -6001.45e-2 -h 45 -n Joe -mmmm";
	parser.parse(test_1);
	cout << "Parsed string [" << test_1 << "]\n";
	parser.print();
	parser.help();

	std::vector<int> v;
	OP::print_series(v.begin(),v.end());

	cout << "Parser testing successful!\n";
}

int main(int argc, char* argv[]) {
	using namespace std;

	test_parser(argc, argv);

	return 0;
}
