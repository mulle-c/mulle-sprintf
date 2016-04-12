/*
 *  MulleFoundation - A tiny Foundation replacement
 *
 *  mulle_sprintfInteger_conversion.c is a part of MulleFoundation
 *
 *  Copyright (C) 2011 Nat!, Mulle kybernetiK 
 *  All rights reserved.
 *
 *  Coded by Nat!
 *
 *  $Id$
 *
 */
#include "mulle_sprintf_int.h"


typedef struct
{
   char  *(*convert_unsigned_int)( unsigned int value, char *s);
   char  *(*convert_locale_unsigned_int)( unsigned int value, char *s);
   char  *(*convert_unsigned_long_long)( unsigned long long value, char *s);
   char  *(*convert_locale_unsigned_long_long)( unsigned long long value, char *s);
   ssize_t   (*set_prefix)( char *s, int value_is_zero, size_t length,
                        size_t precision);
} integer_converters;


#pragma mark -
#pragma mark decimal

static ssize_t   set_decimal_prefix( char *s,
                                 int value_is_zero, 
                                 size_t length,
                                 size_t precision)
{
   return( 0);
}


static char  *convert_decimal_unsigned_int( unsigned int  value, 
                                            char *s)
{
   while( value)
   {
      *--s  = '0' + value % 10;
      value /= 10;
   }
   
   return( s);
}


static char  *convert_decimal_unsigned_long_long( unsigned long long value, 
                                                  char *s)
{
   while( value)
   {
      *--s  = '0' + value % 10;
      value /= 10;
   }
   
   return( s);
}


static integer_converters  decimal_converters = 
{
   convert_decimal_unsigned_int,
   convert_decimal_unsigned_int,
   convert_decimal_unsigned_long_long,
   convert_decimal_unsigned_long_long,
   set_decimal_prefix
};


void   mulle_sprintf_justified( struct mulle_buffer *buffer, 
                                struct mulle_sprintf_formatconversioninfo *info,
                                char *p,
                                ssize_t p_length,
                                char *q,
                                ssize_t q_length,
                                ssize_t  precision,
                                char prefix)
{
   ssize_t    length;
   ssize_t    total;
   ssize_t    used;
   ssize_t    width;
   char       precision_char;
   char       width_char;

   length         = p_length + q_length;
   precision_char = '0';
   width          = info->width;
   width_char     = info->memory.zero_found ? '0' : ' ';

   if( info->memory.left_justify)
   {
      used = mulle_buffer_get_length( buffer);
      if( prefix)
         mulle_buffer_add_byte( buffer, prefix);
      if( q_length)
         mulle_buffer_add_bytes( buffer, q, q_length);
      if( precision > p_length)
         mulle_buffer_memset( buffer, precision_char, precision - p_length);
      mulle_buffer_add_bytes( buffer, p, p_length);
      used = mulle_buffer_get_length( buffer) - used;
      if( width > used)
         mulle_buffer_memset( buffer, width_char, width - used);
   }
   else
   {
      total = length;
      if( prefix)
         total += 1;
      if( precision > p_length)
         total += precision - p_length;
      if( width > total && width_char != '0')
         mulle_buffer_memset( buffer, width_char, width - total);
      if( prefix)
         mulle_buffer_add_byte( buffer, prefix);
      if( width > total && width_char == '0')
         mulle_buffer_memset( buffer, width_char, width - total);
      if( q_length)
         mulle_buffer_add_bytes( buffer, q, q_length);
      if( precision > length)
         mulle_buffer_memset( buffer, precision_char, precision - p_length);
      mulle_buffer_add_bytes( buffer, p, p_length);
   }
}


void   mulle_sprintf_justified_and_prefixed( struct mulle_buffer *buffer, 
                                             struct mulle_sprintf_formatconversioninfo *info,
                                             char *p,
                                             ssize_t p_length,
                                             char prefix,
                                             int is_zero,
                                             ssize_t (*set_prefix)( char *, int, size_t, size_t))
{
   ssize_t    precision;
   ssize_t    q_length;
   char       tmp2[ 4];

   precision = info->memory.precision_found ? info->precision : 1; 
   q_length  = 0;
   
   if( info->memory.hash_found )
   {
      q_length = (*set_prefix)( tmp2, is_zero, p_length, precision);
      if( q_length < 0)
      {
         precision = p_length - q_length;  // octal hack not pretty
         q_length  = 0;
      }
   }
   
   mulle_sprintf_justified( buffer, info, p, p_length, tmp2, q_length, precision, prefix);
}


