#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

void sum_range(const int* data, size_t b, size_t e, long long* out) {
    long long s = 0;
    for (size_t j = b; j < e; ++j)
        s += data[j];
    *out = s;
}

int main() {
    const size_t N = 1'000'000;
    const size_t THREADS = 4;

    std::vector<int> data(N);
    unsigned x = 123456789u;
    for (size_t i = 0; i < N; ++i) {
        x = x * 1103515245u + 12345u;
        data[i] = int(x % 100u) + 1;
    }

    auto t1_start = std::chrono::high_resolution_clock::now();
    long long sum1 = 0;
    for (size_t i = 0; i < N; ++i)
        sum1 += data[i];
    auto t1_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> t1 = t1_end - t1_start;

    std::cout << "Single Thread:\n";
    std::cout << "-----------------------------\n";
    std::cout << "Sum:  " << sum1 << "\n";
    std::cout << "Time: " << t1.count() << " s\n";
    std::cout << "-----------------------------\n\n";

    auto t2_start = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threads(THREADS);
    std::vector<long long> partial(THREADS, 0);

    const size_t chunk = N / THREADS;
    for (size_t i = 0; i < THREADS; ++i) {
        size_t begin = i * chunk;
        size_t end = (i == THREADS - 1) ? N : begin + chunk;
        threads[i] = std::thread(sum_range, data.data(), begin, end, &partial[i]);
    }

    for (auto& th : threads)
        th.join();

    long long sum2 = 0;
    for (size_t i = 0; i < THREADS; ++i)
        sum2 += partial[i];

    auto t2_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> t2 = t2_end - t2_start;

    std::cout << "4 Threads:\n";
    std::cout << "-----------------------------\n";
    std::cout << "Sum:  " << sum2 << "\n";
    std::cout << "Time: " << t2.count() << " s\n";
    std::cout << "-----------------------------\n";
    std::cout << "Speedup: " << (t1.count() / t2.count()) << "x\n";

    return 0;
}
