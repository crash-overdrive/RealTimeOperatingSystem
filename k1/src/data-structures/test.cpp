#include <iostream>
#include "../../include/RingBuffer.hpp"
#include "../../include/PriorityQueue.hpp"

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

    DataStructures::PriorityQueue<int, 5> pqueue;
    pqueue.push(100);
    pqueue.push(300);
    pqueue.push(500);
    pqueue.push(10);
    pqueue.push(5);
    pqueue.reset();
    std::cout << "Pushed 100, 300, 500, 10, 5 and then resetted" << std::endl;
    std::cout << "Is Empty?: " << pqueue.empty() << std::endl;
    std::cout << "Is Full?: " << pqueue.full() << std::endl;
    std::cout << "Value Poppped: " << pqueue.pop() << std::endl;
    std::cout << "Size" << pqueue.size() << std::endl << std::endl;
    
    pqueue.push(100);
    pqueue.push(300);
    pqueue.push(500);
    pqueue.push(10);
    pqueue.push(5);
    std::cout << "Pushed 100, 300, 500, 10, 5" << std::endl;
    std::cout << "Is Empty?: " << pqueue.empty() << std::endl;
    std::cout << "Is Full?: " << pqueue.full() << std::endl;
    std::cout << "Value Poppped: " << pqueue.pop() << std::endl;
    std::cout << "Size" << pqueue.size() << std::endl << std::endl;

    std::cout << "Is Empty?: " <<  pqueue.empty() << std::endl;
    std::cout << "Is Full?: " << pqueue.full() << std::endl;
    std::cout << "Value Poppped: " << pqueue.pop() << std::endl;
    std::cout << "Size" << pqueue.size() << std::endl << std::endl;

    std::cout << "Is Empty?: " <<  pqueue.empty() << std::endl;
    std::cout << "Is Full?: " << pqueue.full() << std::endl;
    std::cout << "Value Poppped: " << pqueue.pop() << std::endl;
    std::cout << "Size" << pqueue.size() << std::endl << std::endl;

    std::cout << "Is Empty?: " <<  pqueue.empty() << std::endl;
    std::cout << "Is Full?: " << pqueue.full() << std::endl;
    std::cout << "Value Poppped: " << pqueue.pop() << std::endl;
    std::cout << "Size" << pqueue.size() << std::endl << std::endl;

    std::cout << "Is Empty?: " <<  pqueue.empty() << std::endl;
    std::cout << "Is Full?: " << pqueue.full() << std::endl;
    std::cout << "Value Poppped: " << pqueue.pop() << std::endl;
    std::cout << "Size" << pqueue.size() << std::endl << std::endl;

    std::cout << "Is Empty?: " <<  pqueue.empty() << std::endl;
    std::cout << "Is Full?: " << pqueue.full() << std::endl;
    std::cout << "Value Poppped: " << pqueue.pop() << std::endl;
    std::cout << "Size" << pqueue.size() << std::endl << std::endl;

    std::cout << "Is Empty?: " <<  pqueue.empty() << std::endl;
    std::cout << "Is Full?: " << pqueue.full() << std::endl;
    
    return 0;

}