static int   integer_conversion( struct mulle_sprintf_formatconversioninfo *info,
                                 struct mulle_buffer *buffer,
                                 struct mulle_sprintf_argumentarray *arguments,
                                 int argc,
                                 integer_converters *converters,
                                 int is_signed)
{
   union mulle_sprintf_argumentvalue   v;
   mulle_sprintf_argumenttype_t        t;
   size_t                              s;
   char                                prefix;
   char                                tmp[ sizeof( long long) * 4];
   char                                *p;
   ssize_t                             p_length;
   unsigned long long                  vLL;
   long long                           vll;
   
   v = arguments->values[ argc];
   t = arguments->types[ argc];
   s = mulle_sprintf_argumentsize[ t];
   
   vll = (s == sizeof( int)) ? v.i : (s == sizeof( long)) ? v.l : v.ll;
   vLL = vll;
   
   prefix = 0;
   
   if( is_signed)
   {
      if( vll < 0)
      {
         vLL     = -vll;
         prefix  = '-';
      }
      else
         if( info->memory.plus_found)
            prefix = '+';
         else
            if( info->memory.space_found)
               prefix  = ' ';
   }

   p = &tmp[ sizeof( tmp)];

   // place ',' appropriately
   if( info->memory.quote_found)
   {
      if( s == sizeof( int))
         p = (*converters->convert_locale_unsigned_int)( (unsigned int) vLL, p);
      else
         p = (*converters->convert_locale_unsigned_long_long)( vLL, p);
   }
   else
      if( s == sizeof( int))
         p = (*converters->convert_unsigned_int)( (unsigned int) vLL, p);
      else
         p = (*converters->convert_unsigned_long_long)( vLL, p);

   // ok we gotz da digit, now build it up, from the front
   p_length = &tmp[ sizeof( tmp)] - p;

   // p is a the front now
   mulle_sprintf_justified_and_prefixed( buffer, info, p, p_length, prefix, vLL == 0, converters->set_prefix);
   
   return( 0);
}


int   mulle_sprintf_int_decimal_conversion( struct mulle_buffer *buffer,
                                         struct mulle_sprintf_formatconversioninfo *info,
                                         struct mulle_sprintf_argumentarray *arguments,
                                         int argc)
{
   return( integer_conversion( info, buffer, arguments, argc, &decimal_converters, 1));
}


int   mulle_sprintf_unsigned_int_decimal_conversion( struct mulle_buffer *buffer,
                                                 struct mulle_sprintf_formatconversioninfo *info,
                                                 struct mulle_sprintf_argumentarray *arguments,
                                                 int argc)
{
   return( integer_conversion( info, buffer, arguments, argc, &decimal_converters, 0));
}


#pragma mark -
#pragma mark octal


static char  *convert_octal_unsigned_int( unsigned int value, 
                                          char *s)
{
   while( value)
   {
      *--s  = '0' + (value & 0x7);
      value >>= 3;
   }
   
   return( s);
}


static char  *convert_octal_unsigned_long_long( unsigned long long value, 
                                                char *s)
{
   while( value)
   {
      *--s  = '0' + (value & 0x7);
      value >>= 3;
   }
   
   return( s);
}


static ssize_t   set_octal_prefix( char *s,
                                   int value_is_zero,
                                   size_t length,
                                   size_t precision)
{
   if( length && precision <= length)
      return( -1);      // increase precision
   return( 0);
}



static integer_converters  octal_converters = 
{
   convert_octal_unsigned_int,
   convert_octal_unsigned_int,
   convert_octal_unsigned_long_long,
   convert_octal_unsigned_long_long,
   set_octal_prefix
};


