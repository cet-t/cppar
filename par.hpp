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

#pragma once

#include <algorithm>
#include <cstddef>
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
        using diff_t = typename std::iterator_traits<I>::difference_type;
        const diff_t length = std::distance(begin, end);
        if (length <= 0)
        {
            return;
        }

        const auto hw = std::thread::hardware_concurrency();
        const diff_t thread_count = (hw == 0) ? 1 : static_cast<diff_t>(hw);

        if (length < thread_count)
        {
            std::for_each(begin, end, func);
            return;
        }

        const diff_t chunk_size = length / thread_count;

        std::vector<std::future<void>> futures;
        futures.reserve(static_cast<std::size_t>(thread_count));
        auto chunk_start = begin;
        for (diff_t i = 0; i < thread_count; ++i)
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
