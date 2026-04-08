#include <cassert>
#include <cmath>
#include <complex>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include "ArraySequence.h"
#include "BitSequence.h"
#include "DynamicArray.h"
#include "ListSequence.h"
#include "Matrix.h"
#include "Performance.h"
#include "Polynomial.h"
#include "Vector.h"

template<typename ExceptionType, typename Func>
void ExpectThrows(Func func) {
    bool caught = false;
    try {
        func();
    } catch (const ExceptionType&) {
        caught = true;
    }
    assert(caught);
}

template<typename T>
void AssertSequenceEquals(const Sequence<T>& sequence, std::initializer_list<T> expected) {
    assert(sequence.GetLength() == static_cast<int>(expected.size()));

    int index = 0;
    for (const T& value : expected) {
        assert(sequence.Get(index) == value);
        index++;
    }
}

void AssertDoubleEquals(double actual, double expected, double epsilon = 1e-9) {
    assert(std::fabs(actual - expected) <= epsilon);
}

void AssertComplexEquals(const std::complex<double>& actual,
                         const std::complex<double>& expected,
                         double epsilon = 1e-9) {
    assert(std::abs(actual - expected) <= epsilon);
}

void AssertBitSequenceEquals(const Sequence<Bit>& sequence, std::initializer_list<int> expected) {
    assert(sequence.GetLength() == static_cast<int>(expected.size()));

    int index = 0;
    for (int value : expected) {
        assert(sequence.Get(index) == Bit(value != 0));
        index++;
    }
}

template<typename T>
void AssertEnumeratorEquals(Sequence<T>& sequence, std::initializer_list<T> expected) {
    std::unique_ptr<IEnumerator<T>> enumerator = sequence.GetEnumerator();
    int index = 0;

    while (enumerator->MoveNext()) {
        assert(index < static_cast<int>(expected.size()));
        auto it = expected.begin();
        std::advance(it, index);
        assert(enumerator->GetCurrent() == *it);
        index++;
    }

    assert(index == static_cast<int>(expected.size()));
    enumerator->Reset();
    if (!expected.size()) {
        assert(!enumerator->MoveNext());
    } else {
        assert(enumerator->MoveNext());
        assert(enumerator->GetCurrent() == *expected.begin());
    }
}

void TestDynamicArray() {
    std::cout << "Testing DynamicArray... ";

    DynamicArray<int> empty;
    assert(empty.GetSize() == 0);

    DynamicArray<int> zeroSized(0);
    assert(zeroSized.GetSize() == 0);

    empty.Resize(3);
    for (int i = 0; i < 3; i++) {
        empty.Set(i, (i + 1) * 10);
    }
    assert(empty.Get(0) == 10);
    assert(empty.Get(2) == 30);

    empty.Resize(1);
    assert(empty.GetSize() == 1);
    assert(empty.Get(0) == 10);

    empty.Resize(0);
    assert(empty.GetSize() == 0);
    ExpectThrows<IndexOutOfRangeException>([&]() { empty.Get(0); });

    int data[] = {4, 5, 6};
    DynamicArray<int> fromItems(data, 3);
    assert(fromItems.GetSize() == 3);
    assert(fromItems.Get(1) == 5);

    DynamicArray<int> fromEmptyItems(nullptr, 0);
    assert(fromEmptyItems.GetSize() == 0);

    ExpectThrows<std::invalid_argument>([]() {
        DynamicArray<int> invalid(nullptr, 2);
    });

    std::cout << "OK" << std::endl;
}

void TestMutableArraySequence() {
    std::cout << "Testing MutableArraySequence... ";

    int base[] = {1, 2, 3};
    ArraySequence<int> sequence(base, 3);
    sequence.Append(4);
    sequence.Prepend(0);
    sequence.InsertAt(99, 2);
    AssertSequenceEquals(sequence, {0, 1, 99, 2, 3, 4});
    assert(sequence[2] == 99);

    int extra[] = {7, 8};
    ArraySequence<int> other(extra, 2);
    sequence.Concat(&other);
    AssertSequenceEquals(sequence, {0, 1, 99, 2, 3, 4, 7, 8});

    std::unique_ptr<Sequence<int>> subsequence(sequence.GetSubsequence(1, 3));
    AssertSequenceEquals(*subsequence, {1, 99, 2});

    int mapData[] = {1, 2, 3, 4, 5};
    ArraySequence<int> mapSequence(mapData, 5);
    mapSequence.Map([](int value) { return value * 3; });
    AssertSequenceEquals(mapSequence, {3, 6, 9, 12, 15});

    mapSequence.Where([](int value) { return value % 2 == 0; });
    AssertSequenceEquals(mapSequence, {6, 12});
    assert(mapSequence.Reduce([](int left, int right) { return left + right; }, 0) == 18);

    Option<int> firstLarge = mapSequence.TryGetFirst([](int value) { return value > 10; });
    assert(firstLarge.IsSome());
    assert(firstLarge.GetValue() == 12);

    Option<int> lastNegative = mapSequence.TryGetLast([](int value) { return value < 0; });
    assert(lastNegative.IsNone());

    mapSequence.Where([](int value) { return value < 0; });
    assert(mapSequence.GetLength() == 0);

    LinkedList<int> linked(base, 3);
    MutableArraySequence<int> fromLinked(linked);
    AssertSequenceEquals(fromLinked, {1, 2, 3});
    AssertEnumeratorEquals(fromLinked, {1, 2, 3});

    std::cout << "OK" << std::endl;
}

