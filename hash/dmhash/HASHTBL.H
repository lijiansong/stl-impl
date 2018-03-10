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

#ifndef HASH_TABLE
#define HASH_TABLE

#include <iostream.h>
#include <stdlib.h>
#include <defalloc.h>
#include <iterator.h>

#define Allocator allocator

#include <vector.h>
#include <deque.h>
#include <function.h>
#include <slist.h>
#include <map.h>   // used only in code for debugging (frequencies function)
 
template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
class hash_table {
  public:
        class iterator;
        class const_iterator;

        friend class iterator;
        friend class const_iterator;

        typedef Key key_type;
        typedef Value value_type;
        typedef HashFunction hash_function;
        typedef slist<value_type>::size_type size_type;

        // for debugging purposes only:   
        typedef map< size_type, size_type, less<size_type> > fmap;

  class bucket;

  typedef deque<bucket>::size_type table_size_type;
  typedef deque<bucket>::difference_type table_difference_type;

  class bucket : public slist<value_type> {
  friend class hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>;

  public:
        typedef slist<value_type>::iterator        iterator;
        typedef slist<value_type>::const_iterator  const_iterator;
        typedef slist<value_type>::size_type       size_type; 
        typedef slist<value_type>::reference       reference;
        typedef slist<value_type>::const_reference const_reference;
        typedef slist<value_type>::difference_type difference_type;

        table_difference_type next_;  // Next non-empty bucket
        table_difference_type prev_;  // Previous non-empty bucket

        bucket() : next_(-1), prev_(-1)	{}

/*
        // For debugging purposes only:
	friend ostream& operator<<(ostream& o,const bucket& b)
	{
          bucket::const_iterator p(b.begin());

          o << "(nextNeb:" << b.next_ << ", prevNeb:" << b.prev_ << ")";
          for (;p != b.end(); p++)
	        o << " " << *p; 
	  return o;
        }
*/
  };  // end class bucket;

  friend ostream& operator<<(ostream& o, const bucket& b);

public:

  typedef bucket::difference_type difference_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;

  class iterator : public forward_iterator<Value, difference_type> {

    friend class hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>;
    friend class const_iterator;

  protected:
    hash_table<Key, Value, KeyOfValue,HashFunction, KeyEqual>*  ht_; 
          // hast table to which the iterator is pointing 
    bucket::iterator  ptr_; // Pointer to the current element in the bucket

  private:

    size_type currBucket() { return (*ht_).S(KeyOfValue()(*ptr_)); }

    iterator(hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>*  ht, 
             bucket::iterator ptr) :  ht_(ht), ptr_(ptr) { }

  public:
    iterator(const iterator& rhs) :  ht_(rhs.ht_), ptr_(rhs.ptr_) { }

    iterator() : ht_(0),  ptr_(0) { }

    iterator& operator=(const iterator& rhs) {
        ptr_=rhs.ptr_;
        return *this;
    }

    ~iterator() {}

    friend bool operator==(const iterator& lhs, const iterator& rhs) { 
         return (lhs.ptr_ == rhs.ptr_); 
    }

    bool operator==(const iterator& rhs) {
         return (ptr_ == rhs.ptr_);   
    }

    friend bool operator!=(const iterator& lhs, const iterator& rhs) {
         return !(lhs == rhs);
    }

    reference operator*() { 
         return *ptr_;
    }

    iterator& operator++() {
          bucket::iterator tmp = ptr_;
          ++tmp;
          bucket& cb = (*ht_).buckets_[currBucket()];
          if (tmp == cb.end() && cb.next_ != -1)
             ptr_ = (*ht_).buckets_[cb.next_].begin();
          else
             ptr_ = tmp;
          return *this;
    }

    iterator operator++(int) 
    {
      iterator tmp = *this;
      ++*this;
      return tmp;
    }
  };   // end class iterator

  class const_iterator : public forward_iterator<Value, difference_type> {

    friend class hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>;
    friend class iterator;

  protected:
    const hash_table<Key, Value, KeyOfValue,HashFunction, KeyEqual>*  ht_; 
          // HT to which the const_iterator is pointing 
    bucket::const_iterator ptr_; // Pointer to current element in the bucket

