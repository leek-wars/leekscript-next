#include <algorithm>
#include "ArraySTD.hpp"
#include "ValueSTD.hpp"
#include "../TypeMutator.hpp"
#include "../../type/Type.hpp"
#include "../../analyzer/semantic/Variable.hpp"
#include "../../analyzer/value/Function.hpp"
#include "../../analyzer/semantic/FunctionVersion.hpp"
#include "../../environment/Environment.hpp"
#if COMPILER
#include "../../vm/value/LSNumber.hpp"
#include "../../vm/value/LSArray.hpp"
#endif

namespace ls {

ArraySTD::ArraySTD(Environment& env) : Module(env, "Array") {

	#if COMPILER
	env.array_class = std::make_unique<LSClass>(clazz.get());
	lsclass = env.array_class.get();
	#endif

	static_field("SORT_ASC", env.real, ADDR(nullptr), LEGACY);
	static_field("SORT_DESC", env.integer, ADDR(nullptr), LEGACY);

	/*
	 * Constructor
	 */
	constructor_({
		{env.tmp_empty_array, {}, ADDR(new_)},
		{Type::tmp_array(env.boolean), { env.integer }, ADDR((void*) LSArray<char>::constructor)},
		{Type::tmp_array(env.integer), {env.integer}, ADDR((void*) LSArray<int>::constructor)},
		{Type::tmp_array(env.long_), {env.integer}, ADDR((void*) LSArray<long>::constructor)},
		{Type::tmp_array(env.real), {env.integer}, ADDR((void*) LSArray<double>::constructor)},
		{Type::tmp_array(env.any), {env.integer}, ADDR((void*) LSArray<LSValue*>::constructor)},
	}, LEGACY);

	/*
	 * Operators
	 */
	auto inA = env.template_("A");
	template_(inA).
	operator_("in", {
		{ Type::meta_base_of(inA, env.array), env.const_any, env.boolean, ADDR(in), THROWS},
	});

	auto eqA1 = env.template_("A1");
	template_(eqA1).
	operator_("==", {
		{ Type::meta_base_of(eqA1, env.array), env.const_any, env.boolean, ADDR(eq) },
	});

	auto ltA1 = env.template_("A1");
	auto ltA2 = env.template_("A2");
	template_(ltA1, ltA2).
	operator_("<", {
		{ Type::meta_base_of(ltA1, env.array), Type::meta_base_of(ltA2, env.array), env.boolean, ADDR(lt) },
	});

	auto aT = env.template_("T");
	auto aE = env.template_("E");
	template_(aT, aE).
	operator_("+", {
		{ Type::meta_base_of(aT, env.array), aE, Type::meta_add(aT, aE), ADDR(op_add) },

		// {Type::const_array(aT), aE, Type::meta_concat(Type::const_array(aT), aE), ADDR(op_add)},
		// {Type::meta_base_of(aT, Type::fixed_array({env.void_})), aE, Type::meta_concat(aT, aE), ADDR(op_add)},
		// {Type::const_array(aT), Type::const_array(aE), Type::tmp_array(Type::meta_concat(aT, aE)), ADDR(op_add)},
		// {Type::meta_base_of(aT, Type::fixed_array({env.void_})), Type::meta_base_of(aE, Type::fixed_array({env.void_})), Type::meta_concat(aT, aE), ADDR(op_add)},
	}, LEGACY);

	/*
	 * X : array += Y ==> X = X + Y
	 */
	auto pqT = env.template_("T");
	auto pqE = env.template_("E");
	template_(pqT, pqE).
	operator_("+=", {
		{ Type::meta_base_of(pqT, env.array), pqE, Type::meta_add(pqT, pqE), ADDR(array_add_eq), 0, { env.convert_mutator }, true},
		// // array<T> += E   ==> array<T | E>
		// {Type::array(pqT), pqE, Type::array(Type::meta_add(pqT, Type::meta_not_temporary(pqE))), ADDR(array_add_eq), 0, { env.convert_mutator }, true},
		// // array<T> += array<E>   ==> array<T | E>
		// {Type::array(pqT), Type::array(pqE), Type::array(Type::meta_add(pqT, pqE)), ADDR(array_add_eq), 0, { env.convert_mutator }, true},
	}, LEGACY);

	auto mulT = env.template_("T");
	template_(mulT).
	operator_("*", {
		{Type::const_array(mulT), env.integer, Type::tmp_array(mulT), ADDR(repeat)},
	});

	auto ttA = env.template_("A");
	auto ttF = env.template_("F");
	template_(ttA, ttF).
	operator_("~~", {
		{ Type::meta_base_of(ttA, env.array), ttF, Type::meta_map(ttA, ttF), ADDR(map) },
	});

	/*
	 * Methods
	 */
	auto avgA = env.template_("A");
	template_(avgA).
	method("average", {
		{ env.real, { Type::meta_base_of(avgA, env.array) }, ADDR(average)}
	}, LEGACY);

	method("chunk", {
		{Type::array(env.array), {env.const_array}, ADDR((void*) chunk_1_ptr)},
		{Type::array(Type::array(env.real)), {Type::const_array(env.real)}, ADDR((void*) chunk_1_float)},
		{Type::array(Type::array(env.long_)), {Type::const_array(env.long_)}, ADDR((void*) chunk_1_long)},
		{Type::array(Type::array(env.integer)), {Type::const_array(env.integer)}, ADDR((void*) chunk_1_int)},
		{Type::array(env.array), {env.const_array, env.const_integer}, ADDR((void*) LSArray<LSValue*>::ls_chunk)},
		{Type::array(Type::array(env.real)), {Type::const_array(env.real), env.const_integer}, ADDR((void*) LSArray<double>::ls_chunk)},
		{Type::array(Type::array(env.long_)), {Type::const_array(env.long_), env.const_integer}, ADDR((void*) LSArray<long>::ls_chunk)},
		{Type::array(Type::array(env.integer)), {Type::const_array(env.integer), env.const_integer}, ADDR((void*) LSArray<int>::ls_chunk)},
    });

	auto clA = env.template_("A");
	template_(clA).
	method("clear", {
		{ env.tmp_empty_array, { Type::meta_base_of(clA, env.array) }, ADDR(clear), 0, { env.convert_mutator }  },
	});

	auto coA = env.template_("A");
	template_(coA).
	method("contains", {
		{ env.boolean, { Type::meta_base_of(coA, env.array), env.const_any }, ADDR(in), THROWS},
	});

	auto iter_ptr = ADDR(&LSArray<LSValue*>::ls_iter<LSFunction*>);
	auto iE = env.template_("E");
	template_(iE).
	method("iter", {
		{env.void_, {Type::const_array(iE), Type::fun(env.void_, {iE})}, (void*) iter_ptr},
		{env.void_, {Type::const_array(iE), Type::fun(env.void_, {iE})}, ADDR(iter)},
	});

	auto aiE = env.template_("E");
	template_(aiE).
	method("arrayIter", {
		{env.void_, {Type::const_array(aiE), Type::fun(env.void_, {aiE})}, (void*) iter_ptr},
		{env.void_, {Type::const_array(aiE), Type::fun(env.void_, {aiE})}, ADDR(iter)},
	}, LEGACY_ONLY);

	method("max", {
		{env.any, {env.const_array}, ADDR((void*) LSArray<LSValue*>::ls_max), THROWS},
		{env.real, {Type::const_array(env.real)}, ADDR((void*) LSArray<double>::ls_max), THROWS},
		{env.long_, {Type::const_array(env.long_)}, ADDR((void*) LSArray<long>::ls_max), THROWS},
		{env.integer, {Type::const_array(env.integer)}, ADDR((void*) LSArray<int>::ls_max), THROWS},
	});

	auto amxT = env.template_("T");
	template_(amxT).
	method("arrayMax", {
		{Type::meta_element(amxT), { Type::meta_base_of(amxT, env.array) }, ADDR(nullptr)},
		{Type::meta_element(amxT), { Type::meta_base_of(amxT, env.array), Type::fun_object(env.integer, {Type::meta_element(amxT)}) }, ADDR(nullptr)},
	}, LEGACY_ONLY);

	auto mT = env.template_("T");
	template_(mT).
	method("min", {
		{env.any, {env.const_array}, ADDR((void*) LSArray<LSValue*>::ls_min), THROWS},
		{env.real, {Type::const_array(env.real)}, ADDR((void*) LSArray<double>::ls_min), THROWS},
		{env.long_, {Type::const_array(env.long_)}, ADDR((void*) LSArray<long>::ls_min), THROWS},
		{env.integer, {Type::const_array(env.integer)}, ADDR((void*) LSArray<int>::ls_min), THROWS},
		{mT, {Type::array(mT), Type::fun_object(env.integer, {mT})}, ADDR(min_fun)},
		{env.any, {env.const_empty_array}, ADDR((void*) LSArray<LSValue*>::ls_min), THROWS},
	});


	auto amT = env.template_("T");
	template_(amT).
	method("arrayMin", {
		{Type::meta_element(amT), { Type::meta_base_of(amT, env.array) }, ADDR(min_fun)},
		{Type::meta_element(amT), { Type::meta_base_of(amT, env.array), Type::fun_object(env.integer, {Type::meta_element(amT)}) }, ADDR(min_fun)},
	}, LEGACY_ONLY);

	auto mapA = env.template_("A");
	auto mapF = env.template_("F");
	template_(mapA, mapF).
	method("map", {
		{ Type::meta_map(mapA, mapF), { Type::meta_base_of(mapA, env.array), mapF }, ADDR(map) },
	});

	auto amapA = env.template_("A");
	auto amapF = env.template_("F");
	template_(amapA, amapF).
	method("arrayMap", {
		{ Type::meta_map(amapA, amapF), { Type::meta_base_of(amapA, env.array), amapF }, ADDR(map) },
	}, LEGACY_ONLY);

	method("unique", {
		{env.array, {env.array}, ADDR((void*) LSArray<LSValue*>::ls_unique)},
		{Type::array(env.real), {Type::array(env.real)}, ADDR((void*) LSArray<double>::ls_unique)},
		{Type::array(env.long_), {Type::array(env.long_)}, ADDR((void*) LSArray<long>::ls_unique)},
		{Type::array(env.integer), {Type::array(env.integer)}, ADDR((void*) LSArray<int>::ls_unique)},
	});

	auto sT = env.template_("T");
	template_(sT).
	method("sort", {
		{env.array, {env.array}, ADDR((void*) LSArray<LSValue*>::ls_sort)},
		{Type::array(env.real), {Type::array(env.real)}, ADDR((void*) LSArray<double>::ls_sort)},
		{Type::array(env.long_), {Type::array(env.long_)}, ADDR((void*) LSArray<long>::ls_sort)},
		{Type::array(env.integer), {Type::array(env.integer)}, ADDR((void*) LSArray<int>::ls_sort)},
		{Type::array(sT), {Type::array(sT), Type::fun_object(env.boolean, {sT, sT})}, ADDR(sort)}
	}, LEGACY);

	auto acsT = env.template_("T");
	template_(acsT).
	method("assocSort", {
		{env.void_, {Type::array(acsT)}, ADDR(nullptr)},
		{env.void_, {Type::array(acsT), env.integer}, ADDR(nullptr)},
	}, LEGACY);

	auto asT = env.template_("T");
	template_(asT).
	method("arraySort", {
		{ls::Type::array(asT), {Type::array(asT), Type::fun_object(env.integer, {asT, asT})}, ADDR(nullptr)},
	}, LEGACY_ONLY);

	auto map2_fun_type = (const Type*) Type::fun_object(env.any, {env.any, env.any});
	auto map2_fun_type_int = (const Type*) Type::fun_object(env.any, {env.any, env.integer});
	auto map2_ptr_ptr = ADDR((&LSArray<LSValue*>::ls_map2<LSFunction*, LSValue*, LSValue*>));
	auto map2_ptr_int = ADDR((&LSArray<LSValue*>::ls_map2<LSFunction*, LSValue*, int>));
	method("map2", {
		{env.tmp_array, {env.const_array, env.const_array, map2_fun_type}, (void*) map2_ptr_ptr},
		{env.tmp_array, {env.const_array, Type::const_array(env.integer), map2_fun_type_int}, (void*) map2_ptr_int},
	});

	auto fiA = env.template_("A");
	auto fiF = env.template_("F");
	template_(fiA, fiF).
	method("filter", {
		{ Type::meta_filter(fiA, fiF), { Type::meta_base_of(fiA, env.array), fiF }, ADDR(filter) },
	});

	auto afiT = env.template_("T");
	template_(afiT).
	method("arrayFilter", {
		{Type::tmp_array(afiT), {Type::const_array(afiT), Type::fun(env.boolean, {afiT})}, ADDR(filter)},
	}, LEGACY_ONLY);

	auto afT = env.template_("T");
	template_(afT).
	method("arrayFlatten", {
		{ls::Type::array(afT), {Type::array(afT)}, ADDR(nullptr)},
		{ls::Type::array(afT), {Type::array(afT), env.integer}, ADDR(nullptr)},
	}, LEGACY_ONLY);

	method("isEmpty", {
		{env.boolean, {env.array}, ADDR((void*) LSArray<LSValue*>::ls_empty)},
		{env.boolean, {Type::array(env.real)}, ADDR((void*) LSArray<double>::ls_empty)},
		{env.boolean, {Type::array(env.integer)}, ADDR((void*) LSArray<int>::ls_empty)},
		{env.boolean, {env.empty_array}, ADDR((void*) LSArray<LSValue*>::ls_empty)},
	}, LEGACY);

	auto perm_int_int = ADDR(&LSArray<int>::is_permutation<int>);
	auto perm_int_real = ADDR(&LSArray<int>::is_permutation<double>);
	auto perm_int_ptr = ADDR(&LSArray<int>::is_permutation<LSValue*>);
	auto perm_real_int = ADDR(&LSArray<double>::is_permutation<int>);
	auto perm_real_real = ADDR(&LSArray<double>::is_permutation<double>);
	auto perm_real_ptr = ADDR(&LSArray<double>::is_permutation<LSValue*>);
	auto perm_ptr_int = ADDR(&LSArray<LSValue*>::is_permutation<int>);
	auto perm_ptr_real = ADDR(&LSArray<LSValue*>::is_permutation<double>);
	auto perm_ptr_ptr = ADDR(&LSArray<LSValue*>::is_permutation<LSValue*>);
	method("isPermutation", {
		{env.boolean, {env.array, env.array}, (void*) perm_ptr_ptr},
		{env.boolean, {env.array, Type::array(env.real)}, (void*) perm_ptr_real},
		{env.boolean, {env.array, Type::array(env.integer)}, (void*) perm_ptr_int},
		{env.boolean, {Type::array(env.real), Type::array(env.any)}, (void*) perm_real_ptr},
		{env.boolean, {Type::array(env.real), Type::array(env.real)}, (void*) perm_real_real},
		{env.boolean, {Type::array(env.real), Type::array(env.integer)}, (void*) perm_real_int},
		{env.boolean, {Type::array(env.integer), Type::array(env.any)}, (void*) perm_int_ptr},
		{env.boolean, {Type::array(env.integer), Type::array(env.real)}, (void*) perm_int_real},
		{env.boolean, {Type::array(env.integer), Type::array(env.integer)}, (void*) perm_int_int},
	});

	auto laT = env.template_("T");
	auto laV = env.template_("V");
	template_(laT, laV).
	method("layer", {
		{Type::tmp_array(Type::meta_add(Type::meta_not_temporary(laT), laV)), {Type::const_array(Type::meta_not_temporary(laT)), Type::const_array(laV), laT}, ADDR(layer)}
	});

	method("nextPermutation", {
		{env.boolean, {Type::array(env.integer)}, ADDR((void*) LSArray<int>::next_permutation)},
		{env.boolean, {Type::array(env.long_)}, ADDR((void*) LSArray<long>::next_permutation)},
	});

	auto parT = env.template_("T");
	template_(parT).
	method("partition", {
		{Type::tmp_array(Type::array(parT)), {Type::const_array(parT), Type::fun(env.any, {parT})}, ADDR(partition)}
	});

	auto fT = env.template_("T");
	template_(fT).
	method("first", {
		{fT, {Type::const_array(fT)}, ADDR(first)}
	});

	auto lT = env.template_("T");
	template_(lT).
	method("last", {
		{lT, {Type::const_array(lT)}, ADDR(last)}
	});

	auto flT = env.template_("T");
	auto flR = env.template_("R");
	template_(flT, flR).
	method("foldLeft", {
		{flR, {Type::const_array(flT), Type::fun(flR, {Type::meta_not_temporary(flR), flT}), flR}, ADDR(fold_left)},
	});

	auto aflA = env.template_("A");
	auto aflR = env.template_("R");
	template_(aflA, aflR).
	method("arrayFoldLeft", {
		{aflR, {Type::meta_base_of(aflA, env.array), Type::fun(aflR, {Type::meta_not_temporary(aflR), Type::meta_element(aflA)}), aflR}, ADDR(fold_left)},
	}, LEGACY_ONLY);


	auto frT = env.template_("T");
	auto frR = env.template_("R");
	template_(frT, frR).
	method("foldRight", {
		{frR, {Type::const_array(frT), Type::fun(frR, {frT, Type::meta_not_temporary(frR)}), frR}, ADDR(fold_right)},
	});

	method("pop", {
		{env.tmp_any, {env.array}, ADDR((void*) LSArray<LSValue*>::ls_pop), THROWS},
		{env.real, {Type::array(env.real)}, ADDR((void*) LSArray<double>::ls_pop), THROWS},
		{env.long_, {Type::array(env.long_)}, ADDR((void*) LSArray<long>::ls_pop), THROWS},
		{env.integer, {Type::array(env.integer)}, ADDR((void*) LSArray<int>::ls_pop), THROWS},
		{env.tmp_any, {env.empty_array}, ADDR((void*) LSArray<LSValue*>::ls_pop), THROWS},
	}, LEGACY);

	method("product", {
		{env.real, {Type::array(env.real)}, ADDR((void*) LSArray<double>::ls_product)},
		{env.long_, {Type::array(env.long_)}, ADDR((void*) LSArray<long>::ls_product)},
		{env.integer, {Type::array(env.integer)}, ADDR((void*) LSArray<int>::ls_product)},
	});

	auto pA = env.template_("A");
	auto pE = env.template_("E");
	template_(pA, pE).
	method("push", {
		{ Type::meta_push(pA, pE), { Type::meta_base_of(pA, env.array), pE }, ADDR(array_add_eq), 0, { env.convert_mutator }, true },

		// {Type::array(env.any), {env.array, env.const_any}, ADDR((void*) LSArray<LSValue*>::ls_push), LEGACY, { env.change_value_mutator }},
		// {Type::meta_concat(pT, Type::meta_not_temporary(pE)), {Type::meta_base_of(pT, env.array), pE}, ADDR(push), LEGACY, { env.change_value_mutator }},
	}, LEGACY);

	auto paX = env.template_("X");
	auto paY = env.template_("Y");
	template_(paX, paY).
	method("pushAll", {
		{Type::meta_add(paX, paY), {Type::meta_base_of(paX, env.array), Type::meta_base_of(paY, env.array)}, ADDR(push_all), LEGACY, { env.change_value_mutator }}
	});

	method("join", {
		{env.string, {env.const_array}, ADDR((void*) LSArray<LSValue*>::ls_join)},
		{env.string, {Type::const_array(env.real)}, ADDR((void*) LSArray<double>::ls_join)},
		{env.string, {Type::const_array(env.integer)}, ADDR((void*) LSArray<int>::ls_join)},
		{env.string, {env.const_empty_array}, ADDR((void*) LSArray<LSValue*>::ls_join)},
		{env.string, {env.const_array, env.const_string}, ADDR((void*) LSArray<LSValue*>::ls_join_glue)},
		{env.string, {Type::const_array(env.real), env.const_string}, ADDR((void*) LSArray<double>::ls_join_glue)},
		{env.string, {Type::const_array(env.integer), env.const_string}, ADDR((void*) LSArray<int>::ls_join_glue)},
		{env.string, {env.const_empty_array, env.const_string}, ADDR((void*) LSArray<LSValue*>::ls_join_glue)},
	}, LEGACY);

	method("json", {
		{env.string, {env.array}, ADDR((void*) LSValue::std_json)},
	});

	auto T = env.template_("T");
	template_(T).
	method("fill", {
		{Type::array(T), {env.array, T}, ADDR(fill), 0, { env.change_value_mutator }},
		{Type::array(T), {env.array, T, env.const_integer}, ADDR(fill), 0, { env.change_value_mutator }},
		{Type::tmp_array(T), {env.tmp_array, T}, ADDR(fill), 0, { env.change_value_mutator }},
		{Type::tmp_array(T), {env.tmp_array, T, env.const_integer}, ADDR(fill), 0, { env.change_value_mutator }},
	}, LEGACY);

	method("insert", {
		{Type::array(env.any), {env.array, env.any, env.integer}, ADDR((void*) LSArray<LSValue*>::ls_insert)},
		{Type::array(env.real), {Type::array(env.real), env.real, env.integer}, ADDR((void*) LSArray<double>::ls_insert)},
		{Type::array(env.integer), {Type::array(env.integer), env.integer, env.integer}, ADDR((void*) LSArray<int>::ls_insert)},
	}, LEGACY);

	method("random", {
		{env.tmp_array, {env.const_array, env.const_integer}, ADDR((void*) LSArray<LSValue*>::ls_random)},
		{Type::tmp_array(env.real), {Type::const_array(env.real), env.const_integer}, ADDR((void*) LSArray<double>::ls_random)},
		{Type::tmp_array(env.integer), {Type::const_array(env.integer), env.const_integer}, ADDR((void*) LSArray<int>::ls_random)},
	});

	method("remove", {
		{env.any, {env.array, env.integer}, ADDR((void*) LSArray<LSValue*>::ls_remove)},
		{env.real, {Type::array(env.real), env.integer}, ADDR((void*) LSArray<double>::ls_remove)},
		{env.integer, {Type::array(env.integer), env.integer}, ADDR((void*) LSArray<int>::ls_remove)},
	});

	method("removeElement", {
		{env.boolean, {env.array, env.const_any}, ADDR(remove_element_any)},
		{env.boolean, {Type::array(env.real), env.const_any}, ADDR(remove_element_real)},
		{env.boolean, {Type::array(env.real), env.const_real}, ADDR(remove_element_real)},
		{env.boolean, {Type::array(env.integer), env.const_any}, ADDR(remove_element_int)},
		{env.boolean, {Type::array(env.integer), env.integer}, ADDR(remove_element_int)},
		{env.boolean, {env.empty_array, env.const_any}, ADDR(remove_element_any)},
		{env.boolean, {env.empty_array, env.integer}, ADDR(remove_element_any)},
	}, LEGACY);

	auto rT = env.template_("T");
	template_(rT).
	method("reverse", {
		{env.tmp_array, {env.const_array}, ADDR((void*) LSArray<LSValue*>::ls_reverse)},
		{Type::tmp_array(rT), {Type::const_array(rT)}, ADDR(reverse)},
	}, LEGACY);

	method("shuffle", {
		{Type::tmp_array(env.any), {env.array}, ADDR((void*) LSArray<LSValue*>::ls_shuffle)},
		{Type::tmp_array(env.real), {Type::array(env.real)}, ADDR((void*) LSArray<double>::ls_shuffle)},
		{Type::tmp_array(env.integer), {Type::array(env.integer)}, ADDR((void*) LSArray<int>::ls_shuffle)},
	});

	method("search", {
		{env.integer, {env.const_array, env.const_any, env.const_integer}, ADDR((void*) LSArray<LSValue*>::ls_search)},
		{env.integer, {Type::const_array(env.real), env.const_real, env.const_integer}, ADDR((void*) LSArray<double>::ls_search)},
		{env.integer, {Type::const_array(env.integer), env.const_integer, env.const_integer}, ADDR((void*) LSArray<int>::ls_search)},
	});

	method("size", {
		{env.any, {env.const_any}, ADDR((void*) LSArray<LSValue*>::ls_size_ptr)},
		{env.integer, {env.const_any}, ADDR(size)},
		{env.integer, {Type::const_array(env.real)}, ADDR(size)},
		{env.integer, {Type::const_array(env.integer)}, ADDR(size)}
	});

	method("shift", {
		{env.any, { env.const_array }, ADDR(nullptr)},
	}, LEGACY);

	auto sumA = env.template_("A");
	template_(sumA).
	method("sum", {
		{ Type::meta_element(sumA), { Type::meta_base_of(sumA, env.array) }, ADDR(sum)}
	}, LEGACY);

	auto saT = env.template_("T");
	template_(saT).
	method("subArray", {
		{Type::tmp_array(saT), {Type::const_array(saT), env.const_integer, env.const_integer}, ADDR((void*) sub)},
	}, LEGACY);

	/** Legacy-only **/
	method("count", {
		{env.integer, {env.any}, ADDR((void*) LSArray<LSValue*>::ls_size)}
	}, LEGACY_ONLY);

	auto iaA = env.template_("A");
	template_(iaA).
	method("inArray", {
		// {env.boolean, {Type::array(iaT), env.any}, ADDR((void*) LSArray<LSValue*>::ls_contains)},
		// {env.boolean, {Type::array(env.real), env.real}, ADDR((void*) LSArray<double>::ls_contains)},
		// {env.boolean, {Type::array(env.integer), env.integer}, ADDR((void*) LSArray<int>::ls_contains)}

		{ Type::meta_base_of(iaA, env.array), env.const_any, env.boolean, ADDR(in), THROWS},
	}, LEGACY_ONLY);

	/** Internal **/
	method("vpush", {
		{env.void_, {Type::array(env.boolean), env.boolean}, ADDR((void*) LSArray<char>::ls_push)},
		{env.void_, {Type::array(env.integer), env.integer}, ADDR((void*) LSArray<int>::ls_push)},
		{env.void_, {Type::array(env.long_), env.long_}, ADDR((void*) LSArray<long>::ls_push)},
		{env.void_, {Type::array(env.real), env.real}, ADDR((void*) LSArray<double>::ls_push)},
		{env.void_, {Type::array(env.any), env.any}, ADDR((void*) LSArray<LSValue*>::std_push_inc)},
	}, PRIVATE | LEGACY);

	method("convert_key", {
		{env.integer, {env.const_any}, ADDR((void*) convert_key)}
	}, PRIVATE | LEGACY);

	method("in", {
		{env.boolean, {Type::const_array(env.any), env.const_any}, ADDR((void*) LSArray<LSValue*>::std_in)},
		{env.boolean, {Type::const_array(env.real), env.const_any}, ADDR((void*) LSArray<double>::std_in)},
		{env.boolean, {Type::const_array(env.integer), env.integer}, ADDR((void*) LSArray<int>::std_in_i)},
	}, PRIVATE | LEGACY);

	method("isize", {
		{env.integer, {Type::array(env.any)}, ADDR((void*) LSArray<LSValue*>::int_size)},
		{env.integer, {Type::array(env.real)}, ADDR((void*) LSArray<double>::int_size)},
		{env.integer, {Type::array(env.integer)}, ADDR((void*) LSArray<int>::int_size)},
	}, PRIVATE | LEGACY);

	method("to_bool", {
		{env.boolean, {Type::array(env.any)}, ADDR((void*) LSArray<LSValue*>::std_to_bool)},
		{env.boolean, {Type::array(env.real)}, ADDR((void*) LSArray<double>::std_to_bool)},
		{env.boolean, {Type::array(env.integer)}, ADDR((void*) LSArray<int>::std_to_bool)},
	}, PRIVATE | LEGACY);

	auto sort_fun_int = ADDR(&LSArray<int>::ls_sort_fun<LSFunction*>);
	auto sort_fun_long = ADDR(&LSArray<long>::ls_sort_fun<LSFunction*>);
	auto sort_fun_real = ADDR(&LSArray<double>::ls_sort_fun<LSFunction*>);
	auto sort_fun_any = ADDR(&LSArray<LSValue*>::ls_sort_fun<LSFunction*>);
	method("sort_fun", {
		{env.array, {env.array, (const Type*) Type::fun_object(env.void_, {})}, (void*) sort_fun_any},
		{env.array, {env.array, (const Type*) Type::fun_object(env.void_, {})}, (void*) sort_fun_real},
		{env.array, {env.array, (const Type*) Type::fun_object(env.void_, {})}, (void*) sort_fun_long},
		{env.array, {env.array, (const Type*) Type::fun_object(env.void_, {})}, (void*) sort_fun_int},
	}, PRIVATE | LEGACY);

	method("fill_fun", {
		{env.array, {env.array, env.any, env.integer}, ADDR((void*) LSArray<LSValue*>::ls_fill)},
		{env.array, {env.array, env.real, env.integer}, ADDR((void*) LSArray<double>::ls_fill)},
		{env.array, {env.array, env.integer, env.integer}, ADDR((void*) LSArray<int>::ls_fill)},
		{env.array, {env.array, env.boolean, env.integer}, ADDR((void*) LSArray<char>::ls_fill)},
	}, PRIVATE | LEGACY);

	method("remove_element_fun", {
		{env.boolean, {env.array, env.any}, ADDR((void*) LSArray<LSValue*>::ls_remove_element)},
		{env.boolean, {env.array, env.any}, ADDR((void*) LSArray<double>::ls_remove_element)},
		{env.boolean, {env.array, env.any}, ADDR((void*) LSArray<int>::ls_remove_element)},
	}, PRIVATE | LEGACY);

	method("int_to_any", {
		{Type::array(env.any), {Type::array(env.integer)}, ADDR((void*) LSArray<int>::to_any_array)}
	}, PRIVATE | LEGACY);

	method("real_to_any", {
		{Type::array(env.any), {Type::array(env.real)}, ADDR((void*) LSArray<double>::to_any_array)}
	}, PRIVATE | LEGACY);

	method("int_to_real", {
		{Type::array(env.any), {Type::array(env.real)}, ADDR((void*) LSArray<int>::to_real_array)}
	}, PRIVATE | LEGACY);

	method("int_to_long", {
		{Type::array(env.any), {Type::array(env.long_)}, ADDR((void*) LSArray<int>::to_long_array)}
	}, PRIVATE | LEGACY);

	method("push_all_fun", {
		{Type::array(env.any), {Type::array(env.any), Type::array(env.any)}, ADDR((void*) LSArray<LSValue*>::ls_push_all_ptr)},
		{Type::array(env.any), {Type::array(env.any), Type::array(env.real)}, ADDR((void*) LSArray<LSValue*>::ls_push_all_flo)},
		{Type::array(env.any), {Type::array(env.any), Type::array(env.integer)}, ADDR((void*) LSArray<LSValue*>::ls_push_all_int)},
		{Type::array(env.real), {Type::array(env.real), Type::array(env.real)}, ADDR((void*) LSArray<double>::ls_push_all_flo)},
		{Type::array(env.real), {Type::array(env.real), Type::array(env.integer)}, ADDR((void*) LSArray<double>::ls_push_all_int)},
		{Type::array(env.integer), {Type::array(env.integer), Type::array(env.integer)}, ADDR((void*) LSArray<int>::ls_push_all_int)},
	}, PRIVATE | LEGACY);

	method("repeat_fun", {
		{Type::tmp_array(env.any), {Type::const_array(env.any), env.integer}, ADDR((void*) LSArray<LSValue*>::repeat)},
		{Type::tmp_array(env.real), {Type::const_array(env.real), env.integer}, ADDR((void*) LSArray<double>::repeat)},
		{Type::tmp_array(env.long_), {Type::const_array(env.long_), env.integer}, ADDR((void*) LSArray<long>::repeat)},
		{Type::tmp_array(env.integer), {Type::const_array(env.integer), env.integer}, ADDR((void*) LSArray<int>::repeat)},
	}, PRIVATE | LEGACY);

	method("reverse_fun", {
		{env.tmp_array, {env.const_array}, ADDR((void*) LSArray<LSValue*>::ls_reverse)},
		{Type::tmp_array(env.real), {Type::const_array(env.real)}, ADDR((void*) LSArray<double>::ls_reverse)},
		{Type::tmp_array(env.long_), {Type::const_array(env.long_)}, ADDR((void*) LSArray<long>::ls_reverse)},
		{Type::tmp_array(env.integer), {Type::const_array(env.integer)}, ADDR((void*) LSArray<int>::ls_reverse)},
	}, PRIVATE | LEGACY);

	method("clear_fun", {
		{ env.void_, { env.const_array }, ADDR((void*) LSArray<int>::ls_clear)}
	}, PRIVATE | LEGACY);
}

#if COMPILER

Compiler::value ArraySTD::new_(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.new_fixed_array(std::vector<Compiler::value> {});
}

Compiler::value ArraySTD::eq(Compiler& c, std::vector<Compiler::value> args, int) {
	auto array = args[0];
	auto value = args[1];

	if (array.t->is_fixed_array() and value.t->is_fixed_array()) {

		if (array.t->size() != value.t->size()) {
			c.insn_delete_temporary(args[0]);
			c.insn_delete_temporary(args[1]);
			return c.new_bool(false);
		}

		auto r = c.create_and_call_std_method("fixed_eq", args, c.env.boolean, fixed_eq);
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return r;
	}

	auto bp = c.insn_to_any(value);
	auto r = c.insn_call(c.env.boolean, { array, bp }, "Value.operator==");
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}

void ArraySTD::fixed_eq(Compiler& c, std::vector<Compiler::value> args) {
	const auto& array1 = args[0];
	const auto& array2 = args[1];

	for (size_t i = 0; i < array1.t->size(); ++i) {
		auto element1 = c.insn_fixed_array_at(array1, i);
		auto element2 = c.insn_fixed_array_at(array2, i);
		c.insn_if_not_return(c.insn_eq(element1, element2), [&]() {
			c.insn_return(c.new_bool(false));
		});
	}
	c.insn_return(c.new_bool(true));
}

Compiler::value ArraySTD::lt(Compiler& c, std::vector<Compiler::value> args, int) {
	auto array = args[0];
	auto value = args[1];

	if (array.t->is_fixed_array() and value.t->is_fixed_array()) {

		auto r = c.create_and_call_std_method("fixed_lt", args, c.env.boolean, fixed_lt);
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return r;
	}

	auto bp = c.insn_to_any(value);
	auto r = c.insn_call(c.env.boolean, { array, bp }, "Value.operator==");
	c.insn_delete_temporary(args[0]);
	c.insn_delete_temporary(args[1]);
	return r;
}

void ArraySTD::fixed_lt(Compiler& c, std::vector<Compiler::value> args) {
	const auto& array1 = args[0];
	const auto& array2 = args[1];

	for (size_t i = 0; i < array1.t->size(); ++i) {
		if (i == array2.t->size()) {
			c.insn_return(c.new_bool(false));
			return;
		}
		auto element1 = c.insn_fixed_array_at(array1, i);
		auto element2 = c.insn_fixed_array_at(array2, i);
		c.insn_if_return(c.insn_lt(element1, element2), [&]() {
			c.insn_return(c.new_bool(true));
		});
		c.insn_if_return(c.insn_lt(element2, element1), [&]() {
			c.insn_return(c.new_bool(false));
		});
	}
	c.insn_return(c.new_bool(true));
}

Compiler::value ArraySTD::in(Compiler& c, std::vector<Compiler::value> args, int) {
	auto array = args[0];
	const auto& type = args[0].t->element()->fold();
	auto f = [&]() {
		if (type->is_integer()) return "Array.in.2";
		if (type->is_real()) return "Array.in.1";
		return "Array.in";
	}();
	if (args[1].t->castable(type)) {

		if (array.t->is_fixed_array_or_ptr()) {
			auto r = c.create_and_call_std_method("fixed_in", args, c.env.boolean, fixed_in);
			c.insn_delete_temporary(args[0]);
			c.insn_delete_temporary(args[1]);
			return r;
		}

		auto v = c.insn_convert(args[1], type);
		auto r = c.insn_call(c.env.boolean, {args[0], v}, f);
		if (args[1].t->is_polymorphic() and type->is_primitive()) {
			c.insn_delete_temporary(args[1]);
		}
		return r;
	} else {
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return c.new_bool(false);
	}
}

/**
 * Fixed array operator in function
 *
 * ```
 * function fixed_in(array: [int, string, int], element: int) {
 *    if (array[0] == element) return true
 *    if (array[2] == element) return true
 *    return false
 * }
 * function fixed_in(array: [int, string, int], element: string) {
 *    if (array[1] == element) return true
 *    return false
 * }
 * ```
 */
void ArraySTD::fixed_in(Compiler& c, std::vector<Compiler::value> args) {
	const auto& array = args[0];
	const auto& needle = args[1];
	const auto needle_type = needle.t->not_temporary();

	for (size_t i = 0; i < array.t->size(); ++i) {
		if (array.t->element(i)->castable(needle_type)) {
			auto element = c.insn_fixed_array_at(array, i);
			c.insn_if_return(c.insn_eq(element, needle), [&]() {
				c.insn_return(c.new_bool(true));
			});
		}
	}
	c.insn_return(c.new_bool(false));
}

Compiler::value ArraySTD::op_add(Compiler& c, std::vector<Compiler::value> args, int) {
	// std::cout << "ArraySTD op_add " << args << std::endl;
	if (args[0].t->is_fixed_array_or_ptr()) {
		auto values = c.insn_get_fixed_array_values(args[0]);
		if (args[1].t->is_fixed_array_or_ptr()) {
			auto values2 = c.insn_get_fixed_array_values(args[1]);
			values.insert(values.end(), values2.begin(), values2.end());
		} else {
			values.push_back(args[1]);
		}
		return c.new_fixed_array(values);
	}
	// if (args[0].t->element() == c.env.never) {
	// 	c.insn_delete_temporary(args[0]);
	// 	return c.new_array(args[1].t->add_temporary(), { args[1] });
	// }
	return c.insn_call(Type::tmp_array(args[0].t->element()), {args[0], c.insn_to_any(args[1])}, "Value.operator+");
}

Compiler::value ArraySTD::array_add_eq(Compiler& c, std::vector<Compiler::value> args, int flags) {

	// std::cout << "ArraySTD add_eq " << args << std::endl;

	if (args[0].t->is_fixed_array_or_ptr()) {
		// auto values = c.insn_get_fixed_array_values(args[0]);
		if (args[1].t->is_fixed_array_or_ptr()) {
			auto values2 = c.insn_get_fixed_array_values(args[1]);
			// values.insert(values.end(), values2.begin(), values2.end());
			auto start = args[0].t->size() - values2.size();
			for (size_t i = 0; i < values2.size(); ++i) {
				c.insn_store(c.insn_fixed_array_at_addr(args[0], start + i), values2[i]);
			}
		} else {
			auto end = args[0].t->size() - 1;
			// std::cout << "ArraySTD += set element " << end << " " << args[1].t << std::endl;
			c.comment("Array += increment refs of element " + args[1].t->getName());
			c.insn_inc_refs(args[1]);
			c.insn_store(c.insn_fixed_array_at_addr(args[0], end), args[1]);
		}
		return args[0];
	}

	if (args[1].t->is_set()) {
		args[1] = c.insn_to_any(args[1]);
		return c.insn_call(c.env.any, args, "Value.operator+=");
	}
	if (args[0].t->pointed()->is_array()) {
		args[0] = c.insn_load(args[0]);
	}
	auto fun = [&]() {
		const auto& type = args[0].t->element()->fold();
		// std::cout << "push array type = " << type << std::endl;
		if (type->is_bool()) return "Array.vpush";
		if (type->is_integer()) return "Array.vpush.1";
		if (type->is_long()) return "Array.vpush.2";
		if (type->is_real()) {
			args[1] = c.insn_convert(args[1], c.env.real);
			return "Array.vpush.3";
		}
		args[1] = c.insn_convert(args[1], c.env.any);
		return "Array.vpush.4";
	}();
	c.insn_call(c.env.void_, args, fun);
	if (flags & NO_RETURN) return { c.env };
	else return args[0];
}

Compiler::value ArraySTD::average(Compiler& c, std::vector<Compiler::value> args, int) {
	const auto array = args[0];

	if (array.t->is_fixed_array_or_ptr()) {

		if (array.t->size() == 0) {
			c.insn_delete_temporary(array);
			return c.new_real(0);
		}
		auto r = c.create_and_call_std_method("fixed_average", args, c.env.real, fixed_average, true);
		c.insn_delete_temporary(array);
		return r;
	} else {

	}
}

void ArraySTD::fixed_average(Compiler& c, std::vector<Compiler::value> args) {
	const auto array = args[0];
	auto sum = c.create_and_call_std_method("fixed_sum", args, array.t->element(), fixed_sum);
	auto r = c.insn_div(sum, c.new_real(array.t->size()), true);
	c.insn_return(r);
}

void ArraySTD::vector_average(Compiler& c, std::vector<Compiler::value> args) {
	const auto array = args[0];

	const auto average = c.create_entry("average", c.env.real);
	c.insn_store(average, c.new_integer(0));

	auto v = Variable::new_temporary("v", args[0].t->element(), *c.fun->parent);
	c.insn_foreach(array, c.env.void_, &v, nullptr, [&](Compiler::value value, Compiler::value key) -> Compiler::value {
		c.insn_store(average, c.insn_add(c.insn_load(average), value));
		return { c.env };
	});

	auto r = c.insn_div(c.insn_load(average), c.insn_array_size(array), true);
	c.insn_return(r);
}

Compiler::value ArraySTD::size(Compiler& c, std::vector<Compiler::value> args, int) {

	if (args[0].t->is_fixed_array_or_ptr()) {
		auto r = c.new_integer(args[0].t->size());
		c.insn_delete_temporary(args[0]);
		return r;
	}

	auto res = c.insn_array_size(args[0]);
	c.insn_delete_temporary(args[0]);
	return res;
}

LSArray<LSValue*>* ArraySTD::chunk_1_ptr(LSArray<LSValue*>* array) {
	return LSArray<LSValue*>::ls_chunk(array, 1);
}
LSArray<LSValue*>* ArraySTD::chunk_1_int(LSArray<int>* array) {
	return LSArray<int>::ls_chunk(array, 1);
}
LSArray<LSValue*>* ArraySTD::chunk_1_long(LSArray<long>* array) {
	return LSArray<long>::ls_chunk(array, 1);
}
LSArray<LSValue*>* ArraySTD::chunk_1_float(LSArray<double>* array) {
	return LSArray<double>::ls_chunk(array, 1);
}

LSValue* ArraySTD::sub(LSArray<LSValue*>* array, int begin, int end) {
	LSValue* r = array->range(begin, end);
	if (array->refs == 0) delete array;
	return r;
}

Compiler::value ArraySTD::fill(Compiler& c, std::vector<Compiler::value> args, int) {
	auto fun = [&]() {
		if (args[0].t->element()->fold()->is_bool()) return "Array.fill_fun.3";
		if (args[0].t->element()->fold()->is_integer()) return "Array.fill_fun.2";
		if (args[0].t->element()->fold()->is_real()) return "Array.fill_fun.1";
		return "Array.fill_fun";
	}();
	auto size = args.size() == 3 ? c.to_int(args[2]) : c.insn_array_size(args[0]);
	return c.insn_call(args[0].t, {args[0], c.insn_convert(args[1], args[0].t->element()->fold()), size}, fun);
}

Compiler::value ArraySTD::fold_left(Compiler& c, std::vector<Compiler::value> args, int) {
	auto array = args[0];
	auto function = args[1];
	auto init_type = function.t->argument(0);
	auto result = Variable::new_temporary("r", init_type, *c.fun->parent);
	result.create_entry(c);
	// c.add_temporary_variable(&result);
	c.insn_store(result.entry, c.insn_convert(c.insn_move_inc(args[2]), init_type));
	auto v = Variable::new_temporary("v", args[0].t->element(), *c.fun->parent);
	c.insn_foreach(array, c.env.void_, &v, nullptr, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		auto r = c.insn_call(function, { c.insn_load(result.entry), v });
		c.insn_delete(c.insn_load(result.entry));
		c.insn_store(result.entry, c.insn_move_inc(r));
		return { c.env };
	});
	auto r = c.insn_load(result.entry);
	c.insn_dec_refs(r);
	return r;
}

