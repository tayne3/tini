# tini

![CMake](https://img.shields.io/badge/CMake-3.14%2B-brightgreen?logo=cmake&logoColor=white)
[![Release](https://img.shields.io/github/v/release/tayne3/tini?include_prereleases&label=release&logo=github&logoColor=white)](https://github.com/tayne3/tini/releases)
[![Tag](https://img.shields.io/github/v/tag/tayne3/tini?color=%23ff8936&style=flat-square&logo=git&logoColor=white)](https://github.com/tayne3/tini/tags)
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/tayne3/tini)

[English](README.md) | **中文**

tini (Tiny INI) 是一个用于读写 INI 配置文件的 C 语言库。

## ✨ 特性

- 轻量级和单文件,使用 ANSI C(C99)标准编写,无任何依赖
- 简单易用的接口设计, 易于维护

## ⌛️ 构建

tini 使用 CMake 作为构建系统，支持跨平台编译。

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

## 📚 使用示例

**初始化**:

```c
tini_t *tini = tini_create("config.ini");
```

**读取值**:

使用 tini_group_begin 进入某个组, 然后使用 tini_value_get 获取指定键名的键值, 若键名不存在则使用默认值。

```c
tini_key_t *key = tini_find_key(tini, "section1", "key1");
const char *value = tini_key_get(key, "default");
```

**解析设置**:

`tini_set_delim()` 用于设置键值分隔符，默认值为 `=`。
`tini_set_nosection()` 用于设置是否启用无节键值（第一节之前的键值）。

**示例代码**:

```c
#include "tini/tini.h"
#include <stdio.h>

int main(void) {
    tini_t *tini = tini_create("/path/to/config.ini");
    if (!tini) {
        return 1;
    }

    tini_section_t *section = tini_find_section(tini, "section1");

    tini_key_t *key = tini_section_find_key(section, "key1");
    printf("section1-key1: %s\n", tini_key_get(key, "default-value"));
    tini_key_set_value(key, "value1");

    tini_section_add_key(section, "key2", "value2");

    tini_save_to(tini, "/path/to/config.ini");
    tini_destroy(tini);
    return 0;
}
```

**C++ 示例**:

```cpp
#include <iostream>

#include "tini/tini.h"

int main() {
	try {
		tini::Ini ini;
		auto      section = ini.getSection("section");
		section.addKey("key", "1");
		section.addKey("enabled", "true").setValue(true);
		section.addKey("timeout").setValue(30.5);
		section.addKey("name", "tini");

		auto key = ini.getKey("section", "key");
		std::cout << "Int value: " << key.getInt(0) << std::endl;
		std::cout << "Bool value: " << ini.findKey("section", "enabled").getBool(false) << std::endl;
		std::cout << "Double value: " << ini.findSection("section").findKey("timeout").getDouble(0.0) << std::endl;
		std::cout << "String value: " << ini.findKey("section", "name").getString("default") << std::endl;

		std::cout << std::endl << "Total sections: " << ini.sections().size() << std::endl;
		for (auto s = ini.firstSection(); s; s = s.next()) {
			std::cout << "[" << s.name() << "]" << std::endl;
			for (auto k = s.firstKey(); k; k = k.next()) {
				std::cout << "  " << k.name() << " = " << k.getValue() << std::endl;
			}
		}
		return 0;
	} catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}
}
```
