#include "ExpressionInstruction.hpp"
#include "../value/Block.hpp"
#include "../value/If.hpp"
#include "../semantic/SemanticAnalyzer.hpp"

namespace ls {

ExpressionInstruction::ExpressionInstruction(Environment& env, std::unique_ptr<Value> value) : Instruction(env), value(std::move(value)) {}

void ExpressionInstruction::print(std::ostream& os, int indent, PrintOptions options) const {
	value->print(os, indent, options);
}

Location ExpressionInstruction::location() const {
	return value->location();
}

void ExpressionInstruction::pre_analyze(SemanticAnalyzer* analyzer) {
	value->pre_analyze(analyzer);
}

void ExpressionInstruction::analyze(SemanticAnalyzer* analyzer, const Type* req_type) {
	auto& env = analyzer->env;
	// std::cout << "ExpressionInstruction::analyze() " << is_void << std::endl;
	value->is_void = is_void;
	value->analyze(analyzer);
	if (req_type->is_void()) {
		type = env.void_;
	} else {
		type = value->type;
	}
	returning = value->returning;
	may_return = value->may_return;
	return_type = value->return_type;
	throws = value->throws;
}

#if COMPILER
Compiler::value ExpressionInstruction::compile(Compiler& c) const {
	auto v = value->compile(c);
	assert(!v.v or v.t->llvm(c) == v.v->getType());
	return v;
}

Compiler::value ExpressionInstruction::compile_end(Compiler& c) const {
	value->compile_end(c);
}
#endif

std::unique_ptr<Instruction> ExpressionInstruction::clone() const {
	return std::make_unique<ExpressionInstruction>(type->env, value->clone());
}

}
