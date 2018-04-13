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

/* 
 * Iterators may have corresponding safe iterator adaptors that check
 * iterator values against the range boundaries during the operations
 * on the iterators. Exceptions are raised if iterators are out of
 * bound. (In this reference implementation, a function out_of_bound
 * is called when out of bound condition is detected, and this function
 * should be implemented with appropriate action to take.)
 * 
 * A safe iterator is constructed from a valid iterator and the lower/
 * upper bounds within which the iterator is supposed to operate. Safe 
 * iterators are passed to the algorithms to ensure the correct 
 * implementation of the algorithms for debugging purpose.
 *
 * For example, to ensure the algorithm binary_search never goes beyond
 * the container boundaries when looks for a value in vector v, we'd do
 *     binary_search(safe_iterator(v.begin(), v.begin(), v.end()),
 *                   safe_iterator(v.end(), v.begin(), v.end()),
 *                   value);
 */

void out_of_bound() {
    cerr << "out of bound" << endl;
    exit(1);
}

template <class ForwardIterator, class T, class Reference, class Distance>
// Reference = T&
// Distance = ptrdiff_t
class safe_forward_iterator : public forward_iterator<T, Distance> {
    typedef safe_forward_iterator<ForwardIterator, T, Reference, Distance> self;
    friend bool operator==(const self& x, const self& y);
protected:
    ForwardIterator current;
    ForwardIterator upper_bound;
public:
    safe_forward_iterator() {}
    safe_forward_iterator(ForwardIterator x, ForwardIterator last)
        : current(x), upper_bound(last) {}
    ForwardIterator base() { return current; }
    Reference operator*() { return *current; }
    self& operator++() {
        if (current == upper_bound) out_of_bound();
        ++current;
        return *this;
    }
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }
};

template <class ForwardIterator, class T, class Reference, class Distance>
inline bool operator==(
    const safe_forward_iterator<ForwardIterator, T, Reference, Distance>& x,
    const safe_forward_iterator<ForwardIterator, T, Reference, Distance>& y) {
    return x.current == y.current;
}

template <class BidirectionalIterator, class T, class Reference,
	  class Distance>
// Reference = T&
// Distance = ptrdiff_t
class safe_bidirectional_iterator 
    : public bidirectional_iterator<T, Distance> {
    typedef safe_bidirectional_iterator<BidirectionalIterator, T, Reference,
					Distance> self;
    friend bool operator==(const self& x, const self& y);
protected:
    BidirectionalIterator current;
    BidirectionalIterator lower_bound;
    BidirectionalIterator upper_bound;
public:
    safe_bidirectional_iterator() {}
    safe_bidirectional_iterator(BidirectionalIterator x, 
				BidirectionalIterator first,
				BidirectionalIterator last)
	: current(x), lower_bound(first), upper_bound(last) {}
    BidirectionalIterator base() { return current; }
    Reference operator*() { return *current; }
    self& operator++() {
	if (current == upper_bound) out_of_bound();
	++current;
	return *this;
    }
    self operator++(int) {
	self tmp = *this;
	++*this;
	return tmp;
    }
    self& operator--() {
	if (current == lower_bound) out_of_bound();
	--current;
	return *this;
    }
    self operator--(int) {
	self tmp = *this;
	--*this;
	return tmp;
    }
};

template <class BidirectionalIterator, class T, class Reference,
	  class Distance>
inline bool operator==(
    const safe_bidirectional_iterator<BidirectionalIterator, T, Reference,
				      Distance>& x,
    const safe_bidirectional_iterator<BidirectionalIterator, T, Reference,
				      Distance>& y) {
    return x.current == y.current;
}

template <class RandomAccessIterator, class T, class Reference,
	  class Distance>
// Reference = T&
// Distance = ptrdiff_t
class safe_iterator : public random_access_iterator<T, Distance> {
    typedef safe_iterator<RandomAccessIterator, T, Reference, Distance>
	self;
    friend bool operator==(const self& x, const self& y);
    friend bool operator<(const self& x, const self& y);
    friend Distance operator-(const self& x, const self& y);
    friend self operator+(Distance n, const self& x);
protected:
    RandomAccessIterator current;
    RandomAccessIterator lower_bound;
    RandomAccessIterator upper_bound;
public:
    safe_iterator() {}
    safe_iterator(RandomAccessIterator x, RandomAccessIterator first,
		  RandomAccessIterator last) 
	: current(x), lower_bound(first), upper_bound(last) {}
    RandomAccessIterator base() { return current; }
    Reference operator*() { return *current; }
    self& operator++() {
	if (current == upper_bound) out_of_bound();
	++current;
	return *this;
    }
    self operator++(int) {
	self tmp = *this;
	++*this;
	return tmp;
    }
    self& operator--() {
	if (current == lower_bound) out_of_bound();
	--current;
	return *this;
    }
    self operator--(int) {
	self tmp = *this;
	--*this;
	return tmp;
    }
    self& operator+=(Distance n) {
        if (n > 0 && upper_bound - current < n || n < 0 && 
	    current - lower_bound < -n) out_of_bound();
        current += n;
        return *this;
    }
    self operator+(Distance n) const {
        self tmp = *this;
        return tmp += n;
    }
    self& operator-=(Distance n) { return *this += -n; }
    self operator-(Distance n) const {
        self tmp = *this;
        return tmp -= n;
    }
    Reference operator[](Distance n) { return *(*this + n); }
};

template <class RandomAccessIterator, class T, class Reference, class Distance>
inline bool operator==(
    const safe_iterator<RandomAccessIterator, T, Reference, Distance>& x,
    const safe_iterator<RandomAccessIterator, T, Reference, Distance>& y) {
    return x.current == y.current;
}

template <class RandomAccessIterator, class T, class Reference, class Distance>
inline bool operator<(
    const safe_iterator<RandomAccessIterator, T, Reference, Distance>& x,
    const safe_iterator<RandomAccessIterator, T, Reference, Distance>& y) {
    return x.current < y.current;
}

template <class RandomAccessIterator, class T, class Reference, class Distance>
inline Distance operator-(
    const safe_iterator<RandomAccessIterator, T, Reference, Distance>& x,
    const safe_iterator<RandomAccessIterator, T, Reference, Distance>& y) {
    return x.current - y.current;
}

template <class RandomAccessIterator, class T, class Reference, class Distance>
inline safe_iterator<RandomAccessIterator, T, Reference, Distance> operator+(
    Distance n,
    const safe_iterator<RandomAccessIterator, T, Reference, Distance>& x) {
    return x + n;
}
