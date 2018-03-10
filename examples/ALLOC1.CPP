// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <ospace/stl/examples/myaloc.h>


int main ()
{
  {
    cout << "vectors:" << endl;
    os_my_allocator<int> alloc;
    vector<int> v3 (alloc);
    v3.push_back (42);
    vector<int> v4 (alloc);
    v4.push_back (42);
  }

  {
    cout << "bit_vectors:" << endl;
    os_my_allocator<unsigned int> alloc;
    bit_vector v1 (alloc);
    v1.push_back (1);
  }

  {
    cout << "deques:" << endl;
    os_my_allocator<int> alloc;
    deque<int> d (alloc);
    d.push_back (42);
  }

  {
    cout << "lists:" << endl;
    os_my_allocator<os_list_node<int> > alloc;
    list<int> l (alloc);
    l.push_back (42);
  }

  {
    cout << "sets:" << endl;
    os_my_allocator<os_value_node<int> > alloc;
    set<int, less<int> > s (alloc);
    s.insert (42);
  }

  {
    cout << "maps" << endl;
    os_my_allocator<os_value_node<os_pair<const int, float> > > alloc;
    map<int, float, less<int> > m (alloc);
    m[4] = 2.0;
  }

  return 0;
}
