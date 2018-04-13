/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

#ifndef TRIPLE_H
#define TRIPLE_H

#include <bool.h>

template <class T1, class T2, class T3>
struct triple {
    T1 first;
    T2 second;
    T3 third;
    triple(const T1& a, const T2& b, const T3& c) :
			first(a), second(b), third(c) {}
};

template <class T1, class T2, class T3>
inline bool operator==(const triple<T1, T2, T3>& x, const triple<T1, T2, T3>& y) {
    return x.first == y.first && x.second == y.second && x.third == y.third;
}

template <class T1, class T2, class T3>
inline bool operator<(const triple<T1, T2, T3>& x, const triple<T1, T2, T3>& y) {
    return x.first < y.first ||
	   (!(y.first < x.first) && (x.second < y.second ||
			(! y.second < x.second) && (x.third == y.third)));
}

template <class T1, class T2, class T3>
inline triple<T1, T2, T3> make_triple(const T1& x, const T2& y, const T3& z) {
    return triple<T1, T2, T3>(x, y, z);
}

#endif