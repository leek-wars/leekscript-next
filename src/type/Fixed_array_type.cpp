#include "Fixed_array_type.hpp"
#include "Array_type.hpp"
#include "../colors.h"
#include "Type.hpp"
#include <iostream>
#include "Struct_type.hpp"
#include "Function_type.hpp"
#include "Any_type.hpp"
#include "../environment/Environment.hpp"
#if COMPILER
#include "../compiler/Compiler.hpp"
#endif

namespace ls {

Fixed_array_type::Fixed_array_type(Environment& env, std::vector<const Type*> elements)
: Struct_type(env, "array[" + Type::vector_to_string(elements) + "]", elements), _elements(elements), _element(Type::compound(env, elements))
{
	// std::cout << "Fixed_array_type() " << elements << std::endl;
}

const Type* Fixed_array_type::key() const {
	return env.integer;
}

const Type* Fixed_array_type::element() const {
	return _element == env.void_ ? env.any : _element;
}

const Type* Fixed_array_type::element(size_t index) const {
	assert(index >= 0 && index < _elements.size());
	// std::cout << "index = " << index << " " << _elements.size() << std::endl;
	return _elements[index];
}

const Type* Fixed_array_type::range(size_t index, size_t index2) const {
	assert(index >= 0 and index < _elements.size());
	assert(index2 >= 0 and index2 < _elements.size() and index2 >= index);
	std::vector<const Type*> range;
	for (size_t i = index; i <= index2; ++i) {
		range.push_back(_elements[i]);
	}
	return Type::fixed_array(env, range);
}

const Type* Fixed_array_type::change_element(size_t index, const Type* type) const {
	assert(index >= 0 and index < _elements.size());
	std::vector<const Type*> new_types = _elements;
	new_types[index] = type;
	return Type::fixed_array(env, new_types);
}

const std::vector<const Type*>& Fixed_array_type::elements() const {
	return _elements;
}

const Type* Fixed_array_type::op_map(SemanticAnalyzer& analyzer, const Type* fun) const {
	// const auto f = dynamic_cast<const Function_type*>(fun);
	// assert(f);
	std::vector<const Type*> elements;
	for (const auto& element : _elements) {
		// std::cout << "op_map will take " << element << std::endl;
		auto type = ((Value*) fun->function())->will_take(&analyzer, { element }, 1);
		elements.push_back(type->return_type());
	}
	return Type::fixed_array(env, elements);
}

const Type* Fixed_array_type::op_map(const Type* fun) const {
	// const auto f = dynamic_cast<const Function_type*>(fun);
	// assert(f);
	// std::cout << fun << " " << (void*) fun->function() << std::endl;
	std::vector<const Type*> elements;
	for (const auto& element : _elements) {
		elements.push_back(fun->function()->version_type({ element })->return_type());
		// std::cout << "op_map " << element << " " << fun->function()->version_type({ element })->return_type() << std::endl;
	}
	return Type::fixed_array(env, elements);
}

const Type* Fixed_array_type::op_filter(SemanticAnalyzer& analyzer, const Type* fun) const {
	for (const auto& element : _elements) {
		((Value*) fun->function())->will_take(&analyzer, { element }, 1);
	}
	return Type::tmp_array(_element);
}

size_t Fixed_array_type::size() const {
	return _elements.size();
}

size_t Fixed_array_type::bits() const {
	size_t bits = 0;
	for (const auto& element : _elements) {
		bits += element->bits();
	}
	return bits;
}

bool Fixed_array_type::operator == (const Type* type) const {
	if (auto array = dynamic_cast<const Array_type*>(type)) {
		return _element == array->element();
	}
	return false;
}

int Fixed_array_type::distance(const Type* type) const {
	if (not temporary and type->temporary) return -1;
	if (dynamic_cast<const Any_type*>(type->folded)) { return 1000; }

	if (auto array = dynamic_cast<const Fixed_array_type*>(type->folded)) {
		// std::cout << size() << " " << array->size() << std::endl;
		// if (array->size() == 1 and array->element(0) == env.void_) return 0;
		// if (_element == env.never) return 0;
		// if (array->element() == env.never) return 999;
		// if (array->element()->is_void()) {
		// 	return 999;
		// }
		// return _element->fold() == array->element()->fold() ? 0 : -1;
		if (size() != array->size()) {
			return -1;
		}
		int distance = 0;
		for (size_t index = 0; index < _elements.size(); ++index) {
			auto d = _elements[index]->distance(array->element(index));
			if (d < 0) return -1;
			distance += d;
		}
		return distance;
	}

	if (auto array = dynamic_cast<const Array_type*>(type->folded)) {
		if (size() == 0) {
			return 0; // An empty array can be converted into any array
		}
		return _element->distance(array->element());
	}
	return -1;
}

const Type* Fixed_array_type::iterator() const {
	const auto merged = _element->fold();
	if (merged->is_bool()) return env.i8->pointer();
	if (merged->is_integer()) return env.integer->pointer();
	if (merged->is_long()) return env.long_->pointer();
	if (merged->is_real()) return env.real->pointer();
	return env.any->pointer();
}

std::string Fixed_array_type::class_name() const {
	return "Array";
}

const std::string Fixed_array_type::getName() const {
	std::string name = "array[";
	for (size_t i = 0; i < _elements.size(); ++i) {
		if (i > 0) { name += ", "; }
		name += _elements[i]->getName();
	}
	name += "]";
	return name;
}

Json Fixed_array_type::json() const {
	Json elements;
	for (const auto& arg : _elements) {
		elements.push_back(arg->json());
	}
	return {
		{ "name", "array" },
		{ "elements", elements }
	};
}

std::ostream& Fixed_array_type::print(std::ostream& os) const {
	os << BLUE_BOLD << "array[" << END_COLOR;
	for (size_t i = 0; i < _elements.size(); ++i) {
		if (i > 0) { os << BLUE_BOLD << ", " << END_COLOR; }
		os << _elements[i];
	}
	os << BLUE_BOLD << "]" << END_COLOR;
	return os;
}

Type* Fixed_array_type::clone() const {
	// std::cout << "clone fixed array" << std::endl;
	return new Fixed_array_type { _element->env, _elements };
}

}