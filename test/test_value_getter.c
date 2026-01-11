#include <float.h>
#include <limits.h>
#include <math.h>

#include "test_common.h"

void test_key_get(void) {
	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");
	tini_key_t     *key = tini_section_add_key(sec, "str", "hello");
	assert_str_eq("hello", tini_key_get(key, "default"));
	assert_str_eq("default", tini_key_get(NULL, "default"));

	tini_key_t *empty_key = tini_section_add_key(sec, "empty", "");
	assert_str_eq("", tini_key_get(empty_key, "default"));

	tini_destroy(ini);
}

void test_key_get_string(void) {
	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");
	tini_key_t     *key = tini_section_add_key(sec, "str", "hello");
	assert_str_eq("hello", tini_key_get_string(key, "default"));
	tini_key_t *empty = tini_section_add_key(sec, "empty", "");
	assert_str_eq("default", tini_key_get_string(empty, "default"));
	tini_key_t *blank = tini_section_add_key(sec, "blank", "   \t  ");
	assert_str_eq("default", tini_key_get_string(blank, "default"));
	tini_key_t *padded = tini_section_add_key(sec, "padded", "  hello  ");
	assert_str_eq("  hello  ", tini_key_get_string(padded, "default"));
	assert_str_eq("default", tini_key_get_string(NULL, "default"));

	tini_destroy(ini);
}

void test_key_get_int_valid(void) {
	struct {
		int         expected;
		const char *value;
	} cases[] = {
		{0, "0"},           {1, "1"},           {-1, "-1"},           {1000, "1000"},     {-42, "-42"},   {077, "077"},
		{-01000, "-01000"}, {0xFFFF, "0xFFFF"}, {-0xFFFF, "-0xFFFF"}, {0x4242, "0x4242"}, {0xFF, "0xff"},
	};

	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
		tini_key_t *key = tini_section_add_key(sec, "int_val", cases[i].value);
		assert_not_null(key);
		assert_int_eq(cases[i].expected, tini_key_get_int(key, -999));
	}

	tini_destroy(ini);
}

void test_key_get_int_invalid(void) {
	const char *bad_values[] = {"", "notanumber", "0x", "k2000", "   ", "0xG1"};

	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	for (size_t i = 0; i < sizeof(bad_values) / sizeof(bad_values[0]); ++i) {
		tini_key_t *key = tini_section_add_key(sec, "bad_int", bad_values[i]);
		assert_int_eq((int)i, tini_key_get_int(key, i));
	}
	assert_int_eq(-999, tini_key_get_int(NULL, -999));

	tini_destroy(ini);
}

void test_key_get_i64_valid(void) {
	struct {
		int64_t     expected;
		const char *value;
	} cases[] = {
		{0, "0"},
		{-1, "-1"},
		{INT64_MAX, "9223372036854775807"},
		{INT64_MIN, "-9223372036854775808"},
		{0x1234ABCD, "0x1234ABCD"},
		{0755, "0755"},
		{123, "123  "},
	};

	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
		tini_key_t *key = tini_section_add_key(sec, "i64", cases[i].value);
		assert_int64_eq(cases[i].expected, tini_key_get_i64(key, -999));
	}

	tini_destroy(ini);
}

void test_key_get_i64_invalid(void) {
	const char *bad_values[] = {"", "abc", "123abc", "0x", "0xGGG", "   "};

	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	for (size_t i = 0; i < sizeof(bad_values) / sizeof(bad_values[0]); ++i) {
		tini_key_t *key = tini_section_add_key(sec, "bad", bad_values[i]);
		assert_int64_eq(-999, tini_key_get_i64(key, -999));
	}
	assert_int64_eq(-999, tini_key_get_i64(NULL, -999));

	tini_destroy(ini);
}

void test_key_get_u64_valid(void) {
	struct {
		uint64_t    expected;
		const char *value;
	} cases[] = {
		{0, "0"},
		{UINT64_MAX, "18446744073709551615"},
		{0xFFFFFFFFFFFFFFFFULL, "0xFFFFFFFFFFFFFFFF"},
		{0755, "0755"},
		{123, "123  "},
	};

	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
		tini_key_t *key = tini_section_add_key(sec, "u64", cases[i].value);
		assert_uint64_eq(cases[i].expected, tini_key_get_u64(key, 999));
	}

	tini_destroy(ini);
}

