/*
 * ArgumentParser.hpp
 *
 *  Created on: Aug 10, 2012
 *      Author: markjr
 */

#ifndef ARGUMENTPARSER_HPP_
#define ARGUMENTPARSER_HPP_

#include <map>
#include <string>
#include <vector>

namespace ArgP {
typedef enum ArgumentType {
	INT, BOOL, DOUBLE, STRING
} ArgType;
enum Action {
	STORE, STORE_TRUE, STORE_FALSE
};
}

class ArgumentParser {
public:

	struct ArgInfo {
		ArgP::ArgType type;
		ArgP::Action action;
		unsigned short count;
		std::string dest;
		bool found;
		bool required;
		char flag;
	};

	ArgumentParser();
	~ArgumentParser();

	template<class T>
	void addArgument(std::string name, T def, ArgP::ArgType type, char flag,
			ArgP::Action action, bool required = false, std::string help = "",
			std::string dest = "");

	unsigned parse(std::string);

private:

	std::map<std::string, ArgInfo> argInfo;
	std::map<std::string, int> intMap;
	std::map<std::string, bool> boolMap;
	std::map<std::string, double> doubleMap;
	std::map<std::string, std::string> stringMap;
	bool done;
};

template<class T>
void ArgumentParser::addArgument(std::string name, T def, ArgP::ArgType type,
		char flag, ArgP::Action action, bool required, std::string help,
		std::string dest) {
	using namespace ArgP;

	ArgInfo& myInfo = argInfo[name];
	myInfo = {type, action, 0, (dest.empty() ? name : dest ), false, required, flag};

	dest = myInfo.dest;
	switch (type) {
	case INT:
		intMap[dest] = def;
		break;
	case BOOL:
		boolMap[dest] = def;
		break;
	case DOUBLE:
		doubleMap[dest] = def;
		break;
	case STRING:
		stringMap[dest] = def;
		break;
	default:
		break;
	}
}

std::vector<std::string> split(const std::string& str);
int find(const std::string s, const std::vector<std::string>);
void print(const std::vector<std::string>);
#endif /* ARGUMENTPARSER_HPP_ */
