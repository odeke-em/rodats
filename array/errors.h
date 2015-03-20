// Author: Emmanuel Odeke <odeke@ualberta.ca>
#ifndef _ERRORS_H
#define _ERRORS_H
  #include <stdarg.h>
  #include <stdlib.h>

  #define raiseWarning(...){                                                \
    fprintf(stderr, "\033[31m[%s: %s]\033[00m Traceback to line: %d:: ",    \
                    __FILE__, __func__, __LINE__);                          \
    fprintf(stderr, __VA_ARGS__);                                           \
  }

  #ifdef assert 
    #undef assert
  #endif // assert

  #define assert(validExpression){\
    if (! (validExpression))\
      raiseError(#validExpression);\
  }

  #define raiseError(...) {\
    raiseWarning(__VA_ARGS__);\
    exit(-2);\
  }

#endif // _ERRORS_H