Compiler::value ArraySTD::fold_right(Compiler& c, std::vector<Compiler::value> args, int) {
	auto function = args[1];
	auto result = Variable::new_temporary("r", args[2].t, *c.fun->parent);
	result.create_entry(c);
	// c.add_temporary_variable(&result);
	c.insn_store(result.entry, c.insn_move(args[2]));
	auto v = Variable::new_temporary("v", args[0].t->element(), *c.fun->parent);
	c.insn_foreach(args[0], c.env.void_, &v, nullptr, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		c.insn_store(result.entry, c.insn_call(function, {v, c.insn_load(result.entry)}));
		return { c.env };
	}, true);
	return c.insn_load(result.entry);
}

Compiler::value ArraySTD::iter(Compiler& c, std::vector<Compiler::value> args, int) {
	auto function = args[1];
	auto v = Variable::new_temporary("v", args[0].t->element(), *c.fun->parent);
	v.create_entry(c);
	// c.add_temporary_variable(&v);
	c.insn_foreach(args[0], c.env.void_, &v, nullptr, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		return c.insn_call(function, {v});
	});
	return { c.env };
}

Compiler::value ArraySTD::remove_element_any(Compiler& c, std::vector<Compiler::value> args, int) {
	return c.insn_call(c.env.boolean, {args[0], c.insn_to_any(args[1])}, "Array.remove_element_fun");
}
Compiler::value ArraySTD::remove_element_real(Compiler& c, std::vector<Compiler::value> args, int) {
	if (args[1].t->castable(c.env.real)) {
		return c.insn_call(c.env.boolean, {args[0], c.to_real(args[1])}, "Array.remove_element_fun.1");
	} else {
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return c.new_bool(false);
	}
}
Compiler::value ArraySTD::remove_element_int(Compiler& c, std::vector<Compiler::value> args, int) {
	if (args[1].t->castable(c.env.integer)) {
		return c.insn_call(c.env.boolean, {args[0], c.to_int(args[1])}, "Array.remove_element_fun.2");
	} else {
		c.insn_delete_temporary(args[0]);
		c.insn_delete_temporary(args[1]);
		return c.new_bool(false);
	}
}

