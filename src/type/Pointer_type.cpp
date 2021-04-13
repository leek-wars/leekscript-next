#include "Pointer_type.hpp"
#include "Any_type.hpp"
#include "Type.hpp"
#include "../colors.h"
#include "../environment/Environment.hpp"

namespace ls {

Pointer_type::Pointer_type(const Type* type, bool native) : Type(type->env, native), _type(type) {}

const Type* Pointer_type::pointed() const {
	return _type;
}

const Value* Pointer_type::function() const {
	return _type->function();
}

const Type* Pointer_type::return_type() const {
	return _type->return_type();
}

const Type* Pointer_type::argument(size_t i) const {
	return _type->argument(i);
}

const std::vector<const Type*>& Pointer_type::arguments() const {
	return _type->arguments();
}

const Type* Pointer_type::element() const {
	if (_type->is_fixed_array()) {
		return _type->element();
	}
	return env.any;
}

const Type* Pointer_type::element(size_t index) const {
	if (_type->is_fixed_array()) {
		return _type->element(index);
	}
	return env.any;
}

const Type* Pointer_type::op_map(SemanticAnalyzer& analyzer, const Type* fun) const {
	if (_type->is_fixed_array()) {
		return _type->op_map(analyzer, fun);
	}
	return env.void_;
}

const Type* Pointer_type::op_map(const Type* fun) const {
	if (_type->is_fixed_array()) {
		return _type->op_map(fun);
	}
	return env.void_;
}

const Type* Pointer_type::op_filter(SemanticAnalyzer& analyzer, const Type* fun) const {
	return _type->op_filter(analyzer, fun);
}

size_t Pointer_type::size() const {
	if (_type->is_fixed_array()) {
		return _type->size();
	}
	return Type::size();
}

bool Pointer_type::container() const {
	if (_type->is_fixed_array()) {
		return true;
	}
	return false;
}

bool Pointer_type::iterable() const {
	if (_type->is_fixed_array()) {
		return true;
	}
	return false;
}

bool Pointer_type::callable() const {
	if (_type->is_function()) {
		return true;
	}
	return false;
}

bool Pointer_type::operator == (const Type* type) const {
	if (auto p = dynamic_cast<const Pointer_type*>(type)) {
		return p->_type == _type;
	}
	return false;
}

int Pointer_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 10; }

	auto p = dynamic_cast<const Pointer_type*>(type->folded);

	if (_type->is_fixed_array()) {
		if (type->is_fixed_array_ptr()) {
			return _type->distance(type->pointed());
		}
		if (type->is_fixed_array()) {
			return _type->distance(type);
		}
		return _type->distance(type);
	}

	// std::cout << "pointer distance to " << type << std::endl;
	if (_type->is_function()) {
		if (p and p->pointed()->is_function()) return 0;
		return _type->distance(type->folded);
	}
	if (p) {
		auto d = _type->distance(p->_type);
		// std::cout << "distance " << _type << " " << type << " " << d << std::endl;
		if (d < 0) return d;
		if (temporary == type->temporary) return d;
		return d + 1;
	}
	// mpz* to number types is possible (but not to mpz)
	if (_type->is_mpz() and not type->is_mpz()) {
		auto d = _type->distance(type->folded);
		if (d < 0) return d;
		if (temporary == type->temporary) return d;
		return d + 1;
	}
	return -1;
}

std::string Pointer_type::class_name() const {
	return _type->class_name();
}

Json Pointer_type::json() const {
	return _type->json();
}

#if COMPILER
llvm::Type* Pointer_type::llvm(Compiler& c) const {
	if (_llvm_type == nullptr) {
		((Pointer_type*) this)->_llvm_type = _type->llvm(c)->getPointerTo();
	}
	return _llvm_type;
}
#endif

std::ostream& Pointer_type::print(std::ostream& os) const {
	os << _type << BLUE_BOLD << "*" << END_COLOR;
	return os;
}

Type* Pointer_type::clone() const {
	return new Pointer_type { _type };
}

}