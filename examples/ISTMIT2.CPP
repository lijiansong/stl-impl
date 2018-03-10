// This example compiles using the new STL<ToolKit> from ObjectSpace, Inc.
// STL<ToolKit> is the EASIEST to use STL that works on most platform/compiler 
// combinations, including cfront, Borland, Visual C++, C Set++, ObjectCenter, 
// and the latest Sun & HP compilers. Read the README.STL file in this 
// directory for more information, or send email to info@objectspace.com.
// For an overview of STL, read the OVERVIEW.STL file in this directory.

#include <iostream.h>
#include <fstream.h>
#include <stl.h>

typedef vector<char> Line;
 
void printLine (const Line* line_)
{
  vector<char>::const_iterator i;
  for (i = line_->begin (); i != line_->end (); i++)
   cout << *i;
  cout << endl;
}

int main ()
{
  Line buffer;
  vector<Line*> lines;
  ifstream s ("data.txt");
  s.unsetf (ios::skipws); // Disable white-space skipping.
  istream_iterator<char, ptrdiff_t> it1 (s); // Position at start of file.
  istream_iterator<char, ptrdiff_t> it2; // Serves as "past-the-end" marker.
  copy (it1, it2, back_inserter (buffer));
  Line::iterator i = buffer.begin ();
  Line::iterator p;
  while (i != buffer.end ())
  {
    p = find (i, buffer.end (), '\n');
    lines.push_back (new Line (i, p));
    i = ++p;
  }
  sort (lines.begin (), lines.end (), less_p<Line*> ());
  cout << "Read " << lines.size () << " lines" << endl;
  vector<Line*>::iterator j;
  for (j = lines.begin (); j != lines.end (); j++)
    printLine (*j);
  release (lines.begin (), lines.end ()); // Release memory.
  return 0;
}
