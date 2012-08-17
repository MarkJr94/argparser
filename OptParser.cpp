/*
 * ArgumentParser.cpp
 *
 *  Created on: Aug 10, 2012
 *      Author: markjr
 */

#include "OptParser.hpp"

#include <cctype>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <sstream>

/* namespace AP functions ================================================== */

using namespace OP;

std::vector<std::string> OP::split(const std::string& str,
		const char separator) {
	using namespace std;

	vector<string> ret;

	string::const_iterator front = str.begin();
	string::const_iterator back;
	string::const_iterator end = str.end();

	while (front != end && back != end) {
		while (*front == separator && front != str.end())
			front++;
		back = front;
		while (*back != separator && back != end)
			back++;
		if (back > front)
			ret.push_back(str.substr(front - str.begin(), back - front));
		front = back;
	}
	return ret;
}

float OP::saferFloat(const std::string & ss) {
	using namespace std;

	float d1;
	int pos, minPos, decPos, ePos;

	string validChars = "0123456789-.e";
	/* Check for valid characters */
	if ((pos = ss.find_first_not_of(validChars)) != string::npos) {
		throw(OptParseExcept("Invalid character in double input"));
	}

	/* Check decimal */
	if ((decPos = ss.find_first_of('.') != ss.find_last_of('.'))) {
		throw(OptParseExcept("Too many decimals"));
	}

	/* Check for exponentiation */
	if ((ePos = ss.find_first_of("eE")) != string::npos) {
		if (ePos != ss.find_last_of("eE")) {
			throw(OptParseExcept("Too many 'e's"));
		}

		if (ePos < decPos) {
			throw(OptParseExcept("Invalid placement of exponentiation"));
		}
	}

	int rminPos;
	/* Check signage */
	if ((minPos = ss.find_first_of('-')) != string::npos) {
		if (minPos != (rminPos = ss.find_last_of('-'))) {
			if (rminPos != ePos + 1)
				throw OptParseExcept("Too many '-'s");
		}

		if (minPos > ss.find_first_not_of(" \t\n\r", 0)) {
			throw OptParseExcept("Incorrect placement of '-'");
		}
	}

	stringstream sss(ss);
	sss >> d1;
	if (sss.fail()) {
		throw OptParseExcept("float input failed");
	}

	return d1;
}

bool OP::isFlag(const std::string& s) {
	if (s.empty())
		return false;
	if (s[0] == '-')
		if (s.size() >= 2)
			if (!isdigit(s[1]) && s[1] != '-')
				return true;
	return false;
}

bool OP::isLongFlag(const std::string& s) {
	if (s.empty())
		return false;
	if (s.size() > 2)
		if (s[0] == '-' && s[1] == '-')
			return true;
	return false;
}

std::vector<std::string> OP::separateFlags(std::vector<std::string>& sVec) {
	using namespace std;

	vector<string> ret;

	for (vector<string>::iterator it = sVec.begin(); it != sVec.end(); it++) {
		if (isFlag(*it)) {
			if (it->size() > 2) {
				ret.push_back(it->substr(0, 2));
				string rest = it->substr(2);
				for (auto c : rest) {
					string s = "-";
					s += c;
					ret.push_back(s);
				}
			} else
				ret.push_back(*it);
		} else {
			ret.push_back(*it);
		}
	}

	return ret;
}

/* ========== Argument parser definitions ========= */

OptParser::OptParser(const std::string& _name) :
		done(false), argv(""), name(_name) {
}

OptParser::~OptParser() {
}

template<>
void OptParser::addOpt<int>(const std::string& name, const int def,
		const char flag, const bool required, const std::string& help) {
	infoMap[name] = {0, false, required, flag, help, OP::OptType::INT};
	intMap[name] = def;
}

template<>
void OptParser::addOpt<bool>(const std::string& name, const bool def,
		const char flag, const bool required, const std::string& help) {
	infoMap[name] = {0, false, required, flag, help, OP::OptType::BOOL};
	boolMap[name] = def;
}

