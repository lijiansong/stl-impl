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

#ifndef HASHBASE_H
#define HASHBASE_H

#include <stddef.h>
#include <cstring.h>
#include <vector.h>
#include <list.h>

#ifndef Allocator
#define Allocator allocator
#endif

// Utility classes -- should become part of STL

#include <triple.h>

template <class A>
class StlTypes {
public:
    typedef A::value_type 	value_type;
    typedef A::pointer 		pointer;
    typedef A::const_pointer 	const_pointer;
    typedef A::reference 	reference;
    typedef A::const_reference 	const_reference;
    typedef A::size_type 	size_type;
    typedef A::difference_type 	difference_type;
};

template <class T>
class SubType {
public:
    T& derived ()		{return * (T*) this;}
    const T& derived()const 	{return * (const T*) this;}
};

// Convenience Notation

#define HBTP template <class Sub, class Key, class ValueType,\
	  class Hasher, class KeyEqual>
#define HB hash_base<Sub, Key, ValueType, Hasher, KeyEqual>

// Control over automatic resizing

// Default is auto_rehash_intermittent

enum auto_rehash_modes {auto_rehash_off, auto_rehash_intermittent,
			auto_rehash_gradual};

//	NOTE:  auto_rehash_gradual is not implemented at this time.

// Hash_base:  Base class for hash table variants

