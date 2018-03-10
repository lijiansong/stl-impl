/*
 *
 * Copyright (c) 1994
 * Javier Barreiro and David R. Musser
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  The authors make no representation 
 * about the suitability of this software for any purpose.  It is 
 * provided "as is" without express or implied warranty.
 *
 */

#ifndef HASH_MULTIMAP_H
#define HASH_MULTIMAP_H

#ifndef Allocator
#define Allocator allocator
#include <defalloc.h>
#endif

#include <hashtbl.h>
#include <projectn.h>

template <class Key, class T, class HashFunction, class KeyEqual>
class hash_multimap {
public:

// typedefs:

    typedef Key key_type;
    typedef pair<const Key, T> value_type;
    typedef HashFunction hash_function;
    typedef KeyEqual key_equal;

private:
    typedef hash_table<key_type, value_type, 
                       select1st<value_type, key_type>,
                       hash_function, key_equal> 
            rep_type;
    rep_type ht;  // hash_table representing hash_multimap

public:
    typedef rep_type::reference reference;
    typedef rep_type::const_reference const_reference;
    typedef rep_type::iterator iterator;
    typedef rep_type::const_iterator const_iterator;
    typedef rep_type::size_type size_type;
    typedef rep_type::difference_type difference_type;

// allocation/deallocation

    hash_multimap(size_type table_size = 10007, float maxLF = 1.1, 
             const HashFunction& hf = HashFunction(), 
             const KeyEqual& equal = KeyEqual()) 
         : ht(table_size, maxLF, hf, equal, true) {}

    hash_multimap(const value_type* first, const value_type* last,
             size_type table_size = 10007, float maxLF = 1.1, 
             const HashFunction& hf = HashFunction(),
             const KeyEqual& equal = KeyEqual()) 
         : ht(first, last, table_size, maxLF, hf, equal, true) {}

    friend void destroy(hash_multimap<Key, T, HashFunction, KeyEqual>* p) {
      p->~hash_multimap();
    }


// copying, assignment, swap

    hash_multimap(const hash_multimap<Key, T, HashFunction, KeyEqual>& x) 
         : ht(x.ht, true) {}

    hash_multimap<Key,T,HashFunction,KeyEqual>& 
    operator=(const hash_multimap<Key, T, HashFunction, KeyEqual>& x) {
        ht = x.ht;
        return *this; 
    }

    void swap(hash_multimap<Key, T, HashFunction, KeyEqual>& x) { 
        ht.swap(x.ht); 
    }

// accessors:

    hash_function hash_funct() const { return ht.hash_funct(); }
    iterator begin() { return ht.begin(); }
    const_iterator begin() const { return ht.begin(); }
    iterator end() { return ht.end(); }
    const_iterator end() const { return ht.end(); }
    bool empty() const { return ht.empty(); }
    size_type size() const { return ht.size(); }
    size_type max_size() const { return ht.max_size(); }

// insert/erase

    iterator insert(const value_type& x) {
        return ht.insert(x).first;
    }
    void insert (const value_type* first, const value_type* last) {
        ht.insert(first, last);
    }
    void erase(iterator position) { ht.erase(position); }
    size_type erase(const key_type& x) { return (ht.erase(x)); }
    void erase(iterator first, iterator last) { ht.erase(first, last); }

// search operations:

    iterator find(const key_type& x) { return ht.find(x); }
    const_iterator find(const key_type& x) const { return ht.find(x); }
    size_type count(const key_type& x) const { return ht.count(x); }
    typedef pair<iterator, iterator> pair_iterator_iterator; 
    typedef pair<const_iterator, const_iterator> pair_citerator_citerator; 
    // typedefs done to get around compiler bug
    pair_iterator_iterator equal_range(const key_type& x) { 
        return ht.equal_range(x); 
    }
    pair_citerator_citerator equal_range(const key_type& x) const { 
         return ht.equal_range(x); 
    }

// hash table size operations
    size_type bucket_count() const { return ht.bucket_count(); }
    void resize(size_type buckets) { ht.resize(buckets); }
    void set_loadfactors(float minLF, float maxLF) {
      ht.set_loadfactors(minLF, maxLF);
    }
    float maxloadfactor() { return ht.maxloadfactor(); }
    float minloadfactor() { return ht.minloadfactor(); }

};

template <class Key, class T, class HashFunction, class KeyEqual>
inline bool operator==(const hash_multimap<Key, T, HashFunction, KeyEqual>& x, 
                       const hash_multimap<Key, T, HashFunction, KeyEqual>& y) 
{
  return (x.size() == y.size() && equal(x.begin(), x.end(), y.begin()));
}

#undef Allocator

#endif
