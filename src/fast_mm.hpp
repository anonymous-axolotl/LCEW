#pragma once

#include <vector>

using std::vector;

using entry = std::pair<int, int>;

/**
 * Represents a square sparse boolean matrix.
 * 
 * Contains the list of all pairs (i,j)
 * such that M[i, j] is true.
 * Indices are 0-indexed.
 */ 
class SparseBoolMatrix
{
public:
    int n;
    vector<entry> entries;

    static SparseBoolMatrix from_dense(vector<vector<bool>> &v);

    bool operator==(const SparseBoolMatrix &other)
    {
        return other.n == n && other.entries == entries;
    }
};

/**
 * Compute boolean matrix multiplication using a reduction
 * to LCEW.
 */
SparseBoolMatrix matrix_mult(const SparseBoolMatrix &a, const SparseBoolMatrix &b);