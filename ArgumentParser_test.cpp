/*
 * ArgumentParser_test.cpp
 *
 *  Created on: Aug 10, 2012
 *      Author: markjr
 */

#include "ArgumentParser.hpp"
#include <iostream>
#include <cassert>

const std::string s = "Hello folks,\n i am a \tginormous elephant";
const std::string hello = "Hello";
const std::string lol =
		"both the antec 620w and the silencer mk3 600w are made by seasonic"
		"and iirc are both on the s12II platform. The pcpc has a 5 year warranty"
		"though rather than antec's 3 year. DOA parts unfortunately happen with any" ""
		"brand, which is why parts should always be tested while you can still return them.";

void test_split() {
	using namespace std;

	vector<string> b = split(s);

	for (auto& p : b)
		cout << p << endl;
	int i = 0;
	assert(b[0] == "Hello");
	assert(b[1] == "folks,");
	assert(b[2] == "i");
	assert(b[5] == "ginormous");
	assert(b[6] == "elephant");
	assert(b.size() == 7);
	i = 0;
}

void test_find() {
	using namespace std;
	int r1 = find(string("are"),split(lol));
	assert (r1 == 9);
	cout << "At index 9 should be \"are\": \"" << split(lol)[r1] << "\"\n";
}

int main() {
	using namespace std;

	ArgumentParser parser;
	parser.addArgument("length", 20, ArgP::DOUBLE, 'l',
			ArgP::STORE,true);

	parser.addArgument("height", 20, ArgP::INT, 'h',
				ArgP::STORE,true);

	parser.addArgument("name", "Kinsman", ArgP::STRING, 'n',
					ArgP::STORE,true);

	test_split();
	test_find();

	parser.parse("./go -l  -6000001.45e-2 -h 45");

	return 0;
}