void TestImmutableArraySequence() {
    std::cout << "Testing ImmutableArraySequence... ";

    int base[] = {1, 2, 3};
    ImmutableArraySequence<int> original(base, 3);

    std::unique_ptr<Sequence<int>> appended(original.Append(4));
    AssertSequenceEquals(original, {1, 2, 3});
    AssertSequenceEquals(*appended, {1, 2, 3, 4});

    std::unique_ptr<Sequence<int>> prepended(original.Prepend(0));
    AssertSequenceEquals(*prepended, {0, 1, 2, 3});
    AssertSequenceEquals(original, {1, 2, 3});

    std::unique_ptr<Sequence<int>> inserted(original.InsertAt(99, 1));
    AssertSequenceEquals(*inserted, {1, 99, 2, 3});

    int extra[] = {7, 8};
    ArraySequence<int> other(extra, 2);
    std::unique_ptr<Sequence<int>> concatenated(original.Concat(&other));
    AssertSequenceEquals(*concatenated, {1, 2, 3, 7, 8});

    std::unique_ptr<Sequence<int>> subsequence(original.GetSubsequence(1, 2));
    AssertSequenceEquals(*subsequence, {2, 3});

    std::unique_ptr<Sequence<int>> mapped(original.Map([](int value) { return value * 2; }));
    AssertSequenceEquals(*mapped, {2, 4, 6});
    AssertSequenceEquals(original, {1, 2, 3});

    std::unique_ptr<Sequence<int>> filtered(original.Where([](int value) { return value > 10; }));
    assert(filtered->GetLength() == 0);

    std::unique_ptr<Sequence<int>> emptyFiltered(
        ImmutableArraySequence<int>().Where([](int value) { return value > 0; })
    );
    assert(emptyFiltered->GetLength() == 0);

    assert(original.Reduce([](int left, int right) { return left + right; }, 0) == 6);

    Option<int> firstEven = original.TryGetFirst([](int value) { return value % 2 == 0; });
    assert(firstEven.IsSome());
    assert(firstEven.GetValue() == 2);

    Option<int> missing = original.TryGetLast([](int value) { return value > 10; });
    assert(missing.IsNone());

    LinkedList<int> linked(base, 3);
    ImmutableArraySequence<int> fromLinked(linked);
    AssertSequenceEquals(fromLinked, {1, 2, 3});
    AssertEnumeratorEquals(fromLinked, {1, 2, 3});

    std::cout << "OK" << std::endl;
}

void TestMutableListSequence() {
    std::cout << "Testing MutableListSequence... ";

    int base[] = {10, 20, 30};
    ListSequence<int> sequence(base, 3);
    sequence.Append(40);
    sequence.Prepend(5);
    sequence.InsertAt(15, 2);
    AssertSequenceEquals(sequence, {5, 10, 15, 20, 30, 40});

    int extra[] = {50, 60};
    ListSequence<int> other(extra, 2);
    sequence.Concat(&other);
    AssertSequenceEquals(sequence, {5, 10, 15, 20, 30, 40, 50, 60});

    std::unique_ptr<Sequence<int>> subsequence(sequence.GetSubsequence(2, 4));
    AssertSequenceEquals(*subsequence, {15, 20, 30});

    int mapData[] = {1, 2, 3, 4};
    ListSequence<int> mapSequence(mapData, 4);
    mapSequence.Map([](int value) { return value + 1; });
    AssertSequenceEquals(mapSequence, {2, 3, 4, 5});

    mapSequence.Where([](int value) { return value % 2 != 0; });
    AssertSequenceEquals(mapSequence, {3, 5});
    assert(mapSequence.Reduce([](int left, int right) { return left * right; }, 1) == 15);

    Option<int> lastLarge = mapSequence.TryGetLast([](int value) { return value > 3; });
    assert(lastLarge.IsSome());
    assert(lastLarge.GetValue() == 5);

    Option<int> noMatch = mapSequence.TryGetFirst([](int value) { return value < 0; });
    assert(noMatch.IsNone());

    LinkedList<int> linked(base, 3);
    MutableListSequence<int> fromLinked(linked);
    AssertSequenceEquals(fromLinked, {10, 20, 30});
    AssertEnumeratorEquals(fromLinked, {10, 20, 30});

    std::cout << "OK" << std::endl;
}

