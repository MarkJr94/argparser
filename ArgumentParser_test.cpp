/*
 * ArgumentParser_test.cpp
 *
 *  Created on: Aug 10, 2012
 *      Author: markjr
 */

#include "ArgumentParser.hpp"
#include <iostream>
#include <cassert>

std::string longstring =
		"Unlike traditional c-strings, which are mere sequences of characters in a memory array,"
		" C++ string objects belong to a class with many built-in features to operate with"
		" strings in a more intuitive way and with some additional useful features common " ""
		"to C++ containers.";

void test_parser(int argc, char *argv[]) {
	using namespace std;

	AP::ArgumentParser parser("ArgumentParser_test.cpp");

	parser.addarg<float>("length", 20, 'l', true);
	parser.addarg<int>("height", 20, 'h', true);
	parser.addarg<string>("name", "Kinsman", 'n', true);

	parser.parse("./go -l  -6001.45e-2 -h 45 -n Joe");

	assert(parser.getarg<string>("name") == "Joe");
	assert(parser.getarg<float>("length") - -6001.45e-2 < .0001);
	assert(parser.getarg<int>("height") == 45);

	try {
		bool b = parser.getarg<bool>("lol");
	} catch (AP::ArgParseExcept& e) {
		cout << "Exception successfully thrown\n";
	}

	parser.clear();

	parser.addarg<bool>("lol", false, 'l', true, "I am here to test you ");
	parser.addarg<int>("rofl", 25, 'r', true, "I am here to test you as well");
	parser.addarg<bool>("mao", true, 'm', false, longstring + " This string tests my wrapping");
	parser.addarglist("hare",'h',true,"I test vector input");

	parser.parse("./go-again -l -r 334 -h 1 2 3 4 5");

	assert(parser.getarg<int>("rofl") == 334);
	assert(parser.getarg<bool>("lol") == true);
	assert(parser.getarg<bool>("mao") == true);

	cout << "Expect: \"1 2 3 4 5\"\n";
	AP::printVec(parser.getarglist("hare"),' ');

	parser.help();

	cout << "Parser testing successful!\n";
}

int main(int argc, char* argv[]) {
	using namespace std;

	test_parser(argc, argv);

	return 0;
}
