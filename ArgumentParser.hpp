/*
 * ArgumentParser.hpp
 *
 *  Created on: Aug 10, 2012
 *      Author: markjr
 */

#ifndef ARGUMENTPARSER_HPP_
#define ARGUMENTPARSER_HPP_

#include <exception>
#include <iosfwd>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

struct ArgParseExcept: public std::exception {
	ArgParseExcept(const char * _msg) :
			msg(_msg) {
	}
	~ArgParseExcept() {
	}
	const char * what() const throw () {
		return msg;
	}
private:
	const char *msg;
};

namespace ArgP {
typedef enum ArgumentType {
	INT, BOOL, FLOAT, STRING
} Type;
enum Action {
	STORE, STORE_TRUE, STORE_FALSE
};
}

union Argument {
	double d;
	int i;
	bool b;
	std::string s;
};

class ArgumentParser {
public:

	struct ArgInfo {
		unsigned short count;
		bool found;
		bool required;
		char flag;
		std::string help;
		ArgP::Type type;
	};

	ArgumentParser();
	~ArgumentParser();

	void clear();
	void addIntArg(const std::string& name, const int def, const char flag,
			const bool required = false, const std::string& help = "");

	void addFloatArg(const std::string& name, const float def, const char flag,
			const bool required = false, const std::string& help = "");

	void addStringArg(const std::string& name, const std::string& def,
			const char flag, const bool required = false,
			const std::string& help = "");

	void addFlagArg(const std::string& name, const bool def, const char flag,
			const bool required = false, const std::string& help = "");

	bool flagArg(const std::string name) const;
	int intArg(const std::string& name) const;
	float floatArg(const std::string& name) const;
	std::string stringArg(const std::string& name) const;

	unsigned parse(int argc, const char *argv[]);
	unsigned parse(const std::string&);

	std::string getArgv() const {
		return argv;
	}
	std::vector<std::string> getSortedArgvVector() const {
		return sortedArgvVector;
	}

private:
	std::map<std::string, ArgInfo> infoMap;
	std::map<std::string, int> intMap;
	std::map<std::string, bool> boolMap;
	std::map<std::string, float> floatMap;
	std::map<std::string, std::string> stringMap;
	std::vector<std::string> sortedArgvVector;
	bool done;
	std::string argv;

	std::vector<std::string> split(const std::string& str);
	int find(const std::string s, const std::vector<std::string>);
	void printStrVec(const std::vector<std::string>&);
	float saferFloat(const std::string &);
};


#endif /* ARGUMENTPARSER_HPP_ */
