#pragma once

#include "ukkonen.hpp"
#include <vector>

using std::vector;

/**
 * Data structure for constant-time LCE queries in a text T.
 * 
 * Stores the inverse suffix array of `T`
 * and an `O(n log n)` space dynamic programming table
 * for constant time Range minimum query over the suffix array.
 */
class Lce
{
private:
    vector<int> isa;
    vector<vector<int>> mem;

public:
    Lce(vector<int> &s)
    {
        s.push_back(-1);
        int n = s.size();

        SuffixTree st;
        st.Create_suffix_tree(&s, n);
        st.Compute_suffix_arrays();
        isa = std::move(st.RANK);
        mem = std::move(st.DBF);
        st.Delete_suffix_tree();

        s.pop_back();
    }

    /**
     * Query the value of the LCE of `T[i..]` and `T[..j]`.
     */
    int lce(int i, int j) const
    {
        int n = isa.size();
        if (i == j)
            return n - i;
        int i1 = isa[i], j1 = isa[j];
        if (i1 > j1)
            std::swap(i1, j1);
        int level = 8 * sizeof(int) - 1 - __builtin_clz(j1 - i1);
        return std::min(mem[level][i1], mem[level][j1 - (1 << level)]);
    }
};