void test_key_get_u64_invalid(void) {
	const char *bad_values[] = {"", "-1", "-0", "abc", "123abc", "   "};

	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	for (size_t i = 0; i < sizeof(bad_values) / sizeof(bad_values[0]); ++i) {
		tini_key_t *key = tini_section_add_key(sec, "bad", bad_values[i]);
		assert_uint64_eq(999, tini_key_get_u64(key, 999));
	}
	assert_uint64_eq(999, tini_key_get_u64(NULL, 999));

	tini_destroy(ini);
}

void test_key_get_double_valid(void) {
	struct {
		double      expected;
		const char *value;
	} cases[] = {
		{0.0, "0"},
		{-0.0, "-0"},
		{1.0, "1.0"},
		{3.1415, "3.1415"},
		{6.6655957, "6.6655957"},
		{-123456789.123456789, "-123456789.123456789"},
		{1e10, "1e10"},
		{1.5e-5, "1.5e-5"},
	};

	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
		tini_key_t *key    = tini_section_add_key(sec, "double_val", cases[i].value);
		double      result = tini_key_get_double(key, -999.0);
		double      diff   = fabs(result - cases[i].expected);
		assert_true(diff < 1e-6);
	}

	tini_destroy(ini);
}

void test_key_get_double_invalid(void) {
	const char *bad_values[] = {"foo", "not_a_number", "NaN_text"};

	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	const double DEFAULT = 42.42;

	for (size_t i = 0; i < sizeof(bad_values) / sizeof(bad_values[0]); ++i) {
		tini_key_t *key    = tini_section_add_key(sec, "bad_double", bad_values[i]);
		double      result = tini_key_get_double(key, DEFAULT);
		assert_float64_le(fabs(result - DEFAULT), 1e-9);
	}
	assert_float64_le(fabs(tini_key_get_double(NULL, DEFAULT) - DEFAULT), 1e-9);

	tini_destroy(ini);
}

void test_key_get_bool_true(void) {
	const char *true_values[] = {
		"1", "true", "t", "TRUE", "T", "yes", "y", "YES", "Y",
	};

	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	for (size_t i = 0; i < sizeof(true_values) / sizeof(true_values[0]); ++i) {
		tini_key_t *key = tini_section_add_key(sec, "bool_val", true_values[i]);
		assert_true(tini_key_get_bool(key, false));
	}

	tini_destroy(ini);
}

void test_key_get_bool_false(void) {
	const char *false_values[] = {
		"0", "false", "f", "FALSE", "F", "no", "n", "NO", "N",
	};

	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	for (size_t i = 0; i < sizeof(false_values) / sizeof(false_values[0]); ++i) {
		tini_key_t *key = tini_section_add_key(sec, "bool_val", false_values[i]);
		assert_false(tini_key_get_bool(key, true));
	}

	tini_destroy(ini);
}

void test_key_get_bool_invalid(void) {
	const char *invalid_values[] = {"", "m'kay", "42", "_true", "maybe"};

	tini_ptr_t      ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	for (size_t i = 0; i < sizeof(invalid_values) / sizeof(invalid_values[0]); ++i) {
		tini_key_t *key = tini_section_add_key(sec, "bad_bool", invalid_values[i]);
		assert_true(tini_key_get_bool(key, true));
		assert_false(tini_key_get_bool(key, false));
	}
	assert_true(tini_key_get_bool(NULL, true));
	assert_false(tini_key_get_bool(NULL, false));

	tini_destroy(ini);
}

int main(void) {
	cunit_init();

	CUNIT_SUITE_BEGIN("String Getters", NULL, NULL)
	CUNIT_TEST("tini_key_get", test_key_get)
	CUNIT_TEST("tini_key_get_string", test_key_get_string)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Integer Getters", NULL, NULL)
	CUNIT_TEST("int valid", test_key_get_int_valid)
	CUNIT_TEST("int invalid", test_key_get_int_invalid)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("64-bit Integer Getters", NULL, NULL)
	CUNIT_TEST("i64 valid", test_key_get_i64_valid)
	CUNIT_TEST("i64 invalid", test_key_get_i64_invalid)
	CUNIT_TEST("u64 valid", test_key_get_u64_valid)
	CUNIT_TEST("u64 invalid", test_key_get_u64_invalid)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Double Getters", NULL, NULL)
	CUNIT_TEST("double valid", test_key_get_double_valid)
	CUNIT_TEST("double invalid", test_key_get_double_invalid)
	CUNIT_SUITE_END()

	CUNIT_SUITE_BEGIN("Boolean Getters", NULL, NULL)
	CUNIT_TEST("bool true values", test_key_get_bool_true)
	CUNIT_TEST("bool false values", test_key_get_bool_false)
	CUNIT_TEST("bool invalid values", test_key_get_bool_invalid)
	CUNIT_SUITE_END()

	return cunit_run();
}
