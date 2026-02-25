#include "common.hpp"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE("Get/Find Section - Get section creates", "[section_api]") {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_get_section(ini, "new_section");
	REQUIRE(sec != nullptr);
	tini_section_t *found = tini_find_section(ini, "new_section");
	REQUIRE(sec == found);

	tini_destroy(ini);
}

TEST_CASE("Get/Find Section - Find section does not create", "[section_api]") {
	tini_t         *ini = tini_empty();
	tini_section_t *sec = tini_find_section(ini, "nonexistent");
	REQUIRE(sec == nullptr);
	tini_destroy(ini);
}

TEST_CASE("Get/Find Section - Section case insensitivity", "[section_api]") {
	tini_t *ini = tini_empty();

	tini_section_t *sec = tini_get_section(ini, "TestSection");
	tini_section_add_key(sec, "key", "value");
	REQUIRE(tini_find_section(ini, "TestSection") != nullptr);
	REQUIRE(tini_find_section(ini, "testsection") != nullptr);
	REQUIRE(tini_find_section(ini, "TESTSECTION") != nullptr);
	REQUIRE(tini_find_section(ini, "TeStsEcTiOn") != nullptr);
	REQUIRE(sec == tini_find_section(ini, "testsection"));

	tini_destroy(ini);
}

TEST_CASE("Get/Find Section - Section empty name", "[section_api]") {
	tini_t         *ini       = tini_empty();
	tini_section_t *sec_empty = tini_get_section(ini, "");
	REQUIRE(sec_empty != nullptr);
	tini_section_t *sec_null = tini_get_section(ini, nullptr);
	REQUIRE(sec_null == nullptr);

	tini_destroy(ini);
}

TEST_CASE("Get/Find Section - Section complex names", "[section_api]") {
	const char *names[] = {
		"Section One", "Section_Two", "Section-Three", "Sec.tion.Four", "Sec@tion#Five",
	};

	tini_t *ini = tini_empty();

	for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
		tini_section_t *sec = tini_get_section(ini, names[i]);
		REQUIRE(sec != nullptr);
		tini_section_add_key(sec, "test", "value");

		tini_section_t *found = tini_find_section(ini, names[i]);
		REQUIRE(sec == found);
	}

	tini_destroy(ini);
}

TEST_CASE("Remove Section - Remove section", "[section_api]") {
	tini_t *ini = tini_empty();

	tini_section_t *sec = tini_get_section(ini, "to_remove");
	tini_section_add_key(sec, "key", "value");

	REQUIRE(tini_find_section(ini, "to_remove") != nullptr);

	REQUIRE(tini_remove_section(ini, "to_remove") == 0);

	REQUIRE(tini_find_section(ini, "to_remove") == nullptr);

	tini_destroy(ini);
}

TEST_CASE("Remove Section - Remove section not found", "[section_api]") {
	tini_t *ini    = tini_empty();
	int     result = tini_remove_section(ini, "nonexistent");
	REQUIRE(result != 0);
	tini_destroy(ini);
}

TEST_CASE("Remove Section - Remove section case insensitive", "[section_api]") {
	tini_t *ini = tini_empty();

	tini_get_section(ini, "MySection");
	REQUIRE(tini_remove_section(ini, "mysection") == 0);

	REQUIRE(tini_find_section(ini, "MySection") == nullptr);

	tini_destroy(ini);
}

TEST_CASE("Bulk Operations - Many sections", "[section_api]") {
	tini_t *ini = tini_empty();

	for (int i = 0; i < 100; i++) {
		char name[32];
		snprintf(name, sizeof(name), "section_%d", i);

		tini_section_t *sec = tini_get_section(ini, name);
		REQUIRE(sec != nullptr);

		for (int j = 0; j < 10; j++) {
			char key[32], value[32];
			snprintf(key, sizeof(key), "key_%d", j);
			snprintf(value, sizeof(value), "value_%d_%d", i, j);
			tini_section_add_key(sec, key, value);
		}
	}
	tini_section_t *sec50 = tini_find_section(ini, "section_50");
	REQUIRE(sec50 != nullptr);

	tini_key_t *key5 = tini_section_find_key(sec50, "key_5");
	REQUIRE(key5 != nullptr);
	REQUIRE(std::string(tini_key_get_value(key5)) == "value_50_5");

	tini_destroy(ini);
}
