#pragma once
#include "Sequence.h"
#include <iostream>
#include <stdexcept>

template<typename T>
class MutableArraySequence : public Sequence<T> {
private:
    DynamicArray<T>* items;
    
public:
    MutableArraySequence() : items(new DynamicArray<T>()) {}
    
    MutableArraySequence(T* arr, int count) : items(new DynamicArray<T>(arr, count)) {}

    explicit MutableArraySequence(const LinkedList<T>& list) : items(new DynamicArray<T>(list.GetLength())) {
        for (int i = 0; i < list.GetLength(); i++) {
            items->Set(i, list.Get(i));
        }
    }
    
    MutableArraySequence(const MutableArraySequence<T>& other) {
        items = new DynamicArray<T>(*other.items);
    }
    
    ~MutableArraySequence() override {
        delete items;
    }
    
    MutableArraySequence<T>* Clone() const {
        return new MutableArraySequence<T>(*this);
    }
    
    T GetFirst() const override {
        if (items->GetSize() == 0) throw EmptySequenceException();
        return items->Get(0);
    }
    
    T GetLast() const override {
        if (items->GetSize() == 0) throw EmptySequenceException();
        return items->Get(items->GetSize() - 1);
    }
    
    T Get(int index) const override {
        return items->Get(index);
    }
    
    int GetLength() const override {
        return items->GetSize();
    }
    
    Sequence<T>* Append(T item) override {
        items->Resize(items->GetSize() + 1);
        items->Set(items->GetSize() - 1, item);
        return this;
    }
    
    Sequence<T>* Prepend(T item) override {
        DynamicArray<T>* newItems = new DynamicArray<T>(items->GetSize() + 1);
        newItems->Set(0, item);
        for (int i = 0; i < items->GetSize(); i++) {
            newItems->Set(i + 1, items->Get(i));
        }
        delete items;
        items = newItems;
        return this;
    }
    
    Sequence<T>* InsertAt(T item, int index) override {
        if (index < 0 || index > items->GetSize()) throw IndexOutOfRangeException(index, items->GetSize());
        DynamicArray<T>* newItems = new DynamicArray<T>(items->GetSize() + 1);
        for (int i = 0; i < index; i++) {
            newItems->Set(i, items->Get(i));
        }
        newItems->Set(index, item);
        for (int i = index; i < items->GetSize(); i++) {
            newItems->Set(i + 1, items->Get(i));
        }
        delete items;
        items = newItems;
        return this;
    }
    
