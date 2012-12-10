/*
 * ArgumentParser.hpp
 *
 *  Created on: Aug 10, 2012
 *      Author: markjr
 */

#ifndef OPTPARSER_HPP_
#define OPTPARSER_HPP_

#include <exception>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace OP {
typedef enum OptType {
	INT, BOOL, FLOAT, STRING, VSTRING
} Type;

/* Takes a std::string and splits it by given separator
 * default is a space into a vector of strings
 */
std::vector<std::string> split(const std::string& str, const char separator =
		' ');

/*
 * Takes in two iterators and prints out the range between them
 * Optional separator argument
 */
template<typename output_iter>
void print_series(output_iter begin, const output_iter end, const std::string& separator = " ");
/* Attempt to make float input a bit safer
 */
float safer_float(const std::string &);



/* Custom exception class for argument parser */
struct OptParseExcept: public std::exception {
	OptParseExcept(const char * _msg) :
			msg(_msg) {
	}
	~OptParseExcept() {
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
 * The parse method can be called either on an (int argc, char **argv) pair or
 * 	a std::string.
 *
 * There are no static or global variables so you can have as many parsers as
 * 	you want.
 *
 * The parser can take arguments of 5 types, int, bool, float, string, list of strings.
 * 	the boolean option flag takes no argument
 *
 * You can specify if any option is required or not, as well as default values for all
 * 	but the string list option types.
 */
class OptParser {
public:

	/* Constructor sets name displayed as program name in help string
	 */
	OptParser(const std::string& name = "program");

	/* Once parsing had been done, you have to
	 * call this function if you want to add new arguments
	 * and parse something else
	 */
	void clear();

	/* Function to register arguments */
	void add_opt(const OP::Type type, const std::string& name, const std::string& def,
			const char flag, const bool required = false, const std::string& help = "");

	/* Function to add argument vector */


	/* Function to return arguments of various types */


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

	std::vector<std::string> getSortedArgv() {
		return sortedArgv;
	}

	/* To print help message */
	void help() const;

	void print() const
	{
		for (auto & opt : _options) {
			std::cout << "(" << opt.first << "," << opt.second.found << "," << opt.second.val << "," << opt.second.count << ")\n";
		}
	}

private:
	struct Option {
		std::string val;
		unsigned short count;
		bool found;
		bool required;
		char flag;
		std::string help;
		OP::Type type;
	};

	enum ParsingState { FINDING_FLAG, FINDING_ARG};

	std::map<std::string, Option> _options;
	std::map<std::string, std::vector<std::string> > vecMap;
	bool done;

	std::vector<std::string> sortedArgv;
	std::string argv;
	std::string name;

	/* checks if a string is a short option or series thereof */
	bool is_flag(const std::string& s);

	/* checks if a string is a long option or series thereof */
	bool is_long_flag(const std::string& s);

	std::vector<std::string> separate_flags(std::vector<std::string>&);

};
}

template<typename output_iter>
void OP::print_series(output_iter begin, const output_iter end, const std::string& separator)
{
	using std::cout;

	cout << "(";
	if (begin != end) cout << *begin++;

	for (;begin != end; begin++) {
		cout << separator << *begin ;
	}
	cout << ")";
}
#endif /* OPTPARSER_HPP_ */
