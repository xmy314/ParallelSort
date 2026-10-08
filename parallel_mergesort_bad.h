#include <algorithm>
#include <parlay/sequence.h>

namespace parallel_mergesort_bad
{
    inline constexpr int MERGE_GRANULARITY = 256;
    inline constexpr int MERGE_SORT_GRANULARITY = 512;

    using iterator_type = parlay::sequence<long>::iterator;
    using slice = parlay::slice<iterator_type, iterator_type>;
    parlay::sequence<long> merge(slice a, slice b)
    {
        int total = a.size() + b.size();
        if (total <= MERGE_GRANULARITY)
        {
            parlay::sequence<long> c(a.size() + b.size());
            int ai = 0, ci = 0, bi = 0;
            while (ai < a.size() && bi < b.size())
            {
                if (a[ai] < b[bi])
                {
                    c[ci] = a[ai];
                    ai++;
                    ci++;
                }
                else
                {
                    c[ci] = b[bi];
                    bi++;
                    ci++;
                }
            }
            while (ai < a.size())
            {
                c[ci] = a[ai];
                ai++;
                ci++;
            }
            while (bi < b.size())
            {
                c[ci] = b[bi];
                bi++;
                ci++;
            }
            return c;
        }
        else
        {
            if (a.size() < b.size())
            {
                slice tmp = a;
                a = b;
                b = tmp;
            }
            // a is longer.

            int a_half = a.size() / 2;
            long mid_value = a[a_half];

            slice right_a = a.cut(a_half, a.size());
            slice left_a = a.cut(0, a_half);

            int b_half = std::upper_bound(b.begin(), b.end(), mid_value) - b.begin();

            slice left_b = b.cut(0, b_half);
            slice right_b = b.cut(b_half, b.size());

            parlay::sequence<long> l_out;
            parlay::sequence<long> r_out;

            parlay::par_do(
                [&]()
                { l_out = merge(left_a, left_b); },
                [&]()
                { r_out = merge(right_a, right_b); });

            parlay::sequence<long> o = parlay::flatten(parlay::sequence<parlay::sequence<long>>{l_out, r_out});
            return o;
        }
    }

    parlay::sequence<long> mergesort_helper(parlay::sequence<long> &array, int start, int end)
    {

        int n = end - start;

        if (n <= MERGE_SORT_GRANULARITY)
        {
            std::sort(array.begin() + start, array.begin() + end);

            return array.subseq(start, end);
        }
        else
        {

            parlay::sequence<long> l_sorted(n / 2);
            parlay::sequence<long> r_sorted(n - (n / 2));

            // merge write to a, so this need to write to b
            parlay::par_do(
                [&]()
                { l_sorted = mergesort_helper(array, start, start + n / 2); },
                [&]()
                { r_sorted = mergesort_helper(array, start + n / 2, end); });

            slice l_slice = parlay::make_slice(l_sorted);
            slice r_slice = parlay::make_slice(r_sorted);
            return merge(l_slice, r_slice);
        }
    }

    void mergesort(parlay::sequence<long> &array)
    {
        mergesort_helper(array, 0, array.size());
        // std::cout << parlay::to_chars(aslice) << std::endl;
    }
}