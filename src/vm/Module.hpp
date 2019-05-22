#ifndef MODULE_HPP_
#define MODULE_HPP_

#include <string>
#include <vector>
#include "../type/Type.hpp"
#include "value/LSClass.hpp"
#include "../compiler/Compiler.hpp"
#include "TypeMutator.hpp"

namespace ls {

class LSValue;
class CallableVersion;

class ModuleStaticField {
public:
	std::string name;
	Type type;
	std::function<Compiler::value(Compiler&)> fun = nullptr;
	void* native_fun = nullptr;
	void* addr = nullptr;
	LSValue* value = nullptr;

	ModuleStaticField(std::string name, Type type, LSValue* value) : name(name), type(type), value(value) {}
	ModuleStaticField(std::string name, Type type, std::function<Compiler::value(Compiler&)> fun) : name(name), type(type), fun(fun) {}
	ModuleStaticField(std::string name, Type type, void* fun) : name(name), type(type), native_fun(fun) {}
	ModuleStaticField(std::string name, Type type, void* addr, bool) : name(name), type(type), addr(addr) {}
};

class Module;

class Template {
public:
	Module* module;
	std::vector<Type> templates;
	template<class... Args>
	Template(Module* module, Args... templates) : module(module), templates({templates...}) {}

	void operator_(std::string name, std::initializer_list<CallableVersion>);

	void method(std::string name, std::initializer_list<CallableVersion> methods);
};

class Module {
public:
	static int THROWS;
	static int LEGACY;

	std::string name;
	LSClass* clazz;

	Module(std::string name);
	virtual ~Module();

	void operator_(std::string name, std::initializer_list<CallableVersion>, std::vector<Type> templates = {});

	template<class... Args>
	Template template_(Args... templates) {
		return { this, templates... };
	}

	void constructor_(std::initializer_list<CallableVersion> methods);

	void method(std::string name, std::initializer_list<CallableVersion> methods, std::vector<Type> templates = {});

	void field(std::string name, Type type);
	void field(std::string name, Type type, std::function<Compiler::value(Compiler&, Compiler::value)> fun);
	void field(std::string name, Type type, void* fun);
	void static_field(std::string name, Type type, std::function<Compiler::value(Compiler&)> fun);
	void static_field(std::string name, Type type, void* addr);
	void static_field_fun(std::string name, Type type, void* fun);

	void generate_doc(std::ostream& os, std::string translation);
};

}

#endif