HBTP class hash_base : public StlTypes<Allocator<ValueType> >,
		  public SubType<Sub> {
protected:
public:  // >>>>> Public to fix Borland 4.0 bug; works in 4.5
    typedef hash_base<Sub, Key, ValueType, Hasher, KeyEqual> base_type;
protected:
    typedef Sub self_type;

    base_type &       base()   	   { return (base_type)*this; }
    const base_type & base()const  { return *(const base_type *)this; }

    typedef unsigned long   hash_value_type;
    typedef size_type       hash_bucket_type;

private:
    struct HashEntry
	{ value_type val;
	  hash_bucket_type hash_bucket;

	  HashEntry(const value_type & v, hash_bucket_type hb)
		: val(v), hash_bucket(hb)
		{}
	  ~HashEntry() {}

	  HashEntry() 	{}	 // Fixes compiler bug for one case <<<
	  bool operator< (const HashEntry & x)   // >>> Fixes compiler bug<<<<
		{ return hash_bucket < x.hash_bucket; }
	  friend bool operator== (const HashEntry & x, const HashEntry & y) 
		{ return x.val == y.val; }
	  HashEntry & operator = (const HashEntry &) 	// >>>> Fixes compiler bug<<<<
		{return *this; }
	};


public: // >>>>> Public to fix 4.0 bug; works protected in 4.5
    typedef list<HashEntry> List;
protected:
    typedef vector<List::iterator> Vector;

    size_type 		table_size;   		// Number of buckets
    List 		table_entries;          // Actual entries
    Vector * 		hash_tab;               // Hash table of iterators

    Hasher 		hash_fn;                // Hash function object
    KeyEqual  		key_eq_fn;              // == compare object

    auto_rehash_modes	auto_mode;              // Mode for auto resize
    size_type		auto_trigger_factor;    // n*table_size causes resize
    size_type 		auto_growth_power;      // grow table to 2<<n * buckets
    size_type           trigger_limit;          // trigger_factor * table_size

public:

//   Types

    typedef Key key_type;
    typedef Hasher hasher;
    typedef KeyEqual key_equal;

//  Constructors:

    hash_base(size_type size, const Hasher& hash, const KeyEqual& comp,
	      auto_rehash_modes rm = auto_rehash_intermittent,
	      size_type ts = 4, size_type grow_power = 3 )
	 : table_size(size),
	   hash_tab(new Vector (size, table_entries.end())),
	   hash_fn(hash),
	   key_eq_fn(comp),
	   auto_mode(rm),
	   auto_trigger_factor(ts),
	   auto_growth_power(grow_power),
	   trigger_limit(size * ts)
	 {}
    hash_base(const base_type & x)
	: table_size(x.table_size),
	  hash_tab(new Vector (x.hash_tab->begin(), x.hash_tab->end())),
	  hash_fn(x.hash_fn),
	  key_eq_fn(x.key_eq_fn),
	  auto_mode(x.auto_mode),
	  auto_trigger_factor(x.auto_trigger_factor),
	  auto_growth_power(x.auto_growth_power),
	  trigger_limit(x.table_size * x.auto_trigger_factor)
	{}

    ~hash_base()		{delete  hash_tab;}

// Assignment

protected:
    void assign(const base_type & x);            // Use for assignment
private:
    base_type operator= (const base_type &); 	// Invalidate =
public:
    void swap(self_type & xx) {
	if (&xx == &derived()) return;
	base_type & x = xx;

	::swap(table_size, x.table_size);

	table_entries.swap(x.table_entries);

	::swap(hash_tab, x.hash_tab);

	::swap(auto_mode, x.auto_mode),
	::swap(auto_trigger_factor, x.auto_trigger_factor),
	::swap(auto_growth_power, x.auto_growth_power),
	::swap(trigger_limit, x.trigger_limit);
    }


// Iterators:

    class const_iterator;
    class iterator:
	     public bidirectional_iterator<value_type, difference_type> {
    public:
	iterator() {}
	iterator(List::iterator x):node(x) {}

	reference operator*() const { return (*node).val; }
	bool operator==(const iterator& x) const { return node == x.node; }
	iterator& operator++() 	    {++node; return *this;}
	iterator operator++(int) {
	    iterator tmp = *this;
	    ++*this;
	    return tmp;
	}
	iterator& operator--() 	    { --node; return *this; }
	iterator operator--(int) {
	    iterator tmp = *this;
	    --*this;
	    return tmp;
	}
    private:
	List::iterator node;
	friend base_type;
	friend const_iterator;
    };

    class const_iterator :
	public bidirectional_iterator<value_type, difference_type> {
    public:
	const_iterator() {}
	const_iterator(List::const_iterator x):node(x) {}
	const_iterator(const iterator & x):node(x.node) {}
	const_reference operator*() const { return (*node).val; }
	bool operator==(const const_iterator& x) const { return node == x.node; }
	const_iterator operator++() 	    { ++node; return *this; }
	const_iterator operator++(int) {
	    const_iterator tmp = *this;
	    ++node;
	    return tmp;
	}
	const_iterator& operator--() 	    { --node; return *this; }
	const_iterator operator--(int) {
	    const_iterator tmp = *this;
	    --node;
	    return tmp;
	}
    private:
	List::const_iterator node;
    };
    typedef reverse_bidirectional_iterator<const_iterator, value_type,
					const_reference, difference_type>
	const_reverse_iterator;
    typedef reverse_bidirectional_iterator<iterator, value_type,
					reference, difference_type>
	reverse_iterator;

// accessors:

    key_equal 		key_eq() const 	{ return key_eq_fn; }
    hasher		hash_funct() const {return hash_fn; }
    iterator 		begin() 	{ return table_entries.begin(); }
    const_iterator 	begin() const 	{ return table_entries.begin(); }
    iterator 		end() 		{ return table_entries.end(); }
    const_iterator 	end() const	{ return table_entries.end(); }
    reverse_iterator 	rbegin()	{ return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return end(); }
    reverse_iterator 	rend()		{ return reverse_iterator(begin()); }
    const_reverse_iterator rend() const
				{ return const_reverse_iterator(begin()); }
    bool 		empty() const 	 { return table_entries.empty(); }
    Vector::size_type	size() const 	 { return table_entries.size(); }
    Vector::size_type	max_size() const { return table_entries.max_size(); }

// Base operations for insert and find

protected:
    hash_bucket_type compute_bucket(const Key & x){
	return hash_fn(x) % table_size;
    }
public: // >>>>> Public only to fix bug in Borland 4.0; works protected in 4.5
    typedef triple<iterator, bool, hash_bucket_type>
	    triple_iterator_bool_bucket;
protected:
    triple_iterator_bool_bucket find_base(const Key& x);

    iterator last_equal(iterator x);
    const_iterator last_equal(const_iterator x)const;
public: // >>>>> Public only to fix bug in Borland 4.0; works protected in 4.5
    typedef List::iterator list_iterator;
protected:
    list_iterator last_bucket(List::iterator x);

// Insertion Operators: derived class selects the appropriate one.

protected:
    typedef pair<iterator, bool> pair_iterator_bool;
    pair_iterator_bool insert_unique(const value_type& x) {
	if (table_entries.size() > trigger_limit &&
	    auto_mode == auto_rehash_intermittent)  auto_resize();
	triple_iterator_bool_bucket tibb =
			find_base(derived().extract_key(x));
	if (tibb.second) return pair_iterator_bool(tibb.first, false);
	hash_bucket_type bucket = tibb.third;
	List::iterator j =
		table_entries.insert(tibb.first.node,HashEntry(x, bucket));
	if ((*hash_tab)[bucket] == table_entries.end())
	    (*hash_tab)[bucket] = j;
	return pair_iterator_bool(j, true);
    }

    iterator insert_multi(const value_type& x) {
	if (table_entries.size() > trigger_limit &&
	    auto_mode == auto_rehash_intermittent)  auto_resize();
	triple_iterator_bool_bucket tibb  =
			find_base(derived().extract_key(x));
	iterator i = tibb.first;
	if (tibb.second) i = last_equal(i);
	hash_bucket_type bucket = tibb.third;
	List::iterator j = table_entries.insert(i.node,HashEntry(x, bucket));
	if ((*hash_tab)[bucket] == table_entries.end())
	    (*hash_tab)[bucket] = j;
	return j;
    }

    void auto_resize() {
	Vector::size_type new_size = table_size * (1<<auto_growth_power) + 1;
	resize(new_size);
    }

public:
    void insert(const value_type* first, const value_type* last);

    void erase(iterator position)  {
	List::iterator end = table_entries.end();
	hash_bucket_type bucket = (*position.node).hash_bucket;
	if ((*hash_tab)[bucket] == position.node)
	{   List::iterator p1 = position.node;
	    p1 ++;
	    (*hash_tab)[bucket] = ((p1 != end) &&
				   ((*p1).hash_bucket == bucket))
				  ?  p1  : end;
	}
	table_entries.erase(position.node);
    }

    size_type erase(const Key& x) ;

    void erase(iterator first, iterator last);


// Find operations:

    iterator find(const Key& x) {
	triple_iterator_bool_bucket res = find_base(x);
	if (res.second) return res.first;
	else return end();
    }

    const_iterator find(const Key& x) const {
	return ((self_type *)this) -> find(x);
    }

    typedef pair<iterator, iterator> pair_iterator_iterator;
    // typedef done to get around compiler bug
    pair_iterator_iterator equal_range(const Key& x) {
	iterator res = find(x);
	return pair<iterator, iterator>(res, last_equal(res));
    }
    typedef pair<const_iterator, const_iterator> pair_citerator_citerator; 
    // typedef done to get around compiler bug
    pair_citerator_citerator equal_range(const Key& x) const {
	const_iterator res = find(x);
	return pair<const_iterator, const_iterator>(res, last_equal(res));
    }

// Hash specific operations

    Vector::size_type bucket_count() const	{return table_size;}
    void resize (size_type new_size);

    friend bool operator==(const Sub& x, const Sub& y) {
    return x.table_entries.size() == y.table_entries.size() &&
           equal(x.table_entries.begin(), x.table_entries.end(), y.table_entries.begin());
    }

// Testing functions

    typedef vector<unsigned> CountVec;
    CountVec analyze_distribution ();
};

