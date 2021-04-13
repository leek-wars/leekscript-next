#ifndef META_PUSH_TYPE_HPP
#define META_PUSH_TYPE_HPP

#include "Type.hpp"

namespace ls {

class Meta_push_type : public Type {
public:
	const Type* t1;
	const Type* t2;
	Meta_push_type(const Type* t1, const Type* t2) : Type(t1->env), t1(t1), t2(t2) {}
	virtual void reset() const override;
	virtual int id() const override { return 0; }
	virtual const std::string getName() const override { return "meta_push"; }
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