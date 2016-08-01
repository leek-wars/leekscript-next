#include "LSFunction.hpp"
#include "LSNull.hpp"
#include "LSClass.hpp"
#include "LSNumber.hpp"

using namespace std;

namespace ls {

LSClass* LSFunction::function_class = new LSClass("Function", 1);

LSFunction::LSFunction(void* function) {
	this->function = function;
}

LSFunction::LSFunction(void* function, int refs) {
	this->function = function;
	this->refs = refs;
}

LSFunction::LSFunction(Json&) {
	// TODO
}

LSFunction::~LSFunction() {}

bool LSFunction::isTrue() const {
	return true;
}

LSValue* LSFunction::operator - () const {
	return this->clone();
}

LSValue* LSFunction::operator ! () const {
	return this->clone();
}

LSValue* LSFunction::operator ~ () const {
	return this->clone();
}

LSValue* LSFunction::operator ++ () {
	return this;
}
LSValue* LSFunction::operator ++ (int) {
	return this;
}

LSValue* LSFunction::operator -- () {
	return this;
}
LSValue* LSFunction::operator -- (int) {
	return LSNull::null_var;
}

LSValue* LSFunction::operator + (const LSValue* v) const {
	return v->operator + (this);
}
LSValue* LSFunction::operator + (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator + (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator + (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator + (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator + (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator + (const LSArray<int>*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator + (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator + (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator + (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSFunction::operator += (LSValue* value) {
	return value->operator += (this);
}
LSValue* LSFunction::operator += (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator += (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator += (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator += (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator += (const LSArray<LSValue*>*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator += (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator += (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator += (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSFunction::operator - (const LSValue* value) const {
	return value->operator - (this);
}
LSValue* LSFunction::operator - (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator - (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator - (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator - (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator - (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator - (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator - (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator - (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSFunction::operator -= (LSValue* value) {
	return value->operator -= (this);
}
LSValue* LSFunction::operator -= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator -= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator -= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator -= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator -= (const LSArray<LSValue*>*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator -= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator -= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator -= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSFunction::operator * (const LSValue* value) const {
	return value->operator * (this);
}
LSValue* LSFunction::operator * (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator * (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator * (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator * (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator * (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator * (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator * (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator * (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSFunction::operator *= (LSValue* value) {
	return value->operator *= (this);
}
LSValue* LSFunction::operator *= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator *= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator *= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator *= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator *= (const LSArray<LSValue*>*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator *= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator *= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator *= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSFunction::operator / (const LSValue* value) const {
	return value->operator / (this);
}
LSValue* LSFunction::operator / (const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator / (const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator / (const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator / (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator / (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator / (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator / (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator / (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSFunction::operator /= (LSValue* value) {
	return value->operator /= (this);
}
LSValue* LSFunction::operator /= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator /= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator /= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator /= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator /= (const LSArray<LSValue*>*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator /= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator /= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator /= (const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSFunction::poww(const LSValue* value) const {
	return value->poww(this);
}
LSValue* LSFunction::poww(const LSNull*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::poww(const LSBoolean*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::poww(const LSNumber*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::poww(const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::poww(const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::poww(const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::poww(const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::poww(const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSFunction::pow_eq(LSValue* value) {
	return value->pow_eq(this);
}
LSValue* LSFunction::pow_eq(const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSFunction::pow_eq(const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSFunction::pow_eq(const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSFunction::pow_eq(const LSString*) {
	return LSNull::null_var;
}
LSValue* LSFunction::pow_eq(const LSArray<LSValue*>*) {
	return LSNull::null_var;
}
LSValue* LSFunction::pow_eq(const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSFunction::pow_eq(const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSFunction::pow_eq(const LSClass*) {
	return LSNull::null_var;
}

LSValue* LSFunction::operator % (const LSValue* value) const {
	return value->operator % (this);
}
LSValue* LSFunction::operator % (const LSNull*)  const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator % (const LSBoolean*)  const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator % (const LSNumber*)  const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator % (const LSString*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator % (const LSArray<LSValue*>*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator % (const LSObject*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator % (const LSFunction*) const {
	return LSNull::null_var;
}
LSValue* LSFunction::operator % (const LSClass*) const {
	return LSNull::null_var;
}

LSValue* LSFunction::operator %= (LSValue* value) {
	return value->operator %= (this);
}
LSValue* LSFunction::operator %= (const LSNull*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator %= (const LSBoolean*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator %= (const LSNumber*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator %= (const LSString*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator %= (const LSArray<LSValue*>*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator %= (const LSObject*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator %= (const LSFunction*) {
	return LSNull::null_var;
}
LSValue* LSFunction::operator %= (const LSClass*) {
	return LSNull::null_var;
}


bool LSFunction::operator == (const LSValue* v) const {
	return v->operator == (this);
}
bool LSFunction::operator == (const LSNull*) const {
	return false;
}
bool LSFunction::operator == (const LSBoolean*) const {
	return false;
}
bool LSFunction::operator == (const LSNumber*) const {
	return false;
}
bool LSFunction::operator == (const LSString*) const {
	return false;
}
bool LSFunction::operator == (const LSArray<LSValue*>*) const {
	return false;
}
bool LSFunction::operator == (const LSObject*) const {
	return false;
}
bool LSFunction::operator == (const LSFunction* f) const {
	return function == f->function;
}
bool LSFunction::operator == (const LSClass*) const {
	return false;
}

bool LSFunction::operator < (const LSValue* v) const {
	return v->operator < (this);
}
bool LSFunction::operator < (const LSNull*) const {
	return false;
}
bool LSFunction::operator < (const LSBoolean*) const {
	return false;
}
bool LSFunction::operator < (const LSNumber*) const {
	return false;
}
bool LSFunction::operator < (const LSString*) const {
	return false;
}
bool LSFunction::operator < (const LSArray<LSValue*>*) const {
	return false;
}
bool LSFunction::operator < (const LSArray<int>* v) const {
	return false;
}
bool LSFunction::operator < (const LSArray<double>* v) const {
	return false;
}
bool LSFunction::operator < (const LSObject*) const {
	return false;
}
bool LSFunction::operator < (const LSFunction*) const {
	return false;
}
bool LSFunction::operator < (const LSClass*) const {
	return true;
}

bool LSFunction::operator > (const LSValue* v) const {
	return v->operator > (this);
}
bool LSFunction::operator > (const LSNull*) const {
	return true;
}
bool LSFunction::operator > (const LSBoolean*) const {
	return true;
}
bool LSFunction::operator > (const LSNumber*) const {
	return true;
}
bool LSFunction::operator > (const LSString*) const {
	return true;
}
bool LSFunction::operator > (const LSArray<LSValue*>*) const {
	return true;
}
bool LSFunction::operator > (const LSObject*) const {
	return true;
}
bool LSFunction::operator > (const LSFunction*) const {
	return true;
}
bool LSFunction::operator > (const LSClass*) const {
	return false;
}

bool LSFunction::operator <= (const LSValue* v) const {
	return v->operator <= (this);
}
bool LSFunction::operator <= (const LSNull*) const {
	return false;
}
bool LSFunction::operator <= (const LSBoolean*) const {
	return false;
}
bool LSFunction::operator <= (const LSNumber*) const {
	return false;
}
bool LSFunction::operator <= (const LSString*) const {
	return false;
}
bool LSFunction::operator <= (const LSArray<LSValue*>*) const {
	return false;
}
bool LSFunction::operator <= (const LSObject*) const {
	return false;
}
bool LSFunction::operator <= (const LSFunction*) const {
	return false;
}
bool LSFunction::operator <= (const LSClass*) const {
	return true;
}

bool LSFunction::operator >= (const LSValue* v) const {
	return v->operator >= (this);
}
bool LSFunction::operator >= (const LSNull*) const {
	return true;
}
bool LSFunction::operator >= (const LSBoolean*) const {
	return true;
}
bool LSFunction::operator >= (const LSNumber*) const {
	return true;
}
bool LSFunction::operator >= (const LSString*) const {
	return true;
}
bool LSFunction::operator >= (const LSArray<LSValue*>*) const {
	return true;
}
bool LSFunction::operator >= (const LSObject*) const {
	return true;
}
bool LSFunction::operator >= (const LSFunction*) const {
	return true;
}
bool LSFunction::operator >= (const LSClass*) const {
	return false;
}

bool LSFunction::in(const LSValue*) const {
	return false;
}

LSValue* LSFunction::at(const LSValue*) const {
	return LSNull::null_var;
}
LSValue** LSFunction::atL(const LSValue*) {
	return &LSNull::null_var;
}

LSValue* LSFunction::range(int, int) const {
	return this->clone();
}
LSValue* LSFunction::rangeL(int, int) {
	return this;
}

LSValue* LSFunction::attr(const LSValue* key) const {
	if (*((LSString*) key) == "class") {
		return getClass();
	}
	return LSNull::null_var;
}
LSValue** LSFunction::attrL(const LSValue*) {
	return &LSNull::null_var;
}

LSValue* LSFunction::abso() const {
	return LSNumber::get(0);
}

LSValue* LSFunction::clone() const {
	return (LSValue*) this;
}

std::ostream& LSFunction::print(std::ostream& os) const {
	os << "<function>";
	return os;
}
string LSFunction::json() const {
	return "\"<function>\"";
}

LSValue* LSFunction::getClass() const {
	return LSFunction::function_class;
}

int LSFunction::typeID() const {
	return 7;
}

const BaseRawType* LSFunction::getRawType() const {
	return RawType::FUNCTION;
}

}