HBTP void
HB::assign(const base_type & x) {
	if (&x == this) return;

	delete hash_tab;
	table_entries.erase(table_entries.begin(), table_entries.end());

	hash_tab = new Vector (x.bucket_count(), table_entries.end());
	table_size = x.table_size;

	table_entries.insert(table_entries.end(),
			     x.table_entries.begin(),
			     x.table_entries.end());

	List::iterator i = table_entries.begin();
	List::iterator last = table_entries.end();
	hash_bucket_type b = table_size;
	auto_mode = x.auto_mode,
	auto_trigger_factor = x.auto_trigger_factor,
	auto_growth_power = x.auto_growth_power,
	trigger_limit= x.trigger_limit;

	for (i = i; i != last; i++)
	    if ((*i).hash_bucket != b) {
		b = (*i).hash_bucket;
		(*hash_tab)[b] = i;
	    }
    }

HBTP HB::triple_iterator_bool_bucket
HB::find_base(const Key& x) {
	hash_value_type  hv = hash_fn(x);
	hash_bucket_type bucket = hv % table_size;
	List::iterator i = (*hash_tab)[bucket];
	List::iterator end = table_entries.end();

	while (i != end)
	{   if (((*i).hash_bucket) != bucket) break;
	    if (key_eq_fn(derived().extract_key((*i).val), x))
		return triple_iterator_bool_bucket(i, true, bucket);
	    i++;
	}
	return triple_iterator_bool_bucket(i, false, bucket);
    }

HBTP HB::iterator
HB::last_equal(iterator x) {
	if (x==end()) return x;
	iterator y = x;  ++y;
	while (y != end() && key_eq_fn(derived().extract_key(*x),
				       derived().extract_key(*y)))
	       ++y;
	return y;
    }

