#ifndef LSVALUE_H_
#define LSVALUE_H_

#include <iostream>
#include <string>
#include <cstddef>
#include <map>

#include "../constants.h"
#include "../../lib/json.hpp"
#include "Type.hpp"

namespace ls {

class LSVar;
class LSNull;
class LSBoolean;
class LSNumber;
class LSString;
template <typename T> class LSArray;
template <typename K, typename T> class LSMap;
template <typename T> class LSSet;
template <class T> class LSFunction;
class LSObject;
class LSClass;
class Context;

#define LSVALUE_OPERATORS \
	LSValue* ls_rmul_eq(LSValue* value) override  { return value->ls_mul_eq(this); } \
	LSValue* ls_rint_div(LSValue* value) override { return value->ls_int_div(this); } \
	LSValue* ls_rdiv_eq(LSValue* value) override  { return value->ls_div_eq(this); } \
	LSValue* ls_rpow(LSValue* value) override     { return value->ls_pow(this); } \
	LSValue* ls_rpow_eq(LSValue* value) override  { return value->ls_pow_eq(this); } \
	LSValue* ls_rmod(LSValue* value) override     { return value->ls_mod(this); } \
	LSValue* ls_rmod_eq(LSValue* value) override  { return value->ls_mod_eq(this); } \
	bool req(const LSValue* value) const override { return value->eq(this); } \
	bool rlt(const LSValue* value) const override { return value->lt(this); } \

class LSValue {
public:

	static int obj_count;
	static int obj_deleted;
	#if DEBUG_LEAKS_DETAILS
		static std::map<void*, LSValue*>& objs() {
			static std::map<void*, LSValue*> objs;
			return objs;
		}
	#endif

	int refs = 0;
	bool native = false;

	LSValue();
	LSValue(const LSValue& other);
	virtual ~LSValue() = 0;

	virtual bool isTrue() const = 0;

	virtual LSValue* ls_minus();
	virtual LSValue* ls_not();
	virtual LSValue* ls_tilde();
	virtual LSValue* ls_preinc(); // ++x
	virtual LSValue* ls_inc(); // x++
	virtual LSValue* ls_predec();
	virtual LSValue* ls_dec();

	virtual LSValue* add(LSValue*);
	virtual LSValue* add_eq(LSValue*);
	virtual LSValue* sub(LSValue*);
	virtual LSValue* sub_eq(LSValue*);
	virtual LSValue* mul(LSValue*);
	virtual LSValue* div(LSValue*);