    Sequence<T>* Concat(Sequence<T>* other) override {
        if (other == nullptr) throw std::invalid_argument("Other sequence must not be null");
        for (int i = 0; i < other->GetLength(); i++) {
            Append(other->Get(i));
        }
        return this;
    }
    
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) override {
        if (startIndex < 0 || endIndex >= items->GetSize() || startIndex > endIndex) {
            throw IndexOutOfRangeException(startIndex, items->GetSize());
        }
        MutableArraySequence<T>* result = new MutableArraySequence<T>();
        for (int i = startIndex; i <= endIndex; i++) {
            result->Append(items->Get(i));
        }
        return result;
    }
    
    Sequence<T>* Map(std::function<T(T)> func) override {
        for (int i = 0; i < items->GetSize(); i++) {
            items->Set(i, func(items->Get(i)));
        }
        return this;
    }
    
    Sequence<T>* Where(std::function<bool(T)> predicate) override {
        MutableArraySequence<T>* result = new MutableArraySequence<T>();
        for (int i = 0; i < items->GetSize(); i++) {
            if (predicate(items->Get(i))) {
                result->Append(items->Get(i));
            }
        }
        delete items;
        items = result->items;
        result->items = nullptr;
        delete result;
        return this;
    }
    
    T Reduce(std::function<T(T, T)> func, T initial) override {
        T result = initial;
        for (int i = 0; i < items->GetSize(); i++) {
            result = func(result, items->Get(i));
        }
        return result;
    }
    
    Option<T> TryGetFirst(std::function<bool(T)> predicate = nullptr) override {
        if (items->GetSize() == 0) return Option<T>::None();
        if (predicate == nullptr) return Option<T>::Some(items->Get(0));
        for (int i = 0; i < items->GetSize(); i++) {
            if (predicate(items->Get(i))) {
                return Option<T>::Some(items->Get(i));
            }
        }
        return Option<T>::None();
    }
    
    Option<T> TryGetLast(std::function<bool(T)> predicate = nullptr) override {
        if (items->GetSize() == 0) return Option<T>::None();
        if (predicate == nullptr) return Option<T>::Some(items->Get(items->GetSize() - 1));
        for (int i = items->GetSize() - 1; i >= 0; i--) {
            if (predicate(items->Get(i))) {
                return Option<T>::Some(items->Get(i));
            }
        }
        return Option<T>::None();
    }
    
    void Print() const override {
        std::cout << "[";
        for (int i = 0; i < items->GetSize(); i++) {
            std::cout << items->Get(i);
            if (i < items->GetSize() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
};

// Immutable version
template<typename T>
class ImmutableArraySequence : public Sequence<T> {
private:
    DynamicArray<T>* items;
    
public:
    // Constructors similar to Mutable, but methods return a NEW object
    ImmutableArraySequence() : items(new DynamicArray<T>()) {}
    ImmutableArraySequence(T* arr, int count) : items(new DynamicArray<T>(arr, count)) {}
    explicit ImmutableArraySequence(const LinkedList<T>& list) : items(new DynamicArray<T>(list.GetLength())) {
        for (int i = 0; i < list.GetLength(); i++) {
            items->Set(i, list.Get(i));
        }
    }
    ImmutableArraySequence(const ImmutableArraySequence<T>& other) {
        items = new DynamicArray<T>(*other.items);
    }
    ~ImmutableArraySequence() override { delete items; }
    
    T GetFirst() const override {
        if (items->GetSize() == 0) throw EmptySequenceException();
        return items->Get(0);
    }
    
    T GetLast() const override {
        if (items->GetSize() == 0) throw EmptySequenceException();
        return items->Get(items->GetSize() - 1);
    }
    
    T Get(int index) const override { return items->Get(index); }
    int GetLength() const override { return items->GetSize(); }
    
    Sequence<T>* Append(T item) override {
        ImmutableArraySequence<T>* result = new ImmutableArraySequence<T>(*this);
        result->items->Resize(result->items->GetSize() + 1);
        result->items->Set(result->items->GetSize() - 1, item);
        return result;
    }
    
    Sequence<T>* Prepend(T item) override {
        ImmutableArraySequence<T>* result = new ImmutableArraySequence<T>();
        DynamicArray<T>* newItems = new DynamicArray<T>(items->GetSize() + 1);
        newItems->Set(0, item);
        for (int i = 0; i < items->GetSize(); i++) {
            newItems->Set(i + 1, items->Get(i));
        }
        delete result->items;
        result->items = newItems;
        return result;
    }
    
    Sequence<T>* InsertAt(T item, int index) override {
        if (index < 0 || index > items->GetSize()) throw IndexOutOfRangeException(index, items->GetSize());
        ImmutableArraySequence<T>* result = new ImmutableArraySequence<T>();
        DynamicArray<T>* newItems = new DynamicArray<T>(items->GetSize() + 1);
        for (int i = 0; i < index; i++) newItems->Set(i, items->Get(i));
        newItems->Set(index, item);
        for (int i = index; i < items->GetSize(); i++) newItems->Set(i + 1, items->Get(i));
        delete result->items;
        result->items = newItems;
        return result;
    }
    
    Sequence<T>* Concat(Sequence<T>* other) override {
        if (other == nullptr) throw std::invalid_argument("Other sequence must not be null");
        ImmutableArraySequence<T>* result = new ImmutableArraySequence<T>(*this);
        for (int i = 0; i < other->GetLength(); i++) {
            result->items->Resize(result->items->GetSize() + 1);
            result->items->Set(result->items->GetSize() - 1, other->Get(i));
        }
        return result;
    }
    
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) override {
        if (startIndex < 0 || endIndex >= items->GetSize() || startIndex > endIndex) {
            throw IndexOutOfRangeException(startIndex, items->GetSize());
        }
        ImmutableArraySequence<T>* result = new ImmutableArraySequence<T>();
        delete result->items;
        result->items = new DynamicArray<T>(endIndex - startIndex + 1);
        for (int i = startIndex; i <= endIndex; i++) {
            result->items->Set(i - startIndex, items->Get(i));
        }
        return result;
    }
    
    Sequence<T>* Map(std::function<T(T)> func) override {
        ImmutableArraySequence<T>* result = new ImmutableArraySequence<T>(*this);
        for (int i = 0; i < result->items->GetSize(); i++) {
            result->items->Set(i, func(result->items->Get(i)));
        }
        return result;
    }
    
    Sequence<T>* Where(std::function<bool(T)> predicate) override {
        ImmutableArraySequence<T>* result = new ImmutableArraySequence<T>();
        for (int i = 0; i < items->GetSize(); i++) {
            if (predicate(items->Get(i))) {
                result->items->Resize(result->items->GetSize() + 1);
                result->items->Set(result->items->GetSize() - 1, items->Get(i));
            }
        }
        return result;
    }
    
    T Reduce(std::function<T(T, T)> func, T initial) override {
        T result = initial;
        for (int i = 0; i < items->GetSize(); i++) {
            result = func(result, items->Get(i));
        }
        return result;
    }
    
    Option<T> TryGetFirst(std::function<bool(T)> predicate = nullptr) override {
        if (items->GetSize() == 0) return Option<T>::None();
        if (predicate == nullptr) return Option<T>::Some(items->Get(0));
        for (int i = 0; i < items->GetSize(); i++) {
            if (predicate(items->Get(i))) return Option<T>::Some(items->Get(i));
        }
        return Option<T>::None();
    }
    
    Option<T> TryGetLast(std::function<bool(T)> predicate = nullptr) override {
        if (items->GetSize() == 0) return Option<T>::None();
        if (predicate == nullptr) return Option<T>::Some(items->Get(items->GetSize() - 1));
        for (int i = items->GetSize() - 1; i >= 0; i--) {
            if (predicate(items->Get(i))) return Option<T>::Some(items->Get(i));
        }
        return Option<T>::None();
    }
    
    void Print() const override {
        std::cout << "[";
        for (int i = 0; i < items->GetSize(); i++) {
            std::cout << items->Get(i);
            if (i < items->GetSize() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
};

// Aliases for convenience
template<typename T>
using ArraySequence = MutableArraySequence<T>;
