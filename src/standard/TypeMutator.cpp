#include "TypeMutator.hpp"
#include "../analyzer/value/LeftValue.hpp"
#include "../analyzer/value/VariableValue.hpp"
#include "../analyzer/value/ArrayAccess.hpp"
#include "../analyzer/value/ObjectAccess.hpp"
#include "../analyzer/value/Number.hpp"
#include "../analyzer/semantic/Variable.hpp"
#include "../type/Type.hpp"
#include "Module.hpp"
#include "../analyzer/value/Phi.hpp"

namespace ls {

void change(Environment& env, Value* value, const Type* return_type) {

	// std::cout << "ChangeMutator:: change " << value << " " << value->type << " => " << return_type << std::endl;

	// Variable : change the type directly
	if (auto vv = dynamic_cast<VariableValue*>(value)) {

		if (return_type->is_fixed_array()) {
			return_type = return_type->pointer();
		}
		vv->previous_type = vv->type;
		vv->type = return_type->not_temporary()->not_constant();
		if (vv->var) {
			vv->var->type = vv->type;
		}
	}

	// Array access : change the array element at index to the return_type
	if (auto aa = dynamic_cast<ArrayAccess*>(value)) {
		if (not aa->key) return; // No key case

		auto array = aa->array.get();

		if (array->type->is_fixed_array_ptr() or array->type->is_fixed_array()) {
			// TODO only Number key here
			if (auto kn = dynamic_cast<const Number*>(aa->key.get())) {
				auto key_int = kn->int_value;
				auto fixed_array_type = array->type->is_fixed_array_ptr() ? array->type->pointed() : array->type;
				if (key_int >= 0 and key_int < fixed_array_type->size()) {
					auto new_type = fixed_array_type->change_element(key_int, return_type);
					// if (array->type->is_fixed_array_ptr()) {
					// 	new_type = new_type->pointer();
					// }
					return change(env, array, new_type);
				}
			}
		} else if (array->type->is_array()) {
			auto element = array->type->element()->operator | (return_type->not_temporary());
			if (env.legacy) {
				// Legacy mode: the array transforms from array<X> to map<integer, X>
				auto key_type = aa->key->type;
				return change(env, array, Type::map(key_type->not_temporary(), element));
			} else {
				// V2 mode: the array juste change is element type
				return change(env, array, Type::array(element));
			}
		} else if (array->type->is_map()) {
			// std::cout << vv->type << " " << aa->key->type << " " << return_type << std::endl;
			auto key = array->type->key()->operator | (aa->key->type->not_temporary());
			auto element = array->type->element()->operator | (return_type->not_temporary());
			return change(env, array, Type::map(key, element));
		}
	}
}

void ChangeValueMutator::apply(SemanticAnalyzer* analyzer, std::vector<Value*> values, const Type* return_type) const {

	auto& env = analyzer->env;

	// std::cout << "Change value mutator " << values[0]->type << " => " << return_type << std::endl;

	change(env, values[0], return_type);
}

#if COMPILER

int ChangeValueMutator::transform(Compiler& c, Value* value, bool convert) const {

	// std::cout << "Mutator transform " << convert << std::endl;

	if (auto vv = dynamic_cast<VariableValue*>(value)) {

		c.comment("Convert variable " + vv->var->name + " to " + vv->var->type->getName());

		// std::cout << "ChangeValueMutator " << vv->var << " ||| " << vv->previous_type << " == " << vv->type << " in_loop " << vv->var->in_loop << std::endl;

		if (vv->previous_type == vv->type) {
			vv->var->entry = vv->var->parent->entry;
			return 0;
		} else if (not convert or (vv->previous_type && vv->previous_type->is_never())) {
			vv->var->create_entry(c);
			c.insn_delete_variable(vv->var->parent->entry);
			return Module::EMPTY_VARIABLE;
		} else {
			auto previous_value = vv->var->parent->entry;
			if (not previous_value.t->is_fixed_array_ptr()) {
				previous_value = c.insn_load(previous_value);
			}
			// std::cout << "ChangeValue::compile Convert value " << previous_value.t << " " << vv->var->type << std::endl;

			auto converted = c.insn_convert(previous_value, vv->var->type, false, not this->convert);
			if (previous_value.v == converted.v) {
				// std::cout << "ChangeValue::compile keep parent" << std::endl;
				vv->var->entry = vv->var->parent->entry;
			} else {
				// std::cout << "ChangeValue::compile new entry" << std::endl;
				c.comment("Convert variable " + vv->var->name + " to " + vv->var->type->getName() + " create entry");
				vv->var->create_entry(c);
				vv->var->store_value(c, c.insn_move_inc(converted));
				c.insn_delete_variable(vv->var->parent->entry);
			}
			return 0;
		}
	} else if (auto aa = dynamic_cast<ArrayAccess*>(value)) {

		// std::cout << "ChangeValue::compile ArrayAccess" << std::endl;

		return transform(c, aa->array.get(), true); // Convert: true

	} else if (auto oa = dynamic_cast<ObjectAccess*>(value)) {

		// std::cout << "ChangeValue::compile ArrayAccess" << std::endl;

		// return transform(c, oa->object.get(), true); // Convert: true
		return 0;
	}
	assert(false && "Not supported mutation");
}

int ChangeValueMutator::compile(Compiler& c, CallableVersion* callable, std::vector<Value*> values) const {
	// std::cout << "ChangeValueMutator compile" << std::endl;

	return transform(c, values[0], convert);
}

#endif

void WillTakeMutator::apply(SemanticAnalyzer* analyzer, std::vector<Value*> values, const Type* return_type) const {
	auto& env = analyzer->env;
	values[1]->will_take(analyzer, { env.any }, 1);
	// std::vector<Type> args;
	// for (const auto& extractor : types) {
	// 	args.push_back(extractor->extract(analyzer, values));
	// }
}

const Type* TypeExtractor::extract(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	return analyzer->env.void_;
}

const Type* ElementExtractor::extract(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	return extractor->extract(analyzer, values)->element();
}

const Type* ArgumentExtractor::extract(SemanticAnalyzer* analyzer, std::vector<Value*> values) const {
	return values.at(index)->type;
}

}
