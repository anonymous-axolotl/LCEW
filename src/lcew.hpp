#pragma once

#include "ukkonen.hpp"
#include "lce.hpp"
#include <vector>
#include <unordered_set>
#include <cassert>

using std::unordered_set;
using std::vector;

const char DEFAULT_WILDCARD = '#';

/**
 * Data structure for efficient longest common extension queries
 * in a text `T` with wildcards (LCEW).
 * 
 * See our paper [Bathie, Charalampopoulos & Starikovskaya, to appear]
 * for more details.
 */
class Lcew
{
private:
    vector<int> text;
    unordered_set<int> wildcards;
    vector<int> next_tr;
    vector<int> next_sel;
    vector<int> sel_rank;
    vector<vector<int>> jump;
    Lce sa;

public:
    /**
     * Build the LCEW data structure.
     * 
     * Constructs the data structure for the text `txt`, with query time `O(t)`
     * and using symbols in `wc` as wildcards.
     * 
     * The parameter `t` must be a positive (> 0) integer.
     * 
     * If `wc` is not specified, it defaults to `DEFAULT_WILDCARD` ('#').
     */
    Lcew(vector<int> txt, int t, vector<int> wc = {DEFAULT_WILDCARD});
    /**
     * Build the LCEW data structure (string text).
     */
    Lcew(string &s, int t, vector<int> wc): Lcew(vector<int>(s.begin(), s.end()), t, wc) {};

    /**
     * Get the value of the LCEW between `T[i..]` and `T[j..]`
     */
    
    int lcew(int i, int j) const;

private:
    inline bool is_selected(int i) const { return next_sel[i] == 0; };
    inline bool is_wildcard(int i) const { return wildcards.contains(text[i]); };
    inline bool matches(int i, int j) const
    {
        return text[i] == text[j] || is_wildcard(i) || is_wildcard(j);
    }

    /**
     * Returns the first selected position or mismatch between
     * `T[i..]` and `T[j..]`.
     */
    int next_selected_or_mism(int i, int j) const;
};