// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>
#include <stdlib.h>
#include <iostream.h>

class MyRandomGenerator
{
  public:
    unsigned long operator () (unsigned long n_);
};

unsigned long
MyRandomGenerator::operator () (unsigned long n_)
{
  return rand () % n_;
}

int main ()
{
  vector <int> v1(10);
  iota (v1.begin (), v1.end (), 0);
  ostream_iterator <int> iter (cout, " ");
  copy (v1.begin (), v1.end (), iter);
  cout << endl;
  MyRandomGenerator r;
  for (int i = 0; i < 3; i++)
  {
    random_shuffle (v1.begin (), v1.end (), r);
    copy (v1.begin (), v1.end (), iter);
    cout << endl;
  }
  return 0;
}
