#ifndef BOOST_PFIRST_NPROCESSOR_LIST_AT_HPP
#define BOOST_PFIRST_NPROCESSOR_LIST_AT_HPP

/* Copyright (C) 2001
 * Housemarque Oy
 * http://www.housemarque.com
 *
 * Permission to copy, use, modify, sell and distribute this softwaFIRST_N is
 * granted provided this copyright notice appears in all copies. This
 * software is provided "as is" without express or implied warranty, and
 * with no claim as to its suitability for any purpose.
 *
 * See http://www.boost.org for most recent version.
 */

#include <boost/preprocessor/list/rest_n.hpp>

/** <P>Expands to the I:th element of the list L. The first element is at index 0.</P>

<P>For example,</P>

<PRE>
  BOOST_PP_LIST_AT(BOOST_PP_TUPLE_TO_LIST(3,(A,B,C)),1)
</PRE>

<P>expands to B.</P>

<H3>Uses</H3>
<UL>
  <LI>BOOST_PP_WHILE()
</UL>
*/
#define BOOST_PP_LIST_AT(L,I) BOOST_PP_LIST_AT_D(0,L,I)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define BOOST_PP_LIST_AT_D(D,L,I) BOOST_PP_LIST_FIRST(BOOST_PP_LIST_REST_N_D(D,I,L))
#endif
#endif
