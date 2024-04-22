#include "lcew.hpp"
#include "pm_wc.hpp"

/**
 * Compute the dynamic programming table used by the LCEW data structure.
 *
 * Refer to the paper for more detail.
 */
vector<vector<int>> compute_jump(vector<int> &t, unordered_set<int> &wc, vector<int> &selected_pos)
{
    int n = t.size();
    int sigma = selected_pos.size();

    auto matches = [&](int i, int j) -> bool
    {
        return t[i] == t[j] || wc.contains(t[i]) || wc.contains(t[j]);
    };

    vector<vector<bool>> occs;
    for (int i = 0; i < sigma - 1; i++)
    {
        vector<int> p(t.begin() + selected_pos[i], t.begin() + selected_pos[i + 1] + 1);
        auto tmp = pm_wc(p, t, wc);
        occs.push_back(tmp);
    }

    vector<vector<int>> jump(sigma, vector<int>(n, 0));
    for (int r = sigma - 2; r >= 0; --r)
    {
        for (int j = 0; j < n; ++j)
        {
            int lr = selected_pos[r + 1] - selected_pos[r];
            if (j + lr < n && matches(selected_pos[r], j) && occs[r][j])
            {
                jump[r][j] = std::max(0, selected_pos[r + 1] - selected_pos[r] - jump[r + 1][j + lr]);
            }
            else
            {
                jump[r][j] = 0;
            }
        }
    }

    return jump;
}

vector<vector<int>> compute_jump2(
    vector<int> &t, unordered_set<int> &wc,
    vector<int> &selected_pos, vector<int> &next_tr)
{
    int n = t.size();
    int sigma = selected_pos.size();

    auto matches = [&](int i, int j) -> bool
    {
        return t[i] == t[j] || wc.contains(t[i]) || wc.contains(t[j]);
    };

    vector<vector<bool>> occs;
    for (int i = 0; i < sigma - 1; i++)
    {
        int l_p = selected_pos[i + 1] - selected_pos[i] + 1;
        auto tmp = pm_wc_jump(selected_pos[i], l_p, t, wc, next_tr);
        occs.push_back(tmp);
    }

    vector<vector<int>> jump(sigma, vector<int>(n, 0));
    for (int r = sigma - 2; r >= 0; --r)
    {
        for (int j = 0; j < n; ++j)
        {
            int lr = selected_pos[r + 1] - selected_pos[r];
            if (j + lr < n && matches(selected_pos[r], j) && occs[r][j])
            {
                jump[r][j] = std::max(0, selected_pos[r + 1] - selected_pos[r] - jump[r + 1][j + lr]);
            }
            else
            {
                jump[r][j] = 0;
            }
        }
    }

    return jump;
}

Lcew::Lcew(vector<int> txt, int t, vector<int> wc) : text(txt), sa(txt)
{
    this->wildcards = unordered_set(wc.begin(), wc.end());
    int n = text.size();
    next_tr = vector(n, 0);
    for (int i = n - 2; i >= 0; --i)
    {
        if (i > 0 && wildcards.contains(text[i - 1]) && !wildcards.contains(text[i]))
        {
            next_tr[i] = 0;
        }
        else
        {
            next_tr[i] = next_tr[i + 1] + 1;
        }
    }

    assert(t > 0);

    vector<int> selected_pos;
    next_sel = vector(n, 1);
    int tr_count = 0;
    for (int i = 0; i < n - 1; i++)
    {
        if (next_tr[i] == 0)
        {
            if (tr_count == 0)
            {
                selected_pos.push_back(i);
                next_sel[i] = 0;
            }

            tr_count = (tr_count + 1) % t;
        }
    }
    selected_pos.push_back(n - 1);
    next_sel[n - 1] = 0;

    for (int i = n - 2; i >= 0; --i)
    {
        if (next_sel[i] != 0)
        {
            next_sel[i] = next_sel[i + 1] + 1;
        }
    }

    sel_rank = vector(n, INT32_MAX);
    for (size_t i = 0; i < selected_pos.size(); i++)
    {
        sel_rank[selected_pos[i]] = i;
    }

    jump = compute_jump(text, wildcards, selected_pos);
    // jump = compute_jump2(text, wildcards, selected_pos, next_tr);
}

int Lcew::next_selected_or_mism(int i, int j) const
{
    int r = 0;
    int m = min(next_sel[i], next_sel[j]);

    while (matches(i + r, j + r) && !(is_selected(i + r) || is_selected(j + r)))
    {
        r += sa.lce(i + r, j + r);
        // Do not go over the first selected position
        r = min(r, m);

        // If either is a wildcard, move to the end of the block of wildcards.
        int jmp = 0;
        if (is_wildcard(i + r))
        {
            jmp = max(jmp, next_tr[i + r]);
        }
        if (is_wildcard(j + r))
        {
            jmp = max(jmp, next_tr[j + r]);
        }
        r += jmp;
        // Do not go over the first selected position
        r = min(r, m);
    }

    return r;
}

int Lcew::lcew(int i, int j) const
{
    int r = 0;
    int n = text.size();

    while (i + r < n && j + r < n)
    {
        r += next_selected_or_mism(i + r, j + r);
        if (!matches(i + r, j + r))
            return r;
        else
        {
            if (is_selected(i + r))
            {
                r += jump[sel_rank[i + r]][j + r] + 1;
            }
            else
            {
                r += jump[sel_rank[j + r]][i + r] + 1;
            }
        }
    }

    return r;
}