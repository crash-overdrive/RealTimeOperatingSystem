#include <iostream>
#include "../../include/RingBuffer.hpp"
#include "RingBuffer.cpp"

int main() {

    DataStructures::RingBuffer<int> temp;
    std::cout << "Capacity before doing anything is: " << temp.capacity() << std::endl;
    temp.put(501);
    temp.put(301);
    temp.put(110);
    std::cout << "Inserted value 501, 301, 110, isfull?: " << temp.full() << std::endl;
    std::cout << "Get: " << temp.get() << std::endl;
    std::cout << "Isfull?: " << temp.full() << std::endl;
    temp.put(100);
    std::cout << "Inserted Value 100, is full?: " << temp.full() << std::endl;
    temp.put(101);
    temp.put(102);
    temp.put(999);
    std::cout << "Inserted Value 101, 102, 999, isfull?: " << temp.full() << std::endl;
    std::cout << "Get: " << temp.get() << std::endl;
    std::cout <<  "Isfull?: " << temp.full() << std::endl;
    temp.put(700);
    std::cout <<  "Put 700, Isfull?: " << temp.full() << std::endl;
    std::cout << "Get: " << temp.get() << std::endl;
    std::cout <<  "Isfull?: " << temp.full() << std::endl;
    return 0;

}