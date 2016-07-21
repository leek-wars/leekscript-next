#ifndef FUNCTIONCALL_HPP
#define FUNCTIONCALL_HPP

#include <vector>

#include "../../compiler/value/Value.hpp"

namespace ls {

class FunctionCall : public Value {
public:

	Value* function;
	std::vector<Value*> arguments;

	bool is_native = false;
	bool is_static_native = false;
	std::string native_func;

	void* std_func;
	Value* this_ptr;

	FunctionCall();
	virtual ~FunctionCall();

	virtual void print(std::ostream&) const override;
	virtual int line() const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

}

#endif
