#include "CallableVersion.hpp"
#include "../../type/Template_type.hpp"
#include "../../type/Meta_or_type.hpp"
#include "../../type/Meta_add_type.hpp"
#include "../../type/Meta_push_type.hpp"
#include "../../type/Meta_mul_type.hpp"
#include "../../type/Meta_baseof_type.hpp"
#include "../../type/Meta_temporary_type.hpp"
#include "../../type/Meta_element_type.hpp"
#include "../../type/Meta_elements_type.hpp"
#include "../../type/Meta_not_temporary_type.hpp"
#include "../../type/Meta_not_void_type.hpp"
#include "../../type/Meta_map_type.hpp"
#include "../../type/Meta_filter_type.hpp"
#include "../../standard/Module.hpp"
#include "../../colors.h"
#include "../value/ObjectAccess.hpp"
#include "FunctionVersion.hpp"
#include "../value/Function.hpp"
#include "../../type/Function_type.hpp"
#include "../value/VariableValue.hpp"
#include "Variable.hpp"

namespace ls {

CallableVersionTemplate::CallableVersionTemplate(std::string name, const Type* type, std::vector<const TypeMutator*> mutators, std::vector<const Type*> templates, bool object, bool v1_addr, bool v2_addr, int flags)
	: name(name), type(type), object(object), symbol(true), mutators(mutators), templates(templates), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags)
	{
		assert(name.find(".") != std::string::npos);
	}
#if COMPILER
CallableVersionTemplate::CallableVersionTemplate(std::string name, const Type* type, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, std::vector<const TypeMutator*> mutators, std::vector<const Type*> templates, bool object, bool v1_addr, bool v2_addr, int flags)
	: name(name), type(type), object(object), mutators(mutators), templates(templates), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags), func(func)
	{
		// std::cout << "callable version compiler fun" << std::endl;
	}
#endif

CallableVersionTemplate::CallableVersionTemplate(std::string name, const Type* type, void* addr, std::vector<const TypeMutator*> mutators, std::vector<const Type*> templates, bool object, bool v1_addr, bool v2_addr, int flags)
	: name(name), type(type), object(object), mutators(mutators), templates(templates), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags), addr(addr)
	{}

CallableVersionTemplate::CallableVersionTemplate(std::string name, const Type* type, bool unknown, std::vector<const TypeMutator*> mutators, std::vector<const Type*> templates, bool object, bool v1_addr, bool v2_addr, int flags)
	: name(name), type(type), object(object), mutators(mutators), templates(templates), unknown(unknown), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags)
	{}

CallableVersionTemplate::CallableVersionTemplate(std::string name, const Type* type, FunctionVersion* f, std::vector<const TypeMutator*> mutators, std::vector<const Type*> templates, bool object, bool v1_addr, bool v2_addr, int flags)
	: name(name), type(type), object(object), user_fun(f), mutators(mutators), templates(templates), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags)
	{}

CallableVersionTemplate::CallableVersionTemplate(const Type* return_type, std::initializer_list<const Type*> arguments, void* addr, int flags, std::vector<const TypeMutator*> mutators, bool v1_addr)
	: type(Type::fun(return_type, arguments)), symbol(true), mutators(mutators), flags(flags), addr(addr), v1_addr(v1_addr)
	{}
#if COMPILER
CallableVersionTemplate::CallableVersionTemplate(const Type* return_type, std::initializer_list<const Type*> arguments, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, int flags, std::vector<const TypeMutator*> mutators, bool v1_addr)
	: type(Type::fun(return_type, arguments)), mutators(mutators), flags(flags), func(func), v1_addr(v1_addr) {}
#endif

CallableVersionTemplate::CallableVersionTemplate(const Type* v1_type, const Type* v2_type, const Type* return_type, void* addr, int flags, std::vector<const TypeMutator*> mutators, bool v1_addr, bool v2_addr)
	: type(Type::fun(return_type, {v1_type, v2_type})), symbol(true), mutators(mutators), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags), addr(addr)
	{}