void TestImmutableListSequence() {
    std::cout << "Testing ImmutableListSequence... ";

    int base[] = {2, 4, 6};
    ImmutableListSequence<int> original(base, 3);

    std::unique_ptr<Sequence<int>> appended(original.Append(8));
    AssertSequenceEquals(original, {2, 4, 6});
    AssertSequenceEquals(*appended, {2, 4, 6, 8});

    std::unique_ptr<Sequence<int>> prepended(original.Prepend(0));
    AssertSequenceEquals(*prepended, {0, 2, 4, 6});

    std::unique_ptr<Sequence<int>> inserted(original.InsertAt(99, 1));
    AssertSequenceEquals(*inserted, {2, 99, 4, 6});

    int extra[] = {10, 12};
    ListSequence<int> other(extra, 2);
    std::unique_ptr<Sequence<int>> concatenated(original.Concat(&other));
    AssertSequenceEquals(*concatenated, {2, 4, 6, 10, 12});

    std::unique_ptr<Sequence<int>> subsequence(original.GetSubsequence(1, 2));
    AssertSequenceEquals(*subsequence, {4, 6});

    std::unique_ptr<Sequence<int>> mapped(original.Map([](int value) { return value / 2; }));
    AssertSequenceEquals(*mapped, {1, 2, 3});
    AssertSequenceEquals(original, {2, 4, 6});

    std::unique_ptr<Sequence<int>> filtered(original.Where([](int value) { return value > 10; }));
    assert(filtered->GetLength() == 0);

    assert(original.Reduce([](int left, int right) { return left + right; }, 0) == 12);

    Option<int> firstLarge = original.TryGetFirst([](int value) { return value > 3; });
    assert(firstLarge.IsSome());
    assert(firstLarge.GetValue() == 4);

    Option<int> missing = original.TryGetLast([](int value) { return value < 0; });
    assert(missing.IsNone());

    LinkedList<int> linked(base, 3);
    ImmutableListSequence<int> fromLinked(linked);
    AssertSequenceEquals(fromLinked, {2, 4, 6});
    AssertEnumeratorEquals(fromLinked, {2, 4, 6});

    std::cout << "OK" << std::endl;
}

void TestBitSequence() {
    std::cout << "Testing BitSequence... ";

    BitSequence sequence;
    sequence.Append(Bit(1));
    sequence.Append(Bit(0));
    sequence.Append(Bit(1));
    sequence.Prepend(Bit(0));
    sequence.InsertAt(Bit(1), 2);
    AssertBitSequenceEquals(sequence, {0, 1, 1, 0, 1});

    BitSequence extra;
    extra.Append(Bit(1));
    extra.Append(Bit(1));
    sequence.Concat(&extra);
    AssertBitSequenceEquals(sequence, {0, 1, 1, 0, 1, 1, 1});

    std::unique_ptr<Sequence<Bit>> subsequence(sequence.GetSubsequence(1, 4));
    AssertBitSequenceEquals(*subsequence, {1, 1, 0, 1});

    BitSequence filterSequence;
    filterSequence.Append(Bit(1));
    filterSequence.Append(Bit(0));
    filterSequence.Append(Bit(1));
    filterSequence.Append(Bit(0));

    filterSequence.Map([](Bit bit) { return ~bit; });
    AssertBitSequenceEquals(filterSequence, {0, 1, 0, 1});

    filterSequence.Where([](Bit bit) { return bit.GetValue(); });
    AssertBitSequenceEquals(filterSequence, {1, 1});

    Bit reduced = filterSequence.Reduce([](Bit left, Bit right) { return left | right; }, Bit(0));
    assert(reduced.GetValue());

    Option<Bit> firstOne = filterSequence.TryGetFirst([](Bit bit) { return bit.GetValue(); });
    assert(firstOne.IsSome());
    assert(firstOne.GetValue() == Bit(1));

    Option<Bit> noZero = filterSequence.TryGetLast([](Bit bit) { return !bit.GetValue(); });
    assert(noZero.IsNone());

    BitSequence left;
    left.Append(Bit(1));
    left.Append(Bit(0));
    left.Append(Bit(1));
    left.Append(Bit(1));

    BitSequence right;
    right.Append(Bit(1));
    right.Append(Bit(1));
    right.Append(Bit(0));
    right.Append(Bit(1));

    std::unique_ptr<BitSequence> andResult(left.BitAnd(right));
    std::unique_ptr<BitSequence> orResult(left.BitOr(right));
    std::unique_ptr<BitSequence> xorResult(left.BitXor(right));
    std::unique_ptr<BitSequence> notResult(left.BitNot());

    AssertBitSequenceEquals(*andResult, {1, 0, 0, 1});
    AssertBitSequenceEquals(*orResult, {1, 1, 1, 1});
    AssertBitSequenceEquals(*xorResult, {0, 1, 1, 0});
    AssertBitSequenceEquals(*notResult, {0, 1, 0, 0});
    AssertEnumeratorEquals(left, {Bit(1), Bit(0), Bit(1), Bit(1)});

    std::cout << "OK" << std::endl;
}

