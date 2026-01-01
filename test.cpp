#include "par.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <mutex>
#include <numeric>
#include <vector>

static void test_empty()
{
    std::vector<int> v;
    par::for_each(v.begin(), v.end(), [](int) { assert(false); });
    std::cout << "test_empty: passed\n";
}

static void test_single()
{
    std::vector<int> v = {42};
    int result = 0;
    std::mutex mx;
    par::for_each(v.begin(), v.end(), [&result, &mx](int i)
    {
        std::lock_guard<std::mutex> lock(mx);
        result = i;
    });
    assert(result == 42);
    std::cout << "test_single: passed\n";
}

static void test_fewer_than_threads()
{
    std::vector<int> v = {1, 2, 3};
    std::vector<int> res;
    std::mutex mx;
    par::for_each(v.begin(), v.end(), [&res, &mx](int i)
    {
        std::lock_guard<std::mutex> lock(mx);
        res.push_back(i);
    });
    std::sort(res.begin(), res.end());
    assert((res == std::vector<int>{1, 2, 3}));
    std::cout << "test_fewer_than_threads: passed\n";
}

static void test_basic()
{
    const int size = 1024;
    std::vector<int> v(size);
    std::iota(v.begin(), v.end(), 0);

    std::vector<int> res;
    res.reserve(size);
    std::mutex mx;
    par::for_each(v.begin(), v.end(), [&res, &mx](int i)
    {
        std::lock_guard<std::mutex> lock(mx);
        res.push_back(i * 2);
    });

    std::sort(res.begin(), res.end());

    std::vector<int> expected(size);
    std::transform(v.begin(), v.end(), expected.begin(), [](int i) { return i * 2; });

    assert(res == expected);
    std::cout << "test_basic: passed\n";
}

static void test_all_elements_processed()
{
    const int size = 10000;
    std::vector<int> v(size, 1);
    std::vector<int> processed(size, 0);
    std::mutex mx;

    par::for_each(v.begin(), v.end(), [&processed, &mx, &v](int &elem)
    {
        std::lock_guard<std::mutex> lock(mx);
        std::size_t idx = static_cast<std::size_t>(&elem - v.data());
        processed[idx] = 1;
    });

    assert(std::all_of(processed.begin(), processed.end(), [](int x) { return x == 1; }));
    std::cout << "test_all_elements_processed: passed\n";
}

int main()
{
    std::cout << "threads: " << std::thread::hardware_concurrency() << "\n";

    test_empty();
    test_single();
    test_fewer_than_threads();
    test_basic();
    test_all_elements_processed();

    std::cout << "all tests passed\n";
    return 0;
}
