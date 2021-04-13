#include "Meta_map_type.hpp"
#include "../colors.h"
#if COMPILER
#include "../compiler/Compiler.hpp"
#endif

namespace ls {

void Meta_map_type::reset() const {
	((Meta_map_type*) this)->result = nullptr;
	array->reset();
	fun->reset();
}

bool Meta_map_type::operator == (const Type* type) const {
	return false;
}

int Meta_map_type::distance(const Type* type) const {
	return -1;
}

#if COMPILER
llvm::Type* Meta_map_type::llvm(Compiler& c) const {
	return llvm::Type::getVoidTy(c.getContext());
}
#endif

std::string Meta_map_type::class_name() const {
	return "";
}

Json Meta_map_type::json() const {
	return {
		{ "name", "map" },
		{ "element", array->json() }
	};
}

std::ostream& Meta_map_type::print(std::ostream& os) const {
	os << C_GREY << array << " map " << fun << END_COLOR;
	return os;
}

Type* Meta_map_type::clone() const {
	return new Meta_map_type(array, fun);
}

}