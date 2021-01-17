#ifndef INTERVAL_HPP
#define INTERVAL_HPP

#include <vector>
#include <memory>
#include "Value.hpp"
#include "../lexical/Token.hpp"

namespace ls {

class Interval : public Value {
public:

	VirtualToken opening_bracket;
	VirtualToken closing_bracket;
	std::unique_ptr<Value> start;
	std::unique_ptr<Value> end;

	Interval(Environment& env, VirtualToken opening_bracket, VirtualToken closing_bracket);

	virtual void print(std::ostream&, int indent, PrintOptions options) const override;
	virtual Location location() const override;

	virtual void pre_analyze(SemanticAnalyzer*) override;
	virtual void analyze(SemanticAnalyzer*) override;

	#if COMPILER
	virtual Compiler::value compile(Compiler&) const override;
	#endif

	virtual std::unique_ptr<Value> clone(Block* parent) const override;
};

}

#endif
