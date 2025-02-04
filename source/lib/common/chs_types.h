#pragma once

#ifdef TARGET_WIN
#include <stdint.h>
#include <cctype>

inline bool isAlphaNumeric(char c) { return std::isalnum(c); }
inline bool isAlpha(char c) { return std::isalpha(c); }
inline bool isAscii(char c) { return isascii(c); }
inline bool isWhitespace(char c) { return std::isblank(c); }
inline bool isControl(char c) { return std::iscntrl(c); }
inline bool isDigit(char c) { return std::isdigit(c); }
inline bool isGraph(char c) { return std::isgraph(c); }
inline bool isLowerCase(char c) { return std::islower(c); }
inline bool isPrintable(char c) { return std::isprint(c); }
inline bool isPunct(char c) { return std::ispunct(c); }
inline bool isSpace(char c) { return std::isspace(c); }
inline bool isUpperCase(char c) { return std::isupper(c); }
inline bool isHexadecimalDigit(char c) { return std::isxdigit(c); }

#endif

#include <vector>
#include <unordered_map>

template <typename T>
using Array = std::vector<T>;

template <typename KEY, typename VALUE>
using Hashmap = std::unordered_map<KEY, VALUE>;

template <typename T1, typename T2>
using Pair = std::pair<T1, T2>;