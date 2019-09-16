#include <iostream>
#include "../../include/RingBuffer.hpp"
#include "../../include/PriorityQueue.hpp"

struct testData {
    int priority;

    testData() {
        priority = INT32_MIN;
    }

    testData (int p) {
        priority = p;
    }

    bool operator> (const testData &test) {
        return priority > test.priority;
    }

    bool operator< (const testData &test) {
        return priority < test.priority;
    }

};

int main() {

    

    DataStructures::RingBuffer<int, 3> temp;
    std::cout << "Capacity before doing anything is: " << temp.capacity() << std::endl;
    temp.push(501);
    temp.push(301);
    temp.push(110);
    std::cout << "Inserted value 501, 301, 110, isfull?: " << temp.full() << std::endl;
    std::cout << "Pop: " << temp.pop() << std::endl;
    std::cout << "Isfull?: " << temp.full() << std::endl;
    temp.push(100);
    std::cout << "Inserted Value 100, is full?: " << temp.full() << std::endl;
    temp.push(101);
    temp.push(102);
    temp.push(999);
    std::cout << "Inserted Value 101, 102, 999, isfull?: " << temp.full() << std::endl;
    std::cout << "pop: " << temp.pop() << std::endl;
    std::cout <<  "Isfull?: " << temp.full() << std::endl;
    temp.push(700);
    std::cout <<  "Put 700, Isfull?: " << temp.full() << std::endl;
    std::cout << "pop: " << temp.pop() << std::endl;
    std::cout <<  "Isfull?: " << temp.full() << std::endl;

    DataStructures::PriorityQueue<char, 5> pqueueChar;
    pqueueChar.push('g');
    pqueueChar.push('a');
    pqueueChar.push('x');
    pqueueChar.push('z');
    pqueueChar.push('k');
    pqueueChar.reset();
    std::cout << "Pushed g, a, x, z, k" << std::endl;
    std::cout << "Is Empty?: " << pqueueChar.empty() << std::endl;
    std::cout << "Is Full?: " << pqueueChar.full() << std::endl;
    std::cout << "Value Poppped: " << pqueueChar.pop() << std::endl;
    std::cout << "Size" << pqueueChar.size() << std::endl << std::endl;
    
    pqueueChar.push('g');
    pqueueChar.push('a');
    pqueueChar.push('x');
    pqueueChar.push('z');
    pqueueChar.push('k');
    std::cout << "Pushed g, a, x, z, k" << std::endl;
    std::cout << "Is Empty?: " << pqueueChar.empty() << std::endl;
    std::cout << "Is Full?: " << pqueueChar.full() << std::endl;
    std::cout << "Value Poppped: " << pqueueChar.pop() << std::endl;
    std::cout << "Size" << pqueueChar.size() << std::endl << std::endl;

    std::cout << "Is Empty?: " << pqueueChar.empty() << std::endl;
    std::cout << "Is Full?: " << pqueueChar.full() << std::endl;
    std::cout << "Value Poppped: " << pqueueChar.pop() << std::endl;
    std::cout << "Size" << pqueueChar.size() << std::endl << std::endl;

    std::cout << "Is Empty?: " << pqueueChar.empty() << std::endl;
    std::cout << "Is Full?: " << pqueueChar.full() << std::endl;
    std::cout << "Value Poppped: " << pqueueChar.pop() << std::endl;
    std::cout << "Size" << pqueueChar.size() << std::endl << std::endl;

    std::cout << "Is Empty?: " << pqueueChar.empty() << std::endl;
    std::cout << "Is Full?: " << pqueueChar.full() << std::endl;
    std::cout << "Value Poppped: " << pqueueChar.pop() << std::endl;
    std::cout << "Size" << pqueueChar.size() << std::endl << std::endl;

    std::cout << "Is Empty?: " << pqueueChar.empty() << std::endl;
    std::cout << "Is Full?: " << pqueueChar.full() << std::endl;
    std::cout << "Value Poppped: " << pqueueChar.pop() << std::endl;
    std::cout << "Size" << pqueueChar.size() << std::endl << std::endl;

    std::cout << "Is Empty?: " << pqueueChar.empty() << std::endl;
    std::cout << "Is Full?: " << pqueueChar.full() << std::endl;
    std::cout << "Value Poppped: " << pqueueChar.pop() << std::endl;
    std::cout << "Size" << pqueueChar.size() << std::endl << std::endl;

    std::cout << "Is Empty?: " <<  pqueueChar.empty() << std::endl;
    std::cout << "Is Full?: " << pqueueChar.full() << std::endl;




    // pqueue.push(testData(100));
    // pqueue.push(testData(300));
    // pqueue.push(testData(500));
    // pqueue.push(testData(10));
    // pqueue.push(testData(5));
    // pqueue.reset();
    // std::cout << "Pushed 100, 300, 500, 10, 5 and then resetted" << std::endl;
    // std::cout << "Is Empty?: " << pqueue.empty() << std::endl;
    // std::cout << "Is Full?: " << pqueue.full() << std::endl;
    // int p = pqueue.pop().priority;
    // std::cout << "Value Poppped: " << p << std::endl;
    // std::cout << "Size" << pqueue.size() << std::endl << std::endl;
    
    // std::cout << (testData(100) > testData(300)) << std::endl;
    // pqueue.push(testData(100));
    // pqueue.push(testData(300));
    // pqueue.push(testData(500));
    // pqueue.push(testData(10));
    // pqueue.push(testData(5));
    // std::cout << "Pushed 100, 300, 500, 10, 5" << std::endl;
    // std::cout << "Is Empty?: " << pqueue.empty() << std::endl;
    // std::cout << "Is Full?: " << pqueue.full() << std::endl;
    // int p = pqueue.pop().priority;
    // std::cout << "Value Poppped: " << p << std::endl;
    // std::cout << "Size" << pqueue.size() << std::endl << std::endl;

    // std::cout << "Is Empty?: " <<  pqueue.empty() << std::endl;
    // std::cout << "Is Full?: " << pqueue.full() << std::endl;
    // p = pqueue.pop().priority;
    // std::cout << "Value Poppped: " << p << std::endl;
    // std::cout << "Size" << pqueue.size() << std::endl << std::endl;

    // std::cout << "Is Empty?: " <<  pqueue.empty() << std::endl;
    // std::cout << "Is Full?: " << pqueue.full() << std::endl;
    // p = pqueue.pop().priority;
    // std::cout << "Value Poppped: " << p << std::endl;
    // std::cout << "Size" << pqueue.size() << std::endl << std::endl;

    // std::cout << "Is Empty?: " <<  pqueue.empty() << std::endl;
    // std::cout << "Is Full?: " << pqueue.full() << std::endl;
    // p = pqueue.pop().priority;
    // std::cout << "Value Poppped: " << p << std::endl;
    // std::cout << "Size" << pqueue.size() << std::endl << std::endl;

    // std::cout << "Is Empty?: " <<  pqueue.empty() << std::endl;
    // std::cout << "Is Full?: " << pqueue.full() << std::endl;
    // p = pqueue.pop().priority;
    // std::cout << "Value Poppped: " << p << std::endl;;
    // std::cout << "Size" << pqueue.size() << std::endl << std::endl;

    // std::cout << "Is Empty?: " <<  pqueue.empty() << std::endl;
    // std::cout << "Is Full?: " << pqueue.full() << std::endl;
    // p = pqueue.pop().priority;
    // std::cout << "Value Poppped: " << p << std::endl;
    // std::cout << "Size" << pqueue.size() << std::endl << std::endl;

    // std::cout << "Is Empty?: " <<  pqueue.empty() << std::endl;
    // std::cout << "Is Full?: " << pqueue.full() << std::endl;
    
    return 0;

}