  private:

    size_type currBucket() { return (*ht_).S(KeyOfValue()(*ptr_)); }

  public:
    const_iterator(const hash_table<Key, Value, KeyOfValue, HashFunction, 
                                    KeyEqual>*  ht, 
                   bucket::const_iterator ptr) : ht_(ht), ptr_(ptr) { }

    const_iterator(const const_iterator& rhs) 
       :  ht_(rhs.ht_), ptr_(rhs.ptr_) { }

    const_iterator(const iterator& rhs) 
       :  ht_(rhs.ht_), ptr_(rhs.ptr_) { }

    const_iterator() : ht_(0),  ptr_(0) { }

    const_iterator& operator=(const const_iterator& rhs) {
        ptr_=rhs.ptr_;
        return (*this);
    }

    ~const_iterator() {}

    friend bool operator==(const const_iterator& lhs, 
                           const const_iterator& rhs) { 
         return (lhs.ptr_ == rhs.ptr_); 
    }

    bool operator==(const const_iterator& rhs) {
         return (ptr_ == rhs.ptr_);   
    }

    friend bool operator!=(const const_iterator& lhs, 
                           const const_iterator& rhs) {
         return !(lhs == rhs);
    }

    const_reference operator*() { 
         return *ptr_;
    }

    const_iterator& operator++() {
          bucket::const_iterator tmp = ptr_;
          ++tmp;
          const bucket& cb = (*ht_).buckets_[currBucket()];
          if (tmp == cb.end() && cb.next_ != -1)
             ptr_ = (*ht_).buckets_[cb.next_].begin();
          else
             ptr_ = tmp;
          return *this;
    }

    const_iterator operator++(int) 
    {
      const_iterator tmp = *this;
      ++*this;
      return tmp;
    }

  };  // end class const_iterator

  typedef pair<iterator, bool> pair_iterator_bool;
  typedef pair<iterator, iterator> pair_iterator_iterator;
  typedef pair<const_iterator, const_iterator> pair_citerator_citerator;

private:
   table_difference_type S(const key_type& k) const;
   void expand();
   void contract();
   void merge(table_difference_type bucketNumber); 
   void split(table_difference_type bucketNumber);

   void eraseNeb(bucket& b, size_type bNumber); 
   void insertNeb(bucket& b, size_type bNumber); 

protected:

   HashFunction  hf_; // Actual hash function being used
   KeyEqual eql_;      // Function object used to compare keys
   bool insertAlways_; 
               // True if duplicates should be inserted, false otherwise
   size_type  elementCount_; // Keeps track of the # of elements
   size_type  totBuckets_;   // # of buckets allocated
   size_type  usedBuckets_;  // # of buckets actually being used 
   float maxLF_;             // LF that triggers expansion
   float minLF_;             // LF that triggers contraction	
   deque<bucket> buckets_;  // Where the elements are actually stored
   table_difference_type  firstNeb_;           // First non-empty bucket, -1 if empty()
   table_difference_type lastNebIns_;   
              // last bucket that became a non-empty bucket, -1 if empty()

public:
	// allocation - deallocation	
	hash_table(size_type table_size, 
                   const HashFunction& hf = HashFunction(), 
                   const KeyEqual& equal = KeyEqual(),
                   bool always = true);
	hash_table(const value_type* first, const value_type* last, 
                   size_type table_size, 
                   const HashFunction& hf = HashFunction(), 
                   const KeyEqual& equal = KeyEqual(),
                   bool always = true);
//       ~hash_table() { cout << "hash_table destructor called" << endl; }

        // copying, assignment (just use default), swap
	hash_table(const hash_table<Key, Value, KeyOfValue, 
                                     HashFunction, KeyEqual>& other, 
                   bool always = true);

        friend void destroy(hash_table<Key, Value, KeyOfValue, 
			    HashFunction, KeyEqual>* p) { 
	  p->~hash_table(); 
	}

/*
        hash_table<Key, Value, KeyOfValue, 
                                     HashFunction, KeyEqual>&
        operator=(const hash_table<Key, Value, KeyOfValue, 
                                     HashFunction, KeyEqual>& other);
*/

