/////////////////////////////////////////////////// vim: sw=3 ts=8 et
//
// Tiny Template Library: a sorted vector of key-value pairs
//
// Can be used as a small map: the underlying data structure is a sorted array.
// It has much less element overhead than a red-black tree implementation.
//
// This code is Public Domain
//
#ifndef _TINY_TEMPLATE_LIBRARY_SORTED_VECTOR_MAP_HPP_
#define _TINY_TEMPLATE_LIBRARY_SORTED_VECTOR_MAP_HPP_ 1

#include "types.hpp"
#include "utility.hpp"
#include "functional.hpp"

namespace ttl
{
   template<typename KT, typename T, typename Compare = ttl::less<KT> >
   class sorted_vector_map // unique keys to values
   {
   public:
      typedef KT key_type;
      typedef T mapped_type;
      typedef ttl::pair<KT, T> value_type;
      typedef ttl::size_t size_type;
      typedef ttl::ptrdiff_t difference_type;
      typedef Compare key_compare;

      typedef value_type &reference;
      typedef const value_type &const_reference;

      typedef value_type *pointer;
      typedef const value_type *const_pointer;

      struct const_iterator;

      struct iterator
      {
      public:
         typedef sorted_vector_map<KT,T,Compare>::value_type value_type;
         typedef ttl::ptrdiff_t difference_type;
         typedef value_type *pointer;
         typedef value_type *reference;

         value_type &operator*() const { return **ptr_; }
         value_type *operator->() const { return *ptr_; }
         iterator &operator++() { ++ptr_; return *this; }
         iterator operator++(int) { iterator tmp(*this); ++ptr_; return tmp; }
         iterator &operator--() { --ptr_; return *this; }
         iterator operator--(int) { iterator tmp(*this); --ptr_; return tmp; }

         bool operator==(const iterator &other) const { return ptr_ == other.ptr_; }
         bool operator!=(const iterator &other) const { return ptr_ != other.ptr_; }
         bool operator==(const const_iterator &other) const { return other == *this; }
         bool operator!=(const const_iterator &other) const { return other != *this; }
      private:
         value_type **ptr_;
         friend class sorted_vector_map<KT,T,Compare>;
         friend class sorted_vector_map<KT,T,Compare>::const_iterator;
         iterator(value_type **ptr): ptr_(ptr) {}
      };
      struct const_iterator
      {
      public:
         typedef sorted_vector_map<KT,T,Compare>::value_type value_type;
         typedef ttl::ptrdiff_t difference_type;
         typedef value_type *pointer;
         typedef value_type *reference;

         const value_type &operator*() const { return **ptr_; }
         const value_type *operator->() const { return *ptr_; }
         const_iterator &operator++() { ++ptr_; return *this; }
         const_iterator operator++(int) { const_iterator tmp(*this); ++ptr_; return tmp; }
         const_iterator &operator--() { --ptr_; return *this; }
         const_iterator operator--(int) { const_iterator tmp(*this); --ptr_; return tmp; }

         bool operator==(const const_iterator &other) const { return ptr_ == other.ptr_; }
         bool operator==(const iterator &other) const { return ptr_ == other.ptr_; }
         bool operator!=(const const_iterator &other) const { return ptr_ != other.ptr_; }
         bool operator!=(const iterator &other) const { return ptr_ != other.ptr_; }

         const_iterator(const iterator &other): ptr_(other.ptr_) {}
      private:
         const value_type * const *ptr_;
         friend class sorted_vector_map<KT,T,Compare>;
         const_iterator(const value_type * const *ptr): ptr_(ptr) {}
      };

   public:
      explicit sorted_vector_map():
         elements_(0), last_(0), end_of_elements_(0)
      {}
      sorted_vector_map(const sorted_vector_map& other);
      explicit sorted_vector_map(size_type prealloc)
      {
         elements_ = last_ = new value_type*[prealloc];
         end_of_elements_ = elements_ + prealloc;
      }
      template<class InputIt>
      sorted_vector_map(InputIt first, InputIt last);

      ~sorted_vector_map()
      {
         for (value_type **i = elements_; i != last_; ++i)
            delete *i;
         delete [] elements_;
      }

      sorted_vector_map &operator=(const sorted_vector_map &other);

      T &operator[](const KT &key)
      {
         iterator i = find_insert_pos(key);
         if (i != end() && i->first == key)
            return i->second;
         return insert_before(i, ttl::make_pair(key, T()))->second;
      }

      T &at(const KT &key) { return find(key)->second; }
      const T &at(const KT &key) const { return find(key)->second; }

      iterator       begin() { return iterator(elements_); }
      const_iterator begin() const { return const_iterator(elements_); }
      iterator       end() { return iterator(last_); }
      const_iterator end() const { return const_iterator(last_); }
      const_iterator cbegin() const { return const_iterator(elements_); }
      const_iterator cend() const { return const_iterator(last_); }

      size_type size() const { return last_ - elements_; }
      bool empty() const { return last_ == elements_; }
      size_type max_size() const { return (size_type)-1 / sizeof(value_type); }

      void clear()
      {
         while (last_ > elements_)
            delete *--last_;
      }

      ttl::pair<iterator,bool> insert(const value_type &value)
      {
         iterator i = find_insert_pos(value.first);
         if (i != end() && i->first == value.first)
            return ttl::pair<iterator, bool>(i, false);
         return ttl::pair<iterator, bool>(insert_before(i, value), true);
      }
      iterator insert(iterator, const value_type &);

