#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>

// Linked-node queue. Used for game event processing.
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
    int count;

public:
    Queue() : frontNode(nullptr), rearNode(nullptr), count(0) {}

    ~Queue() {
        clear();
    }

    Queue(const Queue& other) = delete;
    Queue& operator=(const Queue& other) = delete;

    bool isEmpty() const {
        return count == 0;
    }

    int size() const {
        return count;
    }

    void enqueue(const T& value) {
        // TODO(Student): Implement queue enqueue.
        // Hint: insert at rearNode. If the queue is empty, frontNode and rearNode
        // should point to the same new node.
        (void)value;
    }

    bool dequeue(T& output) {
        // TODO(Student): Implement queue dequeue.
        // Return false if empty. Otherwise remove from frontNode.
        (void)output;
        std::cout << "[TODO] Queue::dequeue is not implemented yet.\n";
        return false;
    }

    bool peek(T& output) const {
        // TODO(Student): Implement queue peek.
        (void)output;
        return false;
    }

    void clear() {
        // Provided cleanup helper: delete every node and reset frontNode/rearNode/count.
        while (frontNode != nullptr) {
            Node* old = frontNode;
            frontNode = frontNode->next;
            delete old;
        }
        rearNode = nullptr;
        count = 0;
    }
};

#endif
