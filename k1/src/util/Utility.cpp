namespace Utility {
    class Util {
        // A utility function to swap two elements 
        public:
            template<class T>
            static void swap(T *x, T *y) {
                //NOTE: This swap is probably broken
                T *temp = x; 
                x = y; 
                y = temp; 
                
            } 
    };
}