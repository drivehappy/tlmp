#pragma once

#include "DataTypes.h"

template<class T>
class slist {
private:
  T*list;
  ptr_t allocated;
  uint count;
public:
  slist() : list(0), allocated(0), count(0) {}
  ~slist() {
    if (!list) return;
    for (index_t i=0;i<count;i++) list[i].~T();
    mm_free(list);
  }
  slist(const slist&n) : list(0), allocated(0), count(0) {
    copy(n);
  }
  slist&operator=(const slist&n) {
    copy(n);
    return *this;
  }
  void copy(const slist&from) {
    resize(from.size());
    for (uint i=0;i<from.size();i++) {
      list[i] = from[i];
    }
  }
  void clear() {
    if (!list) return;
    for (index_t i=0;i<count;i++) list[i].~T();
    count = 0;
  }
  void realloc_bytes(ptr_t new_alloc) {
    if (!list) {
      list=(T*)mm_alloc(new_alloc);
      if (mm_has_advanced()) allocated=mm_block_size(list);
      else allocated=new_alloc;
      return;
    }
//     if (mm_has_advanced()) {
//       if (mm_realloc_inplace(list,allocated)) return;
//     }
    T*old_list = list;
    list=(T*)mm_alloc(new_alloc);
    if (mm_has_advanced()) allocated=mm_block_size(list);
    else allocated=new_alloc;
    if (old_list==list) return;
    else {
      for (size_t i=0;i<count;i++) {
        new (&list[i]) T(old_list[i]);
        old_list[i].~T();
      }
    }
    mm_free(old_list);
  }
  void minalloc_bytes(ptr_t alloc) {
    if (allocated<alloc) realloc_bytes(allocated|0x400+alloc);
  }
  void realloc(uint size) {
    realloc_bytes(size*sizeof(T));
  }
  void minalloc(uint size) {
    minalloc_bytes(size*sizeof(T));
  }
  index_t add(const T&value) {
    index_t i = count;
    minalloc(count+1);
    count++;
    new (&list[i]) T(value);
    return i;
  }
  index_t insert(index_t at,const T&value) {
    if (count>=(at+1)) {
      minalloc(count+1);
      new (&list[count]) T(list[count-1]);
      if (count>at+1)
        for (index_t i=count-1;i>at;i--)  list[i] = list[i-1];
      count++;
    } else {
      minsize(at+1);
    }
    list[at] = value;
    return at;
  }
  T&get(index_t at) const {
    return list[at];
  }
  T&operator[](index_t at) const {
    return get(at);
  }
  index_t getbyval(const T&val) const {
    for (index_t i=0;i<count;i++) if (val==list[i]) return i;
    return -1;
  }
  void remove_swap(index_t at) {
    count--;
    if (at!=count) list[at] = list[count];
    list[count].~T();
  }
  void remove_keeporder(index_t at) {
    count--;
    for (index_t i=at;i<count;i++) list[i]=list[i+1];
    list[count].~T();
  }
  void remove(index_t at) {
    remove_swap(at);
  }
  uint size() const {return count;}
  void minsize(uint size) {
    if (count<size) {
      minalloc(size);
      for (index_t i=count;i<size;i++) new (&list[i]) T;
      count=size;
    }
  }
  void resize(uint size) {
    if (size<count) {
      for (index_t i=count-1;i!=size-1;i--) list[i].~T();
      count=size;
      realloc(size);
    } else if (size>count) {
      realloc(size);
      for (index_t i=count;i<size;i++) new (&list[i]) T();
      count=size;
    }
  }
};

template<class T>
class slist_table_value {
public:
  bool inuse;
  T val;
  slist_table_value() : inuse(false) {}
  slist_table_value(const T&val) : inuse(true), val(val) {}
  bool operator==(const slist_table_value&n) const {
    return inuse==n.inuse && val==n.val;
  }
};

