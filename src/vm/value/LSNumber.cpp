#include "LSNumber.hpp"
#include "LSNull.hpp"
#include "LSFunction.hpp"
#include "LSBoolean.hpp"
#include <cmath>

using namespace std;

namespace ls {

LSClass* LSNumber::number_class = new LSClass("Number");

LSNumber* LSNumber::cache[CACHE_HIGH - CACHE_LOW + 1];

void LSNumber::build_cache() {
	for (int i = CACHE_LOW; i <= CACHE_HIGH; ++i) {
		cache[-CACHE_LOW + i] = new LSNumber(i);
	}
}

LSNumber* LSNumber::get(NUMBER_TYPE i) {
#if USE_CACHE
	if ((i == (int) i) and i >= CACHE_LOW and i <= CACHE_HIGH) {
		return cache[(int) (-CACHE_LOW + i)];
	}
#endif
//	std::cout << "Number() " << i << endl;
	return new LSNumber(i);
}

LSNumber::LSNumber() : value(0) {}

LSNumber::LSNumber(NUMBER_TYPE value) : value(value) {}

LSNumber::LSNumber(Json& json) : value(json) {}

LSNumber::~LSNumber() {
//	cout << "delete LSNumber : " << value << endl;
}

bool LSNumber::isTrue() const {
	return value != 0;
}

LSValue* LSNumber::operator - () const {
	return LSNumber::get(-value);
}

LSValue* LSNumber::operator ! () const {
	return LSBoolean::get(value == 0);
}

LSValue* LSNumber::operator ~ () const {
	return LSNumber::get(~(int)value);
}

LSValue* LSNumber::operator ++ () {
#if !USE_CACHE
	++value;
#endif
	return this;
}
LSValue* LSNumber::operator ++ (int) {
	NUMBER_TYPE old = value;
#if !USE_CACHE
	++value;
#endif
	return LSNumber::get(old);
}

LSValue* LSNumber::operator -- () {
#if !USE_CACHE
	--value;
#endif
	return this;
}
LSValue* LSNumber::operator -- (int) {
	NUMBER_TYPE old = value;
#if !USE_CACHE
	--value;
#endif
	return LSNumber::get(old);
}

LSValue* LSNumber::ls_add(LSNull*) {
	return this;
}
LSValue* LSNumber::ls_add(LSBoolean* boolean) {
	if (boolean->value) {
		if (refs == 0) {
			this->value += 1;
			return this;
		}
		return LSNumber::get(value + 1);
	}
	return this;
}
LSValue* LSNumber::ls_add(LSString* string) {
	LSValue* r = new LSString(toString() + *string);
	if (refs == 0) delete this;
	if (string->refs == 0) delete string;
	return r;
}
LSValue* LSNumber::ls_add(LSNumber* number) {
	if (refs == 0) {
		value += number->value;
		if (number->refs == 0) delete number;
		return this;
	}
	if (number->refs == 0) {
		number->value += value;
		return number;
	}
	return LSNumber::get(this->value + number->value);
}

LSValue* LSNumber::ls_add_eq(LSNull*) {
	return this;
}
LSValue* LSNumber::ls_add_eq(LSBoolean* boolean) {
	value += boolean->value;
	return this;
}
LSValue* LSNumber::ls_add_eq(LSNumber* number) {
	value += number->value;
	if (number->refs == 0) delete number;
	return this;
}

LSValue* LSNumber::ls_sub(LSNull*) {
	return this;
}
LSValue* LSNumber::ls_sub(LSBoolean* boolean) {
	if (boolean->value) {
		if (refs == 0) {
			this->value -= 1;
			return this;
		}
		return LSNumber::get(value - 1);
	}
	return this;
}
LSValue* LSNumber::ls_sub(LSNumber* number) {
	if (refs == 0) {
		value -= number->value;
		return this;
	}
	if (number->refs == 0) {
		number->value = this->value - number->value;
		return number;
	}
	return LSNumber::get(this->value - number->value);
}

LSValue* LSNumber::ls_sub_eq(LSNull*) {
	return this;
}
LSValue* LSNumber::ls_sub_eq(LSBoolean* boolean) {
	value -= boolean->value;
	return this;
}
LSValue* LSNumber::ls_sub_eq(LSNumber* number) {
	value -= number->value;
	if (number->refs == 0) delete number;
	return this;
}

LSValue* LSNumber::ls_mul(LSNull*) {
	if (refs == 0) {
		value = 0;
		return this;
	}
	return LSNumber::get(0);
}
LSValue* LSNumber::ls_mul(LSBoolean* boolean) {
	if (boolean->value) {
		return this;
	}
	if (refs == 0) {
		value = 0;
		return this;
	}
	return LSNumber::get(0);
}
LSValue* LSNumber::ls_mul(LSNumber* number) {
	if (refs == 0) {
		value *= number->value;
		if (number->refs == 0) delete number;
		return this;
	}
	if (number->refs == 0) {
		number->value *= value;
		return number;
	}
	return LSNumber::get(value * number->value);
}

LSValue*LSNumber::ls_mul(LSString* str) {
	string r;
	for (int i = 0; i < value; ++i) {
		r += *str;
	}
	if (refs == 0) delete this;

	if (str->refs == 0) {
		*str = r;
		return str;
	}
	return new LSString(r);
}

LSValue* LSNumber::ls_mul_eq(LSNull*) {
	value = 0;
	return this;
}
LSValue* LSNumber::ls_mul_eq(LSBoolean* boolean) {
	value *= boolean->value;
	return this;
}
LSValue* LSNumber::ls_mul_eq(LSNumber* number) {
	value *= number->value;
	if (number->refs == 0) delete number;
	return this;
}

LSValue* LSNumber::ls_div(LSNull*) {
	if (refs == 0) {
		value = NAN;
		return this;
	}
	return LSNumber::get(NAN);
}
LSValue* LSNumber::ls_div(LSBoolean* boolean) {
	if (boolean->value) {
		return this;
	}
	if (refs == 0) {
		value = NAN;
		return this;
	}
	return LSNumber::get(NAN);
}
LSValue* LSNumber::ls_div(LSNumber* number) {
	if (refs == 0) {
		value /= number->value;
		if (number->refs == 0) delete number;
		return this;
	}
	if (number->refs == 0) {
		number->value = value / number->value;
		return number;
	}
	return LSNumber::get(value / number->value);
}

LSValue* LSNumber::ls_div_eq(LSNull*) {
	value = NAN;
	return this;
}
LSValue* LSNumber::ls_div_eq(LSBoolean* boolean) {
	if (!boolean->value) {
		value = NAN;
	}
	return this;
}
LSValue* LSNumber::ls_div_eq(LSNumber* number) {
	value /= number->value;
	if (number->refs == 0) delete number;
	return this;
}

LSValue* LSNumber::ls_pow(LSNull*) {
	if (refs == 0) {
		value = 1;
		return this;
	}
	return LSNumber::get(1);
}
LSValue* LSNumber::ls_pow(LSBoolean* boolean) {
	if (boolean->value) {
		return this;
	}
	if (refs == 0) {
		value = 1;
		return this;
	}
	return LSNumber::get(1);
}
LSValue* LSNumber::ls_pow(LSNumber* number) {
	if (refs == 0) {
		value = pow(value, number->value);
		if (number->refs == 0) delete number;
		return this;
	}
	if (number->refs == 0) {
		number->value = pow(value, number->value);
		return number;
	}
	return LSNumber::get(pow(value, number->value));
}

LSValue* LSNumber::ls_pow_eq(LSNull*) {
	value = 1;
	return this;
}
LSValue* LSNumber::ls_pow_eq(LSBoolean* boolean) {
	if (!boolean->value) value = 1;
	return this;
}
LSValue* LSNumber::ls_pow_eq(LSNumber* number) {
	value = pow(value, number->value);
	if (number->refs == 0) delete number;
	return this;
}

LSValue* LSNumber::ls_mod(LSNull*) {
	if (refs == 0) {
		value = 0;
		return this;
	}
	return LSNumber::get(0);
}
LSValue* LSNumber::ls_mod(LSBoolean*) {
	if (refs == 0) {
		value = 0;
		return this;
	}
	return LSNumber::get(0);
}
LSValue* LSNumber::ls_mod(LSNumber* number) {
	if (refs == 0) {
		value = fmod(value, number->value);
		if (number->refs == 0) delete number;
		return this;
	}
	if (number->refs == 0) {
		number->value = fmod(value, number->value);
		return number;
	}
	return LSNumber::get(fmod(value, number->value));
}

LSValue* LSNumber::ls_mod_eq(LSNull*) {
	value = 0;
	return this;
}
LSValue* LSNumber::ls_mod_eq(LSBoolean*) {
	value = 0;
	return this;
}
LSValue* LSNumber::ls_mod_eq(LSNumber* number) {
	value = fmod(value, number->value);
	if (number->refs == 0) delete number;
	return this;
}

bool LSNumber::eq(const LSNumber* number) const {
	return this->value == number->value;
}

bool LSNumber::operator < (const LSValue* v) const {
	return v->operator < (this);
}
bool LSNumber::operator < (const LSNumber* v) const {
	return this->value < v->value;
}

bool LSNumber::operator > (const LSValue* v) const {
	return v->operator > (this);
}
bool LSNumber::operator > (const LSNumber* v) const {
	return this->value > v->value;
}

LSValue* LSNumber::at(const LSValue*) const {
	return LSNull::get();
}

LSValue** LSNumber::atL(const LSValue*) {
	return nullptr;
}

LSValue* LSNumber::attr(const LSValue* key) const {
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::get();
}
LSValue** LSNumber::attrL(const LSValue*) {
	return nullptr;
}

LSValue* LSNumber::abso() const {
	return LSNumber::get(abs((int) value));
}

LSValue* LSNumber::clone() const {
	return LSNumber::get(this->value);
}

bool LSNumber::isInteger() const {
	return value == (int)value;
}

void append_dbl2str(std::string &s, double d) {

	size_t len = snprintf(0, 0, "%.18f", d);
	size_t oldsize = s.size();
	s.resize(oldsize + len + 1);

	// technically non-portable
	snprintf(&s[oldsize], len+1, "%.18f", d);
	// remove nul terminator
	s.pop_back();
	// remove trailing zeros
	s.erase(s.find_last_not_of('0') + 1, string::npos);
	// remove trailing point
	if (s.back() == L'.') {
		s.pop_back();
	}
}

string LSNumber::toString() const {
	if (isInteger()) return to_string((int)value);
	string s;
	append_dbl2str(s, value);
	return s;
}
string LSNumber::json() const {
	return toString();
}

std::ostream& LSNumber::print(std::ostream& os) const {
	os << toString();
	return os;
}

LSValue* LSNumber::getClass() const {
	return LSNumber::number_class;
}

int LSNumber::typeID() const {
	return 3;
}

const BaseRawType* LSNumber::getRawType() const {
	return RawType::INTEGER;
}

}
