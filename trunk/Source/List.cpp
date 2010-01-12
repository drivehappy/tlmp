#include "List.h"

using namespace TLMP;

template<typename t>
size_t c_runic_list<t>::size()
{
  return count;
}

template<typename t>
t& c_runic_list<t>::operator[](int index)
{
  return list[index];
}

// copy of function at 525140
template<typename t>
void c_runic_list<t>::grow()
{
  if (!list) {
    allocated = growth;
    list = new t[allocated];
  } else {
    size_t new_alloc = allocated+growth;
    t*new_list = new t[new_alloc];
    for (index_t i=0;i<allocated;i++) {
      new_list[i] = list[i];
    }
    delete [] list;
    allocated = new_alloc;
    list = new_list;
  }
}

template<typename t>
void c_runic_list<t>::append(const t&val) {
  if (count>=allocated) grow();
  list[count++] = val;
}