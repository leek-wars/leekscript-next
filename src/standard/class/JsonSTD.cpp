#include "JsonSTD.hpp"
#include <chrono>
#include "../../type/Type.hpp"
#if COMPILER
#include "../../vm/LSValue.hpp"
#include "../../vm/value/LSNull.hpp"
#include "../../vm/value/LSString.hpp"
#include "../../vm/value/LSNumber.hpp"
#endif

namespace ls {

JsonSTD::JsonSTD(StandardLibrary* stdLib) : Module(stdLib, "Json") {

	method("encode", {
		{Type::tmp_string, {Type::const_any}, ADDR(encode)}
	});
	method("decode", {
		{Type::tmp_any, {Type::const_string}, ADDR((void*) decode)},
	});
}

#if COMPILER

Compiler::value JsonSTD::encode(Compiler& c, std::vector<Compiler::value> args, int) {
	if (args[0].t->is_integer()) {
		return c.insn_call(Type::tmp_string, args, "Number.int_to_string");
	} else if (args[0].t->is_long()) {
		return c.insn_call(Type::tmp_string, args, "Number.long_to_string");
	} else if (args[0].t->is_real()) {
		return c.insn_call(Type::tmp_string, args, "Number.real_to_string");
	} else if (args[0].t->is_bool()) {
		return c.insn_call(Type::tmp_string, args, "Boolean.to_string");
	} else if (args[0].t->is_mpz_ptr()) {
		auto s = c.insn_call(Type::tmp_string, args, "Number.mpz_to_string");
		if (args[0].t->temporary) {
			c.insn_delete_mpz(args[0]);
		}
		return s;
	}
	// Default type : pointer
	return c.insn_call(Type::tmp_string, args, "Value.json");
}

LSValue* JsonSTD::decode(LSString* string) {
	try {
		Json json = Json::parse(*string);
		LSValue::delete_temporary(string);
		return LSValue::get_from_json(json);
	} catch (...) {
		LSValue::delete_temporary(string);
		return LSNull::get();
	}
}

#endif

}
