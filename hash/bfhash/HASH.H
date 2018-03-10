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


#ifndef HASH_H_DEF
#define HASH_H_DEF

#include <hashbase.h>

template <class Key, class Data, class Hasher, class Equal>
class hash_map : public hash_base<hash_map<Key, Data, Hasher, Equal>,
				  Key, pair<const Key, Data>,
				  Hasher, Equal>
{public:

// allocation/deallocation

    hash_map(size_type size, const Hasher& hash = Hasher(),
	     const Equal& comp = Equal(),
	     auto_rehash_modes rm = auto_rehash_intermittent,
	     size_type ts = 4, size_type grow_power = 3 )
	  : base_type (size, hash, comp, rm, ts, grow_power)
	 {}
    hash_map(const value_type* first, const value_type* last, size_type size,
	     const Hasher& hash, const Equal& comp = Equal())
	 : base_type (size, hash, comp)
	{ base().insert(first, last);}
    hash_map(const self_type & x)
	: base_type(x)
	{}

    ~hash_map()		{}

// Extraction

    const Key & extract_key (const value_type & x) const
	{return ((value_type &)x).first;}

// Assignment
    self_type&
    operator=(const self_type & x)
	{ assign(x); return *this; }

// Insertion
    pair_iterator_bool insert(const value_type& x)
	{ return insert_unique(x); }

// Location

    Allocator<Data>::reference operator[](const Key & k) {
	iterator i = find(k);
	if (i != end()) return (*i).second;
	value_type v (k, Data());
	return (*(insert(v).first)).second;
    }
};

template <class Key, class Data, class Hasher, class Equal>
class hash_multimap : public hash_base<hash_multimap<Key, Data, Hasher, Equal>,
				  Key, pair<const Key, Data>,
				  Hasher, Equal>
{public:

// allocation/deallocation

    hash_multimap(size_type size, const Hasher& hash = Hasher(),
	     const Equal& comp = Equal(),
	     auto_rehash_modes rm = auto_rehash_intermittent,
	     size_type ts = 4, size_type grow_power = 3 )
	  : base_type (size, hash, comp, rm, ts, grow_power)
	 {}
    hash_multimap(const value_type* first, const value_type* last, size_type size,
	     const Hasher& hash, const Equal& comp = Equal())
	    : base_type (size, hash, comp) {
       base().insert(first, last);
    }

    hash_multimap(const self_type & x)
	: base_type(x)
	{}

    ~hash_multimap()		{}

// Extraction

    const Key & extract_key (const value_type & x) const
	{return ((value_type &)x).first;}

// Assignment
    self_type&
    operator=(const self_type & x) {
	assign(x);
	return *this;
    }

// Insertion
    iterator insert(const value_type& x) {
	return insert_multi(x);
    }
};

template <class Key, class Hasher, class Equal>
class hash_set : public hash_base<hash_set<Key, Hasher, Equal>,
				  Key, Key,
				  Hasher, Equal>
{public:

// allocation/deallocation

    hash_set(size_type size=1009, const Hasher& hash = Hasher(),
	     const Equal& comp = Equal(),
	     auto_rehash_modes rm = auto_rehash_intermittent,
	     size_type ts = 4, size_type grow_power = 3 )
	  : base_type (size, hash, comp, rm, ts, grow_power)
	 {}
    hash_set(const value_type* first, const value_type* last, size_type size,
	     const Hasher& hash, const Equal& comp = Equal())
	    : base_type (size, hash, comp) {
       base().insert(first, last);
    }

    hash_set(const self_type & x)
	: base_type(x)
	{}

    ~hash_set()		{}

// Extraction

    const Key & extract_key (const value_type & x) const {return x;}

// Assignment
    self_type &
    operator=(const self_type & x) {
	assign(x);
	return *this;
    }

// Insertion
    pair_iterator_bool insert(const value_type& x) {
	return insert_unique(x);
    }
};

template <class Key, class Hasher, class Equal>
class hash_multiset : public hash_base<hash_multiset<Key, Hasher, Equal>,
				  Key, Key,
				  Hasher, Equal>
{public:

// allocation/deallocation

    hash_multiset(size_type size, const Hasher& hash = Hasher(),
	     const Equal& comp = Equal(),
	     auto_rehash_modes rm = auto_rehash_intermittent,
	     size_type ts = 4, size_type grow_power = 3 )
	  : base_type (size, hash, comp, rm, ts, grow_power)
	 {}
    hash_multiset(const value_type* first, const value_type* last, size_type size, 
	     const Hasher& hash, const Equal& comp = Equal())
	    : base_type (size, hash, comp) {
       base().insert(first, last);
    }

    hash_multiset(const self_type & x)
	: base_type(x)
	{}

    ~hash_multiset()		{}

// Extraction

    const Key & extract_key (const Key & x) const {return x;}

// Assignment
    self_type &
    operator=(const self_type & x) {
	assign(x);
	return *this;
    }

// Insertion
    iterator insert(const value_type& x) {
	return insert_multi(x);
    }
};


#endif
