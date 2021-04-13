#include "Test.hpp"

void Test::test_json() {

	header("Json");

	section("Json.encode()");
	code("Json.encode()").error(ls::Error::Type::WRONG_ARGUMENT_COUNT, {"Json.encode", "1", "0"});
	// integer
	code("Json.encode(0)").equals("'0'");
	code("Json.encode(12)").equals("'12'");
	code("Json.encode(-589)").equals("'-589'");
	// real
	code("Json.encode(54.123)").equals("'54.123'");
	code("Json.encode(-65.89)").equals("'-65.89'");
	// long
	code("Json.encode(1l)").equals("'1'");
	code("Json.encode(1234567890987)").equals("'1234567890987'");
	// mpz
	code("Json.encode(1m)").equals("'1'");
	code("Json.encode(123456789098712345678909871234567890987m)").equals("'123456789098712345678909871234567890987'");
	code("Json.encode(15m ** 5)").equals("'759375'");
	// boolean
	code("Json.encode(true)").equals("'true'");
	code("Json.encode(false)").equals("'false'");
	code("Json.encode(12 > 5)").equals("'true'");
	// string
	code("Json.encode('')").equals("'\"\"'");
	code("Json.encode('hello')").equals("'\"hello\"'");
	// array
	DISABLED_code("Json.encode([])").equals("'[]'");
	DISABLED_code("Json.encode([1, 2, 3])").equals("'[1, 2, 3]'");
	// object
	code("Json.encode({})").equals("'{}'");
	code("Json.encode({a: 1, b: 2, c: 3})").equals("'{\"a\":1,\"b\":2,\"c\":3}'");
	code("Json.encode({hello: [], b: {d: 12}, ccccc: [1, 2, [], 4]})").equals("'{\"b\":{\"d\":12},\"ccccc\":[1, 2, [], 4],\"hello\":[]}'");
	// function : not transformable into JSON
	code("Json.encode(x -> x)").equals("''");
	DISABLED_code("Json.encode([1, x -> x, 3])").equals("'[1, 3]'");

	section("Value.json()");
	// null
	code("null.json()").equals("'null'");
	// integer
	code("0.json()").equals("'0'");
	code("12.json()").equals("'12'");
	code("(-589).json()").equals("'-589'");
	// real
	code("54.123.json()").equals("'54.123'");
	code("(-65.89).json()").equals("'-65.89'");
	// long
	code("(1l).json()").equals("'1'");
	code("1234567890987.json()").equals("'1234567890987'");
	// boolean
	code("true.json()").equals("'true'");
	code("false.json()").equals("'false'");
	code("(12 > 5).json()").equals("'true'");
	// string
	code("''.json()").equals("'\"\"'");
	code("'hello'.json()").equals("'\"hello\"'");
	// array
	DISABLED_code("[].json()").equals("'[]'");
	DISABLED_code("[1, 2, 3].json()").equals("'[1, 2, 3]'");
	DISABLED_code("['a', 'b', 'c'].json()").equals("'[\"a\", \"b\", \"c\"]'");
	// set
	code("<1, 2, 3>.json()").equals("'[1, 2, 3]'");
	code("<9.99>.json()").equals("'[9.990000]'");
	code("<'a', 'b', 'c'>.json()").equals("'[\"a\", \"b\", \"c\"]'");
	// map
	code("[1: 1].json()").equals("'{\"1\": 1}'");
	code("['1': 1].json()").equals("'{\"1\": 1}'");
	code("['a': 'b'].json()").equals("'{\"a\": \"b\"}'");
	// object
	code("{}.json()").equals("'{}'");
	code("{a: 1, b: 2, c: 3}.json()").equals("'{\"a\":1,\"b\":2,\"c\":3}'");
	code("{hello: [], b: {d: 12}, ccccc: [1, 2, [], 4]}.json()").equals("'{\"b\":{\"d\":12},\"ccccc\":[1, 2, [], 4],\"hello\":[]}'");
	// class
	code("Number.json()").equals("'\"<class Number>\"'");

	section("Json.decode()");
	code("Json.decode('')").equals("null");
	code("Json.decode('null')").equals("null");
	code("Json.decode('true')").equals("true");
	code("Json.decode('false')").equals("false");

	code("Json.decode('12')").equals("12");
	code("Json.decode('-589')").equals("-589");
	code("Json.decode('54.123')").equals("54.123");
	code("Json.decode('-65.89')").equals("-65.89");
	code("Json.decode('1234567890987')").equals("1234567890987");

	code("Json.decode('\"\"')").equals("''");
	code("Json.decode('\"hello\"')").equals("'hello'");

	code("Json.decode('[]')").equals("[]");
	code("Json.decode('[1,2,3]')").equals("[1, 2, 3]");
	code("Json.decode('[1.6,2.1,3.77]')").equals("[1.6, 2.1, 3.77]");
	code("Json.decode('[\"a\",\"b\",\"c\"]')").equals("['a', 'b', 'c']");
	code("Json.decode('[[],[[],[]],[]]')").equals("[[], [[], []], []]");

	code("Json.decode('{}')").equals("{}");
	code("Json.decode('{\"a\":1,\"b\":2,\"c\":3}')").equals("{a: 1, b: 2, c: 3}");
	code("Json.decode('{\"b\":{\"d\":12},\"ccccc\":[1,2,[],4],\"hello\":[]}')").equals("{b: {d: 12}, ccccc: [1, 2, [], 4], hello: []}");

	section("Combinations");
	code("let v = 'salut' Json.decode(Json.encode(v)) == v").equals("true");
	DISABLED_code("let v = {b: {d: 12}, cc: [[], 4], h: []} Json.decode(Json.encode(v)) == v").equals("true");
	code("let v = 'salut' Json.encode(Json.encode(v))").equals("'\"\\\"salut\\\"\"'");

}
