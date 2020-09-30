#ifndef IDENT_HPP_
#define IDENT_HPP_

#include "VirtualToken.hpp"

namespace ls {

class Ident {
public:

	VirtualToken token;

	Ident(VirtualToken token);
	virtual ~Ident();
};

}

#endif
