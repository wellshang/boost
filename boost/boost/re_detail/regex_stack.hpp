/*
 *
 * Copyright (c) 1998-2000
 * Dr John Maddock
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Dr John Maddock makes no representations
 * about the suitability of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
 *
 */

 /*
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE         regex_stack.hpp
  *   VERSION      3.01
  *   DESCRIPTION: Implements customised internal regex stacks.
  *                Note this is an internal header file included
  *                by regex.hpp, do not include on its own.
  */

#ifndef BOOST_REGEX_STACK_HPP
#define BOOST_REGEX_STACK_HPP

#ifndef BOOST_REGEX_CONFIG_HPP
#include <boost/re_detail/regex_config.hpp>
#endif
#ifndef BOOST_REGEX_RAW_BUFFER_HPP
#include <boost/re_detail/regex_raw_buffer.hpp>
#endif

namespace boost{
   namespace re_detail{

#ifdef __BORLANDC__
   #if __BORLANDC__ == 0x530
    #pragma option push -a4 -b -Ve
   #elif __BORLANDC__ > 0x530
    #pragma option push -a8 -b -Ve
   #endif
#endif

//
// class jstack
// simplified stack optimised for push/peek/pop
// operations, we could use std::stack<std::vector<T>> instead...
//
template <class T, class Allocator BOOST_RE_DEF_ALLOC_PARAM(T) >
class jstack
{
private:
   typedef BOOST_RE_MAYBE_TYPENAME REBIND_TYPE(unsigned char, Allocator) allocator_type;
   typedef typename REBIND_TYPE(T, Allocator)::size_type size_type;
   typedef T value_type;
   struct node
   {
      node* next;
      T* start;  // first item
      T* end;    // last item
      T* last;   // end of storage
   };
   
   //
   // empty base member optimisation:
   struct data : public allocator_type
   {
      padding buf[(sizeof(T) * 16 + sizeof(padding) - 1) / sizeof(padding)];
      data(const Allocator& a) : allocator_type(a){}
   };

   data alloc_inst;
   mutable node* m_stack;
   mutable node* unused;
   node base;
   size_type block_size;

   void BOOST_RE_CALL pop_aux()const;
   void BOOST_RE_CALL push_aux();

public:
   jstack(size_type n = 64, const Allocator& a = Allocator());

   ~jstack();

   node* BOOST_RE_CALL get_node()
   {
      node* new_stack = (node*)alloc_inst.allocate(sizeof(node) + sizeof(T) * block_size);
      new_stack->last = (T*)(new_stack+1);
      new_stack->start = new_stack->end = new_stack->last + block_size;
      new_stack->next = 0;
      return new_stack;
   }

   bool BOOST_RE_CALL empty()
   {
      return (m_stack->start == m_stack->end) && (m_stack->next == 0);
   }

   bool BOOST_RE_CALL good()
   {
      return (m_stack->start != m_stack->end) || (m_stack->next != 0);
   }

   T& BOOST_RE_CALL peek()
   {
      if(m_stack->start == m_stack->end)
         pop_aux();
      return *m_stack->end;
   }

   const T& BOOST_RE_CALL peek()const
   {
      if(m_stack->start == m_stack->end)
         pop_aux();
      return *m_stack->end;
   }

   void BOOST_RE_CALL pop()
   {
      if(m_stack->start == m_stack->end)
         pop_aux();
      jm_destroy(m_stack->end);
      ++(m_stack->end);
   }

   void BOOST_RE_CALL pop(T& t)
   {
      if(m_stack->start == m_stack->end)
         pop_aux();
      t = *m_stack->end;
      jm_destroy(m_stack->end);
      ++(m_stack->end);
   }

   void BOOST_RE_CALL push(const T& t)
   {
      if(m_stack->end == m_stack->last)
         push_aux();
      --(m_stack->end);
      jm_construct(m_stack->end, t);
   }

};

template <class T, class Allocator>
jstack<T, Allocator>::jstack(size_type n, const Allocator& a)
    : alloc_inst(a)
{
  unused = 0;
  block_size = n;
  m_stack = &base;
  base.last = reinterpret_cast<T*>(alloc_inst.buf);
  base.end = base.start = base.last + 16;
  base.next = 0;
}

template <class T, class Allocator>
void BOOST_RE_CALL jstack<T, Allocator>::push_aux()
{
   // make sure we have spare space on TOS:
   register node* new_node;
   if(unused)
   {
      new_node = unused;
      unused = new_node->next;
      new_node->next = m_stack;
      m_stack = new_node;
   }
   else
   {
      new_node = get_node();
      new_node->next = m_stack;
      m_stack = new_node;
   }
}

template <class T, class Allocator>
void BOOST_RE_CALL jstack<T, Allocator>::pop_aux()const
{
   // make sure that we have a valid item
   // on TOS:
   jm_assert(m_stack->next);
   register node* p = m_stack;
   m_stack = p->next;
   p->next = unused;
   unused = p;
}

template <class T, class Allocator>
jstack<T, Allocator>::~jstack()
{
   node* condemned;
   while(good())
      pop();
   while(unused)
   {
      condemned = unused;
      unused = unused->next;
      alloc_inst.deallocate((unsigned char*)condemned, sizeof(node) + sizeof(T) * block_size);
   }
   while(m_stack != &base)
   {
      condemned = m_stack;
      m_stack = m_stack->next;
      alloc_inst.deallocate((unsigned char*)condemned, sizeof(node) + sizeof(T) * block_size);
   }
}

#ifdef __BORLANDC__
 #if __BORLANDC__ > 0x520
  #pragma option pop
 #endif
#endif

} // namespace re_detail
} // namespace boost

#endif








