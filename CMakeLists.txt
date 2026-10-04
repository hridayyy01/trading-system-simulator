cmake_minimum_required(VERSION 3.16)
project(trading-system VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_TSAN "Compile with ThreadSanitizer (-fsanitize=thread)" OFF)
if(ENABLE_TSAN)
    add_compile_options(-g -O1 -fsanitize=thread)
    add_link_options(-fsanitize=thread)
endif()

add_executable(trading-system
    src/main.cpp
    src/feed/FeedSimulator.cpp
    src/engine/MatchingEngine.cpp
    src/orderbook/OrderBook.cpp
)

target_include_directories(trading-system PRIVATE include)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(orderbook_test
    tests/OrderBookTest.cpp
    src/orderbook/OrderBook.cpp
)

target_include_directories(orderbook_test PRIVATE include)
target_link_libraries(orderbook_test PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(orderbook_test)

add_executable(matching_engine_test
    tests/MatchingEngineTest.cpp
    src/engine/MatchingEngine.cpp
    src/orderbook/OrderBook.cpp
)

target_include_directories(matching_engine_test PRIVATE include)
target_link_libraries(matching_engine_test PRIVATE GTest::gtest_main)

gtest_discover_tests(matching_engine_test)

add_executable(spsc_queue_test
    tests/SPSCQueueTest.cpp
    tests/SPSCQueueStressTest.cpp
)

target_include_directories(spsc_queue_test PRIVATE include)
target_link_libraries(spsc_queue_test PRIVATE GTest::gtest_main)

gtest_discover_tests(spsc_queue_test)
