#include <cmath>
#include <string>
#include <vector>

#include "test_common.h"

static void test_cpp_basic(void) {
	tini::Ini ini;
	assert_int_eq(0, ini.lastError());
	ini.getSection("test").addKey("foo", "bar1");
	assert_str_eq("bar1", ini.getSection("test").getKey("foo").getString().c_str());
	{
		tini::Ini ini2 = std::move(ini);
		assert_str_eq("bar1", ini2.getSection("test").getKey("foo").getString().c_str());
		ini2.getSection("test").addKey("foo", "bar2");
		ini = std::move(ini2);
	}
	assert_str_eq("bar2", ini.getSection("test").getKey("foo").getString().c_str());

	auto section = ini.getSection("network");
	assert_true(section.valid());
	assert_str_eq("network", section.name().c_str());

	section.addKey("host", "localhost");
	assert_true(section.hasKey("host"));
	assert_false(section.hasKey("port"));

	section.addKey("int", "42");
	section.addKey("i64", "-1");
	section.addKey("u64", "121");
	section.addKey("double", "3.14");
	section.addKey("true", "true");
	section.addKey("false", "off");

	assert_int_eq(42, section.getKey("int").getInt());
	assert_int64_eq(-1, section.getKey("i64").getI64());
	assert_uint64_eq(121, section.getKey("u64").getU64());
	assert_double_eq(3.14, section.getKey("double").getDouble());
	assert_true(section.getKey("true").getBool());
	assert_false(section.getKey("false").getBool());

	const char *path = test_tmp_path("cpp-test.ini");
	ini.saveTo(path);

	tini::Ini ini3;
	ini3.load(path);
	section = ini3.getSection("network");

	assert_int_eq(42, ini3.getKey("network", "int").getInt());
	assert_int64_eq(-1, ini3.getKey("network", "i64").getI64());
	assert_uint64_eq(121, ini3.getKey("network", "u64").getU64());
	assert_double_eq(3.14, ini3.getKey("network", "double").getDouble());
	assert_true(ini3.getKey("network", "true").getBool());
	assert_false(ini3.getKey("network", "false").getBool());

	remove(path);
}

int main(void) {
	cunit_init();

	CUNIT_SUITE_BEGIN("C++ API", NULL, NULL)
	CUNIT_TEST("Basic", test_cpp_basic)
	CUNIT_SUITE_END()

	return cunit_run();
}
