// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <stl.h>

// Compile this code without defining OS_USE_EXCEPTIONS.

void my_handler (int code_, const char* str_)
{
  cout << "Caught " << str_ << " [code " << code_ << "]" << endl;
}

int main ()
{
  os_handler_function_t old_h = os_set_error_handler (my_handler);
  vector<int> v;
  v.pop_back (); // Generates an empty object error.
  cout << "returned from pop_back()" << endl;
  os_set_error_handler (old_h);
  v.pop_back (); // Generates an empty object error.
  cout << "successful termination" << endl;
  return 0;
}
