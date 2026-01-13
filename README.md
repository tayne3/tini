# tini

![CMake](https://img.shields.io/badge/CMake-3.14%2B-brightgreen?logo=cmake&logoColor=white)
[![Release](https://img.shields.io/github/v/release/tayne3/tini?include_prereleases&label=release&logo=github&logoColor=white)](https://github.com/tayne3/tini/releases)
[![Tag](https://img.shields.io/github/v/tag/tayne3/tini?color=%23ff8936&style=flat-square&logo=git&logoColor=white)](https://github.com/tayne3/tini/tags)
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/tayne3/tini)

**English** | [中文](README_zh.md)

tini (Tiny INI) is a C library for reading and writing INI configuration files.

## ✨ Features

- Lightweight, written in ANSI C (C99), with no external dependencies.
- Simple and easy-to-use interface, designed for maintainability.

## ⌛️ Build

tini uses CMake as the build system and supports cross-platform compilation.

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

## 📚 Usage Examples

**Initialization**:

```c
tini_t * tini = tini_create("config.ini");
```

**Reading Values**:

First find the section, then find the key to retrieve its value (returns `NULL` if not found).

```c
tini_key_t *key = tini_find_key(tini, "section1", "key1");
const char *value = tini_key_get(key, "default");
```

**Full Example**:

```c
#include "tini/tini.h"
#include <stdio.h>

int main(void) {
    tini_t * tini = tini_create("/path/to/config.ini");
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
