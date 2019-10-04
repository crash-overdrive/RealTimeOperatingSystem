#ifndef UTIL_HPP
#define UTIL_HPP


#include "io/bwio.hpp"


namespace Util {
    
    // A utility function to swap two templates by their pointers 
    template<class T>
    void swap(T *x, T *y) {
        T temp = *x; 
        *x = *y; 
        *y = temp; 
    }

    inline void assert(bool expression) {
        if (!expression) {
            bwprintf(COM2, "!!!!!!!!!!!!!!!!!Assertion Failure!!!!!!!!!!!!!!");
        }

    }
    
}

#endif