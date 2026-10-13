# Trading System Simulator

A C++20 trading-system simulator. The **pipeline** is multi-threaded (one
producer, one consumer); **`MatchingEngine` itself is single-threaded**.
A producer thread (`FeedSimulator`) generates random orders and pushes
them into a lock-free SPSC queue. A consumer thread pops those orders
and calls `MatchingEngine::processOrder` on a single `OrderBook` that
matches crossing orders FIFO within each price level.

```
  producer thread                         consumer thread
  +----------------+    +-----------+    +------------------+    +-----------+
  | FeedSimulator  | -> | SPSCQueue | -> | MatchingEngine   | -> | OrderBook |
  | (mt19937 RNG)  |    | (lock-free|    | processOrder()   |    | bids/asks |
  +----------------+    |  ring)    |    +------------------+    +-----------+
                        +-----------+             |
                                                  v
                                           TRADE: BUY 10 @ 100.50
                                           latency samples (ns)
```

## Build

```bash
cmake -B build && cmake --build build
```

Optional ThreadSanitizer build:

```bash
cmake -B build -DENABLE_TSAN=ON && cmake --build build
```

Tests (GoogleTest via FetchContent):

```bash
ctest --test-dir build --output-on-failure
```

## Run

```bash
./build/trading-system [--orders N] [--seed N]
```

Defaults: `--orders 10000`, `--seed 42`.

## Sample output

```
$ ./build/trading-system --orders 20 --seed 1
TRADE: BUY 2 @ 99.59
TRADE: BUY 5 @ 99.80
TRADE: BUY 1 @ 99.80
TRADE: SELL 4 @ 100.03
TRADE: SELL 19 @ 99.70
TRADE: BUY 9 @ 100.06
TRADE: BUY 31 @ 100.06
TRADE: SELL 5 @ 99.77
Total orders: 20
Total trades: 8
Latency p50: 355583 ns
Latency p95: 373458 ns
Latency p99: 373458 ns
Wall-clock runtime: 635 us
```

Latency numbers vary by machine and load; trade lines and counts are
deterministic for a given `--seed`.

## Design Decisions

### Why SPSC over MPMC

The pipeline is one producer (feed) and one consumer (engine). An SPSC
ring buffer is enough for that topology: no CAS loops, no extra
synchronization around the book, and a simpler TSan story. MPMC would
only be needed if multiple feeds or multiple matching threads shared
one queue — that is out of scope for this simulator.

### Memory ordering in the queue

`head_` and `tail_` are `std::atomic<size_t>` on separate `alignas(64)`
cache lines so producer and consumer updates do not false-share.

- **Loads use `memory_order_acquire`.** The consumer must see the slot
  write that happened-before the producer's `head_` store; the producer
  must see that a slot was fully popped before reuse.
- **Stores use `memory_order_release`.** Publishing a new `head_` or
  `tail_` makes the preceding `buffer_[i] = value` / read of the slot
  visible to the other thread.

Together this is the classic SPSC publication protocol: write payload,
then release-store the index; acquire-load the index, then read payload.

### Single-writer OrderBook (no lock)

Only the consumer thread calls `MatchingEngine::processOrder`. The book
is never accessed concurrently, so bids/asks stay as ordinary
`std::map` / `deque` with no mutex. Concurrency is confined to the
queue (and the `running` shutdown flag). Adding a second consumer would
require either sharding books or putting a lock (or a mutex-free
serialization point) around the book — the current design avoids that
on purpose.
