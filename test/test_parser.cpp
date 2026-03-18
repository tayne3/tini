#include "common.hpp"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

TEST_CASE("Inline Comments - Semicolon comments", "[parser]") {
	const char *content = "[comments]\n"
						  "basic = value1 ; this is a comment\n"
						  "spacing = value2   ;   spaced comment\n"
						  "no_space = value3;attached comment\n";

	test_write_temp_ini("inline_semi.ini", content);

	tini_t *ini = tini_create(test_tmp_path("inline_semi.ini"));
	REQUIRE(ini != nullptr);

	tini_section_t *sec = tini_find_section(ini, "comments");
	REQUIRE(sec != nullptr);

	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "basic"))) == "value1");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "spacing"))) == "value2");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "no_space"))) == "value3");

	tini_destroy(ini);
	test_remove_temp_ini("inline_semi.ini");
}

TEST_CASE("Inline Comments - Hash comments", "[parser]") {
	const char *content = "[comments]\n"
						  "hash = value1 # this is a comment\n"
						  "hash_attach = value2#attached\n";

	test_write_temp_ini("inline_hash.ini", content);

	tini_t *ini = tini_create(test_tmp_path("inline_hash.ini"));
	REQUIRE(ini != nullptr);

	tini_section_t *sec = tini_find_section(ini, "comments");
	REQUIRE(sec != nullptr);

	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "hash"))) == "value1");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "hash_attach"))) == "value2");

	tini_destroy(ini);
	test_remove_temp_ini("inline_hash.ini");
}

TEST_CASE("Quote Handling - Double quotes", "[parser]") {
	const char *content = "[quotes]\n"
						  "basic = \"hello world\"\n"
						  "spaces = \"  spaced  \"\n"
						  "empty = \"\"\n";

	test_write_temp_ini("double_quotes.ini", content);

	tini_t *ini = tini_create(test_tmp_path("double_quotes.ini"));
	REQUIRE(ini != nullptr);

	tini_section_t *sec = tini_find_section(ini, "quotes");
	REQUIRE(sec != nullptr);

	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "basic"))) == "hello world");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "spaces"))) == "  spaced  ");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "empty"))) == "");

	tini_destroy(ini);
	test_remove_temp_ini("double_quotes.ini");
}

TEST_CASE("Quote Handling - Single quotes", "[parser]") {
	const char *content = "[quotes]\n"
						  "basic = 'hello world'\n"
						  "spaces = '  spaced  '\n";

	test_write_temp_ini("single_quotes.ini", content);

	tini_t *ini = tini_create(test_tmp_path("single_quotes.ini"));
	REQUIRE(ini != nullptr);

	tini_section_t *sec = tini_find_section(ini, "quotes");
	REQUIRE(sec != nullptr);

	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "basic"))) == "hello world");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "spaces"))) == "  spaced  ");

	tini_destroy(ini);
	test_remove_temp_ini("single_quotes.ini");
}

TEST_CASE("Quote Handling - Quotes protect comments", "[parser]") {
	const char *content = "[quotes]\n"
						  "semi = \"val;ue\"\n"
						  "hash = 'val#ue'\n"
						  "complex = \"val # ue\" ; real comment\n";

	test_write_temp_ini("quotes_comment.ini", content);

	tini_t *ini = tini_create(test_tmp_path("quotes_comment.ini"));
	REQUIRE(ini != nullptr);

	tini_section_t *sec = tini_find_section(ini, "quotes");
	REQUIRE(sec != nullptr);

	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "semi"))) == "val;ue");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "hash"))) == "val#ue");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "complex"))) == "val # ue");

	tini_destroy(ini);
	test_remove_temp_ini("quotes_comment.ini");
}

TEST_CASE("Whitespace Handling - Whitespace trimming", "[parser]") {
	const char *content = "[whitespace]\n"
						  "  key1  =  value1  \n"
						  "\tkey2\t=\tvalue2\t\n";

	test_write_temp_ini("whitespace.ini", content);

	tini_t *ini = tini_create(test_tmp_path("whitespace.ini"));
	REQUIRE(ini != nullptr);

	tini_section_t *sec = tini_find_section(ini, "whitespace");
	REQUIRE(sec != nullptr);

	REQUIRE(tini_section_find_key(sec, "key1") != nullptr);
	REQUIRE(tini_section_find_key(sec, "key2") != nullptr);

	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "key1"))) == "value1");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "key2"))) == "value2");

	tini_destroy(ini);
	test_remove_temp_ini("whitespace.ini");
}

TEST_CASE("Whitespace Handling - Preserved whitespace in quotes", "[parser]") {
	const char *content = "[ws]\n"
						  "key1 = \"  spaced  \"\n"
						  "key2 = '\ttabbed\t'\n";

	test_write_temp_ini("ws_quotes.ini", content);

	tini_t *ini = tini_create(test_tmp_path("ws_quotes.ini"));
	REQUIRE(ini != nullptr);

	tini_section_t *sec = tini_find_section(ini, "ws");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "key1"))) == "  spaced  ");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "key2"))) == "\ttabbed\t");

	tini_destroy(ini);
	test_remove_temp_ini("ws_quotes.ini");
}

