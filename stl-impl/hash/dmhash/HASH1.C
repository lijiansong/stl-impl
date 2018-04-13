// Use a hash_set to store a dictionary and look up a few words

#include <iostream.h>
#include <fstream.h>
#include <bstring.h>
#include <hashset.h>
#include <hashfun.h>

typedef hash_set<string, hash_fun1, equal_to<string> > set_1;

void lookup(const set_1& hs, const string& word)
  // Look up word in the dictionary stored in hash_set hs 
  // and report whether or not it was found.  If the word was found
  // and was not the last word in the dictionary, report the next
  // word following it ("next" in no particular order)

{
  cout << "Looking for " << word << " in the dictionary... ";
  set_1::iterator i = hs.find(word);

  if (i != hs.end())
    cout << "Found it." << endl;
  else {
    cout << "Didn't find it." << endl;
    return;
  }

  if (++i != hs.end())
    cout << "The next word after " << word << " in the dictionary is "
         << *i << endl;
  else
    cout << word << " is the last word in the dictionary.\n" << endl;
}

int main()
{
  string name("/usr/dict/words");

  ifstream ifs(name.c_str());
  typedef istream_iterator<string, ptrdiff_t> string_input;
  string_input i(ifs), eos;

  set_1 hs(10007);
  hs.set_loadfactors(0.5, 1.5);

  cout << "Reading file " << name << endl;

  while (i != eos) 
      hs.insert(*i++);

  cout << "The dictionary has " << hs.size() << " entries\n";
  cout << "There are " << hs.bucket_count() << " hash table buckets.\n\n";

  lookup(hs, "hash");
  lookup(hs, "table");
  lookup(hs, "mispelled");
  lookup(hs, "zygote");

}
