#include "VariableValue.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../../vm/VM.hpp"
#include "Function.hpp"
#include "math.h"

using namespace std;

VariableValue::VariableValue(Token* name) {
	this->name = name;
	this->var = nullptr;
	constant = false;
}

VariableValue::~VariableValue() {}

void VariableValue::print(ostream& os) const {
	os << name->content;
}

void VariableValue::analyse(SemanticAnalyser* analyser, const Type) {

	var = analyser->get_var(name);
	type = var->type;
	attr_types = var->attr_types;

//	cout << "VV " << name->content << " : " << type << endl;
//	cout << "var scope : " << (int)var->scope << endl;
	//for (auto t : attr_types)
	//	cout << t.first << " : " << t.second << endl;
}

extern map<string, jit_value_t> internals;
extern map<string, jit_value_t> globals;
extern map<string, jit_value_t> locals;

LSValue* plus_function(LSValue* x, LSValue* y) {
	return y->operator + (x);
}

int plus_function_int(int x, int y) {
	return x + y;
}
int minus_function_int(int x, int y) {
	return x - y;
}
int mul_function_int(int x, int y) {
	return x * y;
}
int div_function_int(int x, int y) {
	return x / y;
}
int pow_function_int(int x, int y) {
	return pow(x, y);
}
int mod_function_int(int x, int y) {
	return x % y;
}

jit_value_t VariableValue::compile_jit(Compiler&, jit_function_t& F, Type req_type) const {

//	cout << "compile vv " << name->content << " : " << type << endl;
//	cout << "req type : " << req_type << endl;

	if (name->content == "+") {
		return JIT_CREATE_CONST_POINTER(F, new LSFunction((void*) &plus_function_int));
	}
	if (name->content == "-") {
		return JIT_CREATE_CONST_POINTER(F, new LSFunction((void*) &minus_function_int));
	}
	if (name->content == "*") {
		return JIT_CREATE_CONST_POINTER(F, new LSFunction((void*) &mul_function_int));
	}
	if (name->content == "/") {
		return JIT_CREATE_CONST_POINTER(F, new LSFunction((void*) &div_function_int));
	}
	if (name->content == "^") {
		return JIT_CREATE_CONST_POINTER(F, new LSFunction((void*) &pow_function_int));
	}
	if (name->content == "%") {
		return JIT_CREATE_CONST_POINTER(F, new LSFunction((void*) &mod_function_int));
	}

	if (var->scope == VarScope::INTERNAL) {

//		cout << "internal" << endl;

		jit_value_t v = internals[name->content];
		if (var->type.nature != Nature::POINTER and req_type.nature == Nature::POINTER) {
			return VM::value_to_pointer(F, v, req_type);
		}
		return v;

	} else if (var->scope == VarScope::GLOBAL) {

//		cout << "global var : " << name->content << endl;

		jit_value_t v = globals[name->content];
		if (var->type.nature != Nature::POINTER and req_type.nature == Nature::POINTER) {
			return VM::value_to_pointer(F, v, req_type);
		}
		return v;

	} else if (var->scope == VarScope::LOCAL) {

		jit_value_t v = locals[name->content];
		if (var->type.nature != Nature::POINTER and req_type.nature == Nature::POINTER) {
			cout << "convert local" << endl;
			return VM::value_to_pointer(F, v, req_type);
		}
		return v;

	} else { // var->scope == VarScope::PARAMETER

//		cout << "Compile arg : " << type << endl;

		jit_value_t v = jit_value_get_param(F, var->index);
		if (var->type.nature != Nature::POINTER and req_type.nature == Nature::POINTER) {
			return VM::value_to_pointer(F, v, req_type);
		}
		return v;
	}
}

jit_value_t VariableValue::compile_jit_l(Compiler& c, jit_function_t& F, Type req_type) const {

	return compile_jit(c, F, type);
}
