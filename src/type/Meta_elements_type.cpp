#include "Meta_elements_type.hpp"
#include "../colors.h"
#if COMPILER
#include "../compiler/Compiler.hpp"
#endif

namespace ls {

bool Meta_elements_type::operator == (const Type* type) const {
	return this->type == type;
}
int Meta_elements_type::distance(const Type* type) const {
	return 0;
}
#if COMPILER
llvm::Type* Meta_elements_type::llvm(Compiler& c) const {
	return llvm::Type::getVoidTy(c.getContext());
}
#endif
std::string Meta_elements_type::class_name() const {
	return "";
}
Json Meta_elements_type::json() const {
	return {
		{ "name", "elements" },
		{ "elements", type->json() }
	};
}
std::ostream& Meta_elements_type::print(std::ostream& os) const {
	os << type << ".elements";
	return os;
}
Type* Meta_elements_type::clone() const {
	return new Meta_elements_type(type);
}

}