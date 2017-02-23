#include "LSBoolean.hpp"
#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSString.hpp"
#include "LSNumber.hpp"

using namespace std;

namespace ls {

LSValue* LSBoolean::clazz;
LSBoolean* LSBoolean::false_val(new LSBoolean(false));
LSBoolean* LSBoolean::true_val(new LSBoolean(true));

LSBoolean::LSBoolean(bool value) : LSValue(BOOLEAN), value(value) {
	refs = 1;
	native = true;
}

LSBoolean::~LSBoolean() {}

bool LSBoolean::isTrue() const {
	return value;
}

LSValue* LSBoolean::ls_not() {
	return LSBoolean::get(!value);
}

LSValue* LSBoolean::ls_tilde() {
	return LSNumber::get(~value);
}

LSValue* LSBoolean::ls_minus() {
	return LSNumber::get(-value);
}

LSValue* LSBoolean::add(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		if (this->value) {
			if (number->refs == 0) {
				number->value += 1;
				return number;
			}
			auto r = LSNumber::get(number->value + 1);
			LSValue::delete_temporary(number);
			return r;
		}
		return number;
	}
	if (v->type == BOOLEAN) {
		auto boolean = static_cast<LSBoolean*>(v);
		return LSNumber::get(value + boolean->value);
	}
	if (v->type == STRING) {
		auto string = static_cast<LSString*>(v);
		LSValue::delete_temporary(string);
		return new LSString((value ? "true" : "false") + *string);
	}
	LSValue::delete_temporary(v);
	return LSNull::get();
}

LSValue* LSBoolean::sub(LSValue* v) {
	if (v->type == NUMBER) {
		auto number = static_cast<LSNumber*>(v);
		if (this->value) {
			if (number->refs == 0) {
				number->value = 1 - number->value;
				return number;
			}
			return LSNumber::get(1 - number->value);
		}
		if (number->refs == 0) {
			number->value = -number->value;
			return number;
		}
		LSValue::delete_temporary(this);
		return LSNumber::get(-number->value);
	}
	if (v->type == BOOLEAN) {
		auto boolean = static_cast<LSBoolean*>(v);
		return LSNumber::get(value - boolean->value);
	}
	LSValue::delete_temporary(v);
	return LSNull::get();
}

bool LSBoolean::eq(const LSValue* v) const {
	if (v->type == BOOLEAN) {
		auto boolean = static_cast<const LSBoolean*>(v);
		return boolean->value == this->value;
	}
	return false;
}

bool LSBoolean::lt(const LSValue* v) const {
	if (v->type == BOOLEAN) {
		auto boolean = static_cast<const LSBoolean*>(v);
		return this->value < boolean->value;
	}
	return LSValue::lt(v);
}

int LSBoolean::abso() const {
	return value;
}

LSValue* LSBoolean::clone() const {
	return (LSValue*) this;
}

std::ostream& LSBoolean::dump(std::ostream& os) const {
	os << (value ? "true" : "false");
	return os;
}

string LSBoolean::json() const {
	return value ? "true" : "false";
}

LSValue* LSBoolean::getClass() const {
	return LSBoolean::clazz;
}

}