HBTP HB::const_iterator
HB::last_equal(const_iterator x) const {
	if (x==end()) return x;
	const_iterator y = x;  ++y;
	while (y != end() && key_eq_fn(derived().extract_key(*x),
				       derived().extract_key(*y)))
	       ++y;
	return y;
    }

HBTP HB::list_iterator
HB::last_bucket(list_iterator x) {
	list_iterator end = table_entries.end();
	if (x == end) return x;
	hash_bucket_type bucket = (*x).hash_bucket;
	while (++x != end  && bucket == (*x).hash_bucket) ; ;
	return x;
}


HBTP void
HB::insert(const ValueType* first, const ValueType* last)  {
	while (first != last) derived().insert(*first++);
    }

HBTP HB::size_type
HB::erase(const Key& x) {
	iterator fx = derived().find(x);
	iterator gx = last_equal(fx);
	size_type ct = 0;

	while (fx != gx) {
	    erase(fx++);
	    ++ct;
	}
	return ct;
    }

HBTP void
HB::erase(iterator first, iterator last) {
	while (first != last) erase(first++);
    }

HBTP void
HB::resize (size_type new_size) {
    List::iterator curr = table_entries.begin();
    List::iterator end = table_entries.end();
    List::iterator final = end;  --final;
    List::iterator next = curr;

    delete hash_tab;
    hash_tab = new Vector(new_size, end);
    table_size = new_size;
    trigger_limit = new_size * auto_trigger_factor;

    while (curr != end) {
	if (next != final) ++next; else next = end;
	hash_bucket_type bucket =
		compute_bucket(derived().extract_key((*curr).val));
	list_iterator i = (*hash_tab)[bucket];
	table_entries.splice(last_bucket(i), table_entries, curr);
	(*curr).hash_bucket = bucket;
	if ((*hash_tab)[bucket] == end) (*hash_tab)[bucket] = curr;
	curr = next;
    }
}

HBTP  HB::CountVec
HB::analyze_distribution () {
    CountVec ctvec(5, 0);
    list_iterator end = table_entries.end();

    for (int i = 0; i < table_size; ++i) {
	unsigned ct = 0;
	list_iterator curr = (*hash_tab)[i];
	while ( curr != end && (*curr).hash_bucket == i ) ++ct, ++curr;
	++ctvec[0];
	if (!ct) ++ctvec[1];
	else if (ct == 1) ++ctvec[2];
	ctvec[3] = max(ctvec[3], ct);
	ctvec[4] += ct;
    }
    return ctvec;
}


// ****** Hashing Functions ******

typedef size_t HashWord;
HashWord init_hashing_table();

static HashWord hashing_table[256];
static HashWord dummy_var = init_hashing_table();

HashWord init_hashing_table()
{
    randomize();
    for (unsigned i = 0; i < 256; i++)
	hashing_table[i] = i  | (i << 8);   // >>> i for orig algorithm <<<
    for (i = 0; i < 256; ++i)
    {
	HashWord r = (rand() >> 5) & 255;
	HashWord xchg = hashing_table[i];
	hashing_table[i] = hashing_table[r];
	hashing_table[r] = xchg;
    }
    return 1;
}

template <class T>
struct hasher : unary_function<T, HashWord> {
    HashWord operator()(const T &  x) const;
};

template <class T>
HashWord hasher<T>::operator()(const T &  x) const {
    HashWord res = 0;
    size_t len = sizeof(T);
    char * k = (char *) &x;

    while (len--){
	res = hashing_table[(res & 255) ^ *k++] ^ (res << 3);
    }
    return res ;
}

struct hasher<char *> : unary_function<char *, HashWord> {
    HashWord operator()(const char *  k) const ;
};

/*  Original hash function
HashWord hasher<char *>::operator()(const char *  k) const {
    HashWord res1 = 0, res2 = 0;

    while (*k){
	res1 = hashing_table[res1  ^ *k++] ;
	if (*k) res2 = hashing_table[res2  ^ *k++] ;
    }
    return (res1 << 8) + res2 ;
}
*/

///* Modified hash function *****
HashWord hasher<char *>::operator()(const char *  k) const {
    HashWord res = 0;

    while (*k){
	res = hashing_table[(res & 255) ^ *k++] ^ (res << 3);
    }
    return res ;
}
//*/

struct hasher<string> : unary_function<string, HashWord> {
    HashWord operator()(const string & k) const ;
    hasher<char*> hc;
};

HashWord hasher<string>::operator()(const string & k) const {
    return hc(k.c_str());
}

#endif

