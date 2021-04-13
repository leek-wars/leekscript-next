#ifndef META_FILTER_TYPE_HPP
#define META_FILTER_TYPE_HPP

#include "Type.hpp"

namespace ls {

class Meta_filter_type : public Type {
public:
	const Type* array;
	const Type* fun;
	Meta_filter_type(const Type* array, const Type* fun) : Type(array->env), array(array), fun(fun) {}
	virtual void reset() const override;
	virtual int id() const override { return 0; }
	virtual const std::string getName() const override { return "meta_filter"; }
	virtual Json json() const override;
	virtual bool operator == (const Type*) const override;
	virtual int distance(const Type* type) const override;
	#if COMPILER
	virtual llvm::Type* llvm(Compiler& c) const override;
	#endif
	virtual std::string class_name() const override;
	virtual std::ostream& print(std::ostream& os) const override;
	virtual Type* clone() const override;
};

}

#endif