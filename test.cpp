#include "par.hpp"
#include <bits/stdc++.h>
#include <format>

#define SIZE 1024

int main(int argc, const char **argv)
{
#ifdef __cpp_lib_format
    std::cout << std::format("threads   : {}", std::thread::hardware_concurrency()) << std::endl;
    std::cout << std::format("chunks    : {}", SIZE / std::thread::hardware_concurrency()) << std::endl;
#endif

    auto v = std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (int i = 0; i < SIZE; ++i)
    {
        v.push_back(i);
    }

    auto res = std::vector<int>();
    std::mutex res_mx;
    par::for_each(v.begin(), v.end(), [&res, &res_mx](int i)
                  { 
                    std::lock_guard<std::mutex> lock2(res_mx); 
                    res.push_back(i*2); });

    for (auto i = res.begin(); i < res.end(); i += SIZE / 100)
    {
        std::cout << *i << std::endl;
    }

    return 0;
}