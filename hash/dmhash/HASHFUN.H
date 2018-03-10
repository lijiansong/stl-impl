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

// A few hash functions that can be used with unordered associative 
// containers hash_set, hash_multiset, hash_map, or hash_multimap.
// They all map from a string to a long, where a string
// is as defined in bstring.h (basic_string<char>).  Each is defined
// in a function object type suitable for passing to a constructor
// of an unordered associative container.

#ifndef HASH_FUNCTIONS

#define HASH_FUNCTIONS

#include <iostream.h>
#include <bstring.h>

class hash_fun1 {
  protected:
    long prime;

  public:
    hash_fun1(long p = 1073741827) : prime(p) {}
    long operator()(const string& c) const;
};
      
long hash_fun1::operator()(const string& c) const
{
      int n = c.length();
      const char* d = c.data();
      long h = 0; 
      
      for (int i = 0; i < n; ++i, ++d)
          h = (h << 2) + *d;

      return ((h >= 0) ? (h % prime) : (-h % prime)); 
}

class hash_fun2 {
  protected:
    long prime;

  public:
    hash_fun2(long p = 1073741827) : prime(p) {}
    long operator()(const string& c) const;
};

long hash_fun2::operator()(const string& c) const
{
      int n = c.length();
      const char* d = c.data();
      long h = n; 
      
      for (int i = 0; i < n; ++i, ++d)
          h = 613*h + *d;

      return ((h >= 0) ? (h % prime) : (-h % prime)); 
}
      
class hash_fun3 {
  protected:
    long prime;

  public:
    hash_fun3(long p = 1073741827) : prime(p) {}
    long operator()(const string& c) const;
};


long hash_fun3::operator()(const string& c) const 
{
      int n = c.length();
      long h = n; 
      const char* d = c.data();
      const float A = 0.6125423371;
      
      for (int i = 0; i < n; ++i, ++d)
          h = (h ^ ((int)(*d) << (i + 8)));
      
      return ((h >= 0) ? (h*A) : (-h*A)); 
}

#endif

