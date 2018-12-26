#ifndef VALUE_STD_HPP
#define VALUE_STD_HPP

#include "../Module.hpp"

namespace ls {

class ValueSTD : public Module {
public:
	ValueSTD();

	/*
	 * Static fields
	 */
	static Compiler::value unknown(Compiler& c);

	/*
	 * Fields
	 */
	static Compiler::value attr_class(Compiler& c, Compiler::value a);

	/*
	 * Operators
	 */
	static Compiler::value op_instanceof(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_equals(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_not_equals(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_lt(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_le(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_gt(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_ge(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_and(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_or(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_xor(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_bit_and(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_bit_and_eq(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_bit_or(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_bit_or_eq(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_bit_xor(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_bit_xor_eq(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_in(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_swap_val(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_swap_ptr(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_pow(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_int_div(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_int_div_eq(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_mod(Compiler&, std::vector<Compiler::value>);
	static Compiler::value op_div(Compiler&, std::vector<Compiler::value>);

	/*
	 * Methods
	 */
	static Compiler::value copy(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value to_string(Compiler& c, std::vector<Compiler::value> args);

	// Hidden functions
	static Compiler::value typeID(Compiler& c, std::vector<Compiler::value> args);
};

}

#endif
