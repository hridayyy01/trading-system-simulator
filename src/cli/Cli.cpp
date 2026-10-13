#include "cli/Cli.hpp"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

void printUsage(const char* program) {
    std::cerr << "Usage: " << program << " [--orders N] [--seed N]\n";
}

Config parseArgs(int argc, char* argv[]) {
    Config config;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--orders") {
            if (++i >= argc) {
                throw std::invalid_argument("--orders requires a value");
            }
            config.orders = static_cast<std::size_t>(std::stoull(argv[i]));
        } else if (arg == "--seed") {
            if (++i >= argc) {
                throw std::invalid_argument("--seed requires a value");
            }
            config.seed = static_cast<std::uint32_t>(std::stoul(argv[i]));
        } else {
            throw std::invalid_argument("unknown argument: " + arg);
        }
    }

    return config;
}

void printTrades(Side aggressor, const std::vector<Trade>& trades) {
    // Single consumer thread is the only writer to stdout, so plain
    // std::cout is safe here. With multiple consumers later, push log
    // lines onto a lock-free SPSC/MPSC queue and drain them from one
    // dedicated logger thread instead of sharing std::cout.
    const char* side = aggressor == Side::BUY ? "BUY" : "SELL";
    for (const auto& trade : trades) {
        std::cout << "TRADE: " << side << ' ' << trade.qty << " @ "
                  << std::fixed << std::setprecision(2) << trade.price << '\n';
    }
}
