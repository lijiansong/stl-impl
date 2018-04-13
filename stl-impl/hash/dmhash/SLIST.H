/* DRAFT 12/30/94 (incomplete, and substantial revisions are planned)
 *
 * Singly-linked lists, adapted by David R. Musser from
 * the "list.h" source file in Hewlett-Packard Company's reference
 * implementation of the Standard Template Library, which carries
 * the following notices:
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

#ifndef SLIST_H
#define SLIST_H

#include <function.h>
#include <algobase.h>
#include <iterator.h>
#include <bool.h>
    
#ifndef Allocator
#define Allocator allocator
#include <defalloc.h>
#endif

#ifndef slist 
#define slist slist
#endif

template <class T>
class slist {
protected:
    typedef Allocator<void>::pointer void_pointer;
    struct slist_node;
    friend slist_node;
    struct slist_node {
	void_pointer next;
	T data;
    };
    static Allocator<slist_node> slist_node_allocator;
    static Allocator<T> value_allocator;
    static bool separate_copying;
public:      
    typedef T value_type;
    typedef Allocator<T> value_allocator_type;
    typedef Allocator<T>::pointer pointer;
    typedef Allocator<T>::reference reference;
    typedef Allocator<T>::const_reference const_reference;
    typedef Allocator<slist_node> slist_node_allocator_type;
    typedef Allocator<slist_node>::pointer link_type;
    typedef Allocator<slist_node>::size_type size_type;
    typedef Allocator<slist_node>::difference_type difference_type;
protected:
    size_type buffer_size() {
	return slist_node_allocator.init_page_size();
    }
    struct slist_node_buffer;
    friend slist_node_buffer;
    struct slist_node_buffer {
	void_pointer next_buffer;
	link_type buffer;
    };
public:
    typedef Allocator<slist_node_buffer> buffer_allocator_type;
    typedef Allocator<slist_node_buffer>::pointer buffer_pointer;     
protected:
    static Allocator<slist_node_buffer> buffer_allocator;
    static buffer_pointer buffer_slist;
    static link_type free_slist;
    static link_type next_avail;
    static link_type last;
    void add_new_buffer() {
	buffer_pointer tmp = buffer_allocator.allocate((size_type)1);
	tmp->buffer = slist_node_allocator.allocate(buffer_size());
	tmp->next_buffer = buffer_slist;
	buffer_slist = tmp;
	next_avail = buffer_slist->buffer;
	last = next_avail + buffer_size();
    }
    static size_type number_of_slists;
    void deallocate_buffers();
    link_type get_node() {
	link_type tmp = free_slist;
	return free_slist ? (free_slist = (link_type)(free_slist->next), tmp) 
	    : (next_avail == last ? (add_new_buffer(), next_avail++) 
		: next_avail++);
	// ugly code for inlining - avoids multiple returns
    }
    void put_node(link_type p) {
	p->next = free_slist;
	free_slist = p;
    }

protected:
    link_type node;
public:
    class iterator;
    class const_iterator;
    class iterator : public forward_iterator<T, difference_type> {
    friend class slist<T>;
    friend class const_iterator;
//  friend bool operator==(const iterator& x, const iterator& y);
    protected:
	link_type node;
    public:
	iterator(link_type x) : node(x) {}
	iterator() {}
	bool operator==(const iterator& x) const { return node == x.node; }
	reference operator*() const { return (*node).data; }
	iterator& operator++() { 
	    node = (link_type)((*node).next);
	    return *this;
	}
	iterator operator++(int) { 
	    iterator tmp = *this;
	    ++*this;
	    return tmp;
	}
    };
    class const_iterator : public forward_iterator<T, difference_type> {
    friend class slist<T>;
    protected:
	link_type node;
    public:     
	const_iterator(link_type x) : node(x) {}
	const_iterator() {}
	const_iterator(const iterator& x) : node(x.node) {}
	bool operator==(const const_iterator& x) const { return node == x.node; } 
	const_reference operator*() const { return (*node).data; }
	const_iterator& operator++() { 
	    node = (link_type)((*node).next);
	    return *this;
	}
	const_iterator operator++(int) { 
	    const_iterator tmp = *this;
	    ++*this;
	    return tmp;
	}
    };
// Copy control
    static void separate_copy(bool option = true) {
        separate_copying = option;
    }

// Constructors and destructor
    slist() {
	++number_of_slists;
	node = get_node();
	(*node).next = NULL;
    }
    slist(size_type n, const T& value = T()) {
	++number_of_slists;
	node = get_node();
	(*node).next = NULL;
	insert(iterator(node), begin(), n, value);
    }
    slist(const T* first, const T* last) {
	++number_of_slists;
	node = get_node();
	(*node).next = NULL;
	insert(iterator(node), begin(), first, last);
    }
    slist(const slist<T>& x) {
        if (separate_copying) {
           ++number_of_slists;
           node = get_node();
           (*node).next = NULL;
           insert(iterator(node), begin(), x.begin(), x.end());
        } else 
           node = x.node;
    }
    ~slist() {
//      cout << "slist destructor called" << endl;
      if (separate_copying) {
	erase(begin(), end());
	put_node(node);
	if (--number_of_slists == 0) deallocate_buffers();
      }
    }

    // Following can be omitted with a compiler that can handle 
    // the general way destroy is defined in defalloc.h:
    //   template <class T> inline void destroy(T* p) { p->~T();}
    friend void destroy(slist<T>* p) {
      p->~slist();
    }

// Assignment operator
    slist<T>& operator=(const slist<T>& x);

// Accessors
    iterator begin() { return (link_type)((*node).next); }
    const_iterator begin() const { return (link_type)((*node).next); }
    iterator end() { return NULL; }
    const_iterator end() const { return NULL; }
    bool empty() const { return begin() == end(); }
    size_type size() const {
        size_type n = 0;
        for (const_iterator p = begin(); p != end(); ++p) ++n;
        return n;
    }
    size_type max_size() const { return slist_node_allocator.max_size(); }
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }

// Modifiers
    void swap(slist<T>& x) {
	::swap(node, x.node);
    }
    iterator insert(iterator previous, iterator position, const T& x) {
	link_type tmp = get_node();
	construct(value_allocator.address((*tmp).data), x);
	(*tmp).next = position.node;
	(*(link_type(previous.node))).next = tmp;
	return tmp;
    }
    void insert(iterator previous, iterator position,
                size_type n, const T& x = T());
    void insert(iterator previous, iterator position, 
                const T* first, const T* last);
    void insert(iterator previous, iterator position, 
                const_iterator first, const_iterator last);
    void push_front(const T& x) { insert(iterator(node), begin(), x); }
    void erase(iterator position) {
        iterator previous(node);
        for (iterator p = begin(); p != position; ++p)
            previous = p;
	(*previous.node).next = (*position.node).next;
	destroy(value_allocator.address((*position.node).data));
	put_node(position.node);
    }
    void erase(iterator first, iterator last);
    void pop_front() { erase(begin()); }
    void splice(iterator previous, iterator position, slist<T>& x, 
                iterator first, iterator last) {
	if (first != last) {
            iterator prev1(x.node);
            for (iterator p = x.begin(); p != first; ++p)
               prev1 = p;
            iterator prev2 = prev1;
            for ( ; p != last; ++p)
               prev2 = p;
            (*previous.node).next = first.node;
            (*prev1.node).next = (*prev2.node).next;
            (*prev2.node).next = position.node;
	}
    }
    void splice(iterator previous, iterator position, slist<T>& x) {
	if (!x.empty()) {
            splice(previous, position, x, x.begin(), x.end());
	}
    }
    void splice(iterator previous, iterator position, 
                slist<T>& x, iterator i) {
	iterator j = i;
	splice(previous, position, x, i, ++j);
    }
    void splice(slist<T>& x) {
	splice(iterator(node), begin(), x, x.begin(), x.end());
    }
    void splice(slist<T>& x, iterator i) {
	iterator j = i;
	splice(iterator(node), begin(), x, i, ++j);
    }

/*
    void remove(const T& value);
    void unique();
    void merge(slist<T>& x);
    void reverse();
    void sort();
*/
};

