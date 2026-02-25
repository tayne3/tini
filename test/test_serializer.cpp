#include "common.hpp"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE("Basic Save - Save basic", "[serializer]") {
	const char *path = test_tmp_path("save_basic.ini");

	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");
	tini_section_add_key(sec, "key1", "value1");
	tini_section_add_key(sec, "key2", "value2");

	REQUIRE(tini_save_to(ini, path) == 0);

	tini_destroy(ini);
	remove(path);
}

TEST_CASE("Basic Save - Save multiple sections", "[serializer]") {
	const char *path = test_tmp_path("save_multi.ini");

	tini_t *ini = tini_empty();

	tini_section_t *sec1 = tini_get_section(ini, "section1");
	tini_section_add_key(sec1, "key1", "value1");

	tini_section_t *sec2 = tini_get_section(ini, "section2");
	tini_section_add_key(sec2, "key2", "value2");

	tini_section_t *sec3 = tini_get_section(ini, "section3");
	tini_section_add_key(sec3, "key3", "value3");

	REQUIRE(tini_save_to(ini, path) == 0);

	tini_destroy(ini);
	remove(path);
}

TEST_CASE("Round-trip - Round-trip basic", "[serializer]") {
	const char     *path = test_tmp_path("roundtrip.ini");
	tini_t         *ini1 = tini_empty();
	tini_section_t *sec1 = tini_get_section(ini1, "data");
	tini_section_add_key(sec1, "name", "test");
	tini_section_add_key(sec1, "count", "42");
	tini_section_add_key(sec1, "ratio", "3.14159");
	tini_section_add_key(sec1, "enabled", "true");

	REQUIRE(tini_save_to(ini1, path) == 0);
	tini_destroy(ini1);
	tini_t *ini2 = tini_create(path);
	REQUIRE(ini2 != nullptr);

	tini_section_t *sec2 = tini_find_section(ini2, "data");
	REQUIRE(sec2 != nullptr);

	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec2, "name"))) == "test");
	REQUIRE(tini_key_get_int(tini_section_find_key(sec2, "count"), 0) == 42);
	REQUIRE(tini_key_get_bool(tini_section_find_key(sec2, "enabled"), false) == true);

	tini_destroy(ini2);
	remove(path);
}

TEST_CASE("Round-trip - Round-trip special chars", "[serializer]") {
	const char     *path = test_tmp_path("roundtrip_special.ini");
	tini_t         *ini1 = tini_empty();
	tini_section_t *sec1 = tini_get_section(ini1, "special");
	tini_section_add_key(sec1, "equals", "a=b=c");
	tini_section_add_key(sec1, "hash", "a#b#c");
	tini_section_add_key(sec1, "semicolon", "a;b;c");
	tini_section_add_key(sec1, "brackets", "[test]");

	REQUIRE(tini_save_to(ini1, path) == 0);
	tini_destroy(ini1);
	tini_t *ini2 = tini_create(path);
	REQUIRE(ini2 != nullptr);

	tini_section_t *sec2 = tini_find_section(ini2, "special");
	REQUIRE(sec2 != nullptr);

	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec2, "equals"))) == "a=b=c");

	tini_destroy(ini2);
	remove(path);
}

TEST_CASE("Round-trip - Round-trip empty values", "[serializer]") {
	const char *path = test_tmp_path("roundtrip_empty.ini");

	tini_t         *ini1 = tini_empty();
	tini_section_t *sec1 = tini_get_section(ini1, "empty");
	tini_section_add_key(sec1, "empty_val", "");
	tini_section_add_key(sec1, "with_val", "something");

	REQUIRE(tini_save_to(ini1, path) == 0);
	tini_destroy(ini1);

	tini_t *ini2 = tini_create(path);
	REQUIRE(ini2 != nullptr);

	tini_section_t *sec2 = tini_find_section(ini2, "empty");
	REQUIRE(sec2 != nullptr);

	tini_key_t *empty_key = tini_section_find_key(sec2, "empty_val");
	REQUIRE(empty_key != nullptr);
	REQUIRE(std::string(tini_key_get_value(empty_key)) == "");

	tini_destroy(ini2);
	remove(path);
}

TEST_CASE("Round-trip - Round-trip unicode", "[serializer]") {
	const char *path = test_tmp_path("roundtrip_unicode.ini");

	tini_t         *ini1 = tini_empty();
	tini_section_t *sec1 = tini_get_section(ini1, "unicode");
	tini_section_add_key(sec1, "chinese", "中文测试");
	tini_section_add_key(sec1, "japanese", "日本語テスト");
	tini_section_add_key(sec1, "german", "Grüße");
	tini_section_add_key(sec1, "emoji", "🎉✨");

	REQUIRE(tini_save_to(ini1, path) == 0);
	tini_destroy(ini1);

	tini_t *ini2 = tini_create(path);
	REQUIRE(ini2 != nullptr);

	tini_section_t *sec2 = tini_find_section(ini2, "unicode");
	REQUIRE(sec2 != nullptr);

	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec2, "chinese"))) == "中文测试");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec2, "japanese"))) == "日本語テスト");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec2, "german"))) == "Grüße");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec2, "emoji"))) == "🎉✨");

	tini_destroy(ini2);
	remove(path);
}

TEST_CASE("Large Files - Save large file", "[serializer]") {
	const char *path = test_tmp_path("large.ini");

	tini_t *ini = tini_empty();

	for (int i = 0; i < 50; i++) {
		char sec_name[32];
		snprintf(sec_name, sizeof(sec_name), "section_%d", i);

		tini_section_t *sec = tini_get_section(ini, sec_name);

		for (int j = 0; j < 20; j++) {
			char key[32], value[64];
			snprintf(key, sizeof(key), "key_%d", j);
			snprintf(value, sizeof(value), "value_%d_%d_with_some_extra_text", i, j);
			tini_section_add_key(sec, key, value);
		}
	}

	REQUIRE(tini_save_to(ini, path) == 0);
	tini_t *ini2 = tini_create(path);
	REQUIRE(ini2 != nullptr);

	tini_section_t *sec25 = tini_find_section(ini2, "section_25");
	REQUIRE(sec25 != nullptr);

	tini_key_t *key10 = tini_section_find_key(sec25, "key_10");
	REQUIRE(key10 != nullptr);
	REQUIRE(std::string(tini_key_get_value(key10)) == "value_25_10_with_some_extra_text");

	tini_destroy(ini);
	tini_destroy(ini2);
	remove(path);
}