Compiler::value ArraySTD::partition(Compiler& c, std::vector<Compiler::value> args, int) {
	auto array = args[0];
	auto function = args[1];
	auto array_true = c.new_array(array.t->element(), {});
	auto array_false = c.new_array(array.t->element(), {});
	auto v = Variable::new_temporary("v", array.t->element(), *c.fun->parent);
	v.create_entry(c);
	c.insn_foreach(array, c.env.void_, &v, nullptr, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		auto r = c.insn_call(function, {v});
		c.insn_if(r, [&]() {
			c.insn_push_array(array_true, v);
		}, [&]() {
			c.insn_push_array(array_false, v);
		});
		c.insn_delete_temporary(r);
		return { c.env };
	});
	return c.new_array(Type::array(array.t->element()), {array_true, array_false});
}

Compiler::value ArraySTD::map(Compiler& c, std::vector<Compiler::value> args, int flags) {
	auto array = args[0];
	auto function = args[1];

	if (array.t->is_fixed_array_or_ptr()) {
		// std::cout << "STD::map " << array.t << " " << function.t << std::endl;
		// std::cout << array.t->op_map(function.t) << std::endl;
		return c.create_and_call_std_method("map", args, array.t->op_map(function.t), fixed_map, false);
	}

	auto result_type = array.t->op_map(function.t);

	std::cout << array.t << " result type " << result_type << std::endl;

	auto result = flags & NO_RETURN ? Compiler::value { c.env } : c.new_array(result_type->element(), {});
	auto v = Variable::new_temporary("v", array.t->element(), *c.fun->parent);
	// std::cout << "map " << array.t->element() << " " << result_type << " " << result.t << std::endl;

	c.insn_foreach(array, c.env.void_, &v, nullptr, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		auto x = c.clone(v);
		c.insn_inc_refs(x);

		const auto version = ((Function*) args[1].t->function())->versions.at({ array.t->element() }).get();
		version->compile(c);

		auto r = c.insn_call(version->fun, {x});
		if (flags & NO_RETURN) {
			if (not r.t->is_void()) c.insn_delete_temporary(r);
		} else {
			c.insn_push_array(result, r.t->is_void() ? c.new_null() : r);
		}
		c.insn_delete(x);
		return { c.env };
	});
	return flags & NO_RETURN ? Compiler::value { c.env } : result;
}

