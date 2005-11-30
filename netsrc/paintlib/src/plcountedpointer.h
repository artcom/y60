#ifndef PLCOUNTED_POINTER_H
#define PLCOUNTED_POINTER_H


// if the counted pointer is used as a pointer to a fundamental type
// then a warning is generated about the overload of the -> operator
// as its use would now be invalid. Not a lot we can do about this
// except ignore the warning. (this is what the STL does with iterators)
//
// something similar might be needed for other compilers as well
#ifdef _MSC_VER
#pragma warning(disable: 4284)
#endif

#include <stdlib.h>

template <class type>
class PLCountedPointer
{
public:
  explicit  PLCountedPointer(type * pType = 0)
              : pBody(pType), pCount(new size_t(1))         {}
            PLCountedPointer(const PLCountedPointer & copy)
              : pBody(copy.pBody), pCount(copy.pCount)      { incCount(); }
            ~PLCountedPointer()                             { decCount(); }

            PLCountedPointer& operator=(const PLCountedPointer & rhs);

  type *    operator -> () const                            { return pBody; }
  type &    operator * () const                             { return *pBody; }

  type *    get() const                                     { return pBody; }

  bool      operator == (const PLCountedPointer & rhs) const  { return pBody == rhs.pBody; }
  bool      operator != (const PLCountedPointer & rhs) const  { return pBody != rhs.pBody; }

  operator bool () const                                    { return pBody != 0; }

private:
  void      incCount()                                      { ++*pCount; }
  void      decCount();

private:
  type *      pBody;
  size_t *    pCount;
};

template <class type>
PLCountedPointer<type>& PLCountedPointer<type>::operator=(const PLCountedPointer & rhs)
{
  if (pBody != rhs.pBody)
  {
    decCount();
    pBody = rhs.pBody;
    pCount = rhs.pCount;
    incCount();
  }
  return *this;
}

template <class type>
void PLCountedPointer<type>::decCount()
{
  if (!--*pCount)
  {
    delete pBody;
    delete pCount;
  }
}

template <class type>
class PLCountedArrayPointer
{
public:
  explicit  PLCountedArrayPointer(type * pType = 0)
              : pBody(pType), pCount(new size_t(1))         {}
            PLCountedArrayPointer(const PLCountedArrayPointer & copy)
              : pBody(copy.pBody), pCount(copy.pCount)      { incCount(); }
            ~PLCountedArrayPointer()                        { decCount(); }

            PLCountedArrayPointer& operator=(const PLCountedArrayPointer & rhs);

  type *    operator -> () const                            { return pBody; }
  type &    operator * () const                             { return *pBody; }

  type *    get() const                                     { return pBody; }

  bool      operator == (const PLCountedArrayPointer & rhs) const { return pBody == rhs.pBody; }
  bool      operator != (const PLCountedArrayPointer & rhs) const { return pBody != rhs.pBody; }

  type &       operator [] (size_t i)                       { return pBody[i]; }
  const type & operator [] (size_t i) const                 { return pBody[i]; }

  operator bool () const                                    { return pBody != 0; }

private:
  void      incCount()                                      { ++*pCount; }
  void      decCount();

private:
  type *      pBody;
  size_t *    pCount;
};

template <class type>
PLCountedArrayPointer<type>& PLCountedArrayPointer<type>::operator=(const PLCountedArrayPointer & rhs)
{
  if (pBody != rhs.pBody)
  {
    decCount();
    pBody = rhs.pBody;
    pCount = rhs.pCount;
    incCount();
  }
  return *this;
}

template <class type>
void PLCountedArrayPointer<type>::decCount()
{
  if (!--*pCount)
  {
    delete [] pBody;
    delete pCount;
  }
}

#endif
