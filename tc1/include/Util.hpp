#ifndef UTIL_HPP
#define UTIL_HPP

#include "io/bwio.hpp"
#include "user/syscall/UserSyscall.hpp"

/*
 * Because assert is a function-like macro, commas anywhere in condition that are not protected by parentheses are
 * interpreted as macro argument separators. Such commas are often found in template argument lists and list-initialization
*/
#  define ASSERT(expression) \
    if (!(expression)) { \
        bwprintf(COM2, "Assertion Failed: %s in %s:%s:%d \n\r", #expression, __FILE__, __PRETTY_FUNCTION__,  __LINE__); \
        SwitchOff(); \
    }

namespace Util {

    // A utility function to swap two templates by their pointers
    template<class T>
    void swap(T *x, T *y) {
        T temp = *x;
        *x = *y;
        *y = temp;
    }
}

#endif
