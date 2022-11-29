// #include <stdarg.h>
// #include <stdio.h>

// #include "libioP.h"

// #undef printf
// /* Write formatted output to stdout from the format string FORMAT.  */
// /* VARARGS1 */
// int 
// #include "helperFunctions.h"

// int DEBUG(char *debugState, char* format,...) 
// { 
//     if (debugState){
//         va_list arg;
//   int done;
//   va_start (arg, format);
//   done = __vfprintf_internal (stdout, format, arg, 0);
//   va_end (arg);
//   return done;
//     }
// } 

// #undef _IO_printf
// ldbl_strong_alias (__printf, printf);
// ldbl_strong_alias (__printf, _IO_printf);