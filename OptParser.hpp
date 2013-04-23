/*
 * ArgumentParser.hpp
 *
 *  Created on: Aug 10, 2012
 *      Author: markjr
 */

#ifndef OPTPARSER_HPP_
#define OPTPARSER_HPP_

#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace OP {
typedef enum  {
	OPTION, FLAG, LIST
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
void print_series(output_iter begin, const output_iter end,
		const std::string& separator = " ");

/* Custom exception class for argument parser */
struct OptParseExcept: public std::exception {
	OptParseExcept(const char * _msg) :
					msg(_msg)
	{
	}
	~OptParseExcept() noexcept
	{
	}
	const char * what() const noexcept
	{
		return msg;
	}
private:
	const char *msg;
};

/*
 * Simple class to parse arguments. The Highlights are:
 * A simple add_opt method to register arguments. You
 * 	A templated get_opt method, which will return the argument you want.
 * 	It can parse either an (int argc, char **argv) pair, or a std::string.
 * 	You can control how arguments are converted from strings to other types.
 *
 * The arguments are stored as a string in a std::map, and so are accessed
 * 	by the name you	give them in the add_opt method. Since they are all
 * 	stored as strings, they are only converted to the relevant types when
 * 	you call the get_opt<T> method.
 *
 * You can control how your options are converted to the proper types
 * 	by providing a lambda/function object to the get_opt<T> method.
 *
 * The parser also provides a default help message based on your option
 * 	types and help strings you pass in when registering options.
 *
 * The parse method can be called either on an (int argc, char **argv) pair or
 * 	a std::string.
 *
 * There are no static or global variables so you can have as many parsers as
 * 	you want.
 */
class OptParser {
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

	template<class T>
	struct Getter {
		T operator()(const std::string& option)
		{
			std::stringstream ss(option);
			T t;
			ss >> t;
			return t;
		}
	};

public:

	/* Constructor sets name displayed as program name in help string
	 */
	OptParser(const std::string& name = "program");

	OptParser(const OptParser&) = default;
	OptParser(OptParser&&) = default;
	OptParser& operator=(const OptParser&) = default;
	OptParser& operator=(OptParser&&) = default;

	/* Once parsing had been done, you have to
	 * call this function if you want to add new arguments
	 * and parse something else
	 */
	void clear();

	/* Function to register arguments
	 * The type parameter indicates whether the option takes no
	 * arguments, one argument, or many
	 * */
	void add_opt(const std::string& name, const std::string& def,
			const char flag, const bool required = false,
			const std::string& help = "", OP::Type type = OPTION);

	/* Function to return arguments of various types
	 * You can provide your own function for converting from a string
	 * to the desired type. The provided defaults use std::stringstream
	 * capabilities. The boolean case is specialized (internally stored
	 * as "true"/"false" strings.
	 * There is also specialization to return a vector of strings.
	 * */
	template<class T>
	T get_as(const std::string& name, std::function<T(std::string)> getter = Getter<T>());

	/* Can parse argc and argv or  a string */
	unsigned parse(const int argc, char *argv[]);
	unsigned parse(const std::string&);

	/* You can get the argv last parsed.
	 * as well as a vector with each string sorted in
	 * the order they were parsed
	 */
	std::string get_argv() const
	{
		return m_argv;
	}

	/* To print help message */
	void help() const;

	/* Prints out list and status of arguments */
	void print(std::ostream& os = std::cout) const;

private:
	std::map<std::string, Option> m_options;
	bool done;

	std::string m_argv;
	std::string m_name;

	/* checks if a string is a short option or series thereof */
	bool is_flag(const std::string& s);

	/* checks if a string is a long option or series thereof */
	bool is_long_flag(const std::string& s);

	/* Splits repeated option flags for easier parsing:
	 * i.e. -mvgh ==> -m -v -g -h
	 */
	std::vector<std::string> separate_flags(const std::vector<std::string>&);

	enum ParsingState {
		FINDING_FLAG, FINDING_ARG
	};
};


template<typename output_iter>
void print_series(output_iter begin, const output_iter end,
		const std::string& separator)
{
	using std::cout;

	cout << "(";
	if (begin != end)
		cout << *begin++;

	for (; begin != end; begin++) {
		cout << separator << *begin;
	}
	cout << ")" << std::endl;
}

template<class T>
T OptParser::get_as(const std::string& name, std::function<T(std::string)> getter)
{
	auto option = m_options.find(name);
	if (option == m_options.end()) {
		std::string errstr = "Non-existent option \"" + name + "\" requested.\n";
		throw(OptParseExcept(errstr.c_str()));
	}
	return getter(option->second.val);
}


template<>
struct OptParser::Getter<bool> {
	bool operator()(const std::string& option)
	{
		return option == "true" ? true : false;
	}
};

template<>
struct OptParser::Getter<std::vector<std::string> > {
	std::vector<std::string> operator()(const std::string& option)
	{
		return split(option);
	}
};
}
#endif /* OPTPARSER_HPP_ */