template<>
void OptParser::addOpt<float>(const std::string& name, const float def,
		const char flag, const bool required, const std::string& help) {
	infoMap[name] = {0, false, required, flag, help, OP::OptType::FLOAT};
	floatMap[name] = def;
}

template<>
void OptParser::addOpt<std::string>(const std::string& name,
		const std::string def, const char flag, const bool required,
		const std::string& help) {
	infoMap[name] = {0, false, required, flag, help, OP::OptType::STRING};
	stringMap[name] = def;
}

void OptParser::addOptList(const std::string& name, const char flag,
		const bool required, const std::string& help) {
	using namespace OP;

	infoMap[name] = {0, false, required, flag, help, OP::OptType::VSTRING};
}

std::vector<std::string> OptParser::getOptList(
		const std::string& name) const {
	using namespace std;

	map<string, vector<string> >::const_iterator it;
	if ((it = vecMap.find(name)) == vecMap.end()) {
		string errstr = "Non-existent argument \"" + name + "\" requested";
		help();
		throw(OP::OptParseExcept(errstr.c_str()));
	}

	return it->second;
}

template<>
bool OptParser::getOpt<bool>(const std::string& name) const {
	using namespace std;

	map<string, bool>::const_iterator it = boolMap.begin();
	if ((it = boolMap.find(name)) == boolMap.end()) {
		string errstr = "Non-existent argument \"" + name + "\" requested";
		help();
		throw(OP::OptParseExcept(errstr.c_str()));
	}

	return it->second;
}

template<>
int OptParser::getOpt<int>(const std::string& name) const {
	using namespace std;

	map<string, int>::const_iterator it;
	if ((it = intMap.find(name)) == intMap.end()) {
		string errstr = "Non-existent argument \"" + name + "\" requested";
		help();
		throw(OP::OptParseExcept(errstr.c_str()));
	}

	return it->second;
}

template<>
float OptParser::getOpt<float>(const std::string& name) const {
	using namespace std;

	map<string, float>::const_iterator it;
	if ((it = floatMap.find(name)) == floatMap.end()) {
		string errstr = "Non-existent argument \"" + name + "\" requested";
		help();
		throw(OP::OptParseExcept(errstr.c_str()));
	}

	return it->second;
}

template<>
std::string OptParser::getOpt<std::string>(const std::string& name) const {
	using namespace std;

	map<string, string>::const_iterator it;
	if ((it = stringMap.find(name)) == stringMap.end()) {
		string errstr = "Non-existent argument \"" + name + "\" requested";
		help();
		throw(OP::OptParseExcept(errstr.c_str()));
	}

	return it->second;
}

std::map<std::string, OP::OptInfo>::const_iterator OptParser::matchArg(
		const std::string& opt, const bool isFlag) const {
	using namespace std;
	typedef map<string, OP::OptInfo>::const_iterator constit;

	string errstr;
	if (isFlag) {
		char flag = opt[1];
		for (constit it = infoMap.begin(); it != infoMap.end(); it++) {
			if (flag == it->second.flag) {
				return it;
			}
		}
		errstr = "Invalid short flag argument " + opt + " given";
		throw(OP::OptParseExcept(errstr.c_str()));
	} else {
		string base = opt.substr(2);
		for (constit it = infoMap.begin(); it != infoMap.end(); it++) {
			if (base == it->first) {
				return it;
			}
		}
		errstr = "Invalid long flag argument " + opt + " given";
		throw(OP::OptParseExcept(errstr.c_str()));
	}

}

