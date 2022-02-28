#include <iostream>
#include <vector>
#include <numeric>
#include <thread>
#include <iterator>
#include <math.h>
#include <algorithm>
#include <functional>

using namespace std;

template <typename IT, typename T>
struct accumulator_block
{
    void operator()(IT first, IT last, T &result)
    {
        result = accumulate(first, last, result);
    }
};

template <typename IT, typename T>
T parallel_Accumulate(IT first, IT last, T init)
{
    const auto length = distance(first, last);
    cout << "\nLength: " << length;

    if (!length)
        return init;

    constexpr int min_per_thread = 3;
    const unsigned int max_threads = (length + min_per_thread - 1) / min_per_thread;
    const auto hardware_threads = thread::hardware_concurrency();
    const auto num_threads = min(hardware_threads == 0 ? 2 : hardware_threads, max_threads);
    const auto block_size = length / num_threads;

    vector<T> results(num_threads, init);
    vector<thread> threads(num_threads - 1);

    IT block_start = first;
    for (unsigned int i = 0; i < (num_threads - 1); i++)
    {
        IT block_end = block_start;
        advance(block_end, block_size);
        threads[i] = thread(accumulator_block<IT, T>(), block_start, block_end, ref(results[i]));

        block_start = block_end;
    }

    accumulator_block<IT, T>()(block_start, last, ref(results[num_threads - 1]));

    std::for_each(threads.begin(), threads.end(), mem_fn(&thread::join));

    return accumulate(results.cbegin(), results.cend(), init);
}

int main()
{
    cout << "\n\n\nParallel Accumulator\n\n\n";

    vector numbers{1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto seqAccumResult = accumulate(numbers.cbegin(), numbers.cend(), 0);
    cout << "Result from sequential accumulator: " << seqAccumResult;

    auto parAccumResult = parallel_Accumulate(numbers.cbegin(), numbers.cend(), 0);
    cout << "\nResult from parallel accumulator: " << parAccumResult;

    cout << endl;
    return 0;
}