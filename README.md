# Trading System Simulator

A C++20 multi-threaded trading system simulator. Market data flows from a **Feed** into a thread-safe **Queue**, where an **Engine** consumes events and drives an **OrderBook** that maintains bids, asks, and match logic. Components live under `include/` and `src/` in matching subfolders (`feed/`, `queue/`, `engine/`, `orderbook/`, plus `bench/` and `cli/` for benchmarks and the command-line interface).

```
  +------+     +-------+     +--------+     +-----------+
  | Feed | --> | Queue | --> | Engine | --> | OrderBook |
  +------+     +-------+     +--------+     +-----------+
```

## Build

```bash
cmake -B build && cmake --build build
```