void TestPerformance() {
    std::cout << "Testing Performance helpers... ";

    std::vector<PerformanceResult> results = BenchmarkAllSequences(200);
    assert(results.size() == 4);
    for (const PerformanceResult& result : results) {
        assert(!result.name.empty());
        assert(result.microseconds >= 0);
    }

    std::cout << "OK" << std::endl;
}

void TestVectorAdt() {
    std::cout << "Testing Vector ADT... ";

    Vector<int> vector({1, 2, 3});
    assert(vector.Size() == 3);
    assert(vector.Get(1) == 2);

    vector.Set(1, 5);
    assert(vector.Get(1) == 5);

    Vector<int> other({4, 5, 6});
    Vector<int> sum = vector + other;
    assert(sum == Vector<int>({5, 10, 9}));

    Vector<int> scaled = vector * 2;
    assert(scaled == Vector<int>({2, 10, 6}));

    assert(vector.Dot(other) == 47);

    Vector<double> normVector({3.0, 4.0});
    AssertDoubleEquals(normVector.Norm(), 5.0);

    Vector<std::complex<double>> complexVector({{3.0, 4.0}, {0.0, 0.0}});
    AssertDoubleEquals(complexVector.Norm(), 5.0);

    ExpectThrows<std::invalid_argument>([]() {
        Vector<int> left({1, 2});
        Vector<int> right({1, 2, 3});
        left.Add(right);
    });

    ExpectThrows<IndexOutOfRangeException>([]() {
        Vector<int> invalid({1, 2, 3});
        invalid.Set(10, 1);
    });

    std::cout << "OK" << std::endl;
}

void TestMatrixAdt() {
    std::cout << "Testing Matrix ADT... ";

    Matrix<int> matrix({{1, 2}, {3, 4}});
    assert(matrix.Rows() == 2);
    assert(matrix.Cols() == 2);
    assert(matrix.Get(1, 0) == 3);

    matrix.Set(0, 1, 5);
    assert(matrix.Get(0, 1) == 5);

    Matrix<int> other({{2, 0}, {1, 3}});
    Matrix<int> sum = matrix + other;
    assert(sum == Matrix<int>({{3, 5}, {4, 7}}));

    Matrix<int> scaled = matrix * 2;
    assert(scaled == Matrix<int>({{2, 10}, {6, 8}}));

    Matrix<double> normMatrix({{1.0, 2.0}, {2.0, 1.0}});
    AssertDoubleEquals(normMatrix.Norm(), std::sqrt(10.0));

    Matrix<int> transformed({{1, 2}, {3, 4}});
    transformed.SwapRows(0, 1);
    assert(transformed == Matrix<int>({{3, 4}, {1, 2}}));
    transformed.SwapColumns(0, 1);
    assert(transformed == Matrix<int>({{4, 3}, {2, 1}}));
    transformed.MultiplyRow(1, 3);
    assert(transformed == Matrix<int>({{4, 3}, {6, 3}}));
    transformed.AddColumnMultiple(0, 1, -1);
    assert(transformed == Matrix<int>({{4, -1}, {6, -3}}));
    transformed.AddRowMultiple(0, 1, -1);
    assert(transformed == Matrix<int>({{4, -1}, {2, -2}}));

    ExpectThrows<std::invalid_argument>([]() {
        Matrix<int> left({{1, 2}});
        Matrix<int> right({{1, 2}, {3, 4}});
        left.Add(right);
    });

    ExpectThrows<std::invalid_argument>([]() {
        Matrix<int> invalid({{1, 2}, {3}});
    });

    ExpectThrows<IndexOutOfRangeException>([]() {
        Matrix<int> invalid(1, 1, 0);
        invalid.Get(3, 0);
    });

    std::cout << "OK" << std::endl;
}

