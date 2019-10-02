#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

namespace DataStructures {

    template <class T>
    class Node {
        public:
            T* data;
            Node* nextNode;

            Node() {
                data = nullptr;
                next = nullptr;
            }

            Node(T* info) {
                data = info;
                next = nullptr;
            }

            Node(T* info, Node* next) {
                data = info;
                nextNode = next;
            }
        
    };

    template <class T, unsigned int length>
    class LinkedList {

        private:

            Node<T>* head;
            Node<T>* tail;

        public:

            int push(T* info) {

                Node<T> newNode(info);
                
                if (head == nullptr) {

                    head = newNode;
                    tail = newNode;

                } else {

                    tail->nextNode = newNode;
                    tail = tail->nextNode;

                }
            }

    };
}

#endif