template <class T>
slist<T>::buffer_pointer slist<T>::buffer_slist = 0;

template <class T>
slist<T>::link_type slist<T>::free_slist = 0;

template <class T>
slist<T>::link_type slist<T>::next_avail = 0;

template <class T>
slist<T>::link_type slist<T>::last = 0;

template <class T>
slist<T>::size_type slist<T>::number_of_slists = 0;

template <class T>
slist<T>::slist_node_allocator_type slist<T>::slist_node_allocator;

template <class T>
slist<T>::value_allocator_type slist<T>::value_allocator;

template <class T>
slist<T>::buffer_allocator_type slist<T>::buffer_allocator;

template <class T>
bool slist<T>::separate_copying = true;

/* 
 * currently the following does not work - made into a member function

template <class T>
inline bool operator==(const slist<T>::iterator& x, const slist<T>::iterator& y) { 
    return x.node == y.node; 
}
*/

template <class T>
inline bool operator==(const slist<T>& x, const slist<T>& y) {
    return x.size() == y.size() && equal(x.begin(), x.end(), y.begin());
}

template <class T>
inline bool operator<(const slist<T>& x, const slist<T>& y) {
    return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <class T>
void slist<T>::deallocate_buffers() {
    while (buffer_slist) {
	buffer_pointer tmp = buffer_slist;
	buffer_slist = (buffer_pointer)(buffer_slist->next_buffer);
	slist_node_allocator.deallocate(tmp->buffer);
	buffer_allocator.deallocate(tmp);
    }
    free_slist = 0;
    next_avail = 0;
    last = 0;
}

template <class T>
void slist<T>::insert(iterator previous, iterator position, 
                      const T* first, const T* last) {
    while (first != last) {
        insert(previous, position, *first++);
        ++previous;
    }
}
	 
template <class T>
void slist<T>::insert(iterator previous, iterator position, 
                      const_iterator first, const_iterator last) {
    while (first != last) {
        insert(previous, position, *first++);
        ++previous;
    }
}

template <class T>
void slist<T>::insert(iterator previous, iterator position, 
                      size_type n, const T& x) {
    while (n--) {
        insert(previous, position, x);
        ++previous;
    }
}

template <class T>
void slist<T>::erase(iterator first, iterator last) {
    while (first != last) erase(first++);
}

template <class T>
slist<T>& slist<T>::operator=(const slist<T>& x) {
    if (this != &x) {
        erase(begin(), end());
        insert(iterator(node), begin(), x.begin(), x.end());
    }
    return *this;
}

/*
template <class T>
void slist<T>::remove(const T& value) {
    iterator first = begin();
    iterator last = end();
    while (first != last) {
	iterator next = first;
	++next;
	if (*first == value) erase(first);
	first = next;
    }
}

template <class T>
void slist<T>::unique() {
    iterator first = begin();
    iterator last = end();
    if (first == last) return;
    iterator next = first;
    while (++next != last) {
	if (*first == *next)
	    erase(next);
	else
	    first = next;
	next = first;
    }
}

template <class T>
void slist<T>::merge(slist<T>& x) {
    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = x.begin();
    iterator last2 = x.end();
    while (first1 != last1 && first2 != last2)
	if (*first2 < *first1) {
	    iterator next = first2;
	    transfer(first1, first2, ++next);
	    first2 = next;
	} else
	    first1++;
    if (first2 != last2) transfer(last1, first2, last2);
    length += x.length;
    x.length= 0;
}

template <class T>
void slist<T>::reverse() {
    if (size() < 2) return;
    for (iterator first = ++begin(); first != end();) {
	iterator old = first++;
	transfer(begin(), old, first);
    }
}    

template <class T>
void slist<T>::sort() {
    if (size() < 2) return;
    slist<T> carry;
    slist<T> counter[64];
    int fill = 0;
    while (!empty()) {
	carry.splice(carry.begin(), *this, begin());
	int i = 0;
	while(i < fill && !counter[i].empty()) {
	    counter[i].merge(carry);
	    carry.swap(counter[i++]);
	}
	carry.swap(counter[i]);         
	if (i == fill) ++fill;
    } 
    while(fill--) merge(counter[fill]);
}
*/

#undef Allocator
#undef slist

#endif
