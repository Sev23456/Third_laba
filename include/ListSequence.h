#pragma once
#include "Sequence.h"
#include <iostream>
#include <stdexcept>

template<typename T>
class MutableListSequence : public Sequence<T> {
private:
    LinkedList<T>* list;
    
public:
    MutableListSequence() : list(new LinkedList<T>()) {}
    MutableListSequence(T* arr, int count) : list(new LinkedList<T>(arr, count)) {}
    explicit MutableListSequence(const LinkedList<T>& source) : list(new LinkedList<T>(source)) {}
    MutableListSequence(const MutableListSequence<T>& other) : list(new LinkedList<T>(*other.list)) {}
    ~MutableListSequence() override { delete list; }
    
    T GetFirst() const override { return list->GetFirst(); }
    T GetLast() const override { return list->GetLast(); }
    T Get(int index) const override { return list->Get(index); }
    int GetLength() const override { return list->GetLength(); }
    
    Sequence<T>* Append(T item) override {
        list->Append(item);
        return this;
    }
    
    Sequence<T>* Prepend(T item) override {
        list->Prepend(item);
        return this;
    }
    
    Sequence<T>* InsertAt(T item, int index) override {
        list->InsertAt(item, index);
        return this;
    }
    
    Sequence<T>* Concat(Sequence<T>* other) override {
        if (other == nullptr) throw std::invalid_argument("Other sequence must not be null");
        for (int i = 0; i < other->GetLength(); i++) {
            list->Append(other->Get(i));
        }
        return this;
    }
    
    Sequence<T>* GetSubsequence(int startIndex, int endIndex) override {
        LinkedList<T>* subList = list->GetSubList(startIndex, endIndex);
        MutableListSequence<T>* result = new MutableListSequence<T>();
        delete result->list;
        result->list = subList;
        return result;
    }
    
    Sequence<T>* Map(std::function<T(T)> func) override {
        for (int i = 0; i < list->GetLength(); i++) {
            list->Set(i, func(list->Get(i)));  
        }
        return this;
    }

    Sequence<T>* Where(std::function<bool(T)> predicate) override {
        MutableListSequence<T>* result = new MutableListSequence<T>();
        for (int i = 0; i < list->GetLength(); i++) {
            if (predicate(list->Get(i))) {
                result->Append(list->Get(i));
            }
        }
        delete list;
        list = result->list;
        result->list = nullptr;
        delete result;
        return this;
    }
    
    T Reduce(std::function<T(T, T)> func, T initial) override {
        T result = initial;
        for (int i = 0; i < list->GetLength(); i++) {
            result = func(result, list->Get(i));
        }
        return result;
    }
    
    Option<T> TryGetFirst(std::function<bool(T)> predicate = nullptr) override {
        if (list->GetLength() == 0) return Option<T>::None();
        if (predicate == nullptr) return Option<T>::Some(list->GetFirst());
        for (int i = 0; i < list->GetLength(); i++) {
            if (predicate(list->Get(i))) return Option<T>::Some(list->Get(i));
        }
        return Option<T>::None();
    }
    
    Option<T> TryGetLast(std::function<bool(T)> predicate = nullptr) override {
        if (list->GetLength() == 0) return Option<T>::None();
        if (predicate == nullptr) return Option<T>::Some(list->GetLast());
        for (int i = list->GetLength() - 1; i >= 0; i--) {
            if (predicate(list->Get(i))) return Option<T>::Some(list->Get(i));
        }
        return Option<T>::None();
    }
    
    void Print() const override {
        std::cout << "[";
        for (int i = 0; i < list->GetLength(); i++) {
            std::cout << list->Get(i);
            if (i < list->GetLength() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
};

template<typename T>
class ImmutableListSequence : public Sequence<T> {
private:
    LinkedList<T>* list;

public:
    ImmutableListSequence() : list(new LinkedList<T>()) {}
    ImmutableListSequence(T* arr, int count) : list(new LinkedList<T>(arr, count)) {}
    explicit ImmutableListSequence(const LinkedList<T>& source) : list(new LinkedList<T>(source)) {}
    ImmutableListSequence(const ImmutableListSequence<T>& other) : list(new LinkedList<T>(*other.list)) {}
    ~ImmutableListSequence() override { delete list; }

    T GetFirst() const override { return list->GetFirst(); }
    T GetLast() const override { return list->GetLast(); }
    T Get(int index) const override { return list->Get(index); }
    int GetLength() const override { return list->GetLength(); }

    Sequence<T>* Append(T item) override {
        ImmutableListSequence<T>* result = new ImmutableListSequence<T>(*this);
        result->list->Append(item);
        return result;
    }

    Sequence<T>* Prepend(T item) override {
        ImmutableListSequence<T>* result = new ImmutableListSequence<T>(*this);
        result->list->Prepend(item);
        return result;
    }

    Sequence<T>* InsertAt(T item, int index) override {
        ImmutableListSequence<T>* result = new ImmutableListSequence<T>(*this);
        result->list->InsertAt(item, index);
        return result;
    }

    Sequence<T>* Concat(Sequence<T>* other) override {
        if (other == nullptr) throw std::invalid_argument("Other sequence must not be null");
        ImmutableListSequence<T>* result = new ImmutableListSequence<T>(*this);
        for (int i = 0; i < other->GetLength(); i++) {
            result->list->Append(other->Get(i));
        }
        return result;
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) override {
        LinkedList<T>* subList = list->GetSubList(startIndex, endIndex);
        ImmutableListSequence<T>* result = new ImmutableListSequence<T>();
        delete result->list;
        result->list = subList;
        return result;
    }

    Sequence<T>* Map(std::function<T(T)> func) override {
        ImmutableListSequence<T>* result = new ImmutableListSequence<T>(*this);
        for (int i = 0; i < result->list->GetLength(); i++) {
            result->list->Set(i, func(result->list->Get(i)));
        }
        return result;
    }

    Sequence<T>* Where(std::function<bool(T)> predicate) override {
        ImmutableListSequence<T>* result = new ImmutableListSequence<T>();
        for (int i = 0; i < list->GetLength(); i++) {
            T value = list->Get(i);
            if (predicate(value)) {
                result->list->Append(value);
            }
        }
        return result;
    }

    T Reduce(std::function<T(T, T)> func, T initial) override {
        T result = initial;
        for (int i = 0; i < list->GetLength(); i++) {
            result = func(result, list->Get(i));
        }
        return result;
    }

    Option<T> TryGetFirst(std::function<bool(T)> predicate = nullptr) override {
        if (list->GetLength() == 0) return Option<T>::None();
        if (predicate == nullptr) return Option<T>::Some(list->GetFirst());
        for (int i = 0; i < list->GetLength(); i++) {
            T value = list->Get(i);
            if (predicate(value)) return Option<T>::Some(value);
        }
        return Option<T>::None();
    }

    Option<T> TryGetLast(std::function<bool(T)> predicate = nullptr) override {
        if (list->GetLength() == 0) return Option<T>::None();
        if (predicate == nullptr) return Option<T>::Some(list->GetLast());
        for (int i = list->GetLength() - 1; i >= 0; i--) {
            T value = list->Get(i);
            if (predicate(value)) return Option<T>::Some(value);
        }
        return Option<T>::None();
    }

    void Print() const override {
        std::cout << "[";
        for (int i = 0; i < list->GetLength(); i++) {
            std::cout << list->Get(i);
            if (i < list->GetLength() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
};

template<typename T>
using ListSequence = MutableListSequence<T>;
