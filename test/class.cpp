#include "Test.hpp"

void Test::test_classes() {

	header("Classes");
	section("type ID");
	code("Number.typeID()").equals("10");
	code("[Number, ''][0].typeID()").equals("10");
	code("Number = 12").semantic_error(ls::SemanticError::Type::CANT_MODIFY_CONSTANT_VALUE, {"Number"});

	section(".class attribute");
	code("null.class").equals("<class Null>");
	code("true.class").equals("<class Boolean>");
	code("false.class").equals("<class Boolean>");
	code("0.class").equals("<class Number>");
	code("12.class").equals("<class Number>");
	code("(-5).class").equals("<class Number>");
	code("12.5234.class").equals("<class Number>");
	code("''.class").equals("<class String>");
	code("'salut'.class").equals("<class String>");
	code("[].class").equals("<class Array>");
	code("[:].class").equals("<class Map>");
	code("<>.class").equals("<class Set>");
	code("[1, 2, 3].class").equals("<class Array>");
	code("{}.class").equals("<class Object>");
	code("{a: 12, b: 'yo'}.class").equals("<class Object>");
	code("(-> 12).class").equals("<class Function>");
	code("(x, y -> x + y).class").equals("<class Function>");
	code("12.class.class").equals("<class Class>");
	code("[null, true, 12, 'foo', [], [1:12], <>, {}, x -> x, Number] ~~ x -> x.class").equals("[<class Null>, <class Boolean>, <class Number>, <class String>, <class Array>, <class Map>, <class Set>, <class Object>, <class Function>, <class Class>]");

	section("Class.name");
	code("Null.name").equals("'Null'");
	code("Boolean.name").equals("'Boolean'");
	code("Number.name").equals("'Number'");
	code("String.name").equals("'String'");
	code("Array.name").equals("'Array'");
	code("Set.name").equals("'Set'");
	code("Map.name").equals("'Map'");
	code("Object.name").equals("'Object'");
	code("Function.name").equals("'Function'");
	code("Class.name").equals("'Class'");

	section("instanceof operator");
	code("12 instanceof Number").equals("true");
	code("'yo' instanceof Number").equals("false");
	code("'yo' instanceof String").equals("true");
	code("[1, 2, 3] instanceof Array").equals("true");
	code("{a: 12} instanceof Object").equals("true");
	code("(-> 12) instanceof Function").equals("true");
	code("null instanceof Null").equals("true");
	code("true instanceof Boolean").equals("true");
	code("Number instanceof Class").equals("true");

	section("Class creation");
	code("class A {} A").equals("<class A>");
	code("class A {} let a = new A a").equals("A {}");
	code("class A { let b = 2 } let a = new A a").equals("A {}");
//	code("class A { let b = 2 } let a = new A a.b").equals("2");
	code("class A { let b = 2 } let a = new A a.class").equals("<class A>");
//	code("class A {} let t = [A] let a = new t[0] a").equals("A {}");
//	code("class A {} let a = new A let b = new a.class b.class").equals("<class A>");


//	code("let my_map = [].map; my_map([1, 2, 3], x -> x ^ 2)").equals("[1, 4, 9]");
//	code("[].map == [].map").equals("true");
//	code("{}.map == {}.map").equals("true");
//	code("[].map == {}.map").equals("false");
//	code("let a = [].map; a == [].map").equals("true");

	section("Class.operator ==");
	code("Number == 12.class").equals("true");
	code("Number == 'salut'.class").equals("false");

	section("Class.operator <");
	code("Number < String").equals("true");
	code("Number < Array").equals("false");

	section("Class::isTrue()");
	code("Number ? 12 : 5").equals("12");
	code("!(!String)").equals("true");

	section("Class::clone()");
	code("var a = Number a").equals("<class Number>");
}