TEST_CASE("Encoding - UTF-8 parsing", "[parser]") {
	tini_t *ini = tini_create(test_res_path("unicode.ini"));
	REQUIRE(ini != nullptr);
	tini_destroy(ini);
}

TEST_CASE("Edge Cases - Massive line", "[parser]") {
	const size_t SIZE      = 4096;
	char        *large_val = (char *)malloc(SIZE + 1);
	memset(large_val, 'x', SIZE);
	large_val[SIZE] = '\0';

	char *line = (char *)malloc(SIZE + 100);
	sprintf(line, "key = %s\n", large_val);

	FILE *fp = fopen(test_tmp_path("massive.ini"), "w");
	if (fp) {
		fputs("[massive]\n", fp);
		fputs(line, fp);
		fclose(fp);
	}

	tini_t *ini = tini_create(test_tmp_path("massive.ini"));
	REQUIRE(ini != nullptr);

	tini_section_t *sec = tini_find_section(ini, "massive");
	REQUIRE(sec != nullptr);

	tini_key_t *key = tini_section_find_key(sec, "key");
	REQUIRE(key != nullptr);
	REQUIRE(std::string(tini_key_get_value(key)) == std::string(large_val));

	free(large_val);
	free(line);
	tini_destroy(ini);
	test_remove_temp_ini("massive.ini");
}

TEST_CASE("Edge Cases - Global keys", "[parser]") {
	const char *content = "global_key1 = value1\n"
						  "global_key2 = value2\n"
						  "[section]\n"
						  "key = value\n";

	test_write_temp_ini("global.ini", content);

	tini_t *ini = tini_create(test_tmp_path("global.ini"));
	REQUIRE(ini != nullptr);

	tini_section_t *global = tini_find_section(ini, "");
	if (global) {
		tini_key_t *gk1 = tini_section_find_key(global, "global_key1");
		if (gk1) {
			REQUIRE(std::string(tini_key_get_value(gk1)) == "value1");
		}
	}

	tini_section_t *sec = tini_find_section(ini, "section");
	REQUIRE(sec != nullptr);

	tini_destroy(ini);
	test_remove_temp_ini("global.ini");
}

TEST_CASE("Parser Config - No-section keys can be enabled explicitly", "[parser]") {
	const char *content = "global_key1 = value1\n"
						  "global_key2 = value2\n"
						  "[section]\n"
						  "key = value\n";

	test_write_temp_ini("global_options.ini", content);

	tini_t *ini = tini_empty();
	REQUIRE(ini != nullptr);

	tini_set_nosection(ini, true);
	REQUIRE(tini_load(ini, test_tmp_path("global_options.ini")) == 0);

	tini_section_t *global = tini_find_section(ini, "");
	REQUIRE(global != nullptr);
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(global, "global_key1"))) == "value1");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(global, "global_key2"))) == "value2");

	tini_section_t *sec = tini_find_section(ini, "section");
	REQUIRE(sec != nullptr);
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "key"))) == "value");

	tini_destroy(ini);
	test_remove_temp_ini("global_options.ini");
}

TEST_CASE("Parser Config - Colon delimiter can be enabled explicitly", "[parser]") {
	const char *content = "+IPR:9\n"
						  "+LFR:868\n"
						  "+PATH:C:\\\\temp\\\\file\n"
						  "[section]\n"
						  "key:value\n";

	test_write_temp_ini("colon_options.ini", content);

	tini_t *ini = tini_empty();
	REQUIRE(ini != nullptr);

	tini_set_delim(ini, ':');
	tini_set_nosection(ini, true);
	REQUIRE(tini_load(ini, test_tmp_path("colon_options.ini")) == 0);

	tini_section_t *global = tini_find_section(ini, "");
	REQUIRE(global != nullptr);
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(global, "+IPR"))) == "9");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(global, "+LFR"))) == "868");
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(global, "+PATH"))) == "C:\\\\temp\\\\file");

	tini_section_t *sec = tini_find_section(ini, "section");
	REQUIRE(sec != nullptr);
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(sec, "key"))) == "value");

	tini_destroy(ini);
	test_remove_temp_ini("colon_options.ini");
}

TEST_CASE("Parser Config - Delimiter is strict", "[parser]") {
	const char *content = "eq = value1\n"
						  "colon:value2\n";

	test_write_temp_ini("strict_delim.ini", content);

	tini_t *ini = tini_empty();
	REQUIRE(ini != nullptr);

	tini_set_nosection(ini, true);
	tini_set_delim(ini, ':');
	REQUIRE(tini_load(ini, test_tmp_path("strict_delim.ini")) == 0);

	tini_section_t *global = tini_find_section(ini, "");
	REQUIRE(global != nullptr);
	REQUIRE(tini_section_find_key(global, "eq") == nullptr);
	REQUIRE(std::string(tini_key_get_value(tini_section_find_key(global, "colon"))) == "value2");

	tini_destroy(ini);
	test_remove_temp_ini("strict_delim.ini");
}
