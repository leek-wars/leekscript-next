#ifndef OPERATOR_HPP
#define OPERATOR_HPP

#include <memory>
#include "Token.hpp"
#include "VirtualToken.hpp"

namespace ls {

class Operator {
public:

	VirtualToken token;
	TokenType type;
	int priority;
	bool reversed;

	Operator(VirtualToken token);

	void print(std::ostream&);
};

}

#endif
