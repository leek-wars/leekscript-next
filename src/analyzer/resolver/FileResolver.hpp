#ifndef RESOLVER_HPP
#define RESOLVER_HPP

#include "File.hpp"
#include <string>

namespace ls {

class FileResolver {
public:
    File* create(Environment& env, std::string path, std::string code) const;
	File* resolve(Environment& env, std::string path, FileContext context) const;
};

}

#endif