#include "pch.h"
#include "orderbook.h"
#include <random>
#include <vector>

int main(){
    OrderBook book;
    book.setVerbose(false);   // critical — no printing during timing

    const int N = 1'000'000;

    // Pre-generate orders so generation isn't part of the timing
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> priceDist(95, 105);
    std::uniform_int_distribution<int> qtyDist(1, 10);
    std::uniform_int_distribution<int> sideDist(0, 1);

    struct Cmd { Side side; int qty; double price; };
    std::vector<Cmd> orders;
    orders.reserve(N);
    for(int i = 0; i < N; i++){
        Side s = sideDist(rng) ? Side::BUY : Side::SELL;
        orders.push_back({s, qtyDist(rng), (double)priceDist(rng)});
    }

    // Time only the processing
    auto start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < N; i++){
        book.addLimitOrder(i, orders[i].side, orders[i].qty, orders[i].price);
    }

    auto end = std::chrono::high_resolution_clock::now();

    double seconds = std::chrono::duration<double>(end - start).count();
    double throughput = N / seconds;

    std::cout << "Processed " << N << " orders in " << seconds << " s\n";
    std::cout << "Throughput: " << (long long)throughput << " orders/sec\n";
    std::cout << "Avg latency: " << (seconds / N) * 1e9 << " ns/order\n";

    return 0;
}