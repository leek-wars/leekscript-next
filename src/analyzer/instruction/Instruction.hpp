#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <ostream>
#include "../../constants.h"
#include "../lexical/Token.hpp"
#include "../../type/Type.hpp"
#include "../PrintOptions.hpp"
#if COMPILER
#include "../../compiler/Compiler.hpp"
#endif

namespace ls {

class Value;
class SemanticAnalyzer;
class Environment;

class Instruction {
public:

	const Type* type = nullptr;
	const Type* return_type = nullptr;
	bool returning = false;
	bool may_return = false;
	bool is_void = false;
	bool throws = false;

	Instruction(Environment& env);
	virtual ~Instruction() {}

	virtual void print(std::ostream&, int indent, PrintOptions options) const = 0;
	virtual Location location() const = 0;

	virtual void pre_analyze(SemanticAnalyzer* analyzer);
	virtual void analyze(SemanticAnalyzer* analyzer);
	virtual void analyze(SemanticAnalyzer* analyzer, const Type* type) = 0;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const = 0;
	virtual Compiler::value compile_end(Compiler&) const;
	#endif

	virtual std::unique_ptr<Instruction> clone() const = 0;

	std::string tabs(int indent) const;
};

}

#endif
