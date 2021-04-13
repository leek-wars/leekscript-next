#include "Meta_push_type.hpp"
#include "../colors.h"
#if COMPILER
#include "../compiler/Compiler.hpp"
#endif

namespace ls {

void Meta_push_type::reset() const {
	t1->reset();
	t2->reset();
}

bool Meta_push_type::operator == (const Type* type) const {
	return false;
}
int Meta_push_type::distance(const Type* type) const {
	return -1;
}
#if COMPILER
llvm::Type* Meta_push_type::llvm(Compiler& c) const {
	return llvm::Type::getVoidTy(c.getContext());
}
#endif
std::string Meta_push_type::class_name() const {
	return "";
}
Json Meta_push_type::json() const {
	return {
		t1->json(),
		t2->json()
	};
}
std::ostream& Meta_push_type::print(std::ostream& os) const {
	os << C_GREY << t1 << " push " << t2 << END_COLOR;
	return os;
}
Type* Meta_push_type::clone() const {
	return new Meta_push_type(t1, t2);
}

}