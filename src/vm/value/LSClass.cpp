#include "LSClass.hpp"
#include "LSString.hpp"
#include "LSNumber.hpp"
#include "LSFunction.hpp"
#include "../Module.hpp"
#include "../../type/Function_type.hpp"

namespace ls {

LSValue* LSClass::clazz;

LSClass::LSClass(std::string name) : LSValue(CLASS), name(name) {
	parent = nullptr;
	refs = 1;
	native = true;
}

LSClass::~LSClass() {
	for (auto s : static_fields) {
		if (s.second.value != nullptr) {
			delete s.second.value;
		}
	}
	for (auto f : fields) {
		if (f.second.default_value != nullptr) {
			delete f.second.default_value;
		}
	}
}

void LSClass::addMethod(std::string& name, std::vector<Method> method) {
	methods.insert({name, method});
}

void LSClass::addStaticMethod(std::string& name, std::vector<StaticMethod> method) {
	static_methods.insert({name, method});

	// Add first implementation as default method
	auto fun = new LSFunction(method[0].addr);
	fun->refs = 1;
	fun->native = true;
	Type type = method[0].type;
	static_fields.insert({name, ModuleStaticField(name, type, fun)});
}

void LSClass::addField(std::string name, Type type, std::function<Compiler::value(Compiler&, Compiler::value)> fun) {
	fields.insert({name, field(name, type, fun, nullptr)});
}
void LSClass::addField(std::string name, Type type, void* fun) {
	fields.insert({name, field(name, type, fun, nullptr)});
}

void LSClass::addStaticField(ModuleStaticField f) {
	static_fields.insert({f.name, f});
}

void LSClass::addOperator(std::string name, std::vector<Operator> impl) {
	operators.insert({name, impl});
}

Method* LSClass::getMethod(SemanticAnalyser* analyser, std::string& name, Type obj_type, std::vector<Type> arguments) {
	// std::cout << "getMethod " << name << " in class " << this->name <<  " obj type " << obj_type << " with args " << args << std::endl;
	try {
		Method* best = nullptr;
		for (auto& implementation : methods.at(name)) {
			if (implementation.obj_type.may_be_compatible(obj_type)) {
				for (size_t i = 0; i < std::min(implementation.type.arguments().size(), arguments.size()); ++i) {
					const auto& a = arguments.at(i);
					if (auto fun = dynamic_cast<const Function_type*>(a._types[0])) {
						if (fun->function()) {
							auto version = implementation.type.arguments().at(i).arguments();
							((Function*) fun->function())->will_take(analyser, version, 1);
							arguments.at(i) = fun->function()->versions.at(version)->type;
						}
					}
				}
				if (Type::list_may_be_compatible(implementation.type.arguments(), arguments)) {
					best = &implementation;
				}
			}
		}
		return best;
	} catch (std::exception&) {
		return nullptr;
	}
}

StaticMethod* LSClass::getStaticMethod(SemanticAnalyser* analyser, std::string& name, std::vector<Type> arguments) {
	try {
		StaticMethod* best = nullptr;
		for (auto& implementation : static_methods.at(name)) {
			for (size_t i = 0; i < std::min(implementation.type.arguments().size(), arguments.size()); ++i) {
				const auto a = arguments.at(i);
				const auto implem_arg = implementation.type.arguments().at(i);
				if (auto fun = dynamic_cast<const Function_type*>(a._types[0])) {
					if (fun->function() and implem_arg.is_function()) {
						auto version = implem_arg.arguments();
						((Function*) fun->function())->will_take(analyser, version, 1);
						arguments.at(i) = fun->function()->versions.at(version)->type;
					}
				}
			}
			if (Type::list_may_be_compatible(implementation.type.arguments(), arguments)) {
				best = &implementation;
			}
		}
		return best;
	} catch (std::exception&) {
		return nullptr;
	}
}

LSFunction* LSClass::getDefaultMethod(const std::string& name) {
	try {
		auto f = static_fields.at(name);
		f.value->refs++;
		return (LSFunction*) f.value;
	} catch (...) {
		return nullptr;
	}
}

const LSClass::Operator* LSClass::getOperator(std::string& name, Type& obj_type, Type& operand_type) {
	// std::cout << "getOperator(" << name << ", " << obj_type << ", " << operand_type << ")" << std::endl;
	if (name == "is not") name = "!=";
	std::vector<const Operator*> implementations;
	if (operators.find(name) != operators.end()) {
		for (const auto& i : operators.at(name)) implementations.push_back(&i);
	}
	auto parent = name == "Value" ? nullptr : LSValue::ValueClass;
	if (parent && parent->operators.find(name) != parent->operators.end()) {
		for (const auto& i : parent->operators.at(name)) implementations.push_back(&i);
	}
	const Operator* best = nullptr;
	int best_score = std::numeric_limits<int>::max();
	for (const Operator* m : implementations) {
		auto d1 = obj_type.distance(m->object_type);
		auto d2 = operand_type.distance(m->operand_type);
		if (d1 >= 0 and d2 >= 0) {
			int score = d1 + d2;
			// std::cout << " + " << m->object_type << ", " << m->operand_type << " / " << score << std::endl;
			if (best == nullptr or score <= best_score) {
				best_score = score;
				best = m;
			}
			// oppa oppa gangnam style tetetorettt tetetorett ! blank pink in the areaaahhh !! bombayah bomm bayah bom bayahh yah yahh yahhh yahh ! bom bom ba BOMBAYAH !!!ya ya ya ya ya ya OPPA !!
		} else {
			// std::cout << " - " << m->object_type << ", " << m->operand_type << std::endl;
		}
	}
	// if (best) std::cout << " = " << best->object_type << ", " << best->operand_type << std::endl;
	return best;
}

bool LSClass::to_bool() const {
	return true;
}

bool LSClass::ls_not() const {
	return false;
}

bool LSClass::eq(const LSValue* v) const {
	if (auto clazz = dynamic_cast<const LSClass*>(v)) {
		return clazz->name == this->name;
	}
	return false;
}

bool LSClass::lt(const LSValue* v) const {
	if (auto clazz = dynamic_cast<const LSClass*>(v)) {
		return this->name < clazz->name;
	}
	return LSValue::lt(v);
}

LSValue* LSClass::attr(const std::string& key) const {
	if (key == "name") {
		return new LSString(name);
	}
	try {
		return static_fields.at(key).value;
	} catch (std::exception&) {
		return LSValue::attr(key);
	}
}

std::ostream& LSClass::dump(std::ostream& os, int) const {
	os << "<class " << name << ">";
	return os;
}

std::string LSClass::json() const {
	return "\"<class " + name + ">\"";
}

LSValue* LSClass::getClass() const {
	return LSClass::clazz;
}

}
