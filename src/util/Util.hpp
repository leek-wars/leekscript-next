#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <string>
#include <vector>
#include <queue>
#include "../colors.h"

namespace ls {

#define JOIN1(glue, a) a
#define JOIN2(glue, a, b) a glue b
#define JOIN3(glue, a, b, c) a glue b glue c
#define JOIN4(glue, a, b, c, d) a glue b glue c glue d
#define JOIN5(glue, a, b, c, d, e) a glue b glue c glue d glue e
#define JOIN6(glue, a, b, c, d, e, f) a glue b glue c glue d glue e glue f
#define JOIN7(glue, a, b, c, d, e, f, g) a glue b glue c glue d glue e glue f glue g
#define JOIN8(glue, a, b, c, d, e, f, g, h) a glue b glue c glue d glue e glue f glue g glue h
#define GET_OVERRIDE(_1, _2, _3, _4, _5, _6, _7, _8, NAME, ...) NAME
#define JOIN(glue, ...) GET_OVERRIDE(__VA_ARGS__, JOIN8, JOIN7, JOIN6, JOIN5, JOIN4, JOIN3, JOIN2, JOIN1)(glue, __VA_ARGS__)

#define LOG(...) std::cout << JOIN(<< " " <<, __VA_ARGS__) << "\t\t\t\t\t\t\t\t" << C_GREY << __FILE__ << ":" << __LINE__ << END_COLOR << std::endl;

class Util {
public:

	static std::string read_file(std::string file);
	static std::vector<std::string> read_file_lines(std::string file);
	static bool is_file_name(std::string data);
	static std::string replace_all(std::string& haystack, const std::string& needle, const std::string& replacement);
	static std::string file_short_name(std::string path);

	static std::string toupper(const std::string& string);
	static std::string tolower(const std::string& string);
};

}

#endif
