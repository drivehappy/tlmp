#pragma once

// Developed by dengus
//
namespace TLMP {

  template<typename t>
  struct c_runic_list {
    t *list;
    size_t count;
    size_t allocated;
    size_t growth;
    size_t size();
    t& operator[](int index);

    // copy of function at 525140
    void grow();
    void append(const t& val);
  };

};
