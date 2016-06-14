#include "String.hpp"
#include "../../vm/value/LSString.hpp"

using namespace std;

namespace ls {

String::String(string value) {
	this->value = value;
	type = Type::STRING;
	constant = true;
}

String::~String() {}

void String::print(ostream& os) const {
	os << "'";
	os << value;
	os << "'";
}

void String::analyse(SemanticAnalyser*, const Type) {
	//cout << "analyse string " << this->value << endl;
	//cout << "type : " << type << endl;
}

jit_value_t String::compile_jit(Compiler&, jit_function_t& F, Type) const {

	LSString* s = new LSString(value);
	return JIT_CREATE_CONST_POINTER(F,  s);
}

}
