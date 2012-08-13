/*
 * ArgumentParser.cpp
 *
 *  Created on: Aug 10, 2012
 *      Author: markjr
 */

#include "ArgumentParser.hpp"

#include <cctype>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <sstream>

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
	int found = -1;
	int index = 0;
	for (auto& str : strvec) {
		if (str == s) {
			found = index;
			break;
		}
		index++;
	}

	return found;
}

void print(const std::vector<std::string> strVec) {
	for (auto &s : strVec)
		std::cout << s << std::endl;
}

double safeDouble (const std::string & ss) {
	using namespace std;

	double d1;
	int pos, minPos, decPos, ePos;

	string validChars = "0123456789-";
	/* Check for valid characters */
	if ( (pos = ss.find_first_not_of(validChars)) != string::npos) {
		throw (domain_error("Invalid character in double input"));
	}

	/* Check decimal */
	if ((decPos = ss.find_first_of('.') != ss.find_last_of('.'))) {
		throw (domain_error("Too many decimals"));
	}

	/* Check for exponentiation */
	if ((ePos = ss.find_first_of("eE")) != string::npos) {
		if (ePos != ss.find_last_of("eE")) {
			throw (domain_error("Too many 'e's"));
		}

		if (ePos < decPos) {
			throw (domain_error("Invalid placement of exponentiation"));
		}
	}

	/* Check signage */
	if ((minPos= ss.find_first_of('-')) != ) ;


}

unsigned ArgumentParser::parse(std::string argv) {
	using namespace std;
	using namespace ArgP;

	vector<string> sortedArgv;
	vector<string> argvec = split(argv);

	if (argInfo.empty())
		return 0;
//	map<string, ArgInfo>::iterator it = argInfo.begin();
	for (auto it = argInfo.begin(); it != argInfo.end(); it++) {
		string name = it->first;
		ArgInfo& info = it->second;

		string fullflag = "-";
		fullflag += info.flag;
		string fullname = "--" + name;
		int r1 = find(fullname, argvec);
		int r2 = find(fullflag, argvec);
		if (r1 < 1 && r2 < 1) {
			if (info.required)
				throw(domain_error("Required arg not found"));
		}

		int first;
		if (r1 > 0 && r2 > 0)
			first = (r1 < r2 ? r1 : r2);
		else
			first = (r1 > 0 ? r1 : r2);

		double d;
		int i;
		string s;
		if (info.action == STORE) {
			stringstream possible;
			possible << argvec[first + 1];
			string p = argvec[first+1];

			switch (info.type) {
			case INT:
				possible >> i;
				cout << i;
				break;
			case DOUBLE:
				d = atof(p.c_str());
				possible >> d;
				cout << d;
				break;
			case STRING:
				possible >> s;
				cout << s;
				break;
			default:
				break;
			}

			if (possible.fail())
				throw(domain_error("Invalid option for" + name));

		}
		cout << endl;
		cout << name << ": Found at " << first << "\n";
	}

}

