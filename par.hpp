/*
MIT License

Copyright (c) 2026 cet-t

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <algorithm>
#include <cinttypes>
#include <functional>
#include <future>
#include <iterator>
#include <thread>
#include <vector>

class par
{
public:
    template <typename I, typename F>
    static void for_each(I begin, I end, F func)
    {
        const auto length = std::distance(begin, end);
        if (!length || length == 0)
        {
            return;
        }

        auto thread_count = std::thread::hardware_concurrency();
        auto chunk_size = length / thread_count;
        if (chunk_size == 0)
        {
            std::for_each(begin, end, func);
            return;
        }

        std::vector<std::future<void>> futures;
        futures.reserve(thread_count);
        auto chunk_start = begin;
        for (auto i = 0u; i < thread_count; ++i)
        {
            auto chunk_end = (i == thread_count - 1) ? end : std::next(chunk_start, chunk_size);

            futures.emplace_back(std::async(std::launch::async, [chunk_start, chunk_end, &func]()
                                            { std::for_each(chunk_start, chunk_end, func); }));

            chunk_start = chunk_end;
        }

        for (auto &f : futures)
        {
            f.get();
        }
    }
};