#include "PostfixExpression.hpp"
#include "LeftValue.hpp"
#include "../../vm/LSValue.hpp"
#include "../semantic/SemanticAnalyser.hpp"
#include "../semantic/SemanticError.hpp"

using namespace std;

namespace ls {

PostfixExpression::PostfixExpression() {
	expression = nullptr;
	operatorr = nullptr;
	return_value = true;
}

PostfixExpression::~PostfixExpression() {
	delete expression;
}

void PostfixExpression::print(ostream& os, int indent, bool debug, bool condensed) const {
	expression->print(os, indent, debug);
	operatorr->print(os);
	if (debug) {
		os << " " << type;
	}
}

Location PostfixExpression::location() const {
	return expression->location(); // TODO add the op
}

void PostfixExpression::analyse(SemanticAnalyser* analyser) {

	expression->analyse(analyser);

	if (expression->type.constant) {
		analyser->add_error({SemanticError::Type::CANT_MODIFY_CONSTANT_VALUE, location(), expression->location(), {expression->to_string()}});
	}
	if (!expression->isLeftValue()) {
		analyser->add_error({SemanticError::Type::VALUE_MUST_BE_A_LVALUE, location(), expression->location(), {expression->to_string()}});
	}
	type = expression->type;
	if (type == Type::MPZ) {
		type.temporary = true;
	}
	this->return_value = return_value;
}

Compiler::value PostfixExpression::compile(Compiler& c) const {

	c.inc_ops(1);

	switch (operatorr->type) {

		case TokenType::PLUS_PLUS: {

			if (expression->type == Type::MPZ) {
				auto x = expression->compile_l(c);
				auto r = c.insn_clone_mpz(c.insn_load(x));
				auto one = c.new_integer(1);
				c.insn_call({}, {x, x, one}, &mpz_add_ui);
				return r;
			} else if (expression->type.nature == Nature::VALUE) {
				auto x_addr = expression->compile_l(c);
				auto x = c.insn_load(x_addr);
				auto sum = c.insn_add(x, c.new_integer(1));
				c.insn_store(x_addr, sum);
				return x;
			} else {
				auto e = expression->compile_l(c);
				return c.insn_call(Type::POINTER, {e}, (void*) +[](LSValue** x) {
					return (*x)->ls_inc();
				}, true);
			}
			break;
		}
		case TokenType::MINUS_MINUS: {
			if (expression->type.nature == Nature::VALUE) {
				auto x_addr = expression->compile_l(c);
				auto x = c.insn_load(x_addr);
				auto sum = c.insn_sub(x, c.new_integer(1));
				c.insn_store(x_addr, sum);
				return x;
			} else {
				auto e = expression->compile_l(c);
				return c.insn_call(Type::POINTER, {e}, (void*) +[](LSValue** x) {
					return (*x)->ls_dec();
				}, true);
			}
			break;
		}
		default: {}
	}
	return {nullptr, Type::ANY};
}

Value* PostfixExpression::clone() const {
	auto pe = new PostfixExpression();
	pe->expression = (LeftValue*) expression->clone();
	pe->operatorr = operatorr;
	return pe;
}

}
