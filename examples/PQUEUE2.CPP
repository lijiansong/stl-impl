// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <stl.h>

int main ()
{
  priority_queue<deque<char*>, greater_s> q;
  q.push ((char*) "cat");
  q.push ((char*) "dog");
  q.push ((char*) "ape");
  while (!q.empty ())
  {
    cout << q.top () << endl;
    q.pop ();
  }
  return 0;
}
