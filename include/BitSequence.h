#pragma once
#include "Bit.h"
#include "Sequence.h"
#include <algorithm>
#include <stdexcept>
#include <vector>

class BitSequence : public Sequence<Bit> {
private:
    std::vector<bool> bits;
    
public:
    BitSequence() {}
    BitSequence(int size) : bits(size, false) {}
    
    Bit GetFirst() const override {
        if (bits.empty()) throw EmptySequenceException();
        return Bit(bits[0]);
    }
    
    Bit GetLast() const override {
        if (bits.empty()) throw EmptySequenceException();
        return Bit(bits.back());
    }
    
    Bit Get(int index) const override {
        if (index < 0 || index >= (int)bits.size()) throw IndexOutOfRangeException(index, bits.size());
        return Bit(bits[index]);
    }
    
    int GetLength() const override { return bits.size(); }
    
    Sequence<Bit>* Append(Bit item) override {
        bits.push_back(item.GetValue());
        return this;
    }
    
    Sequence<Bit>* Prepend(Bit item) override {
        bits.insert(bits.begin(), item.GetValue());
        return this;
    }
    
    Sequence<Bit>* InsertAt(Bit item, int index) override {
        if (index < 0 || index > (int)bits.size()) throw IndexOutOfRangeException(index, bits.size());
        bits.insert(bits.begin() + index, item.GetValue());
        return this;
    }
    
    Sequence<Bit>* Concat(Sequence<Bit>* other) override {
        if (other == nullptr) throw std::invalid_argument("Other sequence must not be null");
        for (int i = 0; i < other->GetLength(); i++) {
            bits.push_back(other->Get(i).GetValue());
        }
        return this;
    }
    
    Sequence<Bit>* GetSubsequence(int startIndex, int endIndex) override {
        if (startIndex < 0 || endIndex >= (int)bits.size() || startIndex > endIndex) {
            throw IndexOutOfRangeException(startIndex, bits.size());
        }
        BitSequence* result = new BitSequence();
        for (int i = startIndex; i <= endIndex; i++) {
            result->bits.push_back(bits[i]);
        }
        return result;
    }
    
    Sequence<Bit>* Map(std::function<Bit(Bit)> func) override {
        for (int i = 0; i < (int)bits.size(); i++) {
            bits[i] = func(Bit(bits[i])).GetValue();
        }
        return this;
    }
    
    Sequence<Bit>* Where(std::function<bool(Bit)> predicate) override {
        BitSequence* result = new BitSequence();
        for (int i = 0; i < (int)bits.size(); i++) {
            if (predicate(Bit(bits[i]))) {
                result->bits.push_back(bits[i]);
            }
        }
        bits = result->bits;
        delete result;
        return this;
    }
    
    Bit Reduce(std::function<Bit(Bit, Bit)> func, Bit initial) override {
        Bit result = initial;
        for (int i = 0; i < (int)bits.size(); i++) {
            result = func(result, Bit(bits[i]));
        }
        return result;
    }
    
    Option<Bit> TryGetFirst(std::function<bool(Bit)> predicate = nullptr) override {
        if (bits.empty()) return Option<Bit>::None();
        if (predicate == nullptr) return Option<Bit>::Some(Bit(bits[0]));
        for (bool b : bits) {
            if (predicate(Bit(b))) return Option<Bit>::Some(Bit(b));
        }
        return Option<Bit>::None();
    }
    
    Option<Bit> TryGetLast(std::function<bool(Bit)> predicate = nullptr) override {
        if (bits.empty()) return Option<Bit>::None();
        if (predicate == nullptr) return Option<Bit>::Some(Bit(bits.back()));
        for (auto it = bits.rbegin(); it != bits.rend(); ++it) {
            if (predicate(Bit(*it))) return Option<Bit>::Some(Bit(*it));
        }
        return Option<Bit>::None();
    }
    
    // Bit operations
    BitSequence* BitAnd(const BitSequence& other) const {
        BitSequence* result = new BitSequence();
        int minLen = std::min(bits.size(), other.bits.size());
        for (int i = 0; i < minLen; i++) {
            result->bits.push_back(bits[i] && other.bits[i]);
        }
        return result;
    }
    
    BitSequence* BitOr(const BitSequence& other) const {
        BitSequence* result = new BitSequence();
        int minLen = std::min(bits.size(), other.bits.size());
        for (int i = 0; i < minLen; i++) {
            result->bits.push_back(bits[i] || other.bits[i]);
        }
        return result;
    }
    
    BitSequence* BitXor(const BitSequence& other) const {
        BitSequence* result = new BitSequence();
        int minLen = std::min(bits.size(), other.bits.size());
        for (int i = 0; i < minLen; i++) {
            result->bits.push_back(bits[i] != other.bits[i]);
        }
        return result;
    }
    
    BitSequence* BitNot() const {
        BitSequence* result = new BitSequence();
        for (bool b : bits) {
            result->bits.push_back(!b);
        }
        return result;
    }
    
    void Print() const override {
        std::cout << "[";
        for (int i = 0; i < (int)bits.size(); i++) {
            std::cout << (bits[i] ? 1 : 0);
            if (i < (int)bits.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
};
