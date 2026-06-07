#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>

template <typename T>
class Queue {
private:
    struct Node {
        T value;
        Node* next;
        Node(const T& value) : value(value), next(nullptr) {}
    };

    Node* frontNode;
    Node* rearNode;
    int   count;

public:
    Queue() : frontNode(nullptr), rearNode(nullptr), count(0) {}
    ~Queue() { clear(); }

    Queue(const Queue& other) : frontNode(nullptr), rearNode(nullptr), count(0) {
        Node* cur = other.frontNode;
        while (cur) { enqueue(cur->value); cur = cur->next; }
    }
    Queue& operator=(const Queue& other) {
        if (this == &other) return *this;
        clear();
        Node* cur = other.frontNode;
        while (cur) { enqueue(cur->value); cur = cur->next; }
        return *this;
    }

    bool isEmpty() const { return count == 0; }
    int  size()    const { return count; }

    void enqueue(const T& value) {
        Node* node = new Node(value);
        if (!rearNode) { frontNode = rearNode = node; }
        else           { rearNode->next = node; rearNode = node; }
        ++count;
    }

    bool dequeue(T& output) {
        if (!frontNode) return false;
        output        = frontNode->value;
        Node* old     = frontNode;
        frontNode     = frontNode->next;
        if (!frontNode) rearNode = nullptr;
        delete old;
        --count;
        return true;
    }

    bool peek(T& output) const {
        if (!frontNode) return false;
        output = frontNode->value;
        return true;
    }

    void clear() {
        while (frontNode) {
            Node* old = frontNode;
            frontNode = frontNode->next;
            delete old;
        }
        rearNode = nullptr;
        count = 0;
    }
};

#endif
