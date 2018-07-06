//
//  mulle_sprintf_function.h
//  mulle-sprintf
//
//  Created by Nat!
//  Copyright (c) 2011 Nat! - Mulle kybernetiK.
//  Copyright (c) 2011 Codeon GmbH.
//  All rights reserved.
//
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  Redistributions of source code must retain the above copyright notice, this
//  list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
//  Neither the name of Mulle kybernetiK nor the names of its contributors
//  may be used to endorse or promote products derived from this software
//  without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//

#ifndef mulle_sprintf_function_h__
#define mulle_sprintf_function_h__

#include "include.h"


#include <wctype.h>
#include <stdarg.h>


struct _NSDecimal;

struct mulle_sprintf_formatconversionflags
{
   unsigned int   argument_index_found:1;

   unsigned int   zero_found:1;
   unsigned int   minus_found:1;
   unsigned int   space_found:1;

   unsigned int   hash_found:1;
   unsigned int   plus_found:1;
   unsigned int   quote_found:1;

   unsigned int   width_found:1;          // get rid of this with -1 as not found in width
   unsigned int   width_is_argument:1;
   unsigned int   width_is_indexed_argument:1;

   unsigned int   precision_found:1;      // get rid of this with -1 as not found in precision
   unsigned int   precision_is_argument:1;
   unsigned int   precision_is_indexed_argument:1;

   unsigned int   modifier_found:1;
   unsigned int   left_justify:1;         // not filled during parse

   unsigned int   unused:1;
};



struct mulle_sprintf_formatconversioninfo
{
   void    *mystery;       // used for return type conversion...

   int     width;          // %100d = 100
   int     precision;      // %.3d  = 3
   int     argv_index[ 3];
   int     length;         // width of conversion format: e.g.%%=2, %20d = 4

   struct mulle_sprintf_formatconversionflags   memory;

   char    separator;
   char    modifier[ 3];
   char    conversion;
};


enum
{
   mulle_sprintf_int_argumenttype = 0,
   mulle_sprintf_char_argumenttype,
   mulle_sprintf_char_pointer_argumenttype,
   mulle_sprintf_double_argumenttype,

   mulle_sprintf_intmax_t_argumenttype,
   mulle_sprintf_long_argumenttype,
   mulle_sprintf_long_double_argumenttype,
   mulle_sprintf_long_long_argumenttype,

   mulle_sprintf_object_argumenttype,
   mulle_sprintf_ptrdiff_t_argumenttype,
   mulle_sprintf_int64_t_argumenttype,
   mulle_sprintf_short_argumenttype,

   mulle_sprintf_signed_size_t_argumenttype,
   mulle_sprintf_size_t_argumenttype,
   mulle_sprintf_size_t_pointer_argumenttype,
   mulle_sprintf_uint64_t_argumenttype,

   mulle_sprintf_uint64_t_pointer_argumenttype,
   mulle_sprintf_uintmax_t_argumenttype,
   mulle_sprintf_uintmax_t_pointer_argumenttype,
   mulle_sprintf_unsigned_char_argumenttype,

   mulle_sprintf_unsigned_char_pointer_argumenttype,
   mulle_sprintf_unsigned_int_argumenttype,
   mulle_sprintf_unsigned_int_pointer_argumenttype,
   mulle_sprintf_unsigned_long_argumenttype,

   mulle_sprintf_unsigned_long_long_argumenttype,
   mulle_sprintf_unsigned_long_long_pointer_argumenttype,
   mulle_sprintf_unsigned_long_pointer_argumenttype,
   mulle_sprintf_unsigned_ptrdiff_t_argumenttype,

   mulle_sprintf_unsigned_ptrdiff_t_pointer_argumenttype,
   mulle_sprintf_unsigned_short_argumenttype,
   mulle_sprintf_unsigned_short_pointer_argumenttype,
   mulle_sprintf_vector_argumenttype,

   mulle_sprintf_void_argumenttype,
   mulle_sprintf_void_pointer_argumenttype,
   mulle_sprintf_wchar_pointer_argumenttype,
   mulle_sprintf_wint_t_argumenttype,

   // ugliness ensues...
   mulle_sprintf_NSDecimal_pointer_argumenttype
};