#if COMPILER
CallableVersionTemplate::CallableVersionTemplate(const Type* v1_type, const Type* v2_type, const Type* return_type, std::function<Compiler::value(Compiler&, std::vector<Compiler::value>, int)> func, int flags, std::vector<const TypeMutator*> mutators, bool v1_addr, bool v2_addr)
	: type(Type::fun(return_type, {v1_type, v2_type})), mutators(mutators), v1_addr(v1_addr), v2_addr(v2_addr), flags(flags), func(func) {}
#endif

const Type* build(SemanticAnalyzer& analyzer, const Type* type) {
	if (type->is_template()) return ((Template_type*) type)->_implementation;
	if (type->is_fixed_array() or type->is_fixed_array_ptr()) {
		auto t = type->is_fixed_array_ptr() ? type->pointed() : type;
		std::vector<const Type*> new_elements;
		for (const auto& element : t->elements()) {
			new_elements.push_back(build(analyzer, element));
		}
		return type->temporary ? Type::tmp_fixed_array(type->env, new_elements) : Type::fixed_array(type->env, new_elements);
	}
	if (type->is_array()) {
		auto element = build(analyzer, type->element());
		if (element->is_void()) {
			return type;
		}
		return type->temporary ? Type::tmp_array(element) : Type::array(element);
	}
	if (type->is_set()) return Type::set(build(analyzer, type->element()));
	if (type->is_map()) return Type::map(build(analyzer, type->key()), build(analyzer, type->element()));
	if (type->is_function()) {
		std::vector<const Type*> args;
		for (const auto& t : type->arguments()) {
			auto a = build(analyzer, t);
			args.push_back(a);
			// std::cout << "add arg " << t << " => " << a << std::endl;
		}
		auto r = Type::fun(build(analyzer, type->return_type()), args, type->function());
		// std::cout << "build fun " << r << std::endl;
		return r;
	}
	if (type->is_function_object()) {
		std::vector<const Type*> args;
		for (const auto& t : type->arguments()) {
			args.push_back(build(analyzer, t));
		}
		return Type::fun_object(build(analyzer, type->return_type()), args, type->function());
	}
	if (auto tmp = dynamic_cast<const Meta_temporary_type*>(type)) {
		return build(analyzer, tmp->type)->add_temporary();
	}
	if (auto not_tmp = dynamic_cast<const Meta_not_temporary_type*>(type)) {
		return build(analyzer, not_tmp->type)->not_temporary();
	}
	if (auto not_void = dynamic_cast<const Meta_not_void_type*>(type)) {
		auto t = build(analyzer, not_void->type);
		if (t->is_void()) {
			return t->env.null;
		} else {
			return t;
		}
	}
	if (auto mul = dynamic_cast<const Meta_or_type*>(type)) {
		return build(analyzer, mul->t1)->operator | (build(analyzer, mul->t2));
	}
	if (auto element = dynamic_cast<const Meta_element_type*>(type)) {
		return build(analyzer, element->type)->element();
	}
	if (auto add = dynamic_cast<const Meta_add_type*>(type)) {
		return build(analyzer, add->t1)->add(build(analyzer, add->t2));
	}
	if (auto push = dynamic_cast<const Meta_push_type*>(type)) {
		auto r = build(analyzer, push->t1)->push(build(analyzer, push->t2), analyzer.in_loop(1));
		// std::cout << push->t1 << " push " << push->t2 << " = " << r << std::endl;
		return r;
	}
	if (auto mul = dynamic_cast<const Meta_mul_type*>(type)) {
		return build(analyzer, mul->t1)->operator * (build(analyzer, mul->t2));
	}
	if (auto baseof = dynamic_cast<const Meta_baseof_type*>(type)) {
		// if (baseof->result) return baseof->result;
		auto t = build(analyzer, baseof->type);
		auto d = t->distance(baseof->base);
		// std::cout << "d = " << d << std::endl;
		if (d == -1) {
			return ((Meta_baseof_type*) baseof)->result = baseof->base;
		} else if (d <= 100000) {
			return ((Meta_baseof_type*) baseof)->result = t;
		} else {
			return ((Meta_baseof_type*) baseof)->result = [&]() {
				if (t == t->env.boolean) return t->env.integer;
				return t->env.real;
			}();
		}
	}
	if (auto map = dynamic_cast<const Meta_map_type*>(type)) {
		auto r = build(analyzer, map->array)->op_map(analyzer, build(analyzer, map->fun));
		// std::cout << "meta_map " << map->array << " : " << map->fun << " => " << r << std::endl;
		return r;
	}
	if (auto map = dynamic_cast<const Meta_filter_type*>(type)) {
		auto r = build(analyzer, map->array)->op_filter(analyzer, build(analyzer, map->fun));
		// std::cout << "meta_map " << map->array << " : " << map->fun << " => " << r << std::endl;
		return r;
	}
	return type;
}

