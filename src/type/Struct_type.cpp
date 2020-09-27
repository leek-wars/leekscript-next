#include "Struct_type.hpp"
#include "../colors.h"
#include <iostream>
#include "Type.hpp"
#include "Any_type.hpp"
#include "../environment/Environment.hpp"
#if COMPILER
#include "../compiler/Compiler.hpp"
#endif

namespace ls {

Struct_type::Struct_type(Environment& env, const std::string name, std::initializer_list<const Type*> types) : Type(env), _name(name), _types(types) {}
Struct_type::Struct_type(Environment& env, const std::string name, std::vector<const Type*> types) : Type(env), _name(name), _types(types) {}

const Type* Struct_type::member(int p) const {
	return _types.at(p);
}

bool Struct_type::operator == (const Type* type) const {
	if (auto s = dynamic_cast<const Struct_type*>(type)) {
		return s->_types == _types;
	}
	return false;
}

int Struct_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 1; }
	if (auto s = dynamic_cast<const Struct_type*>(type->folded)) {
		if (s->_types == _types) {
			return 0;
		}
	}
	return -1;
}

#if COMPILER
llvm::Type* Struct_type::llvm(Compiler& c) const {
	auto i = c.env.llvm_structure_types.find(_name);
	if (i != c.env.llvm_structure_types.end()) return i->second;

	std::vector<llvm::Type*> llvm_types;
	for (const auto& type : _types) {
		llvm_types.push_back(type->llvm(c));
	}
	if (not _types.size()) {
		// It's not possible to create empty struct, so we put an i8 inside
		llvm_types.push_back(c.env.i8->llvm(c));
	}
	auto llvm_type = llvm::StructType::create(llvm_types, _name);
	return c.env.llvm_structure_types.emplace(_name, llvm_type).first->second;
}
#endif

std::string Struct_type::class_name() const {
	return "Struct";
}

Json Struct_type::json() const {
	return {
		{ "name", _name }
	};
}

std::ostream& Struct_type::print(std::ostream& os) const {
	os << BLUE_BOLD << _name << END_COLOR << " { ";
	for (size_t i = 0; i < _types.size(); ++i) {
		if (i > 0) os << ", ";
		os << _types[i];
	}
	os << " }";
	return os;
}

Type* Struct_type::clone() const {
	return new Struct_type { env, _name, _types };
}

}