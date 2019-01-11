#
# cmake/_Headers.cmake is generated by `mulle-sde`. Edits will be lost.
#
if( MULLE_TRACE_INCLUDE)
   MESSAGE( STATUS "# Include \"${CMAKE_CURRENT_LIST_FILE}\"" )
endif()

set( INCLUDE_DIRS
src
src/functions
)

set( PRIVATE_HEADERS
src/include-private.h
)

set( PUBLIC_HEADERS
src/functions/mulle-sprintf-character.h
src/functions/mulle-sprintf-decimal.h
src/functions/mulle-sprintf-escape.h
src/functions/mulle-sprintf-fp.h
src/functions/mulle-sprintf-integer.h
src/functions/mulle-sprintf-pointer.h
src/functions/mulle-sprintf-return.h
src/functions/mulle-sprintf-string.h
src/include.h
src/mulle-sprintf-function.h
src/mulle-sprintf.h
)

