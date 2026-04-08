#pragma once
#include "Exceptions.h"
#include <iostream>
#include <stdexcept>

template<typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node(T val) : data(val), next(nullptr) {}
    };
    
    Node* head;
    Node* tail;
    int length;
    
public:
    LinkedList() : head(nullptr), tail(nullptr), length(0) {}
    
    LinkedList(T* items, int count) : LinkedList() {
        if (count < 0) throw std::invalid_argument("Count must be non-negative");
        if (count > 0 && items == nullptr) throw std::invalid_argument("Items pointer must not be null");
        for (int i = 0; i < count; i++) {
            Append(items[i]);
        }
    }
    
    LinkedList(const LinkedList<T>& other) : LinkedList() {
        Node* current = other.head;
        while (current != nullptr) {
            Append(current->data);
            current = current->next;
        }
    }
    
    ~LinkedList() {
        Clear();
    }
    
    void Clear() {
        while (head != nullptr) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        length = 0;
    }
    
    T GetFirst() const {
        if (head == nullptr) throw EmptySequenceException();
        return head->data;
    }
    
    T GetLast() const {
        if (tail == nullptr) throw EmptySequenceException();
        return tail->data;
    }
    
    T Get(int index) const {
        if (index < 0 || index >= length) throw IndexOutOfRangeException(index, length);
        Node* current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->data;
    }

    void Set(int index, T value) {
        if (index < 0 || index >= length) throw IndexOutOfRangeException(index, length);
        Node* current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        current->data = value;
    }
    
    int GetLength() const { return length; }
    
    void Append(T item) {
        Node* newNode = new Node(item);
        if (tail == nullptr) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        length++;
    }
    
    void Prepend(T item) {
        Node* newNode = new Node(item);
        newNode->next = head;
        head = newNode;
        if (tail == nullptr) tail = newNode;
        length++;
    }
    
    void InsertAt(T item, int index) {
        if (index < 0 || index > length) throw IndexOutOfRangeException(index, length);
        if (index == 0) {
            Prepend(item);
            return;
        }
        if (index == length) {
            Append(item);
            return;
        }
        Node* current = head;
        for (int i = 0; i < index - 1; i++) {
            current = current->next;
        }
        Node* newNode = new Node(item);
        newNode->next = current->next;
        current->next = newNode;
        length++;
    }
    
    LinkedList<T>* Concat(LinkedList<T>* other) const {
        if (other == nullptr) throw std::invalid_argument("Other list must not be null");
        LinkedList<T>* result = new LinkedList<T>(*this);
        Node* current = other->head;
        while (current != nullptr) {
            result->Append(current->data);
            current = current->next;
        }
        return result;
    }
    
    LinkedList<T>* GetSubList(int startIndex, int endIndex) const {
        if (startIndex < 0 || endIndex >= length || startIndex > endIndex) {
            throw IndexOutOfRangeException(startIndex, length);
        }
        LinkedList<T>* result = new LinkedList<T>();
        for (int i = startIndex; i <= endIndex; i++) {
            result->Append(Get(i));
        }
        return result;
    }
};
