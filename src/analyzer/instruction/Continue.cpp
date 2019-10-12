#include "Continue.hpp"
#include "../semantic/SemanticAnalyzer.hpp"
#include "../error/Error.hpp"

namespace ls {

Continue::Continue(Environment& env) : Instruction(env) {
	deepness = 1;
	jumping = true;
	jump_to_existing_section = true;
}

void Continue::print(std::ostream& os, int, PrintOptions) const {
	os << "continue";
	if (deepness > 1) {
		os << " " << deepness;
	}
}

Location Continue::location() const {
	return {nullptr, {0, 0, 0}, {0, 0, 0}};
}

void Continue::analyze(SemanticAnalyzer* analyzer, const Type*) {

	// continue must be in a loop
	if (!analyzer->in_loop(deepness)) {
		analyzer->add_error({Error::Type::CONTINUE_MUST_BE_IN_LOOP, {nullptr, {0, 0, 0}, {0, 0, 0}}, {nullptr, {0, 0, 0}, {0, 0, 0}}}); // TODO location
	}
}

#if COMPILER
Compiler::value Continue::compile(Compiler& c) const {
	c.delete_variables_block(c.get_current_loop_blocks(deepness));
	// c.insn_branch(c.get_current_loop_cond_section(deepness));
	// c.insert_new_generation_block();
	return { c.env };
}
#endif

std::unique_ptr<Instruction> Continue::clone() const {
	auto c = std::make_unique<Continue>(type->env);
	c->deepness = deepness;
	return c;
}

}
