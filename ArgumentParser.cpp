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

/* namespace AP functions ================================================== */

using namespace AP;

/* Takes a std::string and splits it by whitespaces
 * into a vector of strings
 */
std::vector<std::string> AP::split(const std::string& str, const char separator) {
	using namespace std;

	vector<string> ret;

	string::const_iterator front = str.begin();
	string::const_iterator back;
	string::const_iterator end = str.end();

	while (front !=  end && back != end) {
		while (*front == separator && front !=  str.end())
			front++;
		back = front;
		while (*back != separator && back !=  end)
			back++;
		if  (back > front)
			ret.push_back(str.substr(front - str.begin(), back - front));
		front = back;
	}
	return ret;
}

/* Attempt to make float input a bit safer
 */
float AP::saferFloat(const std::string & ss) {
	using namespace std;

	float d1;
	int pos, minPos, decPos, ePos;

	string validChars = "0123456789-.e";
	/* Check for valid characters */
	if ((pos = ss.find_first_not_of(validChars)) != string::npos) {
		throw(ArgParseExcept("Invalid character in double input"));
	}

	/* Check decimal */
	if ((decPos = ss.find_first_of('.') != ss.find_last_of('.'))) {
		throw(ArgParseExcept("Too many decimals"));
	}

	/* Check for exponentiation */
	if ((ePos = ss.find_first_of("eE")) != string::npos) {
		if (ePos != ss.find_last_of("eE")) {
			throw(ArgParseExcept("Too many 'e's"));
		}

		if (ePos < decPos) {
			throw(ArgParseExcept("Invalid placement of exponentiation"));
		}
	}

	int rminPos;
	/* Check signage */
	if ((minPos = ss.find_first_of('-')) != string::npos) {
		if (minPos != (rminPos = ss.find_last_of('-'))) {
			if (rminPos != ePos + 1)
				throw ArgParseExcept("Too many '-'s");
		}

		if (minPos > ss.find_first_not_of(" \t\n\r", 0)) {
			throw ArgParseExcept("Incorrect placement of '-'");
		}
	}

	stringstream sss(ss);
	sss >> d1;
	if (sss.fail()) {
		throw ArgParseExcept("float input failed");
	}

	return d1;
}

/* ========== Argument parser definitions ========= */

ArgumentParser::ArgumentParser(const std::string& _name) :
		done(false), argv(""), name(_name) {
}

ArgumentParser::~ArgumentParser() {
}

template<>
void ArgumentParser::addarg<int>(const std::string& name, const int def,
		const char flag, const bool required, const std::string& help) {
	infoMap[name] = {0, false, required, flag, help, AP::INT};
	intMap[name] = def;
}

template<>
void ArgumentParser::addarg<bool>(const std::string& name, const bool def,
		const char flag, const bool required, const std::string& help) {
	infoMap[name] = {0, false, required, flag, help, AP::BOOL};
	boolMap[name] = def;
}

template<>
void ArgumentParser::addarg<float>(const std::string& name, const float def,
		const char flag, const bool required, const std::string& help) {
	infoMap[name] = {0, false, required, flag, help, AP::FLOAT};
	floatMap[name] = def;
}

template<>
void ArgumentParser::addarg<std::string>(const std::string& name,
		const std::string def, const char flag, const bool required,
		const std::string& help) {
	infoMap[name] = {0, false, required, flag, help, AP::STRING};
	stringMap[name] = def;
}

void ArgumentParser::addarglist(const std::string& name,const char flag,
		const bool required, const std::string& help) {
	using namespace AP;

	infoMap[name] = {0, false, required, flag, help, VSTRING};
}


std::vector<std::string> ArgumentParser::getarglist(const std::string& name) const {
	using namespace std;

	map<string, vector<string> >::const_iterator it;
	if ((it = vecMap.find(name)) == vecMap.end()) {
		string errstr = "Non-existent argument \"" + name + "\" requested";
		help();
		throw(AP::ArgParseExcept(errstr.c_str()));
	}

	return it->second;
}

template<>
bool ArgumentParser::getarg<bool>(const std::string& name) const {
	using namespace std;

	map<string, bool>::const_iterator it = boolMap.begin();
	if ((it = boolMap.find(name)) == boolMap.end()) {
		string errstr = "Non-existent argument \"" + name + "\" requested";
		help();
		throw(AP::ArgParseExcept(errstr.c_str()));
	}

	return it->second;
}

template<>
int ArgumentParser::getarg<int>(const std::string& name) const {
	using namespace std;

	map<string, int>::const_iterator it;
	if ((it = intMap.find(name)) == intMap.end()) {
		string errstr = "Non-existent argument \"" + name + "\" requested";
		help();
		throw(AP::ArgParseExcept(errstr.c_str()));
	}

	return it->second;
}

template<>
float ArgumentParser::getarg<float>(const std::string& name) const {
	using namespace std;

	map<string, float>::const_iterator it;
	if ((it = floatMap.find(name)) == floatMap.end()) {
		string errstr = "Non-existent argument \"" + name + "\" requested";
		help();
		throw(AP::ArgParseExcept(errstr.c_str()));
	}

	return it->second;
}

