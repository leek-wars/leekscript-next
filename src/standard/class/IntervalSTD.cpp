#include "IntervalSTD.hpp"
#include "ValueSTD.hpp"
#include "../../type/Type.hpp"
#include "ArraySTD.hpp"
#include "../../environment/Environment.hpp"
#if COMPILER
#include "../../vm/value/LSInterval.hpp"
#include "../../vm/value/LSClosure.hpp"
#endif

namespace ls {

IntervalSTD::IntervalSTD(Environment& env) : Module(env, "Interval") {

	#if COMPILER
	env.interval_class = std::make_unique<LSClass>(clazz.get());
	lsclass = env.interval_class.get();
	#endif

	constructor_({
		{env.tmp_interval, {env.integer, env.integer}, ADDR((void*) &LSInterval::constructor)}
	});

	/*
	 * Operators
	 */
	operator_("in", {
		{env.interval, env.integer, env.boolean, ADDR((void*) LSInterval::std_in_i)}
	});

	auto ttA = env.template_("A");
	auto ttF = env.template_("F");
	template_(ttA, ttF).
	operator_("~~", {
		{ Type::meta_base_of(ttA, env.interval), ttF, Type::meta_map(ttA, ttF), ADDR(ArraySTD::map) },
	});

	/*
	 * Methods
	 */
	auto pred_fun_type_int = Type::fun_object(env.boolean, {env.integer});
	auto pred_clo_type_int = Type::closure(env.boolean, {env.integer});
	auto filter_fun = ADDR((void*) LSInterval::std_filter<LSFunction*>);
	auto filter_clo = ADDR((void*) LSInterval::std_filter<LSClosure*>);
	method("filter", {
		{Type::tmp_array(env.integer), {env.interval, pred_fun_type_int}, filter_fun},
		{Type::tmp_array(env.integer), {env.interval, pred_clo_type_int}, filter_clo}
	});

	auto mapA = env.template_("A");
	auto mapF = env.template_("F");
	template_(mapA, mapF).
	method("map", {
		{ Type::meta_map(mapA, mapF), { Type::meta_base_of(mapA, env.interval), mapF }, ADDR(ArraySTD::map) },
	});

	method("sum", {
		{env.long_, {env.interval}, ADDR((void*) LSInterval::std_sum)},
	});
	method("product", {
		{env.long_, {env.interval}, ADDR((void*) LSInterval::std_product)},
	});

	/** Interval **/
	method("at_i_i", {
		{env.integer, {env.interval, env.integer}, ADDR((void*) LSInterval::std_at_i_i)}
	}, PRIVATE);
}

IntervalSTD::~IntervalSTD() {}

}
