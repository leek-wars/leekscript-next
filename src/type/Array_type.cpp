#include "Array_type.hpp"
#include "Fixed_array_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include <iostream>
#include "Struct_type.hpp"
#include "Any_type.hpp"
#include "../environment/Environment.hpp"
#if COMPILER
#include "../compiler/Compiler.hpp"
#endif

namespace ls {

Array_type::Array_type(const Type* element) : Pointer_type(Type::structure(element->env, "array<" + element->getName() + ">", {
	element->env.integer, // ?
	element->env.integer, // ?
	element->env.integer, // ?
	element->env.integer, // refs
	element->env.boolean, // native
	element->pointer(), // vector.begin
	element->pointer(), // vector.end
	element->pointer() // vector.data
})), _element(element) {
	// Void and never types are forbidden for array element
	assert(element != element->env.void_ and "Array element cannot be void");
	assert(element != element->env.never and "Array element cannot be never");
}

const Type* Array_type::key() const {
	return env.integer;
}

const Type* Array_type::element() const {
	return _element;
}

const Type* Array_type::op_map(SemanticAnalyzer& analyzer, const Type* fun) const {

	if (fun->function()) {
		auto type = ((Value*) fun->function())->will_take(&analyzer, { _element }, 1);
		return Type::array(type->return_type());
	} else {
		return Type::array(fun->return_type());
	}
}

const Type* Array_type::op_map(const Type* fun) const {
	if (fun->function()) {
		return Type::array(fun->function()->version_type({ _element })->return_type());
	} else {
		return Type::array(fun->return_type());
	}
}

bool Array_type::operator == (const Type* type) const {
	if (auto array = dynamic_cast<const Array_type*>(type)) {
		return _element == array->_element;
	}
	return false;
}

int Array_type::distance(const Type* type) const {

	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 100; }

	if (auto fixed = dynamic_cast<const Fixed_array_type*>(type->folded)) {
		// It could be possible to convert an array to a fixed array, but you have to have the same element count and types.
		int distance = 2000; // This big score is because we don't know if we have the same element count
		for (const auto& e : fixed->elements()) {
			auto d = _element->distance(e); // Add the distance between the array element and each element of the fixed array
			if (d < 0) return -1;
			distance += d;
		}
		return distance;
	}

	if (auto array = dynamic_cast<const Array_type*>(type->folded)) {
		// if (_element == env.never) return 0;
		// if (array->_element == env.never) return 999;
		// if (array->_element->is_void()) {
		// 	return 999;
		// }
		return _element->distance(array->_element);
		// return _element->fold() == array->_element->fold() ? 0 : -1;
	}
	return -1;
}

const Type* Array_type::iterator() const {
	const auto merged = _element->fold();
	if (merged->is_bool()) return env.i8->pointer();
	if (merged->is_integer()) return env.integer->pointer();
	if (merged->is_long()) return env.long_->pointer();
	if (merged->is_real()) return env.real->pointer();
	return env.any->pointer();
}

std::string Array_type::class_name() const {
	return "Array";
}

const std::string Array_type::getName() const {
	return "array<" + _element->getName() + ">";
}

Json Array_type::json() const {
	return {
		{ "name", "array" },
		{ "element", _element->json() }
	};
}

std::ostream& Array_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "array" << END_COLOR;
	if (_element != env.void_) {
		os << "<" << _element << ">";
	}
	return os;
}

Type* Array_type::clone() const {
	return new Array_type { _element };
}

}