	LSValue* ls_mul_eq(LSValue* value) {
		return value->ls_rmul_eq(this);
	}
	virtual LSValue* ls_rmul_eq(LSValue*) = 0;
	virtual LSValue* ls_mul_eq(LSNull*);
	virtual LSValue* ls_mul_eq(LSBoolean*);
	virtual LSValue* ls_mul_eq(LSNumber*);
	virtual LSValue* ls_mul_eq(LSString*);
	virtual LSValue* ls_mul_eq(LSArray<LSValue*>*);
	virtual LSValue* ls_mul_eq(LSArray<int>*);
	virtual LSValue* ls_mul_eq(LSArray<double>*);
	virtual LSValue* ls_mul_eq(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_mul_eq(LSMap<LSValue*,int>*);
	virtual LSValue* ls_mul_eq(LSMap<LSValue*,double>*);
	virtual LSValue* ls_mul_eq(LSMap<int,LSValue*>*);
	virtual LSValue* ls_mul_eq(LSMap<int,int>*);
	virtual LSValue* ls_mul_eq(LSMap<int,double>*);
	virtual LSValue* ls_mul_eq(LSSet<LSValue*>*);
	virtual LSValue* ls_mul_eq(LSSet<int>*);
	virtual LSValue* ls_mul_eq(LSSet<double>*);
	virtual LSValue* ls_mul_eq(LSObject*);
	virtual LSValue* ls_mul_eq(LSFunction<LSValue*>*);
	virtual LSValue* ls_mul_eq(LSClass*);

	LSValue* ls_int_div(LSValue* value) {
		return value->ls_rint_div(this);
	}
	virtual LSValue* ls_rint_div(LSValue*) = 0;
	virtual LSValue* ls_int_div(LSNull*);
	virtual LSValue* ls_int_div(LSBoolean*);
	virtual LSValue* ls_int_div(LSNumber*);
	virtual LSValue* ls_int_div(LSString*);
	virtual LSValue* ls_int_div(LSArray<LSValue*>*);
	virtual LSValue* ls_int_div(LSArray<int>*);
	virtual LSValue* ls_int_div(LSArray<double>*);
	virtual LSValue* ls_int_div(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_int_div(LSMap<LSValue*,int>*);
	virtual LSValue* ls_int_div(LSMap<LSValue*,double>*);
	virtual LSValue* ls_int_div(LSMap<int,LSValue*>*);
	virtual LSValue* ls_int_div(LSMap<int,int>*);
	virtual LSValue* ls_int_div(LSMap<int,double>*);
	virtual LSValue* ls_int_div(LSSet<LSValue*>*);
	virtual LSValue* ls_int_div(LSSet<int>*);
	virtual LSValue* ls_int_div(LSSet<double>*);
	virtual LSValue* ls_int_div(LSObject*);
	virtual LSValue* ls_int_div(LSFunction<LSValue*>*);
	virtual LSValue* ls_int_div(LSClass*);

	LSValue* ls_div_eq(LSValue* value) {
		return value->ls_rdiv_eq(this);
	}
	virtual LSValue* ls_rdiv_eq(LSValue*) = 0;
	virtual LSValue* ls_div_eq(LSNull*);
	virtual LSValue* ls_div_eq(LSBoolean*);
	virtual LSValue* ls_div_eq(LSNumber*);
	virtual LSValue* ls_div_eq(LSString*);
	virtual LSValue* ls_div_eq(LSArray<LSValue*>*);
	virtual LSValue* ls_div_eq(LSArray<int>*);
	virtual LSValue* ls_div_eq(LSArray<double>*);
	virtual LSValue* ls_div_eq(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_div_eq(LSMap<LSValue*,int>*);
	virtual LSValue* ls_div_eq(LSMap<LSValue*,double>*);
	virtual LSValue* ls_div_eq(LSMap<int,LSValue*>*);
	virtual LSValue* ls_div_eq(LSMap<int,int>*);
	virtual LSValue* ls_div_eq(LSMap<int,double>*);
	virtual LSValue* ls_div_eq(LSSet<LSValue*>*);
	virtual LSValue* ls_div_eq(LSSet<int>*);
	virtual LSValue* ls_div_eq(LSSet<double>*);
	virtual LSValue* ls_div_eq(LSObject*);
	virtual LSValue* ls_div_eq(LSFunction<LSValue*>*);
	virtual LSValue* ls_div_eq(LSClass*);

	LSValue* ls_pow(LSValue* value) {
		return value->ls_rpow(this);
	}
	virtual LSValue* ls_rpow(LSValue*) = 0;
	virtual LSValue* ls_pow(LSNull*);
	virtual LSValue* ls_pow(LSBoolean*);
	virtual LSValue* ls_pow(LSNumber*);
	virtual LSValue* ls_pow(LSString*);
	virtual LSValue* ls_pow(LSArray<LSValue*>*);
	virtual LSValue* ls_pow(LSArray<int>*);
	virtual LSValue* ls_pow(LSArray<double>*);
	virtual LSValue* ls_pow(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_pow(LSMap<LSValue*,int>*);
	virtual LSValue* ls_pow(LSMap<LSValue*,double>*);
	virtual LSValue* ls_pow(LSMap<int,LSValue*>*);
	virtual LSValue* ls_pow(LSMap<int,int>*);
	virtual LSValue* ls_pow(LSMap<int,double>*);
	virtual LSValue* ls_pow(LSSet<LSValue*>*);
	virtual LSValue* ls_pow(LSSet<int>*);
	virtual LSValue* ls_pow(LSSet<double>*);
	virtual LSValue* ls_pow(LSObject*);
	virtual LSValue* ls_pow(LSFunction<LSValue*>*);
	virtual LSValue* ls_pow(LSClass*);

	LSValue* ls_pow_eq(LSValue* value) {
		return value->ls_rpow_eq(this);
	}
	virtual LSValue* ls_rpow_eq(LSValue*) = 0;
	virtual LSValue* ls_pow_eq(LSNull*);
	virtual LSValue* ls_pow_eq(LSBoolean*);
	virtual LSValue* ls_pow_eq(LSNumber*);
	virtual LSValue* ls_pow_eq(LSString*);
	virtual LSValue* ls_pow_eq(LSArray<LSValue*>*);
	virtual LSValue* ls_pow_eq(LSArray<int>*);
	virtual LSValue* ls_pow_eq(LSArray<double>*);
	virtual LSValue* ls_pow_eq(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_pow_eq(LSMap<LSValue*,int>*);
	virtual LSValue* ls_pow_eq(LSMap<LSValue*,double>*);
	virtual LSValue* ls_pow_eq(LSMap<int,LSValue*>*);
	virtual LSValue* ls_pow_eq(LSMap<int,int>*);
	virtual LSValue* ls_pow_eq(LSMap<int,double>*);
	virtual LSValue* ls_pow_eq(LSSet<LSValue*>*);
	virtual LSValue* ls_pow_eq(LSSet<int>*);
	virtual LSValue* ls_pow_eq(LSSet<double>*);
	virtual LSValue* ls_pow_eq(LSObject*);
	virtual LSValue* ls_pow_eq(LSFunction<LSValue*>*);
	virtual LSValue* ls_pow_eq(LSClass*);

	LSValue* ls_mod(LSValue* value) {
		return value->ls_rmod(this);
	}
	virtual LSValue* ls_rmod(LSValue*) = 0;
	virtual LSValue* ls_mod(LSNull*);
	virtual LSValue* ls_mod(LSBoolean*);
	virtual LSValue* ls_mod(LSNumber*);
	virtual LSValue* ls_mod(LSString*);
	virtual LSValue* ls_mod(LSArray<LSValue*>*);
	virtual LSValue* ls_mod(LSArray<int>*);
	virtual LSValue* ls_mod(LSArray<double>*);
	virtual LSValue* ls_mod(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_mod(LSMap<LSValue*,int>*);
	virtual LSValue* ls_mod(LSMap<LSValue*,double>*);
	virtual LSValue* ls_mod(LSMap<int,LSValue*>*);
	virtual LSValue* ls_mod(LSMap<int,int>*);
	virtual LSValue* ls_mod(LSMap<int,double>*);
	virtual LSValue* ls_mod(LSSet<LSValue*>*);
	virtual LSValue* ls_mod(LSSet<int>*);
	virtual LSValue* ls_mod(LSSet<double>*);
	virtual LSValue* ls_mod(LSObject*);
	virtual LSValue* ls_mod(LSFunction<LSValue*>*);
	virtual LSValue* ls_mod(LSClass*);

	LSValue* ls_mod_eq(LSValue* value) {
		return value->ls_rmod_eq(this);
	}
	virtual LSValue* ls_rmod_eq(LSValue*) = 0;
	virtual LSValue* ls_mod_eq(LSNull*);
	virtual LSValue* ls_mod_eq(LSBoolean*);
	virtual LSValue* ls_mod_eq(LSNumber*);
	virtual LSValue* ls_mod_eq(LSString*);
	virtual LSValue* ls_mod_eq(LSArray<LSValue*>*);
	virtual LSValue* ls_mod_eq(LSArray<int>*);
	virtual LSValue* ls_mod_eq(LSArray<double>*);
	virtual LSValue* ls_mod_eq(LSMap<LSValue*,LSValue*>*);
	virtual LSValue* ls_mod_eq(LSMap<LSValue*,int>*);
	virtual LSValue* ls_mod_eq(LSMap<LSValue*,double>*);
	virtual LSValue* ls_mod_eq(LSMap<int,LSValue*>*);
	virtual LSValue* ls_mod_eq(LSMap<int,int>*);
	virtual LSValue* ls_mod_eq(LSMap<int,double>*);
	virtual LSValue* ls_mod_eq(LSSet<LSValue*>*);
	virtual LSValue* ls_mod_eq(LSSet<int>*);
	virtual LSValue* ls_mod_eq(LSSet<double>*);
	virtual LSValue* ls_mod_eq(LSObject*);
	virtual LSValue* ls_mod_eq(LSFunction<LSValue*>*);
	virtual LSValue* ls_mod_eq(LSClass*);

	bool operator == (const LSValue& value) const {
		return value.req(this);
	}
	virtual bool operator == (int) const { return false; }
	virtual bool operator == (double) const { return false; }

	bool operator != (const LSValue& value) const {
		return !value.req(this);
	}

	virtual bool req(const LSValue*) const = 0;
	virtual bool eq(const LSNull*) const;
	virtual bool eq(const LSBoolean*) const;
	virtual bool eq(const LSNumber*) const;
	virtual bool eq(const LSString*) const;
	virtual bool eq(const LSArray<LSValue*>*) const;
	virtual bool eq(const LSArray<int>*) const;
	virtual bool eq(const LSArray<double>*) const;
	virtual bool eq(const LSMap<LSValue*,LSValue*>*) const;
	virtual bool eq(const LSMap<LSValue*,int>*) const;
	virtual bool eq(const LSMap<LSValue*,double>*) const;
	virtual bool eq(const LSMap<int,LSValue*>*) const;
	virtual bool eq(const LSMap<int,int>*) const;
	virtual bool eq(const LSMap<int,double>*) const;
	virtual bool eq(const LSMap<double,LSValue*>*) const;
	virtual bool eq(const LSMap<double,int>*) const;
	virtual bool eq(const LSMap<double,double>*) const;
	virtual bool eq(const LSSet<LSValue*>*) const;
	virtual bool eq(const LSSet<int>*) const;
	virtual bool eq(const LSSet<double>*) const;
	virtual bool eq(const LSFunction<LSValue*>*) const;
	virtual bool eq(const LSObject*) const;
	virtual bool eq(const LSClass*) const;

	bool operator < (const LSValue& value) const {
		return value.rlt(this);
	}
	bool operator > (const LSValue& value) const {
		return !(*this == value) && !(*this < value);
	}
	bool operator <=(const LSValue& value) const {
		return (*this == value) || (*this < value);
	}
	bool operator >=(const LSValue& value) const {
		return !(*this < value);
	}
	virtual bool rlt(const LSValue*) const = 0;
	virtual bool lt(const LSNull*) const;
	virtual bool lt(const LSBoolean*) const;
	virtual bool lt(const LSNumber*) const;
	virtual bool lt(const LSString*) const;
	virtual bool lt(const LSArray<LSValue*>*) const;
	virtual bool lt(const LSArray<int>*) const;
	virtual bool lt(const LSArray<double>*) const;
	virtual bool lt(const LSMap<LSValue*,LSValue*>*) const;
	virtual bool lt(const LSMap<LSValue*,int>*) const;
	virtual bool lt(const LSMap<LSValue*,double>*) const;
	virtual bool lt(const LSMap<int,LSValue*>*) const;
	virtual bool lt(const LSMap<int,int>*) const;
	virtual bool lt(const LSMap<int,double>*) const;
	virtual bool lt(const LSMap<double,LSValue*>*) const;
	virtual bool lt(const LSMap<double,int>*) const;
	virtual bool lt(const LSMap<double,double>*) const;
	virtual bool lt(const LSSet<LSValue*>*) const;
	virtual bool lt(const LSSet<int>*) const;
	virtual bool lt(const LSSet<double>*) const;
	virtual bool lt(const LSFunction<LSValue*>*) const;
	virtual bool lt(const LSObject*) const;
	virtual bool lt(const LSClass*) const;

	virtual bool in(const LSValue*) const { return false; }

	virtual LSValue* at(const LSValue* key) const;
	virtual LSValue** atL(const LSValue* key);

	virtual LSValue* attr(const std::string& key) const;
	virtual LSValue** attrL(const std::string& key);

	virtual LSValue* range(int start, int end) const;
	virtual LSValue* rangeL(int start, int end);

	virtual LSValue* abso() const;

	virtual int integer() const { return 0; }
	virtual double real() const { return 0; }

	virtual std::string to_string() const;
	virtual std::ostream& print(std::ostream&) const;
	virtual std::ostream& dump(std::ostream&) const = 0;
	virtual std::string json() const;

	LSString* ls_json();

	virtual LSValue* clone() const;
	LSValue* clone_inc();
	LSValue* move();
	LSValue* move_inc();

	virtual LSValue* getClass() const = 0;

	bool isInteger() const;

	virtual int typeID() const = 0;

	template <class T> static LSValue* get(T v);
	static LSValue* parse(Json& json);
	static LSValue* get_from_json(Json& json);

	static void delete_ref(LSValue* value);
	static void delete_temporary(const LSValue* const value);
};

template <> LSValue* LSValue::get(int v);
template <> LSValue* LSValue::get(double v);

inline LSValue* LSValue::clone_inc() {
	if (native) {
		return this;
	} else {
		LSValue* copy = clone();
		copy->refs = 1;
		return copy;
	}
}

inline LSValue* LSValue::move() {
	if (native) {
		return this;
	}
	if (refs == 0) {
		return this;
	}
	return clone();
}

inline LSValue* LSValue::move_inc() {
	if (!native) {
		refs++;
	}
	return this;
}

inline void LSValue::delete_ref(LSValue* value) {

	if (value == nullptr) return;
	if (value->native) return;
	if (value->refs == 0) return;

	value->refs--;
	if (value->refs == 0) {
		delete value;
	}
}

inline void LSValue::delete_temporary(const LSValue* const value) {

	if (value == nullptr) return;
	if (value->native) return;

	if (value->refs == 0) {
		delete value;
	}
}

inline std::ostream& operator << (std::ostream& os, const LSValue& value) {
	return value.print(os);
}

}

namespace ls {
	template <typename T>
	std::string to_json(T v) {
		return std::to_string(v);
	}
	template <>
	inline std::string to_json(LSValue* v) {
		return v->json();
	}

	template <typename T>
	std::string print(T v) {
		return std::to_string(v);
	}
	template <>
	inline std::string print(LSValue* v) {
		std::ostringstream oss;
		oss << *v;
		return oss.str();
	}

	template <class T1, class T2>
	bool equals(const T1 v1, const T2 v2) {
		return v1 == v2;
	}

	template <>
	inline bool equals(double v1, ls::LSValue* v2) {
		return v2->operator == (v1);
	}
	template <>
	inline bool equals(ls::LSValue* v1, double v2) {
		return v1->operator == (v2);
	}
	template <>
	inline bool equals(int v1, ls::LSValue* v2) {
		return v2->operator == (v1);
	}
	template <>
	inline bool equals(ls::LSValue* v1, int v2) {
		return v1->operator == (v2);
	}
	template <>
	inline bool equals(ls::LSValue* v1, ls::LSValue* v2) {
		return v1->operator == (*v2);
	}

	template <class T> void release(T) {}
	template <> inline void release(LSValue* v) {
		LSValue::delete_temporary(v);
	}
	template <> inline void release(const LSValue* v) {
		LSValue::delete_temporary(v);
	}

	template <class T> void unref(T) {}
	template <> inline void unref(LSValue* v) {
		LSValue::delete_ref(v);
	}
	template <> inline void unref(const LSValue* v) {
		LSValue::delete_ref((LSValue*) v);
	}

	template <class T> T oneref(T v) { return v; }
	template <> inline LSValue* oneref(LSValue* v) {
		v->refs = 1;
		return v;
	}
	template <> inline const LSValue* oneref(const LSValue* v) {
		((LSValue*) v)->refs = 1;
		return v;
	}

	template <class T> T move(T v) { return v; }
	template <> inline LSValue* move(LSValue* v) {
		return v->move();
	}
	template <> inline const LSValue* move(const LSValue* v) {
		return ((LSValue*) v)->move();
	}

	template <class T> T move_inc(T v) { return v; }
	template <> inline LSValue* move_inc(LSValue* v) {
		return v->move_inc();
	}
	template <> inline const LSValue* move_inc(const LSValue* v) {
		return ((LSValue*) v)->move_inc();
	}

	template <class T> T clone(T v) { return v; }
	template <> inline LSValue* clone(LSValue* v) {
		return v->clone();
	}
	template <> inline const LSValue* clone(const LSValue* v) {
		return ((LSValue*) v)->clone();
	}

	template <class T> T clone_inc(T v) { return v; }
	template <> inline LSValue* clone_inc(LSValue* v) {
		return v->clone_inc();
	}
	template <> inline const LSValue* clone_inc(const LSValue* v) {
		return ((LSValue*) v)->clone_inc();
	}

	template <class R, class T> R convert(T v);
	template <class R, R> R convert(R v) { return v; }

	template <> inline int convert(int v) { return v; }
	template <> inline double convert(double v) { return v; }
	template <> inline LSValue* convert(LSValue* v) { return v; }

	template <> inline int convert(double v) { return v; }
	template <> inline double convert(int v) { return v; }

	template <> inline LSValue* convert(int v) {
		return LSValue::get(v);
	}
	template <> inline LSValue* convert(double v) {
		return LSValue::get(v);
	}
	template <> inline int convert(LSValue* v) {
		return v->integer();
	}
	template <> inline double convert(LSValue* v) {
		return v->real();
	}
}

namespace std {
	std::string to_string(ls::LSValue* value);
	std::ostream& operator << (std::ostream&, const ls::LSValue*);
}

#endif