        void swap(hash_table<Key, Value, KeyOfValue, 
                             HashFunction, KeyEqual>& other);

	// accessors
        hash_function hash_funct() const { return hf_; }
	size_type bucketCount() const { return usedBuckets_;}
	size_type size() const { return elementCount_;}
        size_type max_size() const { 
	  return slist<value_type>().max_size();
	}
	bool empty() const { return (size() == 0);}
	float loadFactor() const { 
	  return (size() == 0 ? 0 : (float)size() / bucketCount()); 
        }
        size_type count(const key_type& k) const;

	// insert - erase
	pair_iterator_bool insert (const value_type& v);
	void insert(const_iterator first, const_iterator last);
        void insert(const value_type* first, const value_type* last);
	void erase(iterator position);
	size_type erase(const key_type& k);
        void erase(iterator first, iterator last);

	// set operations
	iterator find(const key_type& k);
	const_iterator find(const key_type& k) const;

	// iterators
	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
        pair_iterator_iterator equal_range(const key_type& k);
        pair_citerator_citerator equal_range(const key_type& k) const;

        // hash table size operations
        size_type bucket_count() const;
        void resize(size_type buckets);
        void set_loadfactors(float minLF, float maxLF);
        float maxloadfactor();
        float minloadfactor();

        // For debugging purposes only:
        friend ostream& 
         operator<<(ostream& o, 
         const hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>& ht);
        vector<size_type> distribution () const;
	void frequencies() const;

};  // end class hash_table;


// allocation - deallocation
template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
hash_table(size_type table_size, const HashFunction& hf, 
           const KeyEqual& equal,
           bool always) :
    hf_(hf), 
    eql_(equal), 
    insertAlways_(always), 
    elementCount_(0), 
    totBuckets_(table_size), 
    usedBuckets_(table_size), 
    maxLF_(2.0), 
    minLF_(0.5), 
    buckets_(table_size), 
    firstNeb_(-1), 
    lastNebIns_(-1)
{
}

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
hash_table(const value_type* first, const value_type* last, 
           size_type table_size, const HashFunction& hf, 
           const KeyEqual& equal,
           bool always) :
    hf_(hf), 
    eql_(equal), 
    insertAlways_(always), 
    elementCount_(0), 
    totBuckets_(table_size), 
    usedBuckets_(table_size), 
    maxLF_(2.0), 
    minLF_(0.5), 
    buckets_(table_size), 
    firstNeb_(-1), 
    lastNebIns_(-1)
{
  insert(first, last);
}


template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
hash_table(const hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>& other, 
    bool always) :
    hf_(other.hf_), 
    eql_(other.eql_), 
    insertAlways_(always), 
    elementCount_(other.elementCount_), 
    totBuckets_(other.totBuckets_), 
    usedBuckets_(other.usedBuckets_), 
    maxLF_(other.maxLF_), 
    minLF_(other.minLF_), 
    buckets_(other.buckets_), 
    firstNeb_(other.firstNeb_), 
    lastNebIns_(other.lastNebIns_)
{
  #ifdef DEBUG
  cerr << " Copy constructor was called" << endl;
  #endif
}

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
inline operator==(const hash_table<Key, Value, KeyOfValue, HashFunction, 
                                   KeyEqual>& lhs, 
                  const hash_table<Key, Value, KeyOfValue, HashFunction, 
                                   KeyEqual>& rhs) {
  return (lhs.buckets_ == rhs.buckets_);
}

