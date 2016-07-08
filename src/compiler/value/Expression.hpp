#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <vector>

#include "../../compiler/lexical/Operator.hpp"
#include "../../compiler/value/Value.hpp"

namespace ls {

class Expression : public Value {
public:

	Value* v1;
	Value* v2;
	Operator* op;

	bool ignorev2;
	bool no_op;

	int operations;

	Expression();
	Expression(Value*);
	virtual ~Expression();

	void append(Operator*, Value*);

	void print(std::ostream&) const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;
};

}

#endif