template<class T>
class slist_table:public slist<slist_table_value<T> > {
public:
  typedef slist_table_value<T> valtype;
  typedef slist<valtype> base;
  uint mid_frees;
  uint inuse_count;
  slist_table() : mid_frees(0), inuse_count(0) {}
  bool inuse(index_t at) const {
    if (at>=base::size()) return false;
    return base::get(at).inuse;
  }
  T&get(index_t at) {
    if (at>=base::size()) base::insert(at,valtype());
    valtype&v = base::get(at);
    if (!v.inuse) {
      //v.inuse = true;
      v = T();
      inuse_count++;
    }
    return v.val;
  }
  T&operator[](index_t at) {
    return get(at);
  }
  bool remove(index_t at) {
    if (at>=base::size()) return false;
    if (at<base::size()-1) mid_frees++;
    valtype&v = base::get(at);
    if (!v.inuse) return false;
    v.inuse = false;
    inuse_count--;
    return true;
  }
  index_t add(const T&value) {
    if (!mid_frees) {
      inuse_count++;
      if (base::size()) {
        valtype&v=base::get(base::size()-1);
        if (!v.inuse) {
          v = value;
          return base::size()-1;
        }
      }
      return base::add(value);
    } else {
      //for (uint i=base::size()-1;i!=-1;i--) {
      for (uint i=0;i<base::size();i++) {
        valtype&v=base::get(i);
        if (!v.inuse) {
          mid_frees--;
          v = value;
          inuse_count++;
          return i;
        }
      }
      //xcept("slist_table::add inconsistency");
      //throw "slist_table::add inconsistency";
      return -1; // should never get here
    }
  }
  uint inuse_size() const {return inuse_count;}
  class iterator {
  public:
    index_t i;
    slist_table&owner;
    iterator(slist_table&owner) : owner(owner), i(-1) {++*this;}
    iterator(slist_table&owner,index_t i) : owner(owner), i(i) {}
    iterator&operator ++() {
      i++;
      while (!owner.inuse(i)&&i<owner.size()) ++i;
      return *this;
    }
    index_t key() const {
      return i;
    }
    T&val() const {
      return owner[i];
    }
    T&operator *() const {
      return val();
    }
    bool operator !=(const iterator&n) const {
      return i!=n.i;
    }
//     T&operator->() {
//       return val();
//     }
  };
  iterator begin() {
    return ++iterator(*this,-1);
  }
  iterator end() {
    return iterator(*this,base::size());
  }
};

template<class T>
class linked_node {
public:
  linked_node<T>*ll_next,*ll_prev;
  T val;
  linked_node() : ll_next(0), ll_prev(0) {}
  linked_node(linked_node<T>*ll_next,linked_node<T>*ll_prev) : ll_next(ll_next), ll_prev(ll_prev) {}
  linked_node(const T&val) : val(val), ll_next(0), ll_prev(0) {}
  T&operator->() const {return val;}
  operator T&() {return val;}
  operator T*() {return &val;}
  linked_node*ll_ins_next(linked_node*n) {
    if (ll_next) ll_next->prev = n;
    ll_next = n;
    n->ll_prev = this;
    return n;
  }
  linked_node*ll_ins_prev(linked_node*n) {
    if (ll_prev) ll_prev->ll_next = n;
    ll_prev = n;
    n->ll_next = this;
    return n;
  }
  linked_node*ll_tail(linked_node*n) {
    ll_next = n;
    n->ll_prev = this;
    return n;
  }
  linked_node*ll_head(linked_node*n) {
    ll_prev = n;
    n->ll_next = this;
    return n;
  }
  void ll_unlink() {
    if (ll_next) ll_next->ll_prev = ll_prev;
    if (ll_prev) ll_prev->ll_next = ll_next;
  }
};
// class linked_node_ptr {
// public:
//   void*ptr;
//   template<class T>
//   operator linked_node<T>*() const {return (linked_node<T>*)ptr;}
//   operator void*() const {return ptr;}
//   linked_node<int>*operator ->() const {return (linked_node<int>*)ptr;}
//   linked_node_ptr() : ptr(0) {}
//   linked_node_ptr(void*ptr) : ptr(ptr) {}
//   template<class T>
//   linked_node_ptr(linked_node<T>*ptr) : ptr((void*)ptr) {}
// };
template<class T>
class linked_list {
public:
  linked_node<T>*head,*tail;
  linked_list() : head(0), tail(0) {}
  linked_node<T>*mknode(const T&n) {
    return new linked_node<T>(n);
  }
  linked_node<T>*ll_push_head(linked_node<T>*n) {
    n->ll_next = head;
    if (head) head->ll_prev = n;
    return head = n;
  }
  linked_node<T>*ll_push_tail(linked_node<T>*n) {
    n->ll_prev = tail;
    if (tail) tail->ll_next = n;
    return tail = n;
  }
  linked_node<T>*ll_get_next(linked_node<T>*n) const {
    if (!n) return head;
    else return n->ll_next;
  }
  linked_node<T>*get_prev(linked_node<T>*n) const {
    if (!n) return tail;
    else return n->ll_prev;
  }
  linked_node<T>*ll_get_head() const {
    return head;
  }
  linked_node<T>*ll_get_tail() const {
    return tail;
  }
  linked_node<T>*ll_pop_head() const {
    linked_node<T>*n = head;
    head = n->ll_next;
  }
  linked_node<T>*ll_pop_tail() const {
    linked_node<T>*n = tail;
    tail = n->ll_prev;
  }
  linked_node<T>*push_head(const T&n) {
    return ll_push_head(mknode(n));
  }
  class iterator {
  public:
    linked_list&owner;
    linked_node<T>*n;
    iterator(linked_list&owner) : owner(owner) {}
    iterator(linked_list&owner,linked_node<T>*n) : owner(owner), n(n) {}
    T&val() const {return *n;}
    T&operator*() const {return val();}
    iterator&operator ++() {
      n=owner.ll_get_next(n);
      return *this;
    }
    bool operator ==(iterator&o) {
      return n==o.n;
    }
    bool operator !=(iterator&o) {
      return n!=o.n;
    }
  };
  iterator begin() {
    return iterator(*this,head);
  }
  iterator end() {
    return iterator(*this,tail);
  }
};
/*
class linked_list {
public:
  linked_node_ptr head,tail;
  linked_list() : head(0), tail(0) {}
  template<class T>
  linked_node<T>*push_head(linked_node<T>*n) {
    n->ll_next = head;
    if (head) head->ll_prev = n;
    return head = n;
  }
  template<class T>
  linked_node<T>*push_tail(linked_node<T>*n) {
    n->ll_prev = tail;
    if (tail) tail->ll_next = n;
    return tail = n;
  }
  template<class T>
  linked_node<T>*get_next(linked_node<T>*n) const {
    if (!n) return head;
    else return n->ll_next;
  }
  template<class T>
  linked_node<T>*get_prev(linked_node<T>*n) const {
    if (!n) return tail;
    else return n->ll_prev;
  }
  template<class T>
  linked_node<T>*get_head() const {
    return head;
  }
  template<class T>
  linked_node<T>*get_tail() const {
    return tail;
  }
  template<class T>
  linked_node<T>*pop_head() const {
    linked_node<T>*n = head;
    head = n->ll_next;
  }
  template<class T>
  linked_node<T>*pop_tail() const {
    linked_node<T>*n = tail;
    tail = n->ll_prev;
  }
};*/
/*
template<class T>
static index_t table_get_index(const T&n);
template<>
#ifdef _WIN32
static
#endif
index_t table_get_index<uint>(const uint&n) {
  return n;
}*/