      template<class InputIt>
      void insert(InputIt first, InputIt last);

      iterator erase(const_iterator pos);
      iterator erase(const_iterator first, const_iterator last);
      size_type erase(const key_type &key);

      void swap(sorted_vector_map &other);

      iterator find(const KT &key);
      const_iterator find(const KT &key) const;

      size_type count(const KT &key) const { return find(key) == cend(); }

      ttl::pair<iterator,iterator> equal_range(const KT &key)
      {
         iterator i1 = find(key);
         iterator i2 = i1;
         if (i2 != end())
            ++i2;
         return ttl::make_pair(i1, i2);
      }
      ttl::pair<const_iterator,const_iterator> equal_range(const KT &key) const
      {
         const_iterator i1 = find(key);
         const_iterator i2 = i1;
         if (i2 != cend())
            ++i2;
         return ttl::make_pair(i1, i2);
      }

      iterator lower_bound(const KT &key) { return find_insert_pos(key); }
      const_iterator lower_bound(const KT &key) const { return find_insert_pos(key); }

      iterator upper_bound(const KT &key)
      {
         iterator i = find_insert_pos(key);
         while (i != end() && i->first == key)
            ++i;
         return i;
      }
      const_iterator upper_bound(const KT &key) const
      {
         const_iterator i = find_insert_pos(key);
         while (i != cend() && i->first == key)
            ++i;
         return i;
      }

      key_compare key_comp() const { return Compare(); }

      struct value_compare
      {
      protected:
         friend class sorted_vector_map<KT, T, Compare>;
         value_compare() {}
      public:
         typedef value_type first_argument_type;
         typedef value_type second_argument_type;
         typedef bool result_type;

         bool operator()(const value_type &a, const value_type &b) const
         {
            return Compare()(a.first, b.first);
         }
      };
      value_compare value_comp() const { return value_compare(); }

   private:
      value_type **elements_, **last_, **end_of_elements_;
      iterator insert_before(iterator, const value_type &);
      iterator find_insert_pos(const KT &key) const;
      pair<unsigned, bool> bsearch(const KT &key) const;
   };

   template<typename KT, typename T, typename Compare>
   sorted_vector_map<KT,T,Compare>::sorted_vector_map(const sorted_vector_map& other)
   {
      size_type prealloc = other.end_of_elements_ - other.elements_;
      elements_ = last_ = new value_type*[prealloc];
      end_of_elements_ = elements_ + prealloc;
      for (const value_type * const *i = other.elements_; i != other.last_; ++i)
         *last_++ = new value_type(**i);
   }

   template<typename KT, typename T, typename Compare>
   pair<unsigned, bool>
   sorted_vector_map<KT,T,Compare>::bsearch(const KT &key) const
   {
      Compare comp;
      unsigned L = 0, H = size();
      while (L < H) {
         unsigned m = L + (H - L) / 2;
         if (elements_[m]->first == key)
            return pair<unsigned, bool>(m, true);
         if (comp(elements_[m]->first, key))
            L = m + 1;
         else
            H = m;
      }
      return pair<unsigned, bool>(L, false);
   }

   template<typename KT, typename T, typename Compare>
   typename sorted_vector_map<KT,T,Compare>::iterator
   sorted_vector_map<KT,T,Compare>::find(const KT &key)
   {
      pair<unsigned, bool> re = bsearch(key);
      return re.second ? iterator(elements_ + re.first): end();
   }
   template<typename KT, typename T, typename Compare>
   typename sorted_vector_map<KT,T,Compare>::const_iterator
   sorted_vector_map<KT,T,Compare>::find(const KT &key) const
   {
      pair<unsigned, bool> re = bsearch(key);
      return re.second ? const_iterator(elements_ + re.first): end();
   }
   template<typename KT, typename T, typename Compare>
   typename sorted_vector_map<KT,T,Compare>::iterator
   sorted_vector_map<KT,T,Compare>::find_insert_pos(const KT &key) const
   {
      pair<unsigned, bool> re = bsearch(key);
      return elements_ + re.first;
   }
   template<typename KT, typename T, typename Compare>
   typename sorted_vector_map<KT,T,Compare>::iterator
   sorted_vector_map<KT,T,Compare>::insert_before(iterator pos, const value_type& value)
   {
      if (end_of_elements_ - last_ < 1)
      {
         value_type **o;
         value_type **i;
         size_type newcapacity = end_of_elements_ - elements_;
         newcapacity += newcapacity/2; // temporarily funny...
         if (!newcapacity)
            newcapacity = 2;
         value_type **newelements = o = new value_type *[newcapacity];
         for (i = elements_; i != pos.ptr_;)
            *o++ = *i++;
         *o = new value_type(value);
         pos = iterator(o++);
         for (; i != last_;)
            *o++ = *i++;
         delete [] elements_;
         elements_ = newelements;
         end_of_elements_ = elements_ + newcapacity;
         last_ = o;
         while (o < end_of_elements_)
            *o++ = (value_type *)0;
         return pos;
      }
      value_type **i = last_++;
      value_type **o = last_;
      while (i != pos.ptr_)
         *--o = *--i;
      *i = new value_type(value);
      return iterator(i);
   }
}
#endif // _TINY_TEMPLATE_LIBRARY_SORTED_VECTOR_MAP_HPP_
