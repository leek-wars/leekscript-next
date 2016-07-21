#ifndef ARRAYACCESS_HPP
#define ARRAYACCESS_HPP

#include "../../compiler/value/LeftValue.hpp"
#include "../../compiler/value/Value.hpp"

namespace ls {

class ArrayAccess : public LeftValue {
public:

	Value* array;
	Value* key;
	Value* key2;

	ArrayAccess();
	virtual ~ArrayAccess();

	virtual void print(std::ostream&) const override;
	virtual int line() const override;

	virtual void analyse(SemanticAnalyser*, const Type) override;

	virtual bool will_take(SemanticAnalyser* analyser, const unsigned, const Type);

	bool array_access_will_take(SemanticAnalyser* analyser, const unsigned, const Type, int level);

	virtual jit_value_t compile_jit(Compiler&, jit_function_t&, Type) const override;

	virtual jit_value_t compile_jit_l(Compiler&, jit_function_t&, Type) const override;
};

}

#endif