/*
template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>&
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
operator=(const hash_table<Key, value_type, KeyOfValue, 
                                     HashFunction, KeyEqual>& other)
{
    hf_ = other.hf_;
    eql_ = other.eql_;
    elementCount_ = other.elementCount_;
    totBuckets_ = other.totBuckets_;
    usedBuckets_ = other.usedBuckets_;
    maxLF_ = other.maxLF_;
    minLF_ = other.minLF_;
    buckets_ = other.buckets_;
    firstNeb_ = other.firstNeb_;
    lastNebIns_ = other.lastNebIns_;
    return *this;
}
*/

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
void 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
swap(hash_table<Key, Value, KeyOfValue, 
                            HashFunction, KeyEqual>& other)
{
    ::swap(hf_, other.hf_);
    ::swap(eql_, other.eql_);
    ::swap(insertAlways_, other.insertAlways_);
    ::swap(elementCount_, other.elementCount_);
    ::swap(totBuckets_, other.totBuckets_);
    ::swap(usedBuckets_, other.usedBuckets_);
    ::swap(maxLF_, other.maxLF_);
    ::swap(minLF_, other.minLF_);
    buckets_.swap(other.buckets_);
    ::swap(firstNeb_, other.firstNeb_);
    ::swap(lastNebIns_, other.lastNebIns_);
}

// Accessors

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::size_type
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
count(const key_type& k) const 
{
  pair_citerator_citerator pii = equal_range(k);
  size_type n = 0;

  for (const_iterator p = pii.first; p != pii.second; ++p) 
      n++;

  return n;
}


// insert - erase
template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::pair_iterator_bool 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
insert(const value_type& v)
{
  size_type hash_value = S(KeyOfValue()(v));
  bucket& b = buckets_[hash_value];

  for (bucket::iterator p = b.begin(); p != b.end(); ++p)
     if (eql_(KeyOfValue()(*p), KeyOfValue()(v)))
        break;

  bucket::iterator where = p;
  bool inserted = false;
  if (b.empty())
      insertNeb(b, hash_value);
  if (p == b.end() || insertAlways_) {
      if (p == b.end()) {
         b.push_front(v);
         where = b.begin();
      }
      else {
	 bucket::iterator previous = p++;
         where = b.insert(previous, p, v);
      }      
#ifdef DEBUG_HTCE
      cout << "Inserting " << v << " in bucket " << hash_value << endl;
#endif
      inserted = true;
      elementCount_++;
      if (loadFactor() > maxLF_)
          expand(); 
    }
    return pair<iterator, bool>(iterator(this, where), inserted);
}

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
void 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
insert(const_iterator first, const_iterator last)
{
  for (const_iterator p = first; p != last; ++p)
      insert(*p);
}

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
void 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
insert(const value_type* first, const value_type* last)
{
  while (first != last)
      insert(*(first++));
}

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
void 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
erase(iterator position)
{
  table_difference_type hash_value = S(KeyOfValue()((*position.ptr_)));
  bucket& b = buckets_[hash_value];
  b.erase(position.ptr_);
  --elementCount_;
  if (loadFactor() < minLF_)
      contract();
  else if (b.empty())
      eraseNeb(b, hash_value);
}

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::size_type
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
erase(const key_type& k)
{
  pair_iterator_iterator pii = equal_range(k);
  size_type n = 0;

  for (iterator p = pii.first; p != pii.second; ++n) 
      erase(p++);

  return n;
}

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
void 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
erase(iterator first, iterator last)
{
  while (first != last) erase(first++);
}

// Search Operations

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::iterator 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
find(const key_type& k) 
{
  size_type hash_value = S(k);
  bucket& b = buckets_[hash_value];

  for (bucket::iterator p = b.begin(); p != b.end(); ++p)
     if (eql_(KeyOfValue()(*p), k))
         return iterator(this, p);

  return end();
}


template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::const_iterator 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
find(const key_type& k) const
{
  size_type hash_value = S(k);
  const bucket& b = buckets_[hash_value];

  for (bucket::const_iterator p = b.begin(); p != b.end(); ++p)
     if (eql_(KeyOfValue()(*p), k))
         return const_iterator(this, p);

  return end();
}

// accessors
template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::iterator 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
begin()
{
  if (!empty())
      return iterator(this, buckets_[firstNeb_].begin());
    else
      return end();
}

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::const_iterator 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
begin() const
{
  if (!empty())
      return const_iterator(this, buckets_[firstNeb_].begin());
  else
      return end();
}

