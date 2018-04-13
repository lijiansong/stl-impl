#ifndef OS_STL_MYALOC_H
#define OS_STL_MYALOC_H

#include <ospace/stl/allocate.h>


template <class T>
class os_my_allocator : public os_allocator <T>
{
  typedef size_t size_type;

  public:
    // ********** allocation **********

    // Return a pointer to raw memory for n_ objects of type T. The
    // objects should be constructed using the in-place new operator
    // after this function is called.
    /* virtual */ T* allocate (size_type n_)
    {
      T* tmp = ::allocate ((difference_type) n_, (T*) 0);
      cout << "allocate " << n_ << " objects @ " << tmp << endl;
      return tmp;
    }

    // Return a pointer to raw memory for n_ pointers to objects of
    // type T.
    /* virtual */ T** allocate_ptrs (size_type n_)
    {
      T** tmp = ::allocate ((difference_type) n_, (T**) 0);
      cout << "allocate " << n_ << " pointers @ " << tmp << endl;
      return tmp;
    }

#ifdef OS_NO_DESTROY
    // This function is only necessary if your compiler does not 
    // support templatized destructors. Return a pointer to n_ 
    // objects of type T.
    /* virtual */ T* construct_array (size_type n_)
    {
      T* tmp = new T [n_];
      cout << "construct  array of "<<n_ <<" objects @ "<< tmp << endl;
      return tmp;
    }
#endif

    // ********** destruction **********

    // Deallocate the raw storage that used to store objects of type
    // T. The objects should have been destroyed before this function
    // is called.
    /* virtual */ void deallocate (T* ptr_)
    {
      cout << "deallocate objects @ " << ptr_ << endl;
      ::deallocate (ptr_);
    }

    // Deallocate the raw storage that used to store pointers to
    // object of type T.
    /* virtual */ void deallocate_ptrs (T** ptr_)
    {
      cout << "deallocate pointers @ " << ptr_ << endl;
      ::deallocate (ptr_);
    }

#ifdef OS_NO_DESTROY
    // This function is only necessary if your compiler does not
    // support templatized destructors. Deallocate the array of
    // objects pointed to by ptr_, calling each object's destructor.
    /* virtual */ void destroy_array (T* ptr_)
    {
      cout << "destroy array of objects @ " << ptr_ << endl;
      delete [] ptr_;
    }

    // This function is only necessary if your compiler does not
    // support templatized destructors. Deallocate the single object 
    // pointed to by ptr_, calling the object's destructor.
    /* virtual */ void destroy (T* ptr_)
    {
      cout << "destroy object @ " << ptr_ << endl;
      delete ptr_;
    }
#endif

    // ********** sizing ***********

    // Return the number of objects in a page of storage. For 
    // efficiency reasons, vectors and deques objects in blocks of
    // this size.
    /* virtual */ size_type init_page_size () const
    {
      cout << "return # of objects in a page of storage" << endl;
      return max (size_type (1), size_type (4096 / sizeof (T)));
    }

    // Return the number of pointers in a page of storage.
    /* virtual */ size_type init_page_size_ptrs () const
    {
      cout << "return # of ptrs in a page of storage" << endl;
      return max (size_type (1), size_type (4096 / sizeof (T*)));
    }

    // Return the maximum number of objects that I can allocate.
    /* virtual */ size_type max_size () const
    {
      cout << "return max# of objects that may be allocated" << endl;
      return max (size_type (1), size_type (UINT_MAX / sizeof (T)));
    }
};

#endif
