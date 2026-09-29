#include <algorithm>
#include <parlay/sequence.h>

namespace parallel_mergesort
{
    using iterator_type = parlay::sequence<long>::iterator;
    using slice = parlay::slice<iterator_type, iterator_type>;
    void merge(slice &a, slice &b, slice &c)
    {
        int total = a.size() + b.size();
        if (total <= 500)
        {
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

            int b_half;

            if (b[b.size() - 1] < mid_value)
            {
                b_half = b.size();
            }
            else
            {
                int l = 0;
                int r = b.size() - 1;
                while (l <= r)
                {
                    int b_half_test = (l + r) / 2;

                    if (b[b_half_test] > mid_value)
                    {
                        b_half = b_half_test;
                        r = b_half_test - 1;
                    }
                    else
                    {
                        l = b_half_test + 1;
                    }
                }
            }

            slice left_b = b.cut(0, b_half);
            slice right_b = b.cut(b_half, b.size());

            slice left_out = c.cut(0, a_half + b_half);
            slice right_out = c.cut(a_half + b_half, total);

            parlay::par_do(
                [&]()
                { merge(left_a, left_b, left_out); },
                [&]()
                { merge(right_a, right_b, right_out); });
        }
    }

    void mergesort_helper(slice &array, slice &buffer, bool out_to_a)
    {

        int n = array.size();

        if (n <= 500)
        {
            long tmp;

            if (out_to_a)
            {
                // for (int i = 0; i < n; i++)
                // {
                //     for (int j = i + 1; j < n; j++)
                //     {
                //         if (array[j] < array[i])
                //         {
                //             tmp = array[i];
                //             array[i] = array[j];
                //             array[j] = tmp;
                //         }
                //     }
                // }

                std::sort(array.begin(), array.end());
            }
            else
            {

                for (int i = 0; i < n; i++)
                {
                    buffer[i] = array[i];
                }

                // for (int i = 0; i < n; i++)
                // {
                //     for (int j = i + 1; j < n; j++)
                //     {
                //         if (buffer[j] < buffer[i])
                //         {
                //             tmp = buffer[i];
                //             buffer[i] = buffer[j];
                //             buffer[j] = tmp;
                //         }
                //     }
                // }

                std::sort(buffer.begin(), buffer.end());
            }
        }
        else
        {

            slice al = array.cut(0, n / 2);
            slice bl = buffer.cut(0, n / 2);
            slice ar = array.cut(n / 2, array.size());
            slice br = buffer.cut(n / 2, array.size());

            if (out_to_a)
            {
                // merge write to a, so this need to write to b
                parlay::par_do(
                    [&]()
                    { mergesort_helper(al, bl, false); },
                    [&]()
                    { mergesort_helper(ar, br, false); });

                slice buffer_l = buffer.cut(0, n / 2);
                slice buffer_r = buffer.cut(n / 2, array.size());
                merge(buffer_l, buffer_r, array);
            }
            else
            {
                // merge write to b, so this need to write to a
                parlay::par_do(
                    [&]()
                    { mergesort_helper(al, bl, true); },
                    [&]()
                    { mergesort_helper(ar, br, true); });

                slice array_l = array.cut(0, n / 2);
                slice array_r = array.cut(n / 2, array.size());
                merge(array_l, array_r, buffer);
            }
        }
    }

    void mergesort(parlay::sequence<long> &array)
    {
        slice aslice = parlay::make_slice(array);
        parlay::sequence<long> buffer(array.size());
        slice bslice = parlay::make_slice(buffer);
        mergesort_helper(aslice, bslice, true);

        // std::cout << parlay::to_chars(aslice) << std::endl;
    }
}