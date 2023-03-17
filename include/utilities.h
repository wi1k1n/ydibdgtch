#ifndef UTILITIES_H__
#define UTILITIES_H__

#include "Arduino.h"

#include <iterator>
#include <tuple>

template<std::size_t N, class T>
constexpr std::size_t countof(T(&)[N]) { return N; }

// https://stackoverflow.com/a/29451566/5765191
template<typename Range1, typename Range2, typename OutputIterator>
void cartesianProductIt(Range1 const &r1, Range2 const &r2, OutputIterator out) {
    using std::begin; using std::end;
    for (auto i = begin(r1); i != end(r1); ++i)
        for (auto j = begin(r2); j != end(r2); ++j)
            *out++ = std::make_tuple(*i, *j);
}
template<typename T, typename U, typename OutputIterator>
void cartesianProduct(const std::initializer_list<T>& v1, const std::initializer_list<U>& v2, OutputIterator out) {
	cartesianProductIt(v1, v2, out);
}

#endif // UTILITIES_H__