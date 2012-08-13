/*
 * Argument.cpp
 *
 *  Created on: Aug 10, 2012
 *      Author: markjr
 */

#include "Argument.hpp"

Argument::Argument() {
}

Argument::~Argument() {
}

StringArgument::StringArgument(std::string _str) :str(_str) {
}

StringArgument::~StringArgument() {
}

std::string StringArgument::StringArgument::ret() {
	return str;
}

IntArgument::IntArgument(int _n) :n(_n) {
}

IntArgument::~IntArgument() {
}

int IntArgument::IntArgument::ret() {
	return n;
}

BoolArgument::BoolArgument(bool _b) :b(_b) {
}

BoolArgument::~BoolArgument() {
}

bool BoolArgument::BoolArgument::ret() {
	return b;
}

DoubleArgument::DoubleArgument(double _d) :d(_d) {
}

DoubleArgument::~DoubleArgument() {
}

double DoubleArgument::ret() {
	return d;
}
