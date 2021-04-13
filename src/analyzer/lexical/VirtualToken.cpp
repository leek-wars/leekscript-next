#include "VirtualToken.hpp"
#include "../value/Function.hpp"
#include "../resolver/File.hpp"
#include "../../util/Util.hpp"

namespace ls {

VirtualToken::VirtualToken(Function& function) : function(function) {}

VirtualToken::VirtualToken(Function& function, unsigned int index) :
	function(function), index(index)
{}

VirtualToken& VirtualToken::operator = (const VirtualToken& vt) {
	// assert(&function == &vt.function); // On doit garder la même fonction
	index = vt.index;
	return *this;
}

Token* VirtualToken::operator -> () const {
	if (index == std::numeric_limits<unsigned int>::max()) {
		LOG("virtual token is null");
		assert(index != std::numeric_limits<unsigned int>::max());
	}
	if (function.token_offset + index >= function.file.tokens.size()) {
		std::cout << function.token_offset << " " << index << " " << function.file.tokens.size() << std::endl;
		assert(false);
	}
	auto t = &function.file.tokens.at(function.token_offset + index);
	// std::cout << "VT get " << function.token_offset
	// 	<< " " << index
	// 	<< " " << function.token_offset + index
	// 	<< " " << t->content << std::endl;
	return t;
}

VirtualToken::operator bool() const {
	return index != std::numeric_limits<unsigned int>::max();
}

}