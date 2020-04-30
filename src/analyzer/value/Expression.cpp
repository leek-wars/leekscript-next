#include "Expression.hpp"
#include "ArrayAccess.hpp"
#include "ObjectAccess.hpp"
#include "Function.hpp"
#include "LeftValue.hpp"
#include "Number.hpp"
#include "VariableValue.hpp"
#include "../Program.hpp"
#include "../../vm/Exception.hpp"
#include "../semantic/Callable.hpp"
#include "../semantic/FunctionVersion.hpp"
#include "../semantic/Variable.hpp"

namespace ls {

Expression::Expression(Environment& env, Value* v) : Value(env), v1(v), operations(1), callable_version(env) {}

void Expression::append(std::shared_ptr<Operator> op, Value* exp) {
	/*
	 * Single expression (2, 'hello', ...), just add the operator
	 */
	if (this->op == nullptr) {
		this->op = op;
		v2.reset(exp);
		return;
	}
	if (!parenthesis and (op->priority < this->op->priority or (op->priority == this->op->priority and op->character == "="))) {
		/*
		 * We already have '5 + 2' for example,
		 * and try to add a operator with a higher priority,
		 * such as : '× 7' => '5 + (2 × 7)'
		 */
		if (auto exp2 = dynamic_cast<Expression*>(v2.get())) {
			exp2->append(op, exp);
		} else {
			auto ex = new Expression(type->env);
			ex->v1 = std::move(v2);
			ex->op = op;
			ex->v2.reset(exp);
			this->v2.reset(ex);
		}
	} else {
		/*
		 * We already have '5 + 2' for example,
		 * and try to add a operator with a lower priority,
		 * such as : '< 7' => '(5 + 2) < 7'
		 */
		auto newV1 = new Expression(type->env);
		newV1->v1 = std::move(this->v1);
		newV1->op = this->op;
		newV1->v2 = std::move(this->v2);
		this->v1.reset(newV1);
		this->op = op;
		v2.reset(exp);
	}
}

void Expression::print(std::ostream& os, int indent, PrintOptions options) const {
	if (parenthesis or options.parenthesis) {
		os << "(";
	}
	if (v1 != nullptr) {
		v1->print(os, indent, options.add_parenthesis());
		if (op != nullptr) {
			os << " ";
			op->print(os);
			os << " ";
			v2->print(os, indent, options.add_parenthesis(op->token->type != TokenType::EQUAL));
		}
	}
	if (parenthesis or options.parenthesis) {
		os << ")";
	}
	if (options.debug) {
		os << " " << type;
	}
}

Location Expression::location() const {
	auto v1_location = v1->location();
	auto start = v1_location.start;
	auto end = op == nullptr ? v1->location().end : v2->location().end;
	return {v1_location.file, start, end};
}

void Expression::pre_analyze(SemanticAnalyzer* analyzer) {
	if (op == nullptr) {
		v1->pre_analyze(analyzer);
	} else {
		v2->pre_analyze(analyzer);
		v1->pre_analyze(analyzer);
		if (op->type == TokenType::EQUAL or op->type == TokenType::PLUS_EQUAL) {
			if (auto vv = dynamic_cast<VariableValue*>(v1.get())) {
				// std::cout << "update variable " << vv->var << " " << (int) vv->var->scope << std::endl;
				if (vv->var->scope != VarScope::CAPTURE) {
					// std::cout << "EX update_var " << vv->var;
					vv->var = analyzer->update_var(vv->var);
					// std::cout << " to " << vv->var << std::endl;
					vv->update_variable = true;
				}
			} else if (auto aa = dynamic_cast<ArrayAccess*>(v1.get())) {
				if (auto vv = dynamic_cast<VariableValue*>(aa->array.get())) {
					vv->var = analyzer->update_var(vv->var);
					vv->update_variable = true;
				} else if (auto aa2 = dynamic_cast<ArrayAccess*>(aa->array.get())) {
					if (auto vv2 = dynamic_cast<VariableValue*>(aa2->array.get())) {
						vv2->var = analyzer->update_var(vv2->var);
						vv2->update_variable = true;
					}
				}
			}
		}
	}
}

void Expression::analyze(SemanticAnalyzer* analyzer) {
	// std::cout << "Expression::analyze() " << this << std::endl;
	auto& env = analyzer->env;

	operations = 1;
	type = env.any;

	// No operator : just analyze v1 and return
	if (op == nullptr) {
		v1->is_void = is_void;
		v1->analyze(analyzer);
		type = v1->type;
		return_type = v1->return_type;
		throws = v1->throws;
		return;
	}

	v2->analyze(analyzer);
	v1->analyze(analyzer);

	throws = v1->throws or v2->throws;

	if (op->type == TokenType::DOUBLE_QUESTION_MARK) {
		type = v1->type->operator * (v2->type);
		return;
	}
	if (op->type == TokenType::CATCH_ELSE) {
		type = env.any;
		return;
	}

	// in operator : v1 must be a container
	if (op->type == TokenType::IN and not v2->type->can_be_container()) {
		analyzer->add_error({Error::Type::VALUE_MUST_BE_A_CONTAINER, location(), v2->location(), {v2->to_string()}});
		return;
	}

	// A = B, A += B, etc. A must be a l-value
	if (op->type == TokenType::EQUAL or op->type == TokenType::PLUS_EQUAL
		or op->type == TokenType::MINUS_EQUAL or op->type == TokenType::TIMES_EQUAL
		or op->type == TokenType::DIVIDE_EQUAL or op->type == TokenType::MODULO_EQUAL
		or op->type == TokenType::BIT_AND_EQUALS or op->type == TokenType::BIT_OR_EQUALS or op->type == TokenType::BIT_XOR_EQUALS
		or op->type == TokenType::POWER_EQUAL or op->type == TokenType::INT_DIV_EQUAL) {
		// Change the type of x for operator =
		if (op->type == TokenType::EQUAL) {
			if (v2->type->is_void()) {
				analyzer->add_error({Error::Type::CANT_ASSIGN_VOID, location(), v2->location(), {v1->to_string()}});
			}
		}
		// TODO other operators like |= ^= &=
		if (v1->type->constant) {
			analyzer->add_error({Error::Type::CANT_MODIFY_CONSTANT_VALUE, location(), op->token->location, {v1->to_string()}});
			return; // don't analyze more
		}
		// Check if A is a l-value
		if (not v1->isLeftValue()) {
			analyzer->add_error({Error::Type::VALUE_MUST_BE_A_LVALUE, location(), v1->location(), {v1->to_string()}});
			return; // don't analyze more
		}
	}

	// Merge operations count
	// (2 + 3) × 4    ->  2 ops for the × directly
	if (op->type != TokenType::OR or op->type == TokenType::AND) {
		if (Expression* ex1 = dynamic_cast<Expression*>(v1.get())) {
			operations += ex1->operations;
			ex1->operations = 0;
		}
		if (Expression* ex2 = dynamic_cast<Expression*>(v2.get())) {
			operations += ex2->operations;
			ex2->operations = 0;
		}
	}

	auto v1_type = op->reversed ? v2->type : v1->type;
	auto v2_type = op->reversed ? v1->type : v2->type;

	// std::cout << "Expression " << v1_type << " " << op->token->content << " " << v2_type << std::endl;

	if (v1_type->class_name().size() || v1_type->is_void()) {
		auto class_name = v1_type->is_void() ? "Value" : v1_type->class_name();
		auto object_class = analyzer->globals[class_name]->clazz;
		auto call = object_class->getOperator(analyzer, op->character);
		// std::cout << "Callable : " << callable << std::endl;
		callable_version = call.resolve(analyzer, {v1_type, v2_type});
		if (callable_version.template_) {
			// std::cout << "Callable version : " << callable_version << std::endl;
			throws |= callable_version.template_->flags & Module::THROWS;
			callable_version.apply_mutators(analyzer, {v1.get(), v2.get()});
			// For placeholder types, keep them no matter the operator
			auto return_type = callable_version.type->return_type();
			if (op->type == TokenType::PLUS or op->type == TokenType::MINUS or op->type == TokenType::TIMES or op->type == TokenType::MODULO) {
				if (v1->type->placeholder) { return_type = v1_type; }
				if (v2->type->placeholder) { return_type = v2_type; }
			}
			type = is_void ? env.void_ : return_type;
			if ((v2_type->is_function() or v2_type->is_function_pointer() or v2_type->is_function_object()) and (callable_version.type->argument(1)->is_function() or callable_version.type->argument(1)->is_function_pointer())) {
				v2->will_take(analyzer, callable_version.type->argument(1)->arguments(), 1);
				v2->set_version(analyzer, callable_version.type->argument(1)->arguments(), 1);
			}
			// std::cout << "Operator " << v1->to_string() << " (" << v1->type << ") " << op->character << " " << v2->to_string() << "(" << v2->type << ") found! " << return_type << std::endl;
			return;
		}
	}
	// std::cout << "No such operator " << v1->type << " " << op->character << " " << v2->type << std::endl;
	analyzer->add_error({Error::Type::NO_SUCH_OPERATOR, location(), op->token->location, {v1->type->to_string(), op->character, v2->type->to_string()}});
}

Hover Expression::hover(SemanticAnalyzer& analyzer, size_t position) const {
	if (v1->location().contains(position)) {
		return v1->hover(analyzer, position);
	}
	if (v2 and v2->location().contains(position)) {
		return v2->hover(analyzer, position);
	}
	return { type, location() };
}

#if COMPILER
Compiler::value Expression::compile(Compiler& c) const {

	// No operator : compile v1 and return
	if (op == nullptr) {
		return v1->compile(c);
	}

	c.mark_offset(location().start.line);

	// Increment operations
	if (operations > 0) {
		c.inc_ops(operations);
	}

	// Special case x / 0 : compile a throw exception instead
	if ((op->type == TokenType::DIVIDE or op->type == TokenType::DIVIDE_EQUAL or op->type == TokenType::INT_DIV or op->type == TokenType::INT_DIV_EQUAL or op->type == TokenType::MODULO or op->type == TokenType::MODULO_EQUAL) and v2->is_zero()) {
		c.mark_offset(op->token->location.start.line);
		c.insn_throw_object(vm::Exception::DIVISION_BY_ZERO);
		return c.insn_convert(c.new_integer(0), c.fun->getReturnType(c.env));
	}

	// array[] = 12, array push
	if (op->type == TokenType::EQUAL) {
		auto array_access = dynamic_cast<ArrayAccess*>(v1.get());
		if (array_access && array_access->key == nullptr) {
			if (auto vv = dynamic_cast<VariableValue*>(array_access->array.get())) {
				auto previous_value = c.insn_load(vv->var->parent->entry);
				auto converted = c.insn_convert(previous_value, vv->var->type);
				if (previous_value.v == converted.v) {
					vv->var->entry = vv->var->parent->entry;
				} else {
					vv->var->create_entry(c);
					vv->var->store_value(c, c.insn_move_inc(converted));
					c.insn_delete_variable(vv->var->parent->entry);
				}
			}
			auto x_addr = ((LeftValue*) array_access->array.get())->compile_l(c);
			auto y = c.insn_to_any(v2->compile(c));
			auto r = c.insn_invoke(c.env.any, {x_addr, y}, "Value.operator+=");
			v2->compile_end(c);
			return r;
		}
	}

	// x ?? y ==> if (x != null) { x } else { y }
	if (op->type == TokenType::DOUBLE_QUESTION_MARK) {
		Compiler::label label_then = c.insn_init_label("then");
		Compiler::label label_else = c.insn_init_label("else");
		Compiler::label label_end = c.insn_init_label("end");

		auto x = c.insn_convert(v1->compile(c), type);
		v1->compile_end(c);
		auto condition = c.insn_call(c.env.boolean, {x}, "Value.is_null");
		c.insn_if_new(condition, &label_then, &label_else);

		c.insn_label(&label_then);
		auto y = c.insn_convert(v2->compile(c), type);
		v2->compile_end(c);
		c.insn_branch(&label_end);
		label_then.block = c.builder.GetInsertBlock();

		c.insn_label(&label_else);
		c.insn_branch(&label_end);
		label_else.block = c.builder.GetInsertBlock();

		c.insn_label(&label_end);
		auto PN = c.builder.CreatePHI(type->llvm(c), 2);
		PN->addIncoming(y.v, label_then.block);
		PN->addIncoming(x.v, label_else.block);
		return {PN, type};
	}

	if (op->type == TokenType::CATCH_ELSE) {
		auto r = c.create_entry("r", type);
		c.insn_try_catch([&]() {
			auto v1_value = v1->compile(c);
			if (v1_value.v) { // can be void
				c.insn_store(r, c.insn_convert(v1_value, type->fold()));
			}
			v1->compile_end(c);
		}, [&]() {
			auto y = v2->compile(c);
			c.insn_store(r, c.insn_convert(y, type->fold()));
			v2->compile_end(c);
		});
		return c.insn_load(r);
	}

	assert(callable_version.template_);

	std::vector<Compiler::value> args;
	auto compiled_v2 = [&](){ if (callable_version.template_->v2_addr) {
		return ((LeftValue*) v2.get())->compile_l(c);
	} else {
		auto v = v2->compile(c);
		if (callable_version.template_->symbol and v.t->is_primitive() and callable_version.type->argument(1)->is_any()) {
			v = c.insn_to_any(v);
		}
		return v;
	}}();
	c.add_temporary_expression_value(compiled_v2);

	int flags = callable_version.compile_mutators(c, {v1.get(), v2.get()});
	if (is_void) flags |= Module::NO_RETURN;

	auto compiled_v1 = [&](){ if (callable_version.template_->v1_addr) {
		return ((LeftValue*) v1.get())->compile_l(c);
	} else {
		auto v = v1->compile(c);
		if (callable_version.template_->symbol and v.t->is_primitive() and callable_version.type->argument(0)->is_any()) {
			v = c.insn_to_any(v);
		}
		return v;
	}}();
	args.push_back(compiled_v1);
	args.push_back(compiled_v2);
	if (op->reversed) std::reverse(args.begin(), args.end());

	c.pop_temporary_expression_value();

	auto r = callable_version.compile_call(c, args, nullptr, flags);

	v1->compile_end(c);
	v2->compile_end(c);
	return r;
}
#endif

std::unique_ptr<Value> Expression::clone(Block* parent) const {
	auto ex = std::make_unique<Expression>(type->env);
	ex->v1 = v1->clone(parent);
	ex->op = op;
	ex->v2 = v2 ? v2->clone(parent) : nullptr;
	return ex;
}

}
