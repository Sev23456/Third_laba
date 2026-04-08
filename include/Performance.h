#pragma once
#include <chrono>
#include <cstddef>
#include <string>
#include <vector>
#include "ArraySequence.h"
#include "ListSequence.h"

struct PerformanceResult {
    std::string name;
    long long microseconds;
};

inline std::vector<int> BuildBenchmarkData(int size) {
    std::vector<int> data;
    data.reserve(size);
    for (int i = 0; i < size; i++) {
        data.push_back(i % 97);
    }
    return data;
}

template<typename Func>
long long MeasureMicroseconds(Func func) {
    auto start = std::chrono::steady_clock::now();
    func();
    auto finish = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
}

template<typename SequenceType>
long long BenchmarkMutableSequence(int size) {
    const std::vector<int> data = BuildBenchmarkData(size);
    return MeasureMicroseconds([&]() {
        SequenceType sequence;
        for (int value : data) {
            sequence.Append(value);
        }
        sequence.Map([](int value) { return value * 2; });
        sequence.Where([](int value) { return value % 3 == 0; });
        volatile int sum = sequence.Reduce([](int left, int right) { return left + right; }, 0);
        (void)sum;
    });
}

template<typename SequenceType>
long long BenchmarkImmutableSequence(int size) {
    const std::vector<int> data = BuildBenchmarkData(size);
    return MeasureMicroseconds([&]() {
        SequenceType* sequence = new SequenceType();

        for (int value : data) {
            Sequence<int>* next = sequence->Append(value);
            delete sequence;
            sequence = static_cast<SequenceType*>(next);
        }

        Sequence<int>* mapped = sequence->Map([](int value) { return value * 2; });
        delete sequence;
        sequence = static_cast<SequenceType*>(mapped);

        Sequence<int>* filtered = sequence->Where([](int value) { return value % 3 == 0; });
        delete sequence;
        sequence = static_cast<SequenceType*>(filtered);

        volatile int sum = sequence->Reduce([](int left, int right) { return left + right; }, 0);
        (void)sum;
        delete sequence;
    });
}

inline std::vector<PerformanceResult> BenchmarkAllSequences(int size) {
    return {
        {"MutableArraySequence", BenchmarkMutableSequence<MutableArraySequence<int>>(size)},
        {"ImmutableArraySequence", BenchmarkImmutableSequence<ImmutableArraySequence<int>>(size)},
        {"MutableListSequence", BenchmarkMutableSequence<MutableListSequence<int>>(size)},
        {"ImmutableListSequence", BenchmarkImmutableSequence<ImmutableListSequence<int>>(size)}
    };
}
