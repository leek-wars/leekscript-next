#include "Hover.hpp"
#include "../../environment/Environment.hpp"
#include "../lexical/Location.hpp"

namespace ls {

Hover::Hover(Environment& env, File& file) : type(env.void_), location(file) {}

Hover::Hover(const Type* type, Location location, std::string alias) : type(type), location(location), alias(alias) {}

}
