#ifndef VM_STANDARD_ARRAYSTD_HPP_
#define VM_STANDARD_ARRAYSTD_HPP_

#include "../Module.hpp"

namespace ls {

class ArraySTD : public Module {
public:
	ArraySTD(Environment& env);

	#if COMPILER

	static Compiler::value new_(Compiler& c, std::vector<Compiler::value> args, int);

	/**
	 * Operators
	 */
	static Compiler::value eq(Compiler& c, std::vector<Compiler::value> args, int);
	static void fixed_eq(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value lt(Compiler& c, std::vector<Compiler::value> args, int);
	static void fixed_lt(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value in(Compiler& c, std::vector<Compiler::value> args, int);
	static void fixed_in(Compiler& c, std::vector<Compiler::value> args);
	static Compiler::value op_add(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value array_add_eq(Compiler& c, std::vector<Compiler::value> args, int);

	/**
	 * Methods
	 */
	static Compiler::value average(Compiler&, std::vector<Compiler::value>, int);
	static void fixed_average(Compiler&, std::vector<Compiler::value>);
	static void vector_average(Compiler&, std::vector<Compiler::value>);

	static Compiler::value size(Compiler&, std::vector<Compiler::value>, int);

	static LSArray<LSValue*>* chunk_1_ptr(LSArray<LSValue*>* array);
	static LSArray<LSValue*>* chunk_1_int(LSArray<int>* array);
	static LSArray<LSValue*>* chunk_1_long(LSArray<long>* array);
	static LSArray<LSValue*>* chunk_1_float(LSArray<double>* array);

	static LSValue* sub(LSArray<LSValue*>* array, int begin, int end);

	static Compiler::value fill(Compiler&, std::vector<Compiler::value>, int);

	static Compiler::value remove_element_any(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value remove_element_real(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value remove_element_int(Compiler&, std::vector<Compiler::value>, int);

	static Compiler::value partition(Compiler&, std::vector<Compiler::value>, int);

	static Compiler::value map(Compiler&, std::vector<Compiler::value>, int);
	static void fixed_map(Compiler&, std::vector<Compiler::value>);

	static Compiler::value min_fun(Compiler& c, std::vector<Compiler::value> args, int flags);
	static Compiler::value first(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value last(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value fold_left(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value fold_right(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value iter(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value sort(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value push(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value filter(Compiler&, std::vector<Compiler::value>, int);
	static void fixed_filter(Compiler&, std::vector<Compiler::value>);
	static Compiler::value push_all(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value reverse(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value repeat(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value layer(Compiler&, std::vector<Compiler::value>, int);
	static Compiler::value clear(Compiler&, std::vector<Compiler::value>, int);

	static Compiler::value sum(Compiler&, std::vector<Compiler::value>, int);
	static void fixed_sum(Compiler&, std::vector<Compiler::value>);
	static void vector_sum(Compiler&, std::vector<Compiler::value>);

	static int convert_key(LSValue*);

	#endif
};

}

#endif
