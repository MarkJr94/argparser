/*
 * Argument_test.cpp
 *
 *  Created on: Aug 10, 2012
 *      Author: markjr
 */

#include <string>
#include <iostream>
#include <vector>
#include <cassert>

#include "Argument.hpp"

int main() {
	using namespace std;

	StrArg s_Arg ("Hello");
	IntArg i_Arg (500);
	BoolArg b_Arg (false);
//	Argument<vector<int> > v_Arg ( vector<int> (8,100));
	Arg<double> d_Arg (11111.111);
//
	assert (b_Arg.ret() == false);
	assert (s_Arg.ret() == "Hello");
	assert (i_Arg.ret() == 500);
//	assert (v_Arg.ret()[0] == 100);
	assert (d_Arg.get() == 11111.111);
//	cout << "Assertions passed!\n\n";
//

	vector<ArgBase *> vec;
	vec.push_back(new Arg<double> (1.0));
	cout << "Vec arg: " << vec[0]->get();
	cout << "String arg:\t" << s_Arg.ret() << endl;
	cout << "Integer arg:\t" << i_Arg.ret() << endl;
	cout << "Boolean arg:\t" << b_Arg.ret() << endl;
//	cout << "Vector<int> arg:\t" << v_Arg.ret()[0] << endl;
	cout << "Double arg:\t" << d_Arg.get() << endl;

	return 0;
}

