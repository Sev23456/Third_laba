#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include "ArraySequence.h"
#include "ListSequence.h"
#include "BitSequence.h"
#include "Matrix.h"
#include "Performance.h"
#include "Polynomial.h"
#include "Vector.h"

int ReadInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        int value;
        if (std::cin >> value) {
            return value;
        }
        std::cout << "Invalid input. Please enter an integer." << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

void WaitForEnter() {
    std::cout << "\nPress Enter to continue...";
    bool hasBufferedInput = std::cin.rdbuf()->in_avail() > 0;
    std::string dummy;
    std::getline(std::cin, dummy);
    if (hasBufferedInput && dummy.empty()) {
        std::getline(std::cin, dummy);
    }
}

void PrintMenu() {
    std::cout << "\n=== Lab work #2 ===" << std::endl;
    std::cout << "1. Test ArraySequence" << std::endl;
    std::cout << "2. Test ListSequence" << std::endl;
    std::cout << "3. Test BitSequence" << std::endl;
    std::cout << "4. Test Map-Reduce" << std::endl;
    std::cout << "5. Test Immutable" << std::endl;
    std::cout << "6. Test Iterators" << std::endl;
    std::cout << "7. Benchmark performance" << std::endl;
    std::cout << "8. Demo Vector ADT" << std::endl;
    std::cout << "9. Demo Matrix ADT" << std::endl;
    std::cout << "10. Demo Polynomial ADT" << std::endl;
    std::cout << "0. Exit" << std::endl;
}

void TestArraySequence() {
    std::cout << "\n--- ArraySequence ---" << std::endl;
    int arr[] = {1, 2, 3, 4, 5};
    ArraySequence<int>* seq = new ArraySequence<int>(arr, 5);
    
    std::cout << "Original: ";
    seq->Print();
    
    seq->Append(6);
    std::cout << "After Append(6): ";
    seq->Print();
    
    seq->Prepend(0);
    std::cout << "After Prepend(0): ";
    seq->Print();
    
    std::cout << "Get(3) = " << seq->Get(3) << std::endl;
    std::cout << "GetFirst() = " << seq->GetFirst() << std::endl;
    std::cout << "GetLast() = " << seq->GetLast() << std::endl;
    std::cout << "Length = " << seq->GetLength() << std::endl;
    
    delete seq;
}

void TestListSequence() {
    std::cout << "\n--- ListSequence ---" << std::endl;
    int arr[] = {10, 20, 30};
    ListSequence<int>* seq = new ListSequence<int>(arr, 3);
    
    std::cout << "Original: ";
    seq->Print();
    
    seq->Append(40);
    std::cout << "After Append(40): ";
    seq->Print();
    
    delete seq;
}

void TestBitSequence() {
    std::cout << "\n--- BitSequence ---" << std::endl;
    std::unique_ptr<BitSequence> bs1 = std::make_unique<BitSequence>();
    bs1->Append(Bit(1));
    bs1->Append(Bit(0));
    bs1->Append(Bit(1));
    bs1->Append(Bit(1));
    
    std::unique_ptr<BitSequence> bs2 = std::make_unique<BitSequence>();
    bs2->Append(Bit(1));
    bs2->Append(Bit(1));
    bs2->Append(Bit(0));
    bs2->Append(Bit(0));
    
    std::cout << "BS1: "; bs1->Print();
    std::cout << "BS2: "; bs2->Print();
    
    std::unique_ptr<BitSequence> andResult(bs1->BitAnd(*bs2));
    std::unique_ptr<BitSequence> orResult(bs1->BitOr(*bs2));
    std::unique_ptr<BitSequence> xorResult(bs1->BitXor(*bs2));
    std::unique_ptr<BitSequence> notResult(bs1->BitNot());

    std::cout << "BS1 AND BS2: "; andResult->Print();
    std::cout << "BS1 OR BS2: "; orResult->Print();
    std::cout << "BS1 XOR BS2: "; xorResult->Print();
    std::cout << "NOT BS1: "; notResult->Print();
}

void TestMapReduce() {
    std::cout << "\n--- Map-Reduce ---" << std::endl;
    int arr[] = {1, 2, 3, 4, 5};
    ArraySequence<int>* seq = new ArraySequence<int>(arr, 5);
    
    std::cout << "Original: ";
    seq->Print();
    
    // Map: multiply by 2
    seq->Map([](int x) { return x * 2; });
    std::cout << "After Map(x*2): ";
    seq->Print();
    
    // Where: keep only even
    seq->Where([](int x) { return x % 2 == 0; });
    std::cout << "After Where(even): ";
    seq->Print();
    
    // Reduce: sum
    int sum = seq->Reduce([](int a, int b) { return a + b; }, 0);
    std::cout << "Reduce(sum) = " << sum << std::endl;
    
    // TryGet
    Option<int> first = seq->TryGetFirst();
    if (first.IsSome()) {
        std::cout << "TryGetFirst = " << first.GetValue() << std::endl;
    }
    
    delete seq;
}