template <class Key, class Value, class KeyOfValue, class HashFunction, class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::iterator 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
end()
{
  return iterator(this, bucket::iterator(NULL));
}

template <class Key, class Value, class KeyOfValue, class HashFunction, class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::const_iterator 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
end() const
{
  return const_iterator(this, bucket::const_iterator(NULL));
}

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::pair_iterator_iterator 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
equal_range(const key_type& k)
{
  iterator p = find(k);
  iterator q = p;

  while (p != end() && eql_(KeyOfValue()(*p), k))
      ++p;

  return pair_iterator_iterator(q, p);
}

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::pair_citerator_citerator 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
equal_range(const key_type& k) const
{
  const_iterator p = find(k);
  const_iterator q = p;

  while (p != end() && eql_(KeyOfValue()(*p), k))
      ++p;

  return pair_citerator_citerator(q, p);
}

// Private methods

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
table_difference_type  
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
S(const key_type& k) const
  // Function that maps the hash function into the size of the table
{
  table_difference_type hv = hf_(k) % totBuckets_;

  if (hv >= usedBuckets_)
      return hv - (totBuckets_ >> 1);
  else
      return hv;
}


template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
void
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
contract() 
{
  --usedBuckets_;
  merge(usedBuckets_);
  if (usedBuckets_ < (totBuckets_ >> 1)) {
      totBuckets_ /= 2;
  }
}

/*
template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
void
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
expand() 
  // If maxLoadFactor > 1, just by adding one slot the new load
  // factor will be within the limits
{
  usedBuckets_++;
  if (usedBuckets_ > totBuckets_)
      totBuckets_ *= 2;
  if (usedBuckets_ > buckets_.size()) {
      bucket::separate_copy(false); 
        // if push_back causes relocation, lists in buckets will not be copied
      { 
        buckets_.push_back(bucket()); 
      } // these braces limit extent of temporary bucket(), causing it not
        // to be erased, as would happen if its extent included next statement
      bucket::separate_copy(true);  // restore normal copying of lists
  }
  split(usedBuckets_-1);
}
*/

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
void
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
expand() 
  // If maxLoadFactor > 1, just by adding one slot the new load
  // factor will be within the limits
{
  usedBuckets_++;
  if (usedBuckets_ > totBuckets_)
      totBuckets_ *= 2;
  if (usedBuckets_ > buckets_.size())
        buckets_.push_back(bucket()); 
  split(usedBuckets_-1);
}

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
void
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
merge(hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
        table_difference_type btm) 
   // Merge entries from bucket btm to bucket (btm - totBuckets/2)
{
  table_difference_type      target = btm - (totBuckets_ >> 1);
  bucket&  bBtm = buckets_[btm];
  bucket&  bTarget = buckets_[target];

  if (!bBtm.empty()) {
      bTarget.splice(bBtm);
      eraseNeb(bBtm, btm); 
  } 
  #ifdef DEBUG_HTCE
      cerr << "Merged bucket #:" << btm << endl; 
  #endif
}


template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
void
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
split(hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
        table_difference_type target) 
  // Move some elements from bucket (target - totBuckets_/2) to bucket target
{
  table_difference_type              bts = target - (totBuckets_ >> 1);
  bucket&          bBts = buckets_[bts];
  bucket&          bTarget = buckets_[target];
  bucket::iterator p(bBts.begin());

  if (!bBts.empty()) {

      // Note placement of p++ in following code; this allows p to
      // be properly advanced before node referred to by p is moved
      while (p != bBts.end())
          if (S(KeyOfValue()(*p)) != bts)
               // ++ must be done here while p is still valid
               bTarget.splice(bBts, p++);
          else
               ++p;

#ifdef DEBUG_HTCE
      ostream_iterator<Key> out(cout, " ");
      cout << "Splicing from bucket " << bts << ":";
      copy(bBts.begin(), bBts.end(), out);
      cout << endl;
      cout << "to bucket " << target << ":";
      copy(bTarget.begin(), bTarget.end(), out);
      cout << endl;
#endif
      // Update list of non-empty buckets
      if(bBts.empty())
          eraseNeb(bBts, bts);
      if (!bTarget.empty())
          insertNeb(bTarget, target);
  }
}

