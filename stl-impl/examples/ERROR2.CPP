// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>

// Compile this code with the symbol OS_USE_EXCEPTIONS defined.

int main ()
{
  vector<int> v;
  try
  {
    v.pop_back (); // Generates an exception.
  }
  catch (const char* str)
  {
    cout << "Caught exception " << str << endl;
  }
  return 0;
}