int   mulle_sprintf_int_octal_conversion( struct mulle_buffer *buffer,
                                        struct mulle_sprintf_formatconversioninfo *info,
                                        struct mulle_sprintf_argumentarray *arguments,
                                        int argc)
{
   return( integer_conversion( info, buffer, arguments, argc, &octal_converters, 0));
}                   


#pragma mark -
#pragma mark hex


static char   *convert_hex_unsigned_int( unsigned int value, 
                                         char *s)
{
   char   v;
   
   while( value)
   {
      v     = (value & 0xF);
      *--s  = (v >= 10) ? ('a' - 10 + v) : '0' + v;
      value >>= 4;
   }
   
   return( s);
}


static char   *convert_hex_unsigned_long_long( unsigned long long value, 
                                               char *s)
{
   unsigned char   v;
   
   while( value)
   {
      v     = (unsigned char) value & 0xF;
      *--s  = (v >= 10) ? ('a' - 10 + v) : '0' + v;
      value >>= 4;
   }
   
   return( s);
}


static ssize_t   set_hex_prefix( char *s, int value_is_zero,
                                 size_t length, size_t precision)
{
   if( length)
   {
      *s    = '0';
      s[ 1] = 'x';
      return( 2);
   }
   return( 0);
}


static integer_converters  hex_converters =
{
   convert_hex_unsigned_int,
   convert_hex_unsigned_int,
   convert_hex_unsigned_long_long,
   convert_hex_unsigned_long_long,
   set_hex_prefix
};


int   mulle_sprintf_int_hex_conversion( struct mulle_buffer *buffer,
                                            struct mulle_sprintf_formatconversioninfo *info,
                                            struct mulle_sprintf_argumentarray *arguments,
                                            int argc)
{                                      
   return( integer_conversion( info, buffer, arguments, argc, &hex_converters, 0));
}                   


#pragma mark -
#pragma mark long conversions


int   mulle_sprintf_long_decimal_conversion( struct mulle_buffer *buffer,
                                       struct mulle_sprintf_formatconversioninfo *info,
                                       struct mulle_sprintf_argumentarray *arguments,
                                       int argc)
{
   return( integer_conversion( info, buffer, arguments, argc, &decimal_converters, 1));
}                   



int   mulle_sprintf_unsigned_long_decimal_conversion( struct mulle_buffer *buffer,
                                               struct mulle_sprintf_formatconversioninfo *info,
                                               struct mulle_sprintf_argumentarray *arguments,
                                               int argc)
{
   return( integer_conversion( info, buffer, arguments, argc, &decimal_converters, 1));
}                   


int   mulle_sprintf_long_hex_conversion( struct mulle_buffer *buffer,
                                   struct mulle_sprintf_formatconversioninfo *info,
                                   struct mulle_sprintf_argumentarray *arguments,
                                   int argc)
{
   return( integer_conversion( info, buffer, arguments, argc, &hex_converters, 0));
}                   


int   mulle_sprintf_long_octal_conversion( struct mulle_buffer *buffer,
                                     struct mulle_sprintf_formatconversioninfo *info,
                                     struct mulle_sprintf_argumentarray *arguments,
                                     int argc)
{
   return( integer_conversion( info, buffer, arguments, argc, &octal_converters, 0));
}                   


mulle_sprintf_argumenttype_t  mulle_sprintf_get_signed_int_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
      switch( info->modifier[ 0])
      {
      case 'h' :
         if( info->modifier[ 1] == 'h')
            return( mulle_sprintf_char_argumenttype);
         return( mulle_sprintf_short_argumenttype);
         
      case 'l' :
         if( info->modifier[ 1] == 'l')
            return( mulle_sprintf_long_long_argumenttype);
         return( mulle_sprintf_long_argumenttype);
         
      case 'j' : return( mulle_sprintf_intmax_t_argumenttype);
      case 'q' : return( mulle_sprintf_int64_t_argumenttype);
      case 't' : return( mulle_sprintf_ptrdiff_t_argumenttype);
      case 'z' : return( mulle_sprintf_signed_size_t_argumenttype);
      }
      return( mulle_sprintf_int_argumenttype);
}


