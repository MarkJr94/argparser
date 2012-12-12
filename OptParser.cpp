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

bool db = false;
#define $ if(db)

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

bool OP::OptParser::is_flag(const std::string& s)
{
	if (s.empty())
		return false;
	if (s[0] == '-')
		if (s.size() >= 2)
			if (!isdigit(s[1]) && s[1] != '-')
				return true;
	return false;
}

bool OP::OptParser::is_long_flag(const std::string& s)
{
	if (s.empty())
		return false;
	if (s.size() > 2)
		if (s[0] == '-' && s[1] == '-')
			return true;
	return false;
}

std::vector<std::string> OP::OptParser::separate_flags(
		const std::vector<std::string>& sVec)
{
	using namespace std;

	vector<string> ret;

	for (auto it = sVec.begin(); it != sVec.end(); it++) {
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

OptParser::OptParser(const std::string& name) :
				done(false),
				m_argv(""),
				m_name(name)
{
}

void OptParser::add_opt(const std::string& name, const std::string& def,
		const char flag, const bool required, const std::string& help, OP::Type type)
{
	m_options[name] = {def, 0, false, required, flag, help, type};
}

unsigned OptParser::parse(const int argc, char * argv[])
{
	using namespace std;
	using namespace OP;

	if (m_options.empty() || done)
		return 0;

	string argv_str;
	for (int i = 0; i < argc; i++) {
		argv_str += argv[i];
		argv_str += " ";
	}

	return parse(argv_str);
}

unsigned OptParser::parse(const std::string& argv)
{
	using namespace std;
	using namespace OP;

	if (m_options.empty() || done)
		return 0;

	m_argv = argv;

	vector<string> argvec = split(m_argv);
	argvec = separate_flags(argvec);
	string errstr;

	$ cout << "Split up argvec:";
	$ print_series(argvec.begin(), argvec.end());
	$ cout << endl;

	unsigned total = 0;

	ParsingState state = ParsingState::FINDING_FLAG;
	auto last_flag = m_options.begin();
	for (auto it = argvec.begin(); it != argvec.end(); it++) {
		switch (state) {
		case FINDING_FLAG:
			if (is_flag(*it) || is_long_flag(*it)) {
				last_flag =
						std::find_if(m_options.begin(), m_options.end(),
								[&](decltype(*m_options.begin()) pair) -> bool {
									if (pair.second.flag == (*it)[1] || pair.first == *it) return true;
									return false;
								});
				if (last_flag == m_options.end()) {
					errstr = "Invalid option " + *it + " given";
					throw(OP::OptParseExcept(errstr.c_str()));
				}
				last_flag->second.found = true;
				if (last_flag->second.type != Type::FLAG)
					state = FINDING_ARG;
				else {
					auto& option = last_flag->second;
					if (option.count == 0) {
						if (option.val == "true")
							option.val = "false";
						else
							option.val = "true";
					}

					last_flag->second.found = true;
					last_flag->second.count++;
				}
				continue;
			}
			break;
		case FINDING_ARG:
			if (is_flag(*it) || is_long_flag(*it)) {
				if (last_flag->second.count == 0) {
					errstr = "Option not given for argument \"" + *it + "\"";
					help();
					throw(OptParseExcept(errstr.c_str()));
				} else {
					it--;
					state = FINDING_FLAG;
					break;
				}
			}
			if (last_flag->second.type == OP::LIST) {
				if (last_flag->second.count > 0) last_flag->second.val += *it + " ";
				else last_flag->second.val = *it + " ";
				$ cout << "List arg found \n";
			} else {
				last_flag->second.val = *it;
				state = FINDING_FLAG;
			}
			last_flag->second.found = true;
			last_flag->second.count++;
			++total;
			break;
		}

	}

	auto unfound = std::find_if(m_options.begin(), m_options.end(),
			[&](decltype(*m_options.begin()) pair) -> bool {
				if (pair.second.required && !pair.second.found) return true;
				return false;
			});

	if (unfound != m_options.end()) {
		errstr = "Required option \"" + unfound->first + "\" not given.";
		throw(OptParseExcept(errstr.c_str()));
	}

	done = true;
	return total;
}

void OptParser::clear()
{
	done = false;
	m_options.clear();
	m_argv.clear();
}

#define opreq( x ) ( x ? "Yes" : "No")
#define ops( x ) ( x == Type::FLAG ? "" : ( x == Type::LIST ? upname+"... " : upname) )

void OptParser::help() const
{
	using namespace std;
	using namespace OP;

	cout << "Usage:\t./" << m_name << " ";

	for (auto it = m_options.begin(); it != m_options.end(); it++) {
		const Option info = it->second;
		string upname = it->first;
		for (char& c : upname)
			c = toupper(c);

		cout << "[--" << it->first << " " << ops(info.type) << "] ";
	}
	cout << endl;

	cout << "Options:\n\n";
	for (auto it = m_options.begin(); it != m_options.end(); it++) {
		const Option info = it->second;
		string upname = it->first;
		for (char& c : upname)
			c = toupper(c);

		cout << "--" << it->first << " (-" << info.flag << ")" << "\t";

		cout << "Required: " << opreq(info.required);
		cout << "\tType: ";

		switch (info.type) {
		case Type::OPTION:
			cout << "OPTION\n";
			break;
		case Type::LIST:
			cout << "LIST\n";
			break;
		case Type::FLAG:
			cout << "FLAG\n";
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

void OptParser::print(std::ostream& os) const
{
	auto found =
			[](const bool b) -> std::string {return b ? "found" : "not found'";};

	for (auto & opt : m_options) {
		os << "(Name: " << opt.first << ", Status: "
				<< found(opt.second.found) << ", Value: {" << opt.second.val
				<< "}, Count: " << opt.second.count << ")\n";
	}
}
