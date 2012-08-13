/*
 * Argument.hpp
 *
 *  Created on: Aug 10, 2012
 *      Author: markjr
 */

#ifndef ARGUMENT_HPP_
#define ARGUMENT_HPP_

#include <string>


struct ArgBase {
};

template<class T>
class Arg : public ArgBase {
	T val;

public:
	Arg(T _val) :
			val(_val) {
	}
	~Arg() {
	}

	T get() {
		return val;
	}
};

class Argument {
public:
	Argument();
	~Argument();
};

class StringArgument: public Argument {
public:
	StringArgument(std::string _str);
	~StringArgument();
	std::string ret();

private:
	std::string str;
};
typedef StringArgument StrArg;

class IntArgument: public Argument {
public:
	IntArgument(int _n);
	~IntArgument();
	int ret();

private:
	int n;
};
typedef IntArgument IntArg;

class BoolArgument: public Argument {
public:
	BoolArgument(bool _b);
	~BoolArgument();
	bool ret();
private:
	bool b;
};
typedef BoolArgument BoolArg;

class DoubleArgument: public Argument {
public:
	DoubleArgument(double _d);
	~DoubleArgument();
	double ret();
private:
	double d;
};
typedef DoubleArgument DoubleArg;
#endif /* ARGUMENT_HPP_ */