template<>
std::string ArgumentParser::getarg<std::string>(const std::string& name) const {
	using namespace std;

	map<string, string>::const_iterator it;
	if ((it = stringMap.find(name)) == stringMap.end()) {
		string errstr = "Non-existent argument \"" + name + "\" requested";
		help();
		throw(AP::ArgParseExcept(errstr.c_str()));
	}

	return it->second;
}

unsigned ArgumentParser::parse(const int argc, char * _argv[]) {
	using namespace std;
	using namespace AP;

	if (infoMap.empty() || done)
		return 0;

	for (int i = 0; i < argc; i++) {
		argv += _argv[i];
		argv += " ";
	}

	return parse(argv);
}

unsigned ArgumentParser::parse(const std::string& _argv) {
	using namespace std;
	using namespace AP;

	if (infoMap.empty() || done)
		return 0;

	argv = _argv;
	sortedArgvVector.clear();

	vector<string> argvec = split(argv);
	string errstr;

	unsigned total = 0;

	for (auto it = infoMap.begin(); it != infoMap.end(); it++) {
		string name = it->first;
		ArgInfo& info = it->second;

		string fullflag = "-";
		fullflag += info.flag;
		string fullname = "--" + name;
		vector<string>::iterator r1 = find(fullname, argvec);
		vector<string>::iterator r2 = find(fullflag, argvec);
		if (r1 == argvec.end() && r2 == argvec.end()) {
			if (info.required) {
				errstr = "Required arg \"" + name + "\" not found";
				help();
				throw(ArgParseExcept(errstr.c_str()));
			} else {
				continue;
			}
		}

		vector<string>::iterator first;
		if (r1 != argvec.end() && r2 != argvec.end())
			first = (r1 < r2 ? r1 : r2);
		else
			first = (r1 != argvec.end() ? r1 : r2);

		float f;
		int i;
		string s;

		stringstream possible;
		string p;
		if ( (first +1) == argvec.end() && info.type != BOOL) {
			errstr = "Option not given for argument\"" + name+"\"";
			help();
			throw(ArgParseExcept(errstr.c_str()));
		}
		vector<string>::iterator next = first + 1;

		switch (info.type) {
		case INT:
			possible << *next;
			possible >> i;
			intMap[name] = i;
			break;
		case FLOAT:
			p = *next;
			f = saferFloat(p);
			floatMap[name] = f;
			break;
		case STRING:
			stringMap[name] = *next;
			break;
		case BOOL:
			boolMap[name] = !boolMap[name];
			break;
		case VSTRING:
			for (p = *next; next != argvec.end(); next++) {
				p = *next;
				if (p[0] == '-')
					if (p.size() == 2)
						if (!isdigit(p[1]))
							break;
				vecMap[name].push_back(p);
			}
			break;
		}

		if (possible.fail()) {
			errstr = "Invalid argument \"" + p + "\" specified for option: \"";
			errstr += name + "\"";
			help();
			throw(ArgParseExcept(errstr.c_str()));
		}

		sortedArgvVector.push_back(name);
		sortedArgvVector.push_back(p);
		total++;
	}

	done = true;
	return total;
}

void ArgumentParser::clear() {
	done = false;
	sortedArgvVector.clear();
	infoMap.clear();
	intMap.clear();
	boolMap.clear();
	floatMap.clear();
	argv.clear();
}

#define opreq( x ) ( x ? "Yes" : "No")

void ArgumentParser::help() const {
	using namespace std;
	using namespace AP;

	cout << "Usage:\t./" << name << " ";

	for (auto it = infoMap.begin(); it != infoMap.end(); it++) {
		const ArgInfo info = it->second;
		string upname = it->first;
		for (char& c : upname)
			c = toupper(c);

		cout << "[--" << it->first << " " << (info.type == BOOL ? "" : upname)
				<< "] ";
	}
	cout << endl;

	cout << "Options:\n\n";
	for (auto it = infoMap.begin(); it != infoMap.end(); it++) {
		const ArgInfo info = it->second;
		string upname = it->first;
		for (char& c : upname)
			c = toupper(c);

		cout << "--" << it->first << " (-" << info.flag << ")" << "\t";

		cout << "Required: " << opreq(info.required);
		cout << "\tType: ";

		switch (info.type) {
		case INT:
			cout << "INT\n";
			break;
		case FLOAT:
			cout << "FLOAT\n";
			break;
		case BOOL:
			cout << "BOOL\n";
			break;
		case STRING:
			cout << "STRING\n";
			break;
		default:
			cout << "VECTOR\n";
			break;
		}

		cout << "\t";
		int i = 0;
		for (auto& c : info.help) {
			cout << c;
			if (i > 60 && isspace(c)) {
				cout << "\n\t\t";
				i = 0;
			}
			i++;
		}
		cout << "\n\n";
	}
}