class table_get_index {
public:
  template<class T>
  uint operator()(const T&n) {return n;}
  uint operator()(void*p) {return 0;}
};

template<class key_T,class val_T,class get_index=table_get_index,index_t index_size=0x100>
class table {
public:
  struct item {
    key_T key;
    val_T val;
    item() {}
    item(const key_T&k,const val_T&v) : key(k), val(v) {}
  };
  slist_table<item> index_list[index_size];
  void clear() {
    for (uint i=0;i<index_size;i++) index_list[i].clear();
  }
  item&add(index_t index,const key_T&k,const val_T&v) {
    slist_table<item>&l=index_list[index%index_size];
    return l[l.add(item(k,v))];
  }
  val_T&get_by_index(index_t i,const key_T&at) {
    slist_table<item>&l=index_list[i%index_size];
    for (uint it=l.size()-1;it!=-1;it--) {
      if (l.inuse(it)&&l[it].key==at) {
        return l[it].val;
      }
    }
    return add(i,at,val_T()).val;
  }
  bool inuse(const key_T&at) {
    slist_table<item>&l=index_list[get_index()(at)%index_size];
    for (uint it=l.size()-1;it!=-1;it--) {
      if (l.inuse(it)&&l[it].key==at) {
        return true;
      }
    }
    return false;
  }
  bool inuse_get(const key_T&at,val_T*&ret) {
    slist_table<item>&l=index_list[get_index()(at)%index_size];
    for (uint it=l.size()-1;it!=-1;it--) {
      if (l.inuse(it)&&l[it].key==at) {
        ret = &l[it].val;
        return true;
      }
    }
    return false;
  }
  val_T&get(const key_T&at) {
    return get_by_index(get_index()(at),at);
  }
  val_T&operator [](const key_T&at) {
    return get(at);
  }
  bool remove(const key_T&at) {
    slist_table<item>&l=index_list[get_index()(at)%index_size];
    for (uint i=l.size()-1;i!=-1;i--) {
      if (l.inuse(i)&&l[i].key==at) {
        l.remove(i);
      }
    }
    return false;
  }
  class iterator {
  public:
    index_t index;
    index_t i;
    table&owner;
    iterator(table&owner,index_t index, index_t i) : owner(owner), index(index), i(i) {}
    iterator&operator ++() {
      i++;
      if (i>=owner.index_list[index].size()) {index++;i=0;}
      if (index==index_size) return *this;
      while (!owner.index_list[index].inuse(i)) {
        i++;
        if (i>=owner.index_list[index].size()) {index++;i=0;}
        if (index==index_size) return *this;
      }
      return *this;
    }
    key_T&key() {
      return owner.index_list[index][i].key;
    }
    val_T&val() {
      return owner.index_list[index][i].val;
    }
    val_T&operator *() {
      return val();
    }
    bool operator !=(const iterator&n) {
      return index!=n.index||i!=n.i;
    }
  };
  iterator begin() {
    return ++iterator(*this,0,-1);
  }
  iterator end() {
    return iterator(*this,index_size,0);
  }
  uint size() {
    uint count=0;
    for (uint i=index_size-1;i!=-1;i--) {
      count+=index_list[i].inuse_size();
    }
    return count;
  }
};
