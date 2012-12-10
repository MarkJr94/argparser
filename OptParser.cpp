/*
 * ArgumentParser.cpp
 *
 *  Created on: Aug 10, 2012
 *      Author: markjr
 */

#include "OptParser.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <sstream>

/* namespace OP functions ================================================== */

using namespace OP;

std::vector<std::string> OP::split(const std::string& str, const char separator)
{
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

float OP::safer_float(const std::string & ss)
{
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

bool OP::is_flag(const std::string& s)
{
	if (s.empty())
		return false;
	if (s[0] == '-')
		if (s.size() >= 2)
			if (!isdigit(s[1]) && s[1] != '-')
				return true;
	return false;
}

bool OP::is_long_flag(const std::string& s)
{
	if (s.empty())
		return false;
	if (s.size() > 2)
		if (s[0] == '-' && s[1] == '-')
			return true;
	return false;
}

std::vector<std::string> OP::separate_flags(std::vector<std::string>& sVec)
{
	using namespace std;

	vector<string> ret;

	for (vector<string>::iterator it = sVec.begin(); it != sVec.end(); it++) {
		if (is_flag(*it)) {
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
				done(false),
				argv(""),
				name(_name)
{
}

void OptParser::add_opt(const OP::Type type, const std::string& name,
		const std::string& def, const char flag, const bool required,
		const std::string& help)
{
	_options[name] = {def, 0, false, required, flag, help, type};
}

unsigned OptParser::parse(const int argc, char * _argv[])
{
	using namespace std;
	using namespace OP;

	if (_options.empty() || done)
		return 0;

	for (int i = 0; i < argc; i++) {
		argv += _argv[i];
		argv += " ";
	}

	return parse(argv);
}

unsigned OptParser::parse(const std::string& _argv)
{
	using namespace std;
	using namespace OP;

	if (_options.empty() || done)
		return 0;

	argv = _argv;

	vector<string> argvec = split(argv);
	argvec = separate_flags(argvec);
	string errstr;

	bool addToVec = false;
	string prevVec;
	unsigned total = 0;

	ParsingState state = ParsingState::FINDING_FLAG;
	auto last_flag = _options.begin();
	for (auto it = argvec.begin(); it != argvec.end(); it++) {
		switch (state) {
		case FINDING_FLAG:
			if (is_flag(*it) || is_long_flag(*it)) {
				last_flag =
						std::find_if(_options.begin(), _options.end(),
								[&](decltype(*_options.begin()) pair) -> bool {
									if (pair.second.flag == (*it)[1] || pair.first == *it) return true;
									return false;
								});
				if (last_flag == _options.end()) {
					errstr = "Invalid option " + *it + " given";
					throw(OP::OptParseExcept(errstr.c_str()));
				}
				last_flag->second.found = true;
				if (last_flag->second.type != Type::BOOL)
					state = FINDING_ARG;
				else {
					last_flag->second.found = true;
					last_flag->second.count++;
					auto& option = last_flag->second;
					if (option.val == "true")
						option.val = "false";
					else
						option.val = "true";
				}
				continue;
			}
			break;
		case FINDING_ARG:
			if (is_flag(*it) || is_long_flag(*it)) {
				errstr = "Option not given for argument\"" + *it + "\"";
				help();
				throw(OptParseExcept(errstr.c_str()));
			}
			last_flag->second.val = *it;
			last_flag->second.found = true;
			last_flag->second.count++;
			if (last_flag->second.type != OP::VSTRING)
				state = FINDING_FLAG;
			++total;
			break;
		}

	}

	done = true;
	return total;
}

void OptParser::clear()
{
	done = false;
	_options.clear();
	argv.clear();
}

#define opreq( x ) ( x ? "Yes" : "No")
#define ops( x ) ( x == OptType::BOOL ? "" : ( x == OptType::VSTRING ? upname+"... " : upname) )

void OptParser::help() const
{
	using namespace std;
	using namespace OP;

	cout << "Usage:\t./" << name << " ";

	for (auto it = _options.begin(); it != _options.end(); it++) {
		const Option info = it->second;
		string upname = it->first;
		for (char& c : upname)
			c = toupper(c);

		cout << "[--" << it->first << " " << ops(info.type) << "] ";
	}
	cout << endl;

	cout << "Options:\n\n";
	for (auto it = _options.begin(); it != _options.end(); it++) {
		const Option info = it->second;
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