void TestImmutable() {
    std::cout << "\n--- Immutable Sequence ---" << std::endl;
    int arr[] = {1, 2, 3};
    ImmutableArraySequence<int> seq1(arr, 3);
    
    std::cout << "seq1: ";
    seq1.Print();
    
    std::unique_ptr<Sequence<int>> seq2(seq1.Append(4));
    
    std::cout << "seq1 after Append (unchanged): ";
    seq1.Print();
    std::cout << "seq2 (new): ";
    seq2->Print();
}

void TestIterators() {
    std::cout << "\n--- Iterator Demo ---" << std::endl;
    int arr[] = {3, 6, 9, 12};
    ArraySequence<int> seq(arr, 4);
    std::unique_ptr<IEnumerator<int>> enumerator = seq.GetEnumerator();

    std::cout << "Sequence via IEnumerator: [";
    bool first = true;
    while (enumerator->MoveNext()) {
        if (!first) std::cout << ", ";
        std::cout << enumerator->GetCurrent();
        first = false;
    }
    std::cout << "]" << std::endl;
}

void TestPerformance() {
    std::cout << "\n--- Performance Benchmark ---" << std::endl;
    int size = ReadInt("Enter benchmark size: ");
    if (size <= 0) {
        std::cout << "Benchmark size must be positive." << std::endl;
        return;
    }

    std::vector<PerformanceResult> results = BenchmarkAllSequences(size);
    for (const PerformanceResult& result : results) {
        std::cout << result.name << ": " << result.microseconds << " us" << std::endl;
    }
}

void TestVectorAdt() {
    std::cout << "\n--- Vector ADT ---" << std::endl;
    Vector<int> first({1, 2, 3});
    Vector<int> second({4, 5, 6});

    std::cout << "V1 = " << first << std::endl;
    std::cout << "V2 = " << second << std::endl;
    std::cout << "V1 + V2 = " << (first + second) << std::endl;
    std::cout << "V1 * 3 = " << (first * 3) << std::endl;
    std::cout << "V1 dot V2 = " << first.Dot(second) << std::endl;
    std::cout << "||V1|| = " << first.Norm() << std::endl;
}

void TestMatrixAdt() {
    std::cout << "\n--- Matrix ADT ---" << std::endl;
    Matrix<int> matrix({{1, 2}, {3, 4}});
    Matrix<int> other({{5, 6}, {7, 8}});

    std::cout << "A = " << matrix << std::endl;
    std::cout << "B = " << other << std::endl;
    std::cout << "A + B = " << (matrix + other) << std::endl;
    std::cout << "A * 2 = " << (matrix * 2) << std::endl;

    matrix.AddRowMultiple(0, 1, -3);
    std::cout << "After R2 = R2 - 3 * R1: " << matrix << std::endl;
    std::cout << "||B|| = " << other.Norm() << std::endl;
}

void TestPolynomialAdt() {
    std::cout << "\n--- Polynomial ADT ---" << std::endl;
    Polynomial<int> first({1, 2, 3});
    Polynomial<int> second({0, 1});

    std::cout << "P(x) = " << first << std::endl;
    std::cout << "Q(x) = " << second << std::endl;
    std::cout << "P + Q = " << (first + second) << std::endl;
    std::cout << "P * Q = " << (first * second) << std::endl;
    std::cout << "P(2) = " << first.Evaluate(2) << std::endl;
    std::cout << "P(Q(x)) = " << first.Compose(second) << std::endl;
}

int main() {
    setlocale(LC_ALL, "");
    
    while (true) {
        PrintMenu();
        int choice = ReadInt("Choice: ");
        
        try {
            switch (choice) {
                case 1: TestArraySequence(); break;
                case 2: TestListSequence(); break;
                case 3: TestBitSequence(); break;
                case 4: TestMapReduce(); break;
                case 5: TestImmutable(); break;
                case 6: TestIterators(); break;
                case 7: TestPerformance(); break;
                case 8: TestVectorAdt(); break;
                case 9: TestMatrixAdt(); break;
                case 10: TestPolynomialAdt(); break;
                case 0: std::cout << "Exiting..." << std::endl; return 0;
                default: std::cout << "Invalid choice!" << std::endl;
            }
            WaitForEnter();
        } catch (std::exception& ex) {
            std::cout << "Error: " << ex.what() << std::endl;
            WaitForEnter();
        }
    }
    
    return 0;
}
