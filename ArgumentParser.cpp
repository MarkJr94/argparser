/*
 * ArgumentParser.cpp
 *
 *  Created on: Aug 10, 2012
 *      Author: markjr
 */

#include "ArgumentParser.hpp"

#include <cctype>
#include <exception>
#include <iostream>
#include <stdexcept>

ArgumentParser::ArgumentParser() :
		done(false) {
}

ArgumentParser::~ArgumentParser() {
}

std::vector<std::string> split(const std::string& str) {
	using namespace std;

	vector<string> ret;
	unsigned len = str.size();
	int i = 0, j = 0;

	while (i < len & j < len) {
		while (isspace(str[i]) && i < len)
			i++;
		j = i;
		while (!isspace(str[j]) && j < len)
			j++;
		if (j > i)
			ret.push_back(str.substr(i, j - i));
		i = j;
	}
	return ret;
}

int find(const std::string s, const std::vector<std::string> strvec) {
	int found = 0;
	for (auto& str : strvec) {
		if (str == s)
			++found;
	}

	return found;
}

unsigned ArgumentParser::parse(std::string argv) {
	using namespace std;

	vector<string> sortedArgv;
	vector<string> argvec = split(argv);

	if (argInfo.empty()) return 0;
	map<string, ArgInfo>::iterator it = argInfo.begin();
	while (it++ != argInfo.end()) {
		cout << "||||||||||||||||||||||||||||" << it->first;
		string name = it->first;
		cout << "==================" << name << endl;
		ArgInfo& info = it->second;

		string fullflag = "-";
		fullflag += info.flag;
		string fullname = "--";
		fullname += name;
		int r1 = find(fullname, argvec);
		int r2 = find(fullflag, argvec);
		if ( r1 < 1 && r2 < 1) {
			if (info.required)
				throw(domain_error("Required arg not found"));
		}

		cout << name << ": Found " << r1 + r1 << " times\n";
	}

}