void TestPolynomialAdt() {
    std::cout << "Testing Polynomial ADT... ";

    Polynomial<int> polynomial({1, 2, 3});
    assert(polynomial.Degree() == 2);
    assert(polynomial.GetCoefficient(0) == 1);
    assert(polynomial.GetCoefficient(4) == 0);
    assert(polynomial.Evaluate(2) == 17);

    Polynomial<int> other({0, 1});
    Polynomial<int> sum = polynomial + other;
    assert(sum == Polynomial<int>({1, 3, 3}));

    Polynomial<int> product = polynomial * other;
    assert(product == Polynomial<int>({0, 1, 2, 3}));

    Polynomial<int> scaled = polynomial.MultiplyByScalar(2);
    assert(scaled == Polynomial<int>({2, 4, 6}));

    Polynomial<int> outer({1, 0, 1});
    Polynomial<int> inner({1, 1});
    Polynomial<int> composed = outer.Compose(inner);
    assert(composed == Polynomial<int>({2, 2, 1}));

    Polynomial<int> zero({0, 0, 0});
    assert(zero.Degree() == 0);
    assert(zero.ToString() == "0");

    Polynomial<std::complex<double>> complexPolynomial({{1.0, 0.0}, {2.0, 0.0}});
    AssertComplexEquals(complexPolynomial.Evaluate({2.0, 0.0}), {5.0, 0.0});

    ExpectThrows<std::invalid_argument>([]() {
        Polynomial<int> invalid({1, 2});
        invalid.GetCoefficient(-1);
    });

    std::cout << "OK" << std::endl;
}

void TestExceptions() {
    std::cout << "Testing Exceptions... ";

    DynamicArray<int> dynamicArray;
    ExpectThrows<IndexOutOfRangeException>([&]() { dynamicArray.Set(0, 1); });

    ArraySequence<int> emptyArray;
    ExpectThrows<IndexOutOfRangeException>([&]() { emptyArray.Get(0); });
    ExpectThrows<EmptySequenceException>([&]() { emptyArray.GetFirst(); });

    ListSequence<int> emptyList;
    ExpectThrows<EmptySequenceException>([&]() { emptyList.GetLast(); });

    BitSequence emptyBits;
    ExpectThrows<IndexOutOfRangeException>([&]() { emptyBits.Get(0); });

    int values[] = {1, 2, 3};
    ArraySequence<int> arraySequence(values, 3);
    ExpectThrows<IndexOutOfRangeException>([&]() { arraySequence.InsertAt(4, 10); });
    ExpectThrows<std::invalid_argument>([&]() { arraySequence.Concat(nullptr); });

    ListSequence<int> listSequence(values, 3);
    ExpectThrows<IndexOutOfRangeException>([&]() { listSequence.GetSubsequence(2, 5); });
    ExpectThrows<std::invalid_argument>([&]() { listSequence.Concat(nullptr); });

    Option<int> none = Option<int>::None();
    ExpectThrows<std::runtime_error>([&]() { none.GetValue(); });

    BitSequence bits;
    ExpectThrows<std::invalid_argument>([&]() { bits.Concat(nullptr); });

    ExpectThrows<std::invalid_argument>([]() {
        LinkedList<int> invalid(nullptr, 1);
    });

    ExpectThrows<std::invalid_argument>([]() {
        Matrix<int> invalid(-1, 2, 0);
    });

    ExpectThrows<std::invalid_argument>([]() {
        Vector<int> invalid(-1);
    });

    std::cout << "OK" << std::endl;
}

int main() {
    setlocale(LC_ALL, "");
    std::cout << "=== Running tests ===" << std::endl;

    TestDynamicArray();
    TestMutableArraySequence();
    TestImmutableArraySequence();
    TestMutableListSequence();
    TestImmutableListSequence();
    TestBitSequence();
    TestPerformance();
    TestVectorAdt();
    TestMatrixAdt();
    TestPolynomialAdt();
    TestExceptions();

    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}
