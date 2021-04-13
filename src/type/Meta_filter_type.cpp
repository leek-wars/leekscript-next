#include "Meta_filter_type.hpp"
#include "../colors.h"
#if COMPILER
#include "../compiler/Compiler.hpp"
#endif

namespace ls {

void Meta_filter_type::reset() const {
	array->reset();
	fun->reset();
}

bool Meta_filter_type::operator == (const Type* type) const {
	return false;
}

int Meta_filter_type::distance(const Type* type) const {
	return -1;
}

#if COMPILER
llvm::Type* Meta_filter_type::llvm(Compiler& c) const {
	return llvm::Type::getVoidTy(c.getContext());
}
#endif

std::string Meta_filter_type::class_name() const {
	return "";
}

Json Meta_filter_type::json() const {
	return {
		{ "name", "filter" },
		{ "element", array->json() }
	};
}

std::ostream& Meta_filter_type::print(std::ostream& os) const {
	os << C_GREY << array << " filter " << fun << END_COLOR;
	return os;
}

Type* Meta_filter_type::clone() const {
	return new Meta_filter_type(array, fun);
}

}