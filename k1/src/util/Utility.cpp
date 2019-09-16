namespace Utility {
    class Util {
        // A utility function to swap two elements 
        public:
            static void swap(int *x, int *y) {

                int temp = *x; 
                *x = *y; 
                *y = temp; 
                
            } 
    };
}