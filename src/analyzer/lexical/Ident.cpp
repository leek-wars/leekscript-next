#include "Ident.hpp"

namespace ls {

Ident::Ident(VirtualToken token) {
	this->token = token;
}

Ident::~Ident() {
	// Do not own the token
}

}
