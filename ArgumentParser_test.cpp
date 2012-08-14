/*
 * ArgumentParser_test.cpp
 *
 *  Created on: Aug 10, 2012
 *      Author: markjr
 */

#include "ArgumentParser.hpp"
#include <iostream>
#include <cassert>

void test_parser() {
	using namespace std;

	ArgumentParser parser;
	parser.addFloatArg("length", 20, 'l', true);
	parser.addIntArg("height", 20, 'h', true);
	parser.addStringArg("name", "Kinsman", 'n', true);
	parser.parse("./go -l  -6001.45e-2 -h 45 -n Joe");
	assert(parser.stringArg("name") == "Joe");
	assert(parser.floatArg("length") - -6001.45e-2 < .0001);
	assert(parser.intArg("height") == 45);
	try {
		bool b = parser.flagArg("lol");
	} catch (ArgParseExcept& e) {
		cout << "Exception successfully thrown\n";
	}

	parser.clear();
	parser.addFlagArg("lol", false,'l',true,"I am here to test you");
	parser.addIntArg("rofl", 25,'r',true,"I am here to test you as well");
	parser.addFlagArg("mao", true,'m',false,"I am here to test you too");
	parser.parse("./go-again -l -r 334");
	assert(parser.intArg("rofl") == 334);
	assert(parser.flagArg("lol") == true);
	assert(parser.flagArg("mao") == true);

	cout << "Parser testing successful!\n";
}

int main(int argc, char* argv[]) {
	using namespace std;

	test_parser();

	return 0;
}