std::map<std::string, OP::OptInfo>::iterator OptParser::matchArg(
		const std::string& opt, const bool isFlag) {
	using namespace std;
	typedef map<string, OP::OptInfo>::iterator mapit;

	string errstr;
	if (isFlag) {
		char flag = opt[1];
		for (mapit it = infoMap.begin(); it != infoMap.end(); it++) {
			if (flag == it->second.flag) {
				return it;
			}
		}
		errstr = "Invalid short  flag argument " + opt + " given";
		throw(OP::OptParseExcept(errstr.c_str()));
	} else {
		string base = opt.substr(2);
		for (mapit it = infoMap.begin(); it != infoMap.end(); it++) {
			if (base == it->first) {
				return it;
			}
		}
		errstr = "Invalid long flag argument " + opt + " given";
		throw(OP::OptParseExcept(errstr.c_str()));
	}

}

unsigned OptParser::parse(const int argc, char * _argv[]) {
	using namespace std;
	using namespace OP;

	if (infoMap.empty() || done)
		return 0;

	for (int i = 0; i < argc; i++) {
		argv += _argv[i];
		argv += " ";
	}

	return parse(argv);
}

unsigned OptParser::parse(const std::string& _argv) {
	using namespace std;
	using namespace OP;

	if (infoMap.empty() || done)
		return 0;

	argv = _argv;

	vector<string> argvec = split(argv);
	argvec = separateFlags(argvec);
	string errstr;

	bool addToVec = false;
	string prevVec;
	unsigned total = 0;

	map<string, OptInfo>::iterator mapit;
	for (auto it = argvec.begin(); it != argvec.end(); it++) {
		if (isFlag(*it)) {
			mapit = matchArg(*it, true);
			addToVec = false;
		} else if (isLongFlag(*it)) {
			mapit = matchArg(*it, false);
			addToVec = false;
		} else if (addToVec) {
			++total;
			vecMap[prevVec].push_back(*it);
			continue;
		} else
			continue;

		OP::OptInfo& info = mapit->second;
		string name = mapit->first;
		info.found = true;

		stringstream possible;
		string p;
		auto next = it + 1;
		if (next == argvec.end() && info.type != OptType::BOOL) {
			errstr = "Option not given for argument\"" + name + "\"";
			help();
			throw(OptParseExcept(errstr.c_str()));
		}

		float f;
		int i;
		string s;

		switch (info.type) {
		case OptType::INT:
			possible << *next;
			possible >> i;
			intMap[name] = i;
			++total;
			break;
		case OptType::FLOAT:
			p = *next;
			f = saferFloat(p);
			floatMap[name] = f;
			++total;
			break;
		case OptType::STRING:
			stringMap[name] = *next;
			++total;
			break;
		case OptType::BOOL:
			boolMap[name] = !boolMap[name];
			++total;
			break;
		case OptType::VSTRING:
			prevVec = name;
			addToVec = true;
			break;
		}

	}

	done = true;
	return total;
}

void OptParser::clear() {
	done = false;
	infoMap.clear();
	intMap.clear();
	boolMap.clear();
	floatMap.clear();
	argv.clear();
}

#define opreq( x ) ( x ? "Yes" : "No")
#define ops( x ) ( x == OptType::BOOL ? "" : ( x == OptType::VSTRING ? upname+"... " : upname) )

void OptParser::help() const {
	using namespace std;
	using namespace OP;

	cout << "Usage:\t./" << name << " ";

	for (auto it = infoMap.begin(); it != infoMap.end(); it++) {
		const OptInfo info = it->second;
		string upname = it->first;
		for (char& c : upname)
			c = toupper(c);

		cout << "[--" << it->first << " " << ops(info.type)
		<< "] ";
	}
	cout << endl;

	cout << "Options:\n\n";
	for (auto it = infoMap.begin(); it != infoMap.end(); it++) {
		const OptInfo info = it->second;
		string upname = it->first;
		for (char& c : upname)
			c = toupper(c);

		cout << "--" << it->first << " (-" << info.flag << ")" << "\t";

		cout << "Required: " << opreq(info.required);
		cout << "\tType: ";

		switch (info.type) {
		case OptType::INT:
			cout << "INT\n";
			break;
		case OptType::FLOAT:
			cout << "FLOAT\n";
			break;
		case OptType::BOOL:
			cout << "BOOL\n";
			break;
		case OptType::STRING:
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
