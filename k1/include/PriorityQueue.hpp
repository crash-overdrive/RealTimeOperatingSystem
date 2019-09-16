#ifndef PRIORITY_QUEUE_H
    #define PRIORITY_QUEUE_H

    #include "../src/util/Utility.cpp"

    namespace DataStructures {

        // NOTE: T needs to have a default constructor!!
        template <class T, unsigned int length>
        class PriorityQueue {
            protected:
                const unsigned int maxSize = length;
                unsigned int heapSize;
                T heap[length];

                // to get index of parent of node at index "index"
                int parent(int index) { 
                    
                    return (index-1)/2; 
                    
                } 
            
                // to get index of left child of node at index "index"
                int leftChild(int index) { 

                    return (2*index + 1); 
                    
                } 
            
                // to get index of right child of node at index "index"
                int rightChild(int index) { 
                    
                    return (2*index + 2); 
                    
                } 

                // to heapify a subtree with the root at given index 
                void MaxHeapify(int index) {

                    int left = leftChild(index); 
                    int right = rightChild(index); 
                    int largest = index; 
                    if (left < heapSize && heap[left] > heap[largest]) {

                        largest = left; 

                    }
                        
                    if (right < heapSize && heap[right] > heap[largest]) {

                        largest = right;

                    } 
                         
                    if (largest != index) { 

                        Utility::Util::swap(&heap[index], &heap[largest]); 
                        MaxHeapify(largest); 

                    } 

                }
                

            public:
                PriorityQueue<T, length>() {

                    heapSize = 0;

                };

                // Inserts a new key 'key' 
                // Does nothing if queue is already full
                void push(int key) {
                    
                    if (heapSize == maxSize) { 

                        return; 

                    } 
                
                    // First insert the new key at the end 
                    heapSize++; 
                    int index = heapSize - 1; 
                    heap[index] = key; 
                
                    // Fix the max heap property if it is violated 
                    while (index != 0 && heap[parent(index)] < heap[index]) {

                        Utility::Util::swap(&heap[index], &heap[parent(index)]); 
                        index = parent(index); 

                    } 

                }
            
                // Returns the maximum key (key at root) from max heap (and removes it from queue)
                // Returns 0 if called on empty queue
                int pop() { 
                    
                    if (heapSize <= 0) {

                        return INT32_MIN;

                    }
 
                    if (heapSize == 1) { 

                        heapSize--; 
                        return heap[0]; 

                    } 
                
                    // Store the maximum value, and remove it from heap 
                    int root = heap[0]; 
                    heap[0] = heap[heapSize-1]; 
                    heapSize--; 
                    MaxHeapify(0); 
                
                    return root; 
                    
                } 

                void reset() {

                    heapSize = 0;

                }

                bool empty() const {

                    //if head and tail are equal and RingBuffer is not full, then RingBuffer is empty
                    return (heapSize <= 0);

                }

                bool full() const {

                    return (heapSize >= maxSize);

                }

                unsigned int capacity() const {
                    
                    return maxSize;
                
                }

                unsigned int size() const {

                    return heapSize;

                }

        };

    }

#endif