void ArraySTD::fixed_map(Compiler& c, std::vector<Compiler::value> args) {

	const auto array = args[0];
	std::vector<Compiler::value> elements;

	for (size_t i = 0; i < array.t->size(); ++i) {
		// std::cout << array.t->element(i) << std::endl;
		auto element = c.insn_fixed_array_at(array, i);
		const auto version = ((Function*) args[1].t->function())->versions.at({ element.t }).get();
		version->compile(c);
		const auto fun = version->fun;
		// std::cout << "fun " << fun.t << std::endl;
		elements.push_back(c.insn_call(fun, { element }));
	}
	c.insn_return(c.new_fixed_array(elements));
}


Compiler::value ArraySTD::min_fun(Compiler& c, std::vector<Compiler::value> args, int flags) {
	auto array = args[0];
	auto function = args[1];
	auto v = Variable::new_temporary("v", array.t->element(), *c.fun->parent);
	v.create_entry(c);
	auto min_v = Variable::new_temporary("min_v", array.t->element(), *c.fun->parent);
	min_v.create_entry(c);
	auto min = Variable::new_temporary("min", c.env.integer, *c.fun->parent);
	min.create_entry(c);
	min.store_value(c, c.new_integer(std::numeric_limits<int>::max()));
	c.insn_foreach(array, c.env.void_, &v, nullptr, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
		auto x = c.clone(v);
		c.insn_inc_refs(x);
		auto r = c.insn_call(function, {x});
		// c.insn_call(c.env.void_, {r}, "System.print.5");
		c.insn_if(c.insn_lt(r, min.get_value(c)), [&]() {
			min.store_value(c, r);
			min_v.store_value(c, c.clone(v));
		});
		c.insn_delete(x);
		return { c.env };
	});
	return min_v.get_value(c);
}

