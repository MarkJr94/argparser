argparser
=========

A simple argument parser, meant to parse any string. It is inspired by the Python ArgumentParser

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
 * It also prints a default help message, similar to the one Python's argpaarser prints
 */

Example use:

		std::string longstring =
		"Unlike traditional c-strings, which are mere sequences of characters in a memory array,"
		" C++ string objects belong to a class with many built-in features to operate with"
		" strings in a more intuitive way and with some additional useful features common " ""
		"to C++ containers.";

	ArgumentParser parser("ArgumentParser_test.cpp");
	parser.addarg<bool>("lol", false, 'l', true, "I am here to test you ");
	parser.addarg<int>("rofl", 25, 'r', true, "I am here to test you as well");
	parser.addarg<string>("mao", "TseTong", 'm', false, longstring + " This string tests my wrapping");
	parser.addarglist("hare",'h',true,"I test vector input");
	parser.parse("./go-again -l -r 334 -h 1 2 3 4 5");
	assert(parser.getarg<int>("rofl") == 334);
	assert(parser.getarg<bool>("lol") == true);
	assert(parser.getarg<string>("mao") == "TseTong");
	cout << "Expect: \"1 2 3 4 5\"\n";
	printStrVec(parser.getarglist("hare"));
	parser.help();

Output:

	Expect: "1 2 3 4 5"
	1
	2
	3
	4
	5
	Usage:	./ArgumentParser_test.cpp [--hare HARE] [--lol ] [--mao ] [--rofl ROFL] 
	Options:

	--hare (-h)	Required: Yes	Type: VECTOR
		I test vector input
	
	--lol (-l)	Required: Yes	Type: BOOL
		I am here to test you 
	
	--mao (-m)	Required: No	Type: BOOL
		Unlike traditional c-strings, which are mere sequences of characters 
			in a memory array, C++ string objects belong to a class with 
			many built-in features to operate with strings in a more intuitive 
			way and with some additional useful features common to C++ containers. 
			This string tests my wrapping
	
	--rofl (-r)	Required: Yes	Type: INT
		I am here to test you as well

