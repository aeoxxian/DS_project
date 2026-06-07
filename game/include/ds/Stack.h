#ifndef STACK_H
#define STACK_H

#include <iostream>

template <typename T>
class Stack {
private:
    struct Node {
        T value;
        Node* next;
        Node(const T& value) : value(value), next(nullptr) {}
    };

    Node* topNode;
    int   count;

public:
    Stack() : topNode(nullptr), count(0) {}
    ~Stack() { clear(); }

    Stack(const Stack& other) : topNode(nullptr), count(0) {
        // 복사 생성자 — 역순 방지를 위해 임시 배열 사용
        T* tmp = new T[other.count];
        Node* cur = other.topNode;
        for (int i = other.count - 1; cur; --i, cur = cur->next) tmp[i] = cur->value;
        for (int i = 0; i < other.count; ++i) push(tmp[i]);
        delete[] tmp;
    }
    Stack& operator=(const Stack& other) {
        if (this == &other) return *this;
        clear();
        Stack tmp(other);
        topNode = tmp.topNode; count = tmp.count;
        tmp.topNode = nullptr; tmp.count = 0;
        return *this;
    }

    bool isEmpty() const { return count == 0; }
    int  size()    const { return count; }

    void push(const T& value) {
        Node* node = new Node(value);
        node->next = topNode;
        topNode    = node;
        ++count;
    }

    bool pop(T& output) {
        if (!topNode) return false;
        output      = topNode->value;
        Node* old   = topNode;
        topNode     = topNode->next;
        delete old;
        --count;
        return true;
    }

    bool peek(T& output) const {
        if (!topNode) return false;
        output = topNode->value;
        return true;
    }

    void clear() {
        while (topNode) {
            Node* old = topNode;
            topNode = topNode->next;
            delete old;
        }
        count = 0;
    }
};

#endif
