#include "VariableValue.hpp"
#include "math.h"
#include "../semantic/SemanticAnalyser.hpp"
#include "../value/Function.hpp"
#include "../instruction/VariableDeclaration.hpp"
#include "../semantic/Callable.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSSet.hpp"
#include "../../vm/value/LSClass.hpp"
#include "../../vm/Module.hpp"

namespace ls {

VariableValue::VariableValue(std::shared_ptr<Token> token) : token(token) {
	this->name = token->content;
	this->var = nullptr;
	constant = false;
}

bool VariableValue::isLeftValue() const {
	return scope != VarScope::INTERNAL; // Internal variables are not left-value
}

void VariableValue::print(std::ostream& os, int, bool debug, bool condensed) const {
	os << token->content;
	if (debug) {
		os << " ";
		if (has_version && var != nullptr && var->value != nullptr)
			os << var->value->version_type(version);
		else
			os << type;
	}
}

Location VariableValue::location() const {
	return token->location;
}

Callable* VariableValue::get_callable(SemanticAnalyser* analyser) const {
	if (name == "~") {
		auto callable = new Callable(name);
		auto T = Type::template_("T");
		auto R = Type::template_("R");
		auto type = Type::fun(R, {T, Type::fun(R, {T})});
		auto fun = [&](Compiler& c, std::vector<Compiler::value> args) {
			return c.insn_call(args[1].t.return_type(), {args[0]}, args[1]);
		};
		callable->add_version({ name, type, fun, {}, {R, T}, nullptr });
		return callable;
	}
	if (name == "Number") {
		auto callable = new Callable(name);
		callable->add_version({ "Number", Type::fun(Type::integer(), {}), [&](Compiler& c, std::vector<Compiler::value>) {
			return c.new_integer(0);
		}, {}, {}, nullptr });
		callable->add_version({ "Number", Type::fun(Type::real(), {Type::real()}), [&](Compiler& c, std::vector<Compiler::value> args) {
			return c.to_real(args[0]);
		}, {}, {}, nullptr });
		callable->add_version({ "Number", Type::fun(Type::tmp_mpz(), {Type::mpz()}), [&](Compiler& c, std::vector<Compiler::value> args) {
			return args[0];
		}, {}, {}, nullptr });
		return callable;
	}
	if (name == "Boolean") {
		auto callable = new Callable(name);
		auto type = Type::fun(Type::boolean(), {});
		callable->add_version({ "Boolean", type, [&](Compiler& c, std::vector<Compiler::value>) {
			return c.new_bool(false);	
		}, {}, {}, nullptr });
		return callable;
	}
	if (name == "String") {
		auto callable = new Callable(name);
		callable->add_version({ "String", Type::fun(Type::string(), {}), [&](Compiler& c, std::vector<Compiler::value>) {
			return c.new_pointer(new LSString(""), Type::string());
		}, {}, {}, nullptr });
		callable->add_version({ "String", Type::fun(Type::string(), {Type::string()}), [&](Compiler& c, std::vector<Compiler::value> args) {
			return args[0];
		}, {}, {}, nullptr });
		return callable;
	}
	if (name == "Array") {
		auto callable = new Callable(name);
		callable->add_version({ "Array", Type::fun(Type::array(Type::any()), {}), [&](Compiler& c, std::vector<Compiler::value>) {
			return c.new_array({}, {});
		}, {}, {}, nullptr });
		return callable;
	}
	if (name == "Object") {
		auto callable = new Callable(name);
		callable->add_version({ "Object", Type::fun(Type::array(Type::any()), {}), [&](Compiler& c, std::vector<Compiler::value>) {
			return c.new_object();
		}, {}, {}, nullptr });
		return callable;
	}
	if (name == "Set") {
		auto callable = new Callable(name);
		auto type = Type::fun(Type::set(Type::any()), {});
		callable->add_version({ "Set", type, [&](Compiler& c, std::vector<Compiler::value>) {
			return c.new_pointer(new LSSet<LSValue*>(), Type::set(Type::any()));
		}, {}, {}, nullptr });
		return callable;
	}
	if (type == Type::clazz()) {
		auto callable = new Callable(name);
		auto type = Type::fun(Type::any(), {Type::clazz()});
		callable->add_version({ name, type, [&](Compiler& c, std::vector<Compiler::value> args) {
			return c.new_object_class(args[0]);
		}, {}, {}, (Value*) this });
		return callable;
	}
	if (var) {
		if (var->callable) return var->callable;
		if (var->value) {
			auto callable = new Callable(var->name);
			auto c = var->value->get_callable(analyser);
			for (const auto& v : c->versions) {
				auto v2 = v;
				v2.value = this;
				v2.object = nullptr;
				callable->add_version(v2);
			}
			return callable;
		}
	} else {
		auto callable = new Callable(name);
		for (const auto& clazz : analyser->vm->internal_vars) {
			if (clazz.second->type().is_class()) {
				const auto& cl = (LSClass*) clazz.second->lsvalue;
				for (const auto& m : cl->methods) {
					if (m.first == name) {
						for (const auto& i : m.second) {
							auto t = Type::fun(i.type.return_type(), i.type.arguments(), this);
							if (i.native) {
								callable->add_version({ name, t, i.addr, i.mutators, i.templates, nullptr });
							} else {
								callable->add_version({ name, t, (Compiler::value (*)(Compiler&, std::vector<Compiler::value>)) i.addr, i.mutators, i.templates, nullptr });
							}
						}
					}
				}
			}
		}
		if (callable->versions.size()) {
			return callable;
		}
	}
	return nullptr;
}

void VariableValue::analyse(SemanticAnalyser* analyser) {

	var = analyser->get_var(token.get());

	if (var != nullptr) {
		auto function_object = dynamic_cast<Function*>(var->value);
		if (var->value && function_object) {
			// Analyse the real function (if the function is defined below its call for example)
			if (!function_object->analyzed) {
				function_object->analyse(analyser);
			}
		}
		type = var->type();
		var->initial_type = type;
		scope = var->scope;
		attr_types = var->attr_types;
		if (scope != VarScope::INTERNAL and var->function != analyser->current_function()) {
			capture_index = analyser->current_function()->capture(var);
			var->index = capture_index;
			scope = VarScope::CAPTURE;
		}
	} else {
		bool found = false;
		for (const auto& clazz : analyser->vm->internal_vars) {
			if (clazz.second->type().is_class()) {
				const auto& cl = (LSClass*) clazz.second->lsvalue;
				for (const auto& m : cl->methods) {
					if (m.first == name) {
						type = m.second.at(0).type;
						found = true;
						break;
					}
				}
			}
			if (found) break;
		}
		if (!found) {
			type = Type::any();
			analyser->add_error({SemanticError::Type::UNDEFINED_VARIABLE, token->location, token->location, {token->content}});
		}
	}
	type.temporary = false;

	// std::cout << "VV " << name << " : " << type << std::endl;
	// std::cout << "var scope : " << (int)var->scope << std::endl;
	//	for (auto t : attr_types)
	//	cout << t.first << " : " << t.second << endl;
}

bool VariableValue::will_take(SemanticAnalyser* analyser, const std::vector<Type>& args, int level) {
	// std::cout << "VV will take " << args << " type " << type << std::endl;
	if (var != nullptr and var->value != nullptr) {
		var->value->will_take(analyser, args, level);
		if (auto f = dynamic_cast<Function*>(var->value)) {
			if (f->versions.find(args) != f->versions.end()) {
				var->version = args;
				var->has_version = true;
			}
		}
		type = var->type();
	}
	set_version(args, level);
	return false;
}

void VariableValue::set_version(const std::vector<Type>& args, int level) {
	// std::cout << "VariableValue::set_version " << args << " " << level << std::endl;
	if (var != nullptr and var->value != nullptr) {
		var->value->set_version(args, level);
	}
	if (level == 1) {
		version = args;
		has_version = true;
	}
}

bool VariableValue::will_store(SemanticAnalyser* analyser, const Type& type) {
	// std::cout << "VV will_store " << type << std::endl;
	if (var != nullptr and var->value != nullptr) {
		var->value->will_store(analyser, type);
		this->type = var->type();
	}
	return false;
}

bool VariableValue::elements_will_store(SemanticAnalyser* analyser, const Type& type, int level) {
	if (var != nullptr and var->value != nullptr) {
		var->value->elements_will_store(analyser, type, level);
		this->type = var->value->type.not_temporary();
	}
	return false;
}

void VariableValue::change_value(SemanticAnalyser*, Value* value) {
	if (var != nullptr) {
		var->value = value;
		var->value->type.constant = false;
	}
}

Type VariableValue::version_type(std::vector<Type> version) const {
	// std::cout << "VariableValue::version_type " << version << std::endl;
	if (var != nullptr && var->value != nullptr) {
		// std::cout << "VariableValue " << this << " version_type() " << version << std::endl;
		return var->value->version_type(version);
	}
	if (var && var->callable) {
		for (const auto& v : var->callable->versions) {
			if (v.type.arguments() == version) {
				return v.type;
			}
		}
	}
	return type;
}

Compiler::value VariableValue::compile(Compiler& c) const {
	// std::cout << "Compile var " << name << " " << version << std::endl;
	// std::cout << "compile vv " << name << " : " << type << "(" << (int) scope << ")" << std::endl;
	Compiler::value v;
	if (scope == VarScope::CAPTURE) {
		v = c.insn_get_capture(capture_index, type);
	} else if (scope == VarScope::INTERNAL) {
		auto f = dynamic_cast<Function*>(var->value);
		if (f) {
			if (has_version) {
				return f->compile_version(c, version);
			} else {
				return f->compile_default_version(c);
			}
		}
		v = c.vm->internals.at(name);
	} else if (scope == VarScope::LOCAL) {
		assert(var != nullptr);
		auto f = dynamic_cast<Function*>(var->value);
		auto vv = dynamic_cast<VariableValue*>(var->value);
		if (has_version && (f or vv)) {
			return var->value->compile_version(c, version);
		}
		v = c.insn_load(c.get_var(name));
	} else if (scope == VarScope::PARAMETER) {
		v = c.insn_load(c.insn_get_argument(name));
	} else {
		assert(false);
	}
	c.assert_value_ok(v);
	return v;
}

Compiler::value VariableValue::compile_version(Compiler& c, std::vector<Type> version) const {
	auto f = dynamic_cast<Function*>(var->value);
	if (f) {
		return f->compile_version(c, version);
	}
	return c.insn_load(c.get_var(name));
}

Compiler::value VariableValue::compile_l(Compiler& c) const {
	Compiler::value v;
	// No internal values here
	if (scope == VarScope::LOCAL) {
		v = c.get_var(name);
	} else if (scope == VarScope::CAPTURE) {
		v = c.insn_get_capture_l(capture_index, type);
	} else { /* if (scope == VarScope::PARAMETER) */
		v = c.insn_get_argument(name);
	}
	return v;
}

Value* VariableValue::clone() const {
	auto vv = new VariableValue(token);
	return vv;
}

}
