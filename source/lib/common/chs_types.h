#pragma once

#ifdef TARGET_WIN
#include <stdint.h>
#include <cctype>

inline bool IsAlphaNumeric(char c) { return std::isalnum(c); }
inline bool IsAlpha(char c) { return std::isalpha(c); }
inline bool IsAscii(char c) { return isascii(c); }
inline bool IsWhitespace(char c) { return std::isblank(c); }
inline bool IsControl(char c) { return std::iscntrl(c); }
inline bool IsDigit(char c) { return std::isdigit(c); }
inline bool IsGraph(char c) { return std::isgraph(c); }
inline bool IsLowerCase(char c) { return std::islower(c); }
inline bool IsPrintable(char c) { return std::isprint(c); }
inline bool IsPunct(char c) { return std::ispunct(c); }
inline bool IsSpace(char c) { return std::isspace(c); }
inline bool IsUpperCase(char c) { return std::isupper(c); }
inline bool IsHexadecimalDigit(char c) { return std::isxdigit(c); }

#endif

#include <vector>
#include <unordered_map>

template <typename T>
using Array = std::vector<T>;

template <typename KEY, typename VALUE>
using Hashmap = std::unordered_map<KEY, VALUE>;

template <typename T1, typename T2>
using Pair = std::pair<T1, T2>;