std::pair<int, CallableVersion> CallableVersionTemplate::get_score(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments, Callable* callable, size_t index) const {
	// std::cout << "CallableVersion " << name << "::get_score(" << arguments << ") " << type << std::endl;

	// Template resolution
	CallableVersion new_version { analyzer->env, callable, index, type };
	if (templates.size()) {
		resolve_templates(analyzer, arguments);
		auto version_type = build(*analyzer, type);
		// Reset template implementations
		for (const auto& t : templates) {
			// std::cout << "template " << t << std::endl;
			t->reset();
		}
		// std::cout << "Resolved version = " << version_type << std::endl;
		new_version.type = version_type;
	}

	auto f = type->function();
	bool valid = true;
	for (size_t i = 0; i < new_version.type->arguments().size(); ++i) {
		if (i < arguments.size()) {
			const auto& a = arguments.at(i);
			const auto implem_arg = new_version.type->argument(i);
			// std::cout << "argument " << i << " " << implem_arg << std::endl;
			if (a->function() and (implem_arg->is_function() or implem_arg->is_function_pointer() or implem_arg->is_function_object())) {
				bool is_placeholder = false;
				for (const auto& arg : implem_arg->arguments()) {
					if (arg->placeholder) is_placeholder = true;
				}
				if (not is_placeholder) {
					// std::cout << "implem will_take " << implem_arg->arguments() << std::endl;
					arguments.at(i) = ((Value*) a->function())->will_take(analyzer, implem_arg->arguments(), 1);
				}
			}
		} else if (f and ((Function*) f)->defaultValues.at(i)) {
			// Default argument
		} else {
			valid = false;
		}
	}
	if ((!valid or arguments.size() > new_version.type->arguments().size()) and not unknown) {
		return { std::numeric_limits<int>::max(), { analyzer->env } };
	}
	int d = 0;
	if (!unknown) {
		for (size_t i = 0; i < new_version.type->arguments().size(); ++i) {
			auto type = [&]() { if (i < arguments.size()) {
				return arguments.at(i);
			} else if (f and ((Function*) f)->defaultValues.at(i)) {
				return ((Function*) f)->defaultValues.at(i)->type;
			} else {
				assert(false);
			}}();
			auto di = type->distance(new_version.type->arguments().at(i));
			// std::cout << type << " distance to " << new_version.type->arguments().at(i) << " " << di << std::endl;
			if (di < 0) return { std::numeric_limits<int>::max(), { analyzer->env } };
			d += di;
		}
	}
	return { d, new_version };
}

void solve(SemanticAnalyzer* analyzer, const Type* t1, const Type* t2) {
	// std::cout << "Solve " << t1 << " ||| " << t2 << std::endl;
	if (t1->is_template()) {
		if (t1 != t2) {
			t1->implement(t2);
		}
	}
	else if (auto baseof = dynamic_cast<const Meta_baseof_type*>(t1)) {
		// std::cout << "solve baseof " << baseof->type << " " << t2 << std::endl;
		solve(analyzer, baseof->type, t2);
	}
	else if (auto not_void = dynamic_cast<const Meta_not_void_type*>(t1)) {
		// std::cout << "not_void " << not_void->type << std::endl;
		solve(analyzer, not_void->type, t2);
	}
	else if ((t1->is_fixed_array() or t1->is_fixed_array_ptr()) and (t2->is_fixed_array() or t2->is_fixed_array_ptr())) {
		auto t1t = t1->is_fixed_array_ptr() ? t1->pointed() : t1;
		auto t2t = t2->is_fixed_array_ptr() ? t2->pointed() : t2;
		// std::cout << "solve array " << t1t << " " << t2t << std::endl;
		if (t1t->size() == t2t->size()) {
			for (size_t i = 0; i < t1t->size(); ++i) {
				solve(analyzer, t1t->element(i), t2t->element(i));
			}
		}
	}
	else if ((t1->is_array() or t1->is_fixed_array_ptr()) /* and (t2->is_array() or t2->is_fixed_array_ptr()) */) {
		auto t1t = t1->is_fixed_array_ptr() ? t1->pointed() : t1;
		auto t2t = t2->is_fixed_array_ptr() ? t2->pointed() : t2;
		// std::cout << "solve array " << t1t << " " << t2t << std::endl;
		solve(analyzer, t1t->element(), t2t->element());
	}
	else if (t1->is_set() and t2->is_set()) {
		solve(analyzer, t1->element(), t2->element());
	}
	else if (t1->is_map() and t2->is_map()) {
		solve(analyzer, t1->key(), t2->key());
		solve(analyzer, t1->element(), t2->element());
	}
	else if ((t1->is_function() or t1->is_function_pointer() or t1->is_function_object()) and (t2->is_function() or t2->is_function_pointer() or t2->is_function_object())) {
		// std::cout << "solve function " << t1 << " " << t2 << std::endl;
		auto f = t2->function();
		if (f) {
			auto t1_args = build(*analyzer, t1)->arguments();
			// std::cout << "t1 args " << t1_args << std::endl;
			auto type = ((Value*) f)->will_take(analyzer, t1_args, 1);
			solve(analyzer, t1->return_type(), type->return_type());
		}
	}
}

void CallableVersionTemplate::resolve_templates(SemanticAnalyzer* analyzer, std::vector<const Type*> arguments) const {
	// std::cout << "CallableVersion::resolve_templates(" << arguments << ")" << std::endl;
	// First passage to solve easy types
	for (size_t i = 0; i < arguments.size(); ++i) {
		const auto& t1 = type->argument(i);
		if (t1->is_template()) {
			const auto& t2 = arguments.at(i);
			// std::cout << t1 << " <=> " << t2 << std::endl;
			solve(analyzer, t1, t2);
		}
	}
	for (size_t i = 0; i < arguments.size(); ++i) {
		const auto& t1 = type->argument(i);
		if (not t1->is_template()) {
			const auto& t2 = arguments.at(i);
			solve(analyzer, t1, t2);
		}
	}
}

}

namespace std {
	std::ostream& operator << (std::ostream& os, const ls::CallableVersionTemplate& v) {
		if (v.templates.size()) {
			os << "template<";
			for (size_t i = 0; i < v.templates.size(); ++i) {
				if (i != 0) std::cout << ", ";
				std::cout << v.templates.at(i);
			}
			os << "> ";
		}
		os << v.name << " ";
		// if (v.object) os << "★ " << v.object << ":" << v.object->type << " ";
		os << v.type->arguments() << BLUE_BOLD << " => " << END_COLOR << v.type->return_type();
		if (v.user_fun) {
			os << " (user func " << v.user_fun << ")";
		} else if (v.symbol) {
			os << " (symbol " << v.name << ")";
		#if COMPILER
		} else if (v.func) {
			os << " (compiler func)";
		#endif
		}
		if (v.unknown) os << " (unknown)";
		if (v.flags) os << " " << v.flags;
		return os;
	}
	std::ostream& operator << (std::ostream& os, const ls::CallableVersionTemplate* v) {
		os << *v;
		return os;
	}
}