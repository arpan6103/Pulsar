# Pulsar

A high-frequency limit order book simulator in C++ — the core matching engine that sits at the heart of every exchange.

Pulsar implements price-time priority matching with limit and market orders, true O(1) cancellation, and is benchmarked at roughly **10 million orders/sec** on a single thread.

---

## What is an order book?

An order book is the central data structure of every exchange. It holds two sides:

- **Bids** — buy orders, sorted highest price first
- **Asks** — sell orders, sorted lowest price first

A trade happens when an incoming order *crosses the spread* — a buyer willing to pay at least what a seller is asking. Orders are matched in **price-time priority**: better prices fill first, and at the same price, earlier orders fill first (FIFO).

---

## Features

- **Limit orders** — rest on the book at a specified price until matched
- **Market orders** — execute immediately at the best available price, never rest
- **Partial fills** — large orders consume multiple price levels
- **O(1) cancellation** — remove any resting order instantly by ID
- **O(1) best bid/ask** — the most frequently read values in trading
- **Book display** — inspect current resting liquidity
- **File-driven test harness** — run arbitrary scenarios without recompiling
- **Benchmark suite** — measure throughput and latency

---

## Design

### Core data structure

```cpp
std::map<double, std::list<Order>> bids;   // price level -> FIFO list of orders
std::map<double, std::list<Order>> asks;
std::unordered_map<int, OrderLocation> cancel_order;  // id -> exact location
```

- **`std::map`** keeps price levels automatically sorted. Best ask is `begin()`, best bid is `prev(end())`.
- **`std::list`** at each price level holds orders in FIFO arrival order. Critically, list iterators stay valid across insertions and erasures elsewhere — which is what makes O(1) cancellation possible.
- **`unordered_map<int, OrderLocation>`** maps each order ID to its exact location (price, side, and a list iterator), so cancellation is a direct hash lookup followed by an O(1) list erase — no scanning.

### Why a list, not a queue?

A `std::queue` only allows removal from the front. But cancellation must remove a *specific* order from the *middle* of a price level. Using a queue forces either an O(n) scan or a "lazy deletion" hack that leaves dangling pointers behind.

A `std::list` is a doubly-linked list: given an iterator to a node, `erase(it)` unlinks it in O(1) by rewiring its two neighbors. We store that iterator at insertion time, so cancellation jumps straight to the node and removes it cleanly. This is the design real exchanges use.

### Memory

Orders live directly inside the list nodes — no manual `new`/`delete`. The list owns each node and destroys it on `erase` or `pop_front`, so there are no leaks and no dangling pointers.

---

## File layout

| File | Purpose |
|------|---------|
| `pch.h` | Common includes |
| `order.h` | `Order` struct, `Side` and `OrderType` enums |
| `orderbook.h` | `OrderBook` class declaration |
| `orderbook.cpp` | Matching engine, add/cancel logic, book display |
| `main.cpp` | Interactive / file-driven command harness |
| `benchmark.cpp` | Throughput and latency benchmark |
| `test.txt` | Sample command script |

---

## Build & run

### Interactive harness

```bash
g++ *.cpp -o main
./main < test.txt
```

The harness reads commands from stdin:

```
LIMIT  BUY|SELL  <id> <price> <qty>
MARKET BUY|SELL  <id> <qty>
CANCEL <id>
PRINT
```

Example `test.txt`:

```
LIMIT SELL 1 100 5
LIMIT SELL 2 101 3
LIMIT SELL 3 102 8
LIMIT BUY 4 101 10
MARKET BUY 5 5
LIMIT SELL 6 105 10
CANCEL 6
MARKET BUY 7 5
PRINT
```

### Benchmark

```bash
g++ -O2 benchmark.cpp orderbook.cpp -o bench
./bench
```

The `-O2` optimization flag is essential — unoptimized C++ benchmarks are meaningless.

---

## Performance

Single-threaded, 1,000,000 random limit orders, prices clustered in a realistic crossing range, output suppressed:

```
Processed 1000000 orders in ~0.10 s
Throughput: ~9.8 million orders/sec
Avg latency: ~102 ns/order
```

That's roughly 300-400 CPU cycles per order, covering a price-level lookup, matching, a list insert, and a hash-map insert.

---

## What's next

Directions for pushing throughput higher, in rough order of impact:

1. **Replace `std::map` with a flat price array.** Prices are discrete ticks in a bounded range, so a red-black tree (scattered heap nodes, cache misses) can be swapped for a contiguous array indexed by tick. Typically a 3-5x gain.
2. **Custom memory pool.** Pre-allocate a block and hand out order slots, eliminating per-order heap allocation.
3. **Integer tick prices.** Drop floating point entirely — faster and avoids precision bugs.

Other feature directions: in-place order modification (preserving time priority), a trade log with VWAP and volume statistics, and multi-threaded matching.

---

## Notes

Pulsar is a learning and portfolio project built to understand exchange internals from the ground up. The design mirrors how production matching engines are structured, while staying readable enough to reason about every line.