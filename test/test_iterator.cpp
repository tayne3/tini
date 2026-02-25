#include "common.hpp"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE("Section Iteration - Empty config", "[iterator]") {
	tini_t *ini = tini_empty();

	tini_section_t *sec = tini_first_section(ini);
	REQUIRE(sec == nullptr);

	tini_destroy(ini);
}

TEST_CASE("Section Iteration - Single section", "[iterator]") {
	tini_t *ini = tini_empty();
	tini_get_section(ini, "only");

	tini_section_t *sec = tini_first_section(ini);
	REQUIRE(sec != nullptr);
	REQUIRE(std::string(tini_section_name(sec)) == "only");

	REQUIRE(tini_section_next(sec) == nullptr);

	tini_destroy(ini);
}

TEST_CASE("Section Iteration - Multiple sections", "[iterator]") {
	tini_t *ini = tini_empty();

	tini_get_section(ini, "a");
	tini_get_section(ini, "b");
	tini_get_section(ini, "c");

	int count = 0;
	for (tini_section_t *sec = tini_first_section(ini); sec; sec = tini_section_next(sec)) {
		REQUIRE(tini_section_name(sec) != nullptr);
		count++;
	}
	REQUIRE(count == 3);

	tini_destroy(ini);
}

TEST_CASE("Section Iteration - Section name", "[iterator]") {
	tini_t *ini = tini_empty();

	tini_section_t *sec = tini_get_section(ini, "TestSection");
	REQUIRE(std::string(tini_section_name(sec)) == "TestSection");

	tini_section_t *empty_sec = tini_get_section(ini, "");
	REQUIRE(std::string(tini_section_name(empty_sec)) == "");

	tini_destroy(ini);
}

TEST_CASE("Section Iteration - NULL safety", "[iterator]") {
	REQUIRE(tini_first_section(nullptr) == nullptr);
	REQUIRE(tini_section_next(nullptr) == nullptr);
	REQUIRE(tini_section_name(nullptr) == nullptr);
}

TEST_CASE("Key Iteration - Empty section", "[iterator]") {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key = tini_section_first_key(sec);
	REQUIRE(key == nullptr);

	tini_destroy(ini);
}

TEST_CASE("Key Iteration - Single key", "[iterator]") {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");
	tini_section_add_key(sec, "only_key", "value");

	tini_key_t *key = tini_section_first_key(sec);
	REQUIRE(key != nullptr);
	REQUIRE(std::string(tini_key_name(key)) == "only_key");
	REQUIRE(std::string(tini_key_get_value(key)) == "value");

	REQUIRE(tini_key_next(key) == nullptr);

	tini_destroy(ini);
}

TEST_CASE("Key Iteration - Multiple keys", "[iterator]") {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_section_add_key(sec, "key1", "v1");
	tini_section_add_key(sec, "key2", "v2");
	tini_section_add_key(sec, "key3", "v3");

	int count = 0;
	for (tini_key_t *key = tini_section_first_key(sec); key; key = tini_key_next(key)) {
		REQUIRE(tini_key_name(key) != nullptr);
		count++;
	}
	REQUIRE(count == 3);

	tini_destroy(ini);
}

TEST_CASE("Key Iteration - Key name", "[iterator]") {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "test");

	tini_key_t *key = tini_section_add_key(sec, "MyKey", "value");
	REQUIRE(std::string(tini_key_name(key)) == "MyKey");

	tini_destroy(ini);
}

TEST_CASE("Key Iteration - NULL safety", "[iterator]") {
	REQUIRE(tini_section_first_key(nullptr) == nullptr);
	REQUIRE(tini_key_next(nullptr) == nullptr);
	REQUIRE(tini_key_name(nullptr) == nullptr);
}

TEST_CASE("Full Enumeration - Nested iteration", "[iterator]") {
	tini_t *ini = tini_empty();

	for (int i = 1; i <= 3; i++) {
		char sec_name[32];
		snprintf(sec_name, sizeof(sec_name), "section%d", i);
		tini_section_t *sec = tini_get_section(ini, sec_name);

		for (int j = 1; j <= 2; j++) {
			char key_name[32], value[32];
			snprintf(key_name, sizeof(key_name), "key%d", j);
			snprintf(value, sizeof(value), "value_%d_%d", i, j);
			tini_section_add_key(sec, key_name, value);
		}
	}

	int section_count = 0;
	int total_keys    = 0;

	for (tini_section_t *sec = tini_first_section(ini); sec; sec = tini_section_next(sec)) {
		section_count++;
		for (tini_key_t *key = tini_section_first_key(sec); key; key = tini_key_next(key)) {
			total_keys++;
		}
	}

	REQUIRE(section_count == 3);
	REQUIRE(total_keys == 6);

	tini_destroy(ini);
}