mulle_sprintf_argumenttype_t  mulle_sprintf_get_unsigned_int_argumenttype( struct mulle_sprintf_formatconversioninfo *info)
{
      switch( info->modifier[ 0])
      {
      case 'h' :
         if( info->modifier[ 1] == 'h')
            return( mulle_sprintf_unsigned_char_argumenttype);
         return( mulle_sprintf_unsigned_short_argumenttype);
         
      case 'l' :
         if( info->modifier[ 1] == 'l')
            return( mulle_sprintf_unsigned_long_long_argumenttype);
         return( mulle_sprintf_unsigned_long_argumenttype);
         
      case 'j' : return( mulle_sprintf_uintmax_t_argumenttype);
      case 'q' : return( mulle_sprintf_uint64_t_argumenttype);
      case 't' : return( mulle_sprintf_unsigned_ptrdiff_t_argumenttype);
      case 'z' : return( mulle_sprintf_size_t_argumenttype);
      }
      return( mulle_sprintf_unsigned_int_argumenttype);
}


struct mulle_sprintf_function     mulle_sprintf_int_decimal_functions = 
{
   mulle_sprintf_get_signed_int_argumenttype,
   mulle_sprintf_long_decimal_conversion
};


struct mulle_sprintf_function     mulle_sprintf_unsigned_int_decimal_functions = 
{
   mulle_sprintf_get_unsigned_int_argumenttype,
   mulle_sprintf_unsigned_int_decimal_conversion
};


struct mulle_sprintf_function     mulle_sprintf_int_octal_functions = 
{
   mulle_sprintf_get_unsigned_int_argumenttype,
   mulle_sprintf_long_octal_conversion
};


struct mulle_sprintf_function     mulle_sprintf_int_hex_functions = 
{
   mulle_sprintf_get_unsigned_int_argumenttype,
   mulle_sprintf_int_hex_conversion
};


struct mulle_sprintf_function     mulle_sprintf_long_decimal_functions = 
{
   mulle_sprintf_get_signed_int_argumenttype,
   mulle_sprintf_long_decimal_conversion
};


struct mulle_sprintf_function     mulle_sprintf_unsigned_long_decimal_functions = 
{
   mulle_sprintf_get_unsigned_int_argumenttype,
   mulle_sprintf_long_decimal_conversion
};


struct mulle_sprintf_function     mulle_sprintf_long_hex_functions = 
{
   mulle_sprintf_get_unsigned_int_argumenttype,
   mulle_sprintf_long_hex_conversion
};


struct mulle_sprintf_function     mulle_sprintf_long_octal_functions = 
{
   mulle_sprintf_get_unsigned_int_argumenttype,
   mulle_sprintf_long_octal_conversion
};


void  _mulle_sprintf_register_integer_functions( struct mulle_sprintf_conversion *tables)
{
   _mulle_sprintf_register_functions( tables, &mulle_sprintf_int_decimal_functions, 'i');
   _mulle_sprintf_register_functions( tables, &mulle_sprintf_int_decimal_functions, 'd');
   _mulle_sprintf_register_functions( tables, &mulle_sprintf_long_decimal_functions, 'D');

   _mulle_sprintf_register_functions( tables, &mulle_sprintf_unsigned_int_decimal_functions, 'u');
   _mulle_sprintf_register_functions( tables, &mulle_sprintf_int_octal_functions, 'o');
   _mulle_sprintf_register_functions( tables, &mulle_sprintf_int_hex_functions, 'x');

   _mulle_sprintf_register_functions( tables, &mulle_sprintf_unsigned_long_decimal_functions, 'U');
   _mulle_sprintf_register_functions( tables, &mulle_sprintf_long_octal_functions, 'O');
   _mulle_sprintf_register_functions( tables, &mulle_sprintf_long_hex_functions, 'X');
   
   _mulle_sprintf_register_modifiers( tables, "hljtzq");
}


__attribute__((constructor))
static void  mulle_sprintf_register_default_integer_functions()
{
  _mulle_sprintf_register_integer_functions( &mulle_sprintf_defaultconversion);
}
 