// Functions for maintaining list of non-empty buckets

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
void
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
eraseNeb(bucket& b, size_type bNumber) 
  // Eliminate bucket b from the list of non-empty buckets
  // Precondition: b is a non-empty bucket (i.e b.next_ != -1)
{
  if (b.prev_ != -1)
      buckets_[b.prev_].next_ = b.next_;
  if (b.next_ != -1)
      buckets_[b.next_].prev_ = b.prev_;
  if (bNumber == firstNeb_) 
      firstNeb_ = b.next_;
  if (bNumber == lastNebIns_)
    lastNebIns_ = (b.prev_ != -1 ? b.prev_ : b.next_);
  b.next_ = b.prev_ = -1;  
}

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
void
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
insertNeb(bucket& b, size_type bNumber) 
  // Insert bucket b in the list of non-empty buckets
  // Precondition: b doesn't belong to the list of non-empty buckets, 
  //      i.e. b.next_ == -1
{
  if (firstNeb_ != -1) {   // The hash_table is not empty
      buckets_[lastNebIns_].next_ = bNumber;
      b.prev_ = lastNebIns_;
  } else 
      firstNeb_ = bNumber;
  lastNebIns_ = bNumber;
}

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::size_type 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
bucket_count() const
{
  return usedBuckets_;
}



template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
void 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
resize(size_type buckets)
{
  // not yet implemented
}


template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
void 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
set_loadfactors(float minLF, float maxLF) 
{
  if (0.0 < minLF && minLF < maxLF) {
    minLF_ = minLF;
    maxLF_ = maxLF;
  }
}


template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
inline float 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
maxloadfactor()
{
  return maxLF_;
}

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
inline float 
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
minloadfactor()
{
  return minLF_;
}

// Following functions are for debugging purposes only:

template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
ostream& operator<<(ostream& o, 
                    const hash_table<Key, Value, KeyOfValue, 
                                     HashFunction, KeyEqual>& ht)
{
  o << "HASH TABLE"        << endl
    << "Simulated Size    : " << ht.totBuckets_     << endl
    << "Actual size       : " << ht.usedBuckets_    << endl
    << "Element Count     : " << ht.size()          << endl
    << "Load Factor       : " << ht.loadFactor()    << endl
    << "Max Load Factor   : " << ht.maxLF_          << endl
    << "Min Load Factor   : " << ht.minLF_          << endl
    << "First_NEB         : " << ht.firstNeb_       << endl;

/*
  o << "Non-empty Buckets:" << endl;
  for (int i = 0; i < ht.bucketCount(); i++) 
     if (!ht.buckets_[i].empty())
	o << "Bucket[" << i << "] : "
          << ht.buckets_[i] << endl;
*/
  ht.frequencies();

  return o;
}
 
template <class Key, class Value, class KeyOfValue, class HashFunction, 
          class KeyEqual>
vector<hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::size_type>
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
distribution() const 
{
  vector<size_type> v(bucketCount());

  for (table_size_type i = 0; i < bucketCount(); i++)
      v[i] = buckets_[i].size();

  return v;
}

template <class Key, class Value, class KeyOfValue, class HashFunction, class KeyEqual>
void
hash_table<Key, Value, KeyOfValue, HashFunction, KeyEqual>::
frequencies() const 
{
  fmap m;

  for (table_size_type i = 0; i < bucketCount(); i++) 
      if (m.find(buckets_[i].size()) == m.end())
         m[buckets_[i].size()] = 1;
      else
         ++m[buckets_[i].size()];

  cout << "\nFrequencies:" << endl;
  for (fmap::iterator q = m.begin(); q != m.end(); ++q)
    cout << (*q).first << " " << (*q).second << endl;
  cout << endl;

  cout << "\nAverage loading of non-empty buckets: " << endl;
  float total = 0.0;
  for (q = m.begin(); q != m.end(); ++q)
     total += ((*q).second) * ((*q).first)*((*q).first + 1)/2;
  cout << total/size() << endl;

}

#endif