Compiler::value ArraySTD::first(Compiler& c, std::vector<Compiler::value> args, int) {
	auto array = args[0];
	auto array_size = c.insn_array_size(array);
	c.insn_if(c.insn_ge(c.new_integer(0), array_size), [&]() {
		c.insn_delete_temporary(array);
		c.insn_throw_object(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	});
	auto e = c.insn_move(c.insn_load(c.insn_array_at(array, c.new_integer(0))));
	c.insn_delete_temporary(array);
	return e;
}
Compiler::value ArraySTD::last(Compiler& c, std::vector<Compiler::value> args, int) {
	auto array = args[0];
	auto array_size = c.insn_array_size(array);
	c.insn_if(c.insn_ge(c.new_integer(0), array_size), [&]() {
		c.insn_delete_temporary(array);
		c.insn_throw_object(vm::Exception::ARRAY_OUT_OF_BOUNDS);
	});
	auto k = c.insn_sub(array_size, c.new_integer(1));
	auto e = c.insn_move(c.insn_load(c.insn_array_at(array, k)));
	c.insn_delete_temporary(array);
	return e;
}

Compiler::value ArraySTD::sort(Compiler& c, std::vector<Compiler::value> args, int) {
	const auto& array = args[0];
	const auto& fun = args[1];
	auto f = [&]() {
		if (args[0].t->element()->fold()->is_integer()) {
			return "Array.sort_fun.3";
		} else if (args[0].t->element()->fold()->is_long()) {
			return "Array.sort_fun.2";
		} else if (args[0].t->element()->fold()->is_real()) {
			return "Array.sort_fun.1";
		} else {
			return "Array.sort_fun";
		}
	}();
	return c.insn_call(array.t, {array, fun}, f);
}

Compiler::value ArraySTD::reverse(Compiler& c, std::vector<Compiler::value> args, int flags) {
	const auto& array = args[0];
	auto f = [&]() {
		if (args[0].t->element()->fold()->is_integer()) {
			return "Array.reverse_fun.3";
		} else if (args[0].t->element()->fold()->is_long()) {
			return "Array.reverse_fun.2";
		} else if (args[0].t->element()->fold()->is_real()) {
			return "Array.reverse_fun.1";
		} else {
			return "Array.reverse_fun";
		}
	}();
	return c.insn_call(array.t, {array}, f);
}

Compiler::value ArraySTD::repeat(Compiler& c, std::vector<Compiler::value> args, int) {
	auto fun = [&]() {
		if (args[0].t->element()->fold()->is_integer()) return "Array.repeat_fun.3";
		if (args[0].t->element()->fold()->is_long()) return "Array.repeat_fun.2";
		if (args[0].t->element()->fold()->is_real()) return "Array.repeat_fun.1";
		return "Array.repeat_fun";
	}();
	return c.insn_call(args[0].t->add_temporary(), args, fun);
}

Compiler::value ArraySTD::layer(Compiler& c, std::vector<Compiler::value> args, int) {
	auto array1 = args[0];
	auto array2 = args[1];
	auto filter = args[2];
	c.insn_inc_refs(filter);
	auto result = c.new_array(args[0].t->element()->operator | (args[1].t->element()), {});
	auto v = Variable::new_temporary("v", args[0].t->element(), *c.fun->parent);
	v.create_entry(c);
	auto i = Variable::new_temporary("i", args[0].t->key(), *c.fun->parent);
	i.create_entry(c);
	c.insn_foreach(args[0], c.env.void_, &v, &i, [&](Compiler::value v, Compiler::value i) -> Compiler::value {
		auto x = c.clone(v);
		c.insn_inc_refs(x);
		c.insn_if(c.insn_eq(c.clone(x), c.clone(filter)), [&]() {
			auto x2 = c.clone(c.insn_load(c.insn_array_at(array2, i)));
			c.insn_push_array(result, x2);
		}, [&]() {
			auto x2 = c.clone(x);
			c.insn_push_array(result, x2);
		});
		c.insn_delete(x);
		return { c.env };
	});
	c.insn_delete_temporary(args[1]);
	c.insn_delete(filter);
	return result;
}

Compiler::value ArraySTD::push(Compiler& c, std::vector<Compiler::value> args, int flags) {
	auto fun = [&]() {
		if (args[0].t->element()->fold()->is_bool()) return "Array.vpush";
		if (args[0].t->element()->fold()->is_integer()) return "Array.vpush.1";
		if (args[0].t->element()->fold()->is_long()) return "Array.vpush.2";
		if (args[0].t->element()->fold()->is_real()) return "Array.vpush.3";
		auto convert_type = args[1].t->is_function() or args[1].t->is_function_pointer() ? c.env.any : args[0].t->element();
		args[1] = c.insn_convert(args[1], convert_type);
		return "Array.vpush.4";
	}();
	c.insn_call(c.env.void_, args, fun);
	if (flags & NO_RETURN) return { c.env };
	else return args[0];
}

Compiler::value ArraySTD::filter(Compiler& c, std::vector<Compiler::value> args, int) {

	auto array = args[0];

	if (array.t->is_fixed_array_or_ptr()) {

		if (array.t->size() == 0) {
			return array;
		}
		auto r = c.create_and_call_std_method("fixed_filter", args, Type::array(array.t->element()), fixed_filter, true);
		c.insn_delete_temporary(array);
		return r;

	} else {

		auto function = args[1];
		auto result = c.new_array(args[0].t->element(), {});
		auto v = Variable::new_temporary("v", args[0].t->element(), *c.fun->parent);
		v.create_entry(c);
		c.insn_foreach(args[0], c.env.void_, &v, nullptr, [&](Compiler::value v, Compiler::value k) -> Compiler::value {
			auto r = c.insn_call(function, {v});
			c.insn_if(r, [&]() {
				c.insn_push_array(result, c.clone(v));
			});
			return { c.env };
		});
		return result;
	}
}

void ArraySTD::fixed_filter(Compiler& c, std::vector<Compiler::value> args) {

	const auto array = args[0];
	auto result = c.new_array(array.t->element(), {});

	auto function = (Function*) args[1].t->function();
	// std::cout << (void*) function << " " << function->versions.size() << std::endl;
	// for (const auto& version : function->versions) {
	// 	std::cout << "version : " << version.first << std::endl;
	// }

	for (size_t i = 0; i < array.t->size(); ++i) {
		// std::cout << array.t->element(i) << std::endl;

		auto element = c.insn_fixed_array_at(array, i);

		const auto version = function->versions.at({ element.t }).get();
		version->compile(c);
		auto r = c.insn_call(version->fun, { element });

		c.insn_if(r, [&]() {
			auto e = c.insn_move(c.insn_convert(element, array.t->element()));
			c.insn_push_array(result, e);
		});
	}
	c.insn_return(result);
}

Compiler::value ArraySTD::push_all(Compiler& c, std::vector<Compiler::value> args, int) {
	auto& array1 = args[0];
	auto& array2 = args[1];
	if (array2.t->element() == c.env.never) return array1;
	if (array1.t->element() == c.env.never) {
		c.insn_delete_temporary(array1);
		return array2;
	}
	auto fun = [&]() {
		if (array1.t->element()->is_integer() and array2.t->element()->is_integer()) return "Array.push_all_fun.5";
		if (array1.t->element()->is_integer() and array2.t->element()->is_real()) {
			array1 = c.insn_convert(array1, Type::array(c.env.real), true);
			return "Array.push_all_fun.3";
		}
		if (array1.t->element()->is_real() and array2.t->element()->is_integer()) return "Array.push_all_fun.4";
		if (array1.t->element()->is_real() and array2.t->element()->is_real()) return "Array.push_all_fun.3";
		if (array1.t->element()->is_real() and array2.t->element()->is_polymorphic()) {
			array1 = c.insn_convert(array1, Type::array(c.env.any), true);
			return "Array.push_all_fun";
		}
		if (array2.t->element()->is_integer()) return "Array.push_all_fun.2";
		if (array2.t->element()->is_real()) return "Array.push_all_fun.1";
		return "Array.push_all_fun";
	}();
	return c.insn_call(Type::array(array1.t->element()->operator | (array2.t->element())), args, fun);
}

int ArraySTD::convert_key(LSValue* key_pointer) {
	auto n = dynamic_cast<LSNumber*>(key_pointer);
	if (!n) {
		LSValue::delete_temporary(key_pointer);
		throw vm::ExceptionObj(vm::Exception::ARRAY_KEY_IS_NOT_NUMBER);
	}
	int key_int = n->value;
	LSValue::delete_temporary(key_pointer);
	return key_int;
}

Compiler::value ArraySTD::clear(Compiler& c, std::vector<Compiler::value> args, int) {
	return args[0];
}

Compiler::value ArraySTD::sum(Compiler& c, std::vector<Compiler::value> args, int) {
	const auto array = args[0];

	if (array.t->is_fixed_array_or_ptr()) {

		if (array.t->size() == 0) {
			c.insn_delete_temporary(array);
			return c.new_real(0);
		}
		auto r = c.create_and_call_std_method("fixed_sum", args, array.t->element(), fixed_sum, true);
		c.insn_delete_temporary(array);
		return r;
	} else {

	}
}

void ArraySTD::fixed_sum(Compiler& c, std::vector<Compiler::value> args) {

	const auto array = args[0];
	Compiler::value sum { c.env };

	for (size_t i = 0; i < array.t->size(); ++i) {
		// std::cout << array.t->element(i) << std::endl;
		auto element = c.insn_fixed_array_at(array, i);
		sum = sum.t->is_void() ? element : c.insn_add(sum, element);
	}
	c.insn_return(sum);
}

void ArraySTD::vector_sum(Compiler& c, std::vector<Compiler::value> args) {

	const auto sum = c.create_entry("sum", c.env.real);
	c.insn_store(sum, c.new_integer(0));

	auto v = Variable::new_temporary("v", args[0].t->element(), *c.fun->parent);
	c.insn_foreach(args[0], c.env.void_, &v, nullptr, [&](Compiler::value value, Compiler::value key) -> Compiler::value {
		c.insn_store(sum, c.insn_add(c.insn_load(sum), value));
		return { c.env };
	});
	c.insn_return(c.insn_load(sum));
}

#endif

}
