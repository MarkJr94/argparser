/*
 * ArgumentParser.hpp
 *
 *  Created on: Aug 10, 2012
 *      Author: markjr
 */

#ifndef ARGUMENTPARSER_HPP_
#define ARGUMENTPARSER_HPP_

#include <exception>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace AP {
typedef enum ArgumentType {
	INT, BOOL, FLOAT, STRING, VSTRING
} Type;

std::vector<std::string> split(const std::string& str, const char separator = ' ');

template <typename T>
typename std::vector<T>::iterator find(const T& elem, std::vector<T>&);

template <typename T>
void printVec(const std::vector<T>&, const char separator = '\n');

float saferFloat(const std::string &);

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

/*
 * Simple class to parse arguments. The Highlights are:
 * 	A templated addarg method, to tell it what to look for.
 * 	A templated getarg method, which will return the argument you want.
 *
 * The arguments are stored in maps, and so are accessed by the name you
 * 	give them in the addarguments method
 *
 * The parse method can be called either on an (int argc char **argv) pair or
 * 	a std::string.
 *
 * There are no static or global variables so you can have as many parsers as
 * 	you want.
 *
 * The parser can take arguments of 5 types, int, bool, float, string, list of strings.
 * 	the boolean argument flag takes no option
 *
 * You can specify if any argument is required or not, as well as default values for all
 * 	but the string list argument types.
 */
class ArgumentParser {
public:

	struct ArgInfo {
		unsigned short count;
		bool found;
		bool required;
		char flag;
		std::string help;
		AP::Type type;
	};

	/* Constructor sets name displaed as program name in help string
	 */
	ArgumentParser(const std::string& name = "program");
	~ArgumentParser();

	/* Once parsing had been done, you have to
	 * call this function if you want to add new arguments
	 * and parse something else
	 */
	void clear();

	/* Function to add arguments */
	template<typename T>
	void addarg(const std::string& name, const T def, const char flag,
			const bool required = false, const std::string& help = "");

	/* Function to add argument vector */
	void addarglist(const std::string& name,
			const char flag, const bool required = false,
			const std::string& help = "");

	/* Function to return arguments of various types */
	template<typename T>
	T getarg(const std::string&) const;

	/* Function to return argument vector */
	std::vector<std::string> getarglist(const std::string&) const;

	/* Can parse argc and argv or  a string */
	unsigned parse(const int argc, char *argv[]);
	unsigned parse(const std::string&);

	/* You can get the argv last parsed.
	 * as well as a vector with each string sorted in
	 * the order they were parsed
	 */
	std::string getArgv() const {
		return argv;
	}
	std::vector<std::string> getSortedArgvVector() const {
		return sortedArgvVector;
	}

	/* To print help message */
	void help() const;

private:
	std::map<std::string, ArgInfo> infoMap;
	std::map<std::string, int> intMap;
	std::map<std::string, bool> boolMap;
	std::map<std::string, float> floatMap;
	std::map<std::string, std::string> stringMap;
	std::map<std::string, std::vector<std::string> > vecMap;
	std::vector<std::string> sortedArgvVector;
	bool done;
	std::string argv;
	std::string name;
};
}

/*
 * Prints a vector of std::strings, with
 * each string on a newline
 */
template <typename T>
void AP::printVec(const std::vector<T>& vec,const char separator) {
	for (auto &elem : vec)
		std::cout << elem << separator;
	std::cout << '\n';
}

/* Takes a std::string and a vector of std::strings
 * and searches for the string involved, returning an int
 * indicating it's position
 */
template <typename T>
typename std::vector<T>::iterator AP::find(const T& elem,
		std::vector<T>& vec) {
	typename std::vector<T>::iterator it;
	for (it = vec.begin(); it != vec.end(); it++) {
		if (*it == elem) break;
	}
	return it;
}
#endif /* ARGUMENTPARSER_HPP_ */
