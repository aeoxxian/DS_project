#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <iostream>

template <typename T>
class LinkedList {
public:
    struct Node {
        T     value;
        Node* next;
        Node(const T& v) : value(v), next(nullptr) {}
    };

private:
    Node* head;
    Node* tail;
    int   count;

public:
    LinkedList() : head(nullptr), tail(nullptr), count(0) {}
    ~LinkedList() { clear(); }

    LinkedList(const LinkedList& other) : head(nullptr), tail(nullptr), count(0) {
        Node* cur = other.head;
        while (cur) { pushBack(cur->value); cur = cur->next; }
    }
    LinkedList& operator=(const LinkedList& other) {
        if (this == &other) return *this;
        clear();
        Node* cur = other.head;
        while (cur) { pushBack(cur->value); cur = cur->next; }
        return *this;
    }

    int  size()    const { return count; }
    bool isEmpty() const { return count == 0; }
    Node* getHead() const { return head; }

    void pushBack(const T& value) {
        Node* node = new Node(value);
        if (!tail) { head = tail = node; }
        else       { tail->next = node; tail = node; }
        ++count;
    }

    void pushFront(const T& value) {
        Node* node = new Node(value);
        node->next = head;
        head = node;
        if (!tail) tail = node;
        ++count;
    }

    bool popFront(T& output) {
        if (!head) return false;
        output    = head->value;
        Node* old = head;
        head      = head->next;
        if (!head) tail = nullptr;
        delete old;
        --count;
        return true;
    }

    bool removeAt(int index, T& output) {
        if (index < 0 || index >= count) return false;
        if (index == 0) return popFront(output);
        Node* prev = head;
        for (int i = 0; i < index - 1; ++i) prev = prev->next;
        Node* target = prev->next;
        output       = target->value;
        prev->next   = target->next;
        if (target == tail) tail = prev;
        delete target;
        --count;
        return true;
    }

    bool getAt(int index, T& output) const {
        if (index < 0 || index >= count) return false;
        Node* cur = head;
        for (int i = 0; i < index; ++i) cur = cur->next;
        output = cur->value;
        return true;
    }

    bool find(const T& value, int& outIndex) const {
        Node* cur = head;
        for (int i = 0; i < count; ++i, cur = cur->next) {
            if (cur->value == value) { outIndex = i; return true; }
        }
        return false;
    }

    void clear() {
        while (head) {
            Node* old = head;
            head = head->next;
            delete old;
        }
        tail = nullptr;
        count = 0;
    }
};

#endif