typedef unsigned char   mulle_sprintf_argumenttype_t;
typedef unsigned char   mulle_sprintf_conversioncharacter_t;

union mulle_sprintf_argumentvalue
{
   int                 i;
   char                c;
   char                *pc;
   double              d;
   intmax_t            imt;
   long                l;
   long double         ld;
   long long           ll;
   ptrdiff_t           dif;
   void                *obj;
   int64_t             qt;
   short               s;
   size_t              St;
   size_t              *pSt;
   uint64_t            Qt;
   uint64_t            *pQt;
   uintmax_t           Imt;
   uintmax_t           *pImt;
   unsigned char       C;
   unsigned char       *pC;
   unsigned int        I;
   unsigned int        *pI;
   unsigned long       L;
   unsigned long long  LL;
   unsigned long       *pL;
   unsigned long long  *pLL;
   unsigned __PTRDIFF_TYPE__  Dif;
   unsigned __PTRDIFF_TYPE__  *pDif;
   unsigned short      S;
   unsigned short      *pS;
   void                *pv;
   wchar_t             *pwc;
   wint_t              wc;
   struct _NSDecimal   *pDecimal;
};


extern unsigned char   mulle_sprintf_argumentsize[];

struct mulle_sprintf_argumentarray
{
   union mulle_sprintf_argumentvalue   *values;
   mulle_sprintf_argumenttype_t        *types;
   size_t                               size;
};


void  mulle_mvsprintf_set_values( union mulle_sprintf_argumentvalue *p,
                                  mulle_sprintf_argumenttype_t  *type,
                                  unsigned int n,
                                  mulle_vararg_list va);

void  mulle_vsprintf_set_values( union mulle_sprintf_argumentvalue *p,
                                 mulle_sprintf_argumenttype_t  *type,
                                 unsigned int n,
                                 va_list va);


struct mulle_sprintf_function
{
   mulle_sprintf_argumenttype_t   (*determine_argument_type)( struct mulle_sprintf_formatconversioninfo *info);
   int                            (*convert_argument)( struct mulle_buffer *buffer,
                                                 struct mulle_sprintf_formatconversioninfo *info,
                                                 struct mulle_sprintf_argumentarray *arguments,
                                                 int i);
};


#define mulle_sprintf_vectorsize    ('~' + 1 - ' ')
#define mulle_sprintf_modifiersize  ('~' + 1 - ' ')

typedef char  mulle_sprintf_modifiercharacter;

typedef struct mulle_sprintf_function      *mulle_sprintf_vector_t[  mulle_sprintf_vectorsize];
typedef mulle_sprintf_modifiercharacter    mulle_sprintf_modifier_t[ mulle_sprintf_modifiersize];


struct mulle_sprintf_conversion
{
   mulle_sprintf_vector_t     jumps;
   mulle_sprintf_modifier_t   modifiers;
};


static inline unsigned int   mulle_sprintf_index_for_character( int c)
{
   assert( c >= ' ' && c <= '~');
   return( c - ' ');
}


static inline int   mulle_sprintf_is_modifier_character( mulle_sprintf_modifier_t table, int c)
{
   unsigned int    i;

   i = mulle_sprintf_index_for_character( c);
   return( table[ i]);
}


int   mulle_sprintf_register_functions( struct mulle_sprintf_conversion *table,
                                        struct mulle_sprintf_function *functions,
                                        mulle_sprintf_conversioncharacter_t c);
int   mulle_sprintf_register_modifier( struct mulle_sprintf_conversion *table,
                                       mulle_sprintf_modifiercharacter c);
int   mulle_sprintf_register_modifiers( struct mulle_sprintf_conversion *table,
                                        mulle_sprintf_modifiercharacter *s);

int   mulle_sprintf_register_default_functions( struct mulle_sprintf_function *functions,
                                                mulle_sprintf_conversioncharacter_t c);
int   mulle_sprintf_register_default_modifier( mulle_sprintf_modifiercharacter c);
int   mulle_sprintf_register_default_modifiers( mulle_sprintf_modifiercharacter *s);

void  mulle_sprintf_register_default_modifiers_on_load( void);
int   mulle_sprintf_register_standardmodifiers( struct mulle_sprintf_conversion *table);

#endif
