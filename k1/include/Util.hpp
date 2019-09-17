#ifndef UTIL_HPP
    #define UTIL_HPP

    namespace Util {
        
        // A utility function to swap two templates by their pointers 
        template<class T>
        void swap(T *x, T *y) {
            T temp = *x; 
            *x = *y; 
            *y = temp; 
        }

        void assert(bool expression) {
            if (!expression) {
                for(;;);
            }

        }
        
    }

#endif