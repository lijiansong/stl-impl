// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

class X
{
  public:
    X (int i_ = 0) : i (i_) {}
    operator int () const { return i; }
 
  private:
    int i;
};

int main ()
{
  os_heap_allocator<X> a;
  // Allocate (but do not construct) storage for 5 elements.
  os_heap_allocator<X>::pointer p = a.allocate (5); 
  raw_storage_iterator<X*, X> r (p);
  for (int i = 0; i < 5; i++)
    *r++ = X (i);
  for (i = 0; i < 5; i++)
    cout << *p++ << endl;
  return 0;
}
