// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <iostream.h>

class X
{
  public:
    X (int i_) : i (i_) {}
    ~X () { cout << "Delete X(" << i << ")" << endl; }
    int i;
};

ostream& operator << (ostream& stream_, const X& x_)
{
  return stream_ << "X(" << x_.i << ")";
}

int main ()
{
  vector<X*> v;
  v.push_back (new X (2));
  v.push_back (new X (1));
  v.push_back (new X (4));
  vector<X*>::iterator i;
  for (i = v.begin (); i != v.end (); i++)
    cout << *(*i) << endl;
  release (v.begin (), v.end ()); // Delete heap-based objects.
  return 0;
}
