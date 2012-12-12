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
 * It also prints a default help message, similar to the one Python's argparser prints
 */

Example use:

		std::string longstring =
		"Unlike traditional c-strings, which are mere sequences of characters in a memory array,"
		" C++ string objects belong to a class with many built-in features to operate with"
		" strings in a more intuitive way and with some additional useful features common " ""
		"to C++ containers.";

	using namespace std;

	OP::OptParser parser("OptParser_test.cpp");

	parser.add_opt("length", "20", 'l', true, " Length of arms in centimeters");
	parser.add_opt("height", "20", 'h', true, "Height of individual");
	parser.add_opt("name", "Kinsman", 'n', true, "Name of individual");
	parser.add_opt("mao", "false", 'm', false,
			"Is the individual Mao Zedong? " + longstring
					+ " This string tests my wrapping", OP::FLAG);
	parser.add_opt("numbers", "20 52 52 63 58", 'a', true,
			"Individual's favorite numbers", OP::LIST);

	string test_1 =
			"./go -l  -6001.45e-2 -h 45 -a 25 48 62  48 62 98 16 4 84 -n Joe -mmmm";
	parser.parse(test_1);

	auto always_zero = [](const std::string&) -> float {return 0;};
	auto str_to_bool =
			[](const string& s) -> bool {return s == "true" ? true : false;};

	assert(parser.get_as<int>("height") == 45);
	assert(parser.get_as<float>("length") - -6001.45e-2 < .00001);
	assert(parser.get_as<bool>("mao") == true);
	assert(parser.get_as<string>("name") == "Joe");
	assert(parser.get_as<bool>("mao", str_to_bool) == true);
	assert(parser.get_as<float>("length", always_zero) == 0);

	/* Testing series parsing */
	auto numbers = parser.get_as<vector<string>>("numbers");

	cout << "Expect: (25, 48, 62, 48, 62, 98, 16, 4, 84)\n";
	cout << "Result: ";
	OP::print_series(numbers.begin(),numbers.end(),", ");
	cout << endl;

	parser.help();
	cout << "Parser testing successful!\n";

Terminal Output:

	Expect: (25, 48, 62, 48, 62, 98, 16, 4, 84)
	Result: (25, 48, 62, 48, 62, 98, 16, 4, 84)
	Usage:	./OptParser_test.cpp [--height HEIGHT] [--length LENGTH] [--mao ] [--name NAME] [--numbers NUMBERS... ] 
	Options:
	
	--height (-h)	Required: Yes	Type: OPTION
		Height of individual
	
	--length (-l)	Required: Yes	Type: OPTION
		 Length of arms in centimeters
	
	--mao (-m)	Required: No	Type: FLAG
		Is the individual Mao Zedong? Unlike traditional c-strings, which 
			are mere sequences of characters in a memory array, C++ string 
			objects belong to a class with many built-in features to operate 
			with strings in a more intuitive way and with some additional 
			useful features common to C++ containers. This string tests my 
			wrapping
	
	--name (-n)	Required: Yes	Type: OPTION
		Name of individual
	
	--numbers (-a)	Required: Yes	Type: LIST
		Individual's